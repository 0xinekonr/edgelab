#include <simulation_config.h>

#include <edgelab/telemetry_formatter.h>
#include <edgelab/random_temperature_profile.h>
#include <edgelab/virtual_device.h>

#include <cstddef>
#include <iostream>
#include <string>


namespace {

void print_temperature_reading(const edgelab::VirtualDevice& device) {
    // device 以 const 引用传入，表示这个函数只读取设备状态，不修改设备。
    // 这也要求 collect_temperature() 必须是 const 成员函数。
    const edgelab::TelemetryReading reading = device.collect_temperature();

    std::cout << edgelab::format_as_json(reading) << '\n';
}

} // namespace

int main() {
    const edgelab::simulator::SimulationConfig config =
        edgelab::simulator::default_simulation_config();

    edgelab::simulator::validate_config(config);

    edgelab::VirtualDevice device{
        config.device_id,
        config.initial_temperature,
        config.collected_at
    };

    // RandomTemperatureProfile 负责产生随机温度变化量。
    // 这样 VirtualDevice 不需要关心变化策略，只负责维护设备状态。
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

    return 0;
}