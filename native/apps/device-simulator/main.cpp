#include <edgelab/telemetry_formatter.h>
#include <edgelab/virtual_device.h>

#include <iostream>
#include <string>
#include <vector>

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

    // std::vector 是 C++ 标准库动态数组，类似 Java 中常用的 ArrayList。
    // 这里保存每次采集之后要应用的温度变化量。
    const std::vector<double> temperature_deltas{0.3, -0.1};

    print_temperature_reading(device);

    for (double delta : temperature_deltas) {
        device.apply_temperature_delta(delta);
        print_temperature_reading(device);
    }

    return 0;
}
