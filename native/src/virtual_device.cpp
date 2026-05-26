#include <edgelab/virtual_device.h>

#include <utility>

namespace edgelab
{
    VirtualDevice::VirtualDevice(std::string device_id)
        : device_id_(std::move(device_id))
    {
        // 成员初始化列表在构造对象时直接初始化 device_id_。
        // std::move 表示 device_id 这个参数后续不再使用，可以把字符串资源转移给成员变量。
    }

    TelemetryReading VirtualDevice::collect_temperature() const
    {
        // const 成员函数承诺不会修改当前 VirtualDevice 对象。
        // 业务上，这里表示“采集一次温度”，返回一条新的遥测读数。
        return TelemetryReading{
            device_id_,
            "temperature",
            72.5,
            "celsius",
            "2026-05-20T10:30:00Z"
        };
    }

} // namespace edgelab