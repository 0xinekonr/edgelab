#include <command_line_parser.h>
#include <simulation_config.h>
#include <edgelab/random_temperature_profile.h>
#include <edgelab/telemetry_formatter.h>
#include <edgelab/virtual_device.h>
#include <http_client.h>

#include <cstddef>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>

namespace {

/**
 * 采集并打印一条遥测数据；配置了端点时，再同步上报。
 * 两个参数都是只读引用：借用已有对象，不复制，不接管所有权。
 * 网络失败或 HTTP 非成功响应通过异常交给 main 处理。
 */
void publish_temperature_reading(
    const edgelab::VirtualDevice& device,
    const std::optional<edgelab::simulator::HttpEndpoint>& endpoint) {
    const edgelab::TelemetryReading reading = device.collect_temperature();
    const std::string json = edgelab::format_as_json(reading);

    // JSON 写标准输出，诊断信息写标准错误，便于以后单独重定向数据。
    std::cout << json << '\n';

    // optional 为空，表示没有配置上报地址，只打印即可。
    if (!endpoint.has_value()) {
        return;
    }

    // * 调用 optional 的 operator*，取得内部端点的引用，不复制端点。
    // 必须先确认有值；对空 optional 使用 * 不会自动安全地抛出异常。
    const edgelab::simulator::HttpResponse response =
        edgelab::simulator::post_json(*endpoint, json);

    // constexpr 表示可用于编译期计算的常量；这里命名 HTTP 成功区间。
    constexpr int success_status_begin = 200;
    constexpr int success_status_end = 300;
    if (response.status_code < success_status_begin || response.status_code >= success_status_end) {
        throw std::runtime_error(
            "Telemetry POST failed, HTTP status: " + std::to_string(response.status_code));
    }

    std::cerr << "Telemetry POST succeeded, HTTP status: "
              << response.status_code << '\n';
}

} // namespace

int main(int argc, char* argv[]) {
    try { 
        const edgelab::simulator::CommandLineParseResult parse_result =
            edgelab::simulator::parse_command_line(argc, argv);

        if (parse_result.help_requested) {
            std::cout << edgelab::simulator::usage_text();
            return EXIT_SUCCESS;
        }

        const edgelab::simulator::SimulationConfig& config = parse_result.config;

        std::optional<edgelab::simulator::HttpEndpoint> endpoint;
        if (!config.server_url.empty()) {
            // 接收解析结果，在 optional 内构造端点。
            // 复用的是地址信息；目前 post_url 每次仍会新建 TCP 连接。
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
        // 按引用捕获，避免复制异常时丢失派生类型信息。
        // 网络错误无需附带整段命令行用法；用户仍可使用 --help。
        std::cerr << "device_simulator: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
}