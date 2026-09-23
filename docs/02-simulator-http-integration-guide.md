# M01：在模拟器中接通 HTTP 上报

最后更新：2026-09-14

状态：参考代码已提供，等待用户手写、静态检查和本地联调。本文代码尚未在项目中构建运行。

## 恢复现场

Java 已有 `POST /api/v1/telemetry`；C++ 已有设备模拟、JSON 格式化、CLI、`server_url` 和 `http_client`。目前 `main.cpp` 只打印 JSON，尚未调用 HTTP client。

本轮业务：模拟泵设备采集温度，把同一条读数打印到控制台，并按配置发送给云端。遥测（telemetry）是远端设备上报的测量数据。HTTP 202 表示后端接受请求，不等于数据已经持久化；当前还没进入 MySQL 持久化模块。

调用链：解析配置 → 可选解析上报地址 → 创建设备 → 采集并生成 JSON → 打印 → 可选 POST → 检查响应。每次温度变化仍由 main 显式编排，设备采集保持只读。

## 1. 在 CLion 中定位文件

打开 `native/apps/device-simulator/main.cpp`。本轮不新增源码文件，不需要 Add to targets，也不改 CMake。

先补齐下面参考代码中的 include，将原来的 `print_temperature_reading` 换成双参数 `publish_temperature_reading` 声明，同时把两个调用点改为传入 `endpoint`。随后填写函数体。这样可以先让 IDE 识别接口再补实现，不需要先写无法编译的测试。完成函数体后统一构建。

## 2. 完整参考代码

核心文件由你手写；按以下版本调整 main.cpp，保留教学注释，并使用项目 clang-format 格式化。

```cpp
#include <command_line_parser.h>
#include <http_client.h>
#include <simulation_config.h>
#include <edgelab/random_temperature_profile.h>
#include <edgelab/telemetry_formatter.h>
#include <edgelab/virtual_device.h>

#include <cstddef>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>

// 未命名命名空间让辅助函数只在当前翻译单元中可见。
// 翻译单元可理解为一个 cpp 文件经过预处理后参与编译的内容。
namespace {

/**
 * 采集一条温度数据，输出 JSON，并在配置端点时同步上报。
 * device 和 endpoint 均为借用的只读引用，不复制对象，也不接管所有权。
 * endpoint 为空时只打印；HTTP 非 2xx 时抛出 runtime_error。
 * 网络异常继续向 main 传播，由进程入口统一报告并返回失败退出码。
 */
void publish_temperature_reading(
    const edgelab::VirtualDevice& device,
    const std::optional<edgelab::simulator::HttpEndpoint>& endpoint) {
    const edgelab::TelemetryReading reading = device.collect_temperature();
    const std::string json = edgelab::format_as_json(reading);

    // 标准输出只放 JSON，方便以后重定向给文件或其他进程。
    std::cout << json << '\n';

    // optional 有“空”和“包含一个对象”两种状态；空表示未启用上报。
    if (!endpoint.has_value()) {
        return;
    }

    // *endpoint 调用 optional 的 operator*，取得其中对象的引用。
    // 这里没有裸指针，也没有复制 HttpEndpoint；上面已确认对象存在。
    // post_json 的 const 引用参数会在本次同步调用期间借用这个对象。
    const edgelab::simulator::HttpResponse response =
        edgelab::simulator::post_json(*endpoint, json);

    // 2xx 是 HTTP 成功响应类别；本项目接口当前约定返回 202。
    // 传输层负责读响应，应用层决定是否把这个状态视为业务调用成功。
    constexpr int success_status_begin = 200;
    constexpr int success_status_end = 300;
    if (response.status_code < success_status_begin ||
        response.status_code >= success_status_end) {
        throw std::runtime_error(
            "Telemetry POST failed, HTTP status: " +
            std::to_string(response.status_code));
    }

    // 诊断信息写标准错误流，避免混入标准输出中的 JSON 数据。
    std::cerr << "Telemetry POST accepted, HTTP status: "
              << response.status_code << '\n';
}

} // namespace

/**
 * 编排配置、模拟设备和可选上报；正常返回 EXIT_SUCCESS。
 * 任一步骤抛出标准异常时报告原因并返回 EXIT_FAILURE，停止后续采集。
 * 当前没有自动重试：请求失败时服务器可能已经接收数据，贸然重试会重复。
 */
int main(int argc, char* argv[]) {
    try {
        const edgelab::simulator::CommandLineParseResult parse_result =
            edgelab::simulator::parse_command_line(argc, argv);

        if (parse_result.help_requested) {
            std::cout << edgelab::simulator::usage_text();
            return EXIT_SUCCESS;
        }

        const edgelab::simulator::SimulationConfig& config = parse_result.config;

        // 默认构造的 optional 为空，不会构造一个无意义的端点对象。
        // optional 自身包含端点的存储空间；其中的 string 仍可能分配堆内存。
        std::optional<edgelab::simulator::HttpEndpoint> endpoint;
        if (!config.server_url.empty()) {
            // 空 optional 接收解析结果后变为“有值”；这里接收的是临时值，
            // 使用 HttpEndpoint 的隐式移动构造来构造内部对象。
            // 只解析一次 URL，复用的是地址信息；post_json 仍每次新建连接。
            endpoint = edgelab::simulator::parse_http_url(config.server_url);
        }

        edgelab::VirtualDevice device{
            config.device_id,
            config.initial_temperature,
            config.collected_at
        };

        edgelab::RandomTemperatureProfile temperature_profile{
            config.min_temperature_delta,
            config.max_temperature_delta,
            config.random_seed
        };

        publish_temperature_reading(device, endpoint);

        for (std::size_t i = 0; i < config.additional_reading_count; ++i) {
            device.apply_temperature_delta(temperature_profile.next_delta());
            publish_temperature_reading(device, endpoint);
        }

        return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        // 按 const 引用捕获，保留异常实际类型，避免按值复制造成对象切片。
        // what() 返回异常说明的 C 字符串，在异常对象有效期间读取。
        // 网络错误不适合附带整段 CLI 用法；--help 仍可单独查询用法。
        std::cerr << "device_simulator: " << e.what() << '\n';
        return EXIT_FAILURE;
    }
}
```

## 3. 本轮重点：optional 与对象生命周期

`std::optional<HttpEndpoint>` 是模板实例：用 `HttpEndpoint` 作为类型参数，得到“可能包含端点”的具体类型。Java 的泛型 Optional 保存对象引用；C++ optional 管理内部对象本身的构造与析构。

`std::optional<HttpEndpoint> endpoint;` 默认是空值，不是一个 null 指针。`endpoint.has_value()` 判断内部对象是否已经存在。离开作用域时，若其中有端点，就自动析构该端点及其 string 成员，无需显式 delete 或 GC。

`const std::optional<HttpEndpoint>&` 要从外向内读：传入 optional 对象的引用，并禁止通过此引用修改它。main 中的 endpoint 在同步辅助函数返回前始终存在，因此这里不会悬空。以后改成异步任务时，必须重新审查借用引用的生命周期，不能直接照搬。

`*endpoint` 中的星号是 optional 提供的运算符重载，不意味着 endpoint 是指针。表达式取得内部 HttpEndpoint 的引用。必须先确认有值；C++17 对空 optional 使用 `operator*` 不会替你安全地抛出异常。需要受检查访问时可用 `.value()`，空时抛出 `std::bad_optional_access`。这里已有分支检查，所以使用 `*endpoint`。

`constexpr int` 将整数声明为可用于编译期计算的常量；此处用具名常量表达成功区间边界。它不同于仅表示不可重新赋值的 Java final；C++ constexpr 还要求初始化满足常量表达式规则。

`endpoint = parse_http_url(...)` 会在空 optional 内构造返回的端点。临时返回值可移动，编译器为这个结构体生成的移动构造会逐个移动成员。这里不需要手写移动函数，也不需要额外加 `std::move`。

## 4. 一次完成联调

在 CLion 顶部选择 `device_simulator` 运行配置，构建该目标。通过 Run → Edit Configurations 打开配置，在 Program arguments 中填写参数。若界面文字不同，找运行配置中的程序参数字段。

按顺序运行以下场景，每次替换参数后运行同一个目标：

| 场景 | Program arguments | 预期 |
| --- | --- | --- |
| 离线打印 | `--readings 1` | 输出 2 条 JSON，退出码 0，无 HTTP 状态 |
| 成功上报 | `--readings 1 --server-url http://127.0.0.1:8080/api/v1/telemetry` | 先在 IDEA 启动 backend；输出 2 条 JSON 和两次 HTTP 202，退出码 0 |
| 非成功状态 | `--readings 0 --server-url http://127.0.0.1:8080/api/v1/not-found` | 后端运行时预期 HTTP 404，报告失败，非零退出码 |
| 连接失败 | `--readings 0 --server-url http://127.0.0.1:8080/api/v1/telemetry` | 在 IDEA 停止 backend 后运行，报告连接错误，非零退出码 |

当前 `--readings` 是额外读数数量：总数为初始一条加额外数量。`--readings 0` 仍采集一条。本轮不改这个已有约定。

CLion 控制台可能交错展示 stdout 和 stderr；JSON 与状态文本相邻顺序不作为验收条件。先打印 JSON 也不代表已上报成功，成功以 HTTP 状态为准。

当前 HTTP 客户端没有完整超时与响应体解码机制，依赖服务器关闭连接结束读取。此次用本机 Spring Boot 做受控验证；若程序持续等待，在 CLion 停止进程并记录卡住的场景，后续检查接收与超时边界。这里不解析 response.body，因为现有 extract_body 尚不处理 chunked 编码。

写完后告诉 Codex“main 已接通”及四个场景结果。Codex 先静态检查，再决定模块收尾需要哪些验证。本轮不新增测试文件，不单独为这几处调用提交；M01 验收后再统一提醒 Git 收尾。
