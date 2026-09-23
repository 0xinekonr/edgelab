#ifndef EDGELAB_DEVICE_SIMULATOR_HTTP_CLIENT_H
#define EDGELAB_DEVICE_SIMULATOR_HTTP_CLIENT_H

#include <cstdint>
#include <string>

/**
 * 表示从 HTTP URL 中解析出的连接信息。
 *
 * host 用于域名解析和建立 TCP 连接，例如 localhost。
 * port 是 TCP 端口，例如 8080。
 * target 是发送给 HTTP 服务器的请求目标，例如 /api/v1/telemetry。
 *
 * 这里使用 struct，是因为它只是公开的数据集合，没有需要保护的不变量。
 * Java 通常会使用 record 或简单 DTO 表达相同概念。
 */
namespace edgelab::simulator {

struct HttpEndpoint {
    std::string host;
    std::uint16_t port;
    std::string target;
};

/**
 * 表示当前最小 HTTP 客户端关心的响应信息。
 *
 * 第一版只保留状态码和响应体，不解析全部响应头。
 */
struct HttpResponse {
    int status_code;
    std::string body;
};

/**
 * 将 http://host[:port]/target 格式的 URL 拆分为连接信息。
 *
 * @param url 仅支持普通 HTTP URL，不支持 HTTPS 和 IPv6 字面量。
 * @return 可用于建立连接和发送请求的 HttpEndpoint。
 * @throws std::invalid_argument URL 格式或端口不合法时抛出。
 *
 * [[nodiscard]] 提醒调用者不要忽略返回值。它不是 Java 注解，而是 C++ 属性；
 * 编译器或静态检查器可以据此警告“调用了解析函数却丢弃了解析结果”。
 */
[[nodiscard]] HttpEndpoint parse_http_url(const std::string& url);

/**
 * 使用短连接向指定端点发送 JSON 格式的 HTTP POST 请求。
 *
 * 整体流程：初始化 Winsock -> 建立 TCP 连接 -> 组装 HTTP 文本 ->
 * 完整发送 -> 接收完整响应 -> 解析状态码和响应体。
 *
 * @throws std::runtime_error 网络初始化、解析、连接、发送、接收或响应解析失败时抛出。
 */
[[nodiscard]] HttpResponse post_json(const HttpEndpoint& endpoint, const std::string& json_body);

} // namespace edgelab::simulator

#endif // EDGELAB_DEVICE_SIMULATOR_HTTP_CLIENT_H