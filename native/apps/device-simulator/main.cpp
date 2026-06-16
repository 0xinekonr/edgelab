#include <command_line_parser.h>
#include <simulation_config.h>
#include <edgelab/random_temperature_profile.h>
#include <edgelab/telemetry_formatter.h>
#include <edgelab/virtual_device.h>

#include <cstddef>
#include <cstdlib>
#include <exception>
#include <iostream>

namespace {

void print_temperature_reading(const edgelab::VirtualDevice& device) {
    // device 以 const 引用传入，表示这个函数只读取设备状态，不修改设备。
    // 这也要求 collect_temperature() 必须是 const 成员函数。
    const edgelab::TelemetryReading reading = device.collect_temperature();

    std::cout << edgelab::format_as_json(reading) << '\n';
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

        print_temperature_reading(device);

        for (std::size_t i = 0; i < config.additional_reading_count; ++i) {
            device.apply_temperature_delta(temperature_profile.next_delta());
            print_temperature_reading(device);
        }

        return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        std::cerr << "device_simulator: " << e.what() << "\n\n";
        std::cerr << edgelab::simulator::usage_text();
        return EXIT_FAILURE;
    }
}