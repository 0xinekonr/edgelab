#pragma once

#include <edgelab/telemetry_reading.h>

#include <string>

namespace edgelab
{
    class VirtualDevice
    {
    public:
        // explicit 防止字符串被意外隐式转换成 VirtualDevice。
        // 这属于 C++ 中很重要的类型安全习惯：单参数构造函数默认可能参与隐式转换。
        explicit VirtualDevice(std::string device_id);

        // 业务含义：模拟设备采集一次温度。
        // const 放在成员函数后面，表示这个函数不会修改当前设备对象的内部状态。
        TelemetryReading collect_temperature() const;

    private:
        // C++ 常见命名习惯：私有成员变量使用尾随下划线。
        // 这样可以和构造函数参数 device_id 区分开。
        std::string device_id_;

    };

} // namespace edgelab