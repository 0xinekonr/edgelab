#include <edgelab/telemetry_formatter.h>
#include <edgelab/virtual_device.h>

#include <iostream>
#include <string>
#include <vector>

int main() {
    // 这里不能使用 const，因为后面要调用 apply_temperature_delta 修改设备温度状态。
    edgelab::VirtualDevice device{"pump-001", 72.5, "2026-05-20T10:30:00Z"};

    // std::vector 是 C++ 标准库动态数组，类似 Java 中常用的 ArrayList。
    // 这里保存每次采集之后要应用的温度变化量。
    const std::vector<double> temperature_deltas{0.3, -0.1};

    std::cout << edgelab::format_as_json(device.collect_temperature()) << '\n';

    for (double delta : temperature_deltas) {
        device.apply_temperature_delta(delta);

        std::cout << edgelab::format_as_json(device.collect_temperature()) << '\n';
    }

    return 0;
}
