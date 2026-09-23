#include "http_client.h"

// 减少 Windows 头文件引入的内容，降低编译时间和名称污染。
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// Windows 头文件历史上会定义 max(a, b) 和 min(a, b) 宏。
// 它们会干扰 std::numeric_limits<T>::max()、 std::max() 等标准 C++ API。
// NOMINMAX 必须在任何 Windows 头文件之前定义，阻止这些宏被声明。
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <winsock2.h>
#include <ws2tcpip.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace {

/**
 * 管理当前调用所需的 Windows Winsock 运行环境。
 *
 * Windows 要求程序在调用 socket API 前执行 WSAStartup，并在结束后执行 WSACleanup。
 * 该类把初始化放进构造函数，把清理放进析构函数，使两者一定成对执行。
 *
 * 这就是 RAII：资源获取即初始化。对象离开作用域时，析构函数自动运行；
 * 即使中途抛出异常，也不会跳过清理。Java 中最接近的是 try-with-resources，
 * 但 C++ RAII 不局限于实现某个接口，它是对象生命周期的普遍机制。
 */
class WinsockSession {
public:
    /**
     * 初始化 Winsock 2.2。
     *
     * MAKEWORD(2, 2) 把主版本号和次版本号编码成 Winsock 要求的 WORD。
     * data{} 使用值初始化，把结构体字段清零，避免把未初始化内存交给 C API。
     */
    WinsockSession() {
        WSADATA data{};
        const int result = WSAStartup(MAKEWORD(2, 2), &data);

        if (result != 0) {
            throw std::runtime_error("WSAStartup failed with code: " +
                                     std::to_string(result));
        }
    }

    /**
     * 结束本对象对应的 Winsock 会话。
     *
     * C++ 析构函数以 ~ 开头，没有返回类型。对象离开作用域时自动调用，
     * 不需要像 Java 那样等待 GC，也不需要调用者记住 finally。
     */
    ~WinsockSession() {
        WSACleanup();
    }

    // 复制该对象不会再次调用 WSAStartup，却会多调用一次 WSACleanup，因此禁止复制。
    // = delete 表示这个函数在类型接口中明确存在，但任何调用都会在编译期失败。
    WinsockSession(const WinsockSession&) = delete;

    WinsockSession& operator=(const WinsockSession&) = delete;

    // 当前类也不需要转移所有权，明确禁止移动，避免会话计数的生命周期变得含糊。
    WinsockSession(WinsockSession&&) = delete;

    WinsockSession& operator=(WinsockSession&&) = delete;
};

/**
 * 独占管理一个 Windows SOCKET 句柄。
 *
 * SOCKET 是 Winsock C API 返回的操作系统资源句柄，不是自动管理的 C++ 对象。
 * SocketHandle 确保每个有效句柄最终只被 closesocket 一次。
 *
 * 该类不可复制但可以移动：
 * - 不可复制：两个对象不能同时认为自己拥有同一个 socket，否则会重复关闭。
 * - 可以移动：允许把唯一所有权安全地转交给返回值或另一个变量。
 *
 * Java 的 Socket 本身已经是对象封装；这里是在 C++ 中为裸 C 句柄补上相同的生命周期边界。
 */
class SocketHandle {
public:
    /**
     * 接管一个由 Winsock 创建的 socket 句柄。
     *
     * explicit 禁止把 SOCKET 隐式转换为 SocketHandle，调用者必须明确写出构造动作。
     * 这可以防止普通整数或句柄在参数传递中意外变成拥有资源的对象。
     *
     * 冒号后的 socket_(socket) 是成员初始化列表：在进入构造函数体之前，
     * 直接使用参数 socket 初始化成员 socket_。它不是 Java 的 super(...) 调用。
     */
    explicit SocketHandle(SOCKET socket) : socket_(socket) {
    };

    /**
    * 释放当前对象仍然拥有的 socket。
    */
    ~SocketHandle() {
        close();
    }

    // 禁止复制构造和复制赋值，保证 socket 只有一个所有者。
    SocketHandle(const SocketHandle&) = delete;

    SocketHandle& operator=(const SocketHandle&) = delete;

    /**
     * 移动构造：从 other 接管 socket，创建一个新的 SocketHandle。
     *
     * && 表示右值引用，这里代表一个允许被“搬空”的临时对象或显式移动对象。
     * std::exchange 返回 other 原来的句柄，同时把 other 设为 INVALID_SOCKET。
     * 搬空后的 other 仍可安全析构，因为 close() 会忽略 INVALID_SOCKET。
     *
     * noexcept 声明该操作不会抛异常。标准容器在重分配元素时通常只有在移动操作
     * 保证不抛异常时才放心使用移动，否则可能退回复制；本类又禁止复制，因此该保证很重要。
     */
    SocketHandle(SocketHandle&& other) noexcept
        : socket_(std::exchange(other.socket_, INVALID_SOCKET)) {
    }

    /**
     * 移动赋值：让已经存在的对象释放旧 socket，再接管 other 的 socket。
     *
     * operator= 是 C++ 的运算符重载语法：这里定义 SocketHandle 对象执行 a = b 时的行为。
     * Java 不能重载赋值运算符；Java 的引用赋值也不会转移底层资源所有权。
     */
    SocketHandle& operator=(SocketHandle&& other) noexcept {
        // this 是指向当前对象的隐藏指针；&other 是参数对象的地址。
        // 比较地址用于防止 socket_handle = std::move(socket_handle) 这种自移动赋值。
        if (this != &other) {
            // 当前对象可能已经拥有 socket，接管新资源前必须先释放旧资源。
            close();

            // 取走 other 的句柄，并立刻把 other 标记为空所有者。
            socket_ = std::exchange(other.socket_, INVALID_SOCKET);
        }

        // *this 表示对 this 指针解引用，得到“当前对象本身”。
        // 返回 SocketHandle&（当前对象的引用）符合赋值运算符惯例，并允许 a = b = c 链式赋值。
        return *this;
    }

    /**
     * 借出底层句柄给 Winsock 函数使用，但不转移所有权。
     *
     * 方法末尾的 const 表示该方法不会修改当前 SocketHandle 对象的可观察状态。
     */
    [[nodiscard]] SOCKET get() const {
        return socket_;
    }

private:
    /**
     * 如果当前持有有效 socket，则关闭它并把成员恢复为空状态。
     *
     * 该方法是幂等的：调用多次也只会真正关闭一次。
     */
    void close() {
        if (socket_ != INVALID_SOCKET) {
            closesocket(socket_);
            socket_ = INVALID_SOCKET;
        }
    }

    SOCKET socket_;
};

/**
 * 将 URL 中的十进制端口文本转换成 16 位无符号端口号。
 */
std::uint16_t parse_port(const std::string& value) {
    // stoul 对 "-1" 的处理不符合端口语义，因此先拒绝负号。
    if (value.empty() || value.front() == '-') {
        throw std::invalid_argument("Invalid HTTP port: " + value);
    }

    std::size_t parsed_char_count = 0;
    unsigned long parsed_value = 0;

    try {
        // stoul = string to unsigned long。
        // 第二个参数接收成功解析的字符数，用于拒绝 "8080abc" 这种部分合法输入。
        parsed_value = std::stoul(value, &parsed_char_count);
    } catch (const std::exception&) {
        throw std::invalid_argument("Invalid HTTP port: " + value);
    }

    // constexpr 表示该变量必须能在编译期求值，并且之后不可修改。
    // 它比 const 更强：const 只保证运行期间不修改，初始值仍可能在运行期才能得到。
    //
    // numeric_limits<T>::max() 返回类型 T 能表示的最大值。
    // 外层 (max)() 是兼容 Windows max 宏的防御性写法：
    // 宏只有在标识符 max 后立刻出现左括号时才会展开；先用括号包住函数名可阻止宏匹配，
    // 最后的 () 再真正调用这个静态成员函数。
    constexpr unsigned long max_tcp_port =
        static_cast<unsigned long>((std::numeric_limits<std::uint16_t>::max)());

    if (parsed_char_count != value.size() || parsed_value > max_tcp_port) {
        throw std::invalid_argument("Invalid HTTP port: " + value);
    }

    // 前面的范围检查证明值不会溢出，现在才把较宽的 unsigned long 缩窄为 uint16_t。
    return static_cast<std::uint16_t>(parsed_value);
}

/**
 * 解析主机名并依次尝试候选地址，直到建立 TCP 连接。
 */
SocketHandle connect_to_endpoint(const edgelab::simulator::HttpEndpoint& endpoint) {
    // addrinfo 是 getaddrinfo 使用的 C 结构体。{} 将全部字段清零。
    addrinfo hints{};
    hints.ai_family = AF_UNSPEC;       // 同时接受 IPv4 和 IPv6 解析结果。
    hints.ai_socktype = SOCK_STREAM;   // HTTP/1.1 使用面向连接的 TCP 字节流。
    hints.ai_protocol = IPPROTO_TCP;   // 明确要求 TCP。

    // C API 通过二级指针输出一条链表；初始值必须为空。
    addrinfo* raw_results = nullptr;
    const std::string port_text = std::to_string(endpoint.port);

    // c_str() 暴露以 '\0' 结尾的 const char*，供只理解 C 字符串的 Winsock API 使用。
    const int lookup_result = getaddrinfo(endpoint.host.c_str(),
                                          port_text.c_str(), &hints, &raw_results);
    if (lookup_result != 0) {
        throw std::runtime_error("Failed to resolve host '" + endpoint.host +
                                 "': " + gai_strerrorA(lookup_result));
    }

    // getaddrinfo 分配的链表必须由 freeaddrinfo 释放。
    // unique_ptr 的第一个模板参数是被管理的资源类型，第二个参数是删除器类型。
    // decltype(&freeaddrinfo) 让编译器推导“freeaddrinfo 函数指针”的准确类型。
    // 离开作用域时，results 自动执行 freeaddrinfo(raw_results)。
    std::unique_ptr<addrinfo, decltype(&freeaddrinfo)> results{raw_results, freeaddrinfo};

    // current 是遍历 C 链表的非拥有裸指针；资源所有权仍属于 results。
    // current->ai_next 等价于 (*current).ai_next。
    for (addrinfo* current = results.get(); current != nullptr; current = current->ai_next) {
        // 根据当前候选地址的协议参数创建底层 socket。
        SOCKET raw_socket =
            socket(current->ai_family, current->ai_socktype, current->ai_protocol);

        if (raw_socket == INVALID_SOCKET) {
            continue;
        }

        // 立即把裸句柄交给 RAII 对象。之后即使 connect 失败或抛异常也不会泄漏。
        SocketHandle socket_handle(raw_socket);

        // connect 返回 0 表示连接成功。
        if (connect(socket_handle.get(),
                    current->ai_addr,
                    static_cast<int>(current->ai_addrlen)) == 0) {
            // SocketHandle 禁止复制，因此返回局部对象时编译器会执行移动或直接省略复制。
            // 返回后，调用方成为 socket 的唯一所有者。
            return socket_handle;
        }

        // 本轮失败时 socket_handle 离开作用域，其析构函数关闭当前候选 socket。
    }

    throw std::runtime_error("Failed to connect to " + endpoint.host + ":" + port_text);
}

/**
 * 循环调用 send，直到请求的全部字节都发送完成。
 *
 * TCP send 只保证返回“本次实际写入的字节数”，不保证一次写完整个字符串。
 * 因此不能只调用一次 send。
 */
void send_all(SOCKET socket, const std::string& data) {
    constexpr std::size_t max_chunk_size = 4096;
    std::size_t sent_total = 0;

    while (sent_total < data.size()) {
        const std::size_t remaining = data.size() - sent_total;
        const std::size_t current_chunk_size = std::min(remaining, max_chunk_size);

        // data.data() 返回首字符指针；加 sent_total 后指向尚未发送部分的起点。
        // Winsock send 的长度参数是 int，因此先把块大小限制在 4096，再显式转换。
        const int sent = send(socket,
                              data.data() + sent_total,
                              static_cast<int>(current_chunk_size),
                              0);

        if (sent == SOCKET_ERROR || sent == 0) {
            throw std::runtime_error("Failed to send HTTP request, WSA error: " +
                                     std::to_string(WSAGetLastError()));
        }

        sent_total += static_cast<std::size_t>(sent);
    }
}

/**
 * 持续接收响应字节，直到服务器关闭连接。
 *
 * 当前请求显式发送 Connection: close，所以服务器关闭连接就是响应结束标志。
 * 这简化了第一版实现，但不支持连接复用，也不适合高吞吐生产客户端。
 */
std::string receive_all(SOCKET socket) {
    constexpr std::size_t buffer_size = 4096;
    std::array<char, buffer_size> buffer{};
    std::string response;

    while (true) {
        const int received = recv(socket, buffer.data(), sizeof(buffer), 0);

        // recv 返回 0 表示对端已经正常关闭连接。
        if (received == 0) {
            break;
        }

        if (received == SOCKET_ERROR) {
            throw std::runtime_error("Failed to receive HTTP response, WSA error: " +
                                     std::to_string(WSAGetLastError()));
        }

        // 网络数据不保证以 '\0' 结尾，因此必须使用“指针 + 长度”追加，不能当 C 字符串处理。
        response.append(buffer.data(), static_cast<std::size_t>(received));
    }

    return response;
}

/**
 * 从 HTTP 响应首行中解析状态码，例如从 "HTTP/1.1 202 Accepted" 得到 202。
 */
int parse_status_code(const std::string& raw_response) {
    const std::size_t line_end = raw_response.find("\r\n");

    if (line_end == std::string::npos) {
        throw std::runtime_error("Invalid HTTP response: missing status line ending");
    }

    const std::string status_line = raw_response.substr(0, line_end);
    std::istringstream status_stream(status_line);

    std::string http_version;
    int status_code = 0;
    status_stream >> http_version >> status_code;

    if (!status_stream || http_version.rfind("HTTP/", 0) != 0 ||
        status_code < 100 || status_code > 599) {
        throw std::runtime_error("Invalid HTTP status line: " + status_line);
    }

    return status_code;
}

/**
 * 返回 HTTP 头部结束标记之后的响应体。
 */
std::string extract_body(const std::string& raw_response) {
    constexpr std::size_t separator_length = 4;
    const std::size_t separator = raw_response.find("\r\n\r\n");

    if (separator == std::string::npos) {
        throw std::runtime_error("Invalid HTTP response: missing header separator");
    }

    return raw_response.substr(separator + separator_length);
}

} // namespace

namespace edgelab::simulator {

/**
 * 解析第一版客户端支持的最小 HTTP URL。
 */
HttpEndpoint parse_http_url(const std::string& url) {
    const std::string scheme = "http://";

    // rfind(value, 0) 只在下标 0 查找，相当于 C++17 中的 starts_with。
    if (url.rfind(scheme, 0) != 0) {
        throw std::invalid_argument("Only http:// URLs are supported: " + url);
    }

    // 移除协议部分，剩余内容形如 localhost:8080/api/v1/telemetry。
    const std::string without_scheme = url.substr(scheme.size());
    const std::size_t path_start = without_scheme.find('/');

    // 如果没有路径，则请求目标使用 HTTP 规定的根路径 "/"。
    const std::string host_and_port =
        path_start == std::string::npos
        ? without_scheme
        : without_scheme.substr(0, path_start);

    const std::string target =
        path_start == std::string::npos ? "/" : without_scheme.substr(path_start);

    if (host_and_port.empty()) {
        throw std::invalid_argument("HTTP URL host must not be empty");
    }

    // 第一版不支持 IPv6 字面量，因此最后一个冒号被视为 host 和 port 的分隔符。
    const std::size_t port_separator = host_and_port.rfind(':');

    if (port_separator == std::string::npos) {
        // HttpEndpoint{...} 是聚合初始化，按结构体字段声明顺序构造值。
        return HttpEndpoint{host_and_port, 80, target};
    }

    const std::string host = host_and_port.substr(0, port_separator);
    const std::string port_text = host_and_port.substr(port_separator + 1);

    if (host.empty() || port_text.empty()) {
        throw std::invalid_argument("Invalid HTTP URL: " + url);
    }

    return HttpEndpoint{host, parse_port(port_text), target};
}

/**
 * 完成一次 JSON HTTP POST 短连接调用。
 */
HttpResponse post_json(const HttpEndpoint& endpoint, const std::string& json_body) {
    // 1. 初始化 Winsock。后声明的局部对象会先析构，因此 socket 会在 Winsock 清理前关闭。
    WinsockSession winsock_session;

    // 2. 解析主机名并建立 TCP 连接。
    SocketHandle socket_handle = connect_to_endpoint(endpoint);

    // 3. 按 HTTP/1.1 文本协议组装请求行、请求头、空行和 JSON 请求体。
    // Content-Length 是请求体的字节数；std::string::size() 返回的正是字节数量。
    std::ostringstream request;
    request << "POST " << endpoint.target << " HTTP/1.1\r\n"
        << "Host: " << endpoint.host << ":" << endpoint.port << "\r\n"
        << "Content-Type: application/json\r\n"
        << "Content-Length: " << json_body.size() << "\r\n"
        << "Connection: close\r\n"
        << "\r\n"
        << json_body;

    // 4. TCP 可能分段发送，因此由 send_all 保证所有请求字节都写出。
    send_all(socket_handle.get(), request.str());

    // 5. 因为请求要求服务器关闭连接，所以持续读取到 recv 返回 0。
    const std::string raw_response = receive_all(socket_handle.get());

    // 6. 从原始 HTTP 文本中提取调用方当前关心的状态码和响应体。
    return HttpResponse{
        parse_status_code(raw_response),
        extract_body(raw_response)
    };
}

} // namespace edgelab::simulator