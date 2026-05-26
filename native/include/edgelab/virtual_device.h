#pragma once

#include <edgelab/telemetry_reading.h>

#include <string>

namespace edgelab {
class VirtualDevice {
public:
    // explicit 防止构造函数参与不必要的隐式转换。
    // 这里构造一台虚拟设备时，需要设备 ID 和当前温度值。
    explicit VirtualDevice(std::string device_id, double temperature_value,
                           std::string collected_at);

    // 业务含义：模拟设备采集一次温度。
    // const 放在成员函数后面，表示这个函数不会修改当前设备对象的内部状态。
    [[nodiscard]] TelemetryReading collect_temperature() const;

    // 修改设备内部温度，用于模拟环境温度变化。
    // 和 collect_temperature() 不同，这个函数明确会改变对象状态。
    void apply_temperature_delta(double delta);

private:
    // C++ 常见命名习惯：私有成员变量使用尾随下划线。
    // 这样可以和构造函数参数 device_id 区分开。
    std::string device_id_;

    // 当前阶段用固定温度值模拟设备采集结果。
    // 后续会把它扩展成可波动的数据生成逻辑。
    double temperature_value_;

    // 当前阶段由外部传入采集时间，保证测试稳定。
    // 后续再学习如何使用 std::chrono 生成真实当前时间。
    std::string collected_at_;
};

} // namespace edgelab