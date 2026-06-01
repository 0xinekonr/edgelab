#include <edgelab/telemetry_formatter.h>
#include <edgelab/random_temperature_profile.h>
#include <edgelab/virtual_device.h>

#include <iostream>
#include <string>

namespace {

// 这个函数只在当前 main.cpp 文件内部使用，所以放进匿名命名空间。
// 匿名命名空间可以避免函数名暴露到整个程序，减少链接时的名字冲突风险。
void print_temperature_reading(const edgelab::VirtualDevice& device) {
    // device 以 const 引用传入，表示这个函数只读取设备状态，不修改设备。
    // 这也要求 collect_temperature() 必须是 const 成员函数。
    const edgelab::TelemetryReading reading = device.collect_temperature();

    std::cout << edgelab::format_as_json(reading) << '\n';
}

} // namespace

int main() {
    // 这里不能使用 const，因为后面要调用 apply_temperature_delta 修改设备温度状态。
    edgelab::VirtualDevice device{"pump-001", 72.5, "2026-05-20T10:30:00Z"};

    // RandomTemperatureProfile 负责按预设序列产生温度变化量。
    // 这样 VirtualDevice 不需要关心变化策略，只负责维护设备状态。
    edgelab::RandomTemperatureProfile temperature_profile{-0.5, 0.5, 42};

    print_temperature_reading(device);

    for (int i = 0; i < 5; ++i) {
        device.apply_temperature_delta(temperature_profile.next_delta());
        print_temperature_reading(device);
    }

    return 0;
}