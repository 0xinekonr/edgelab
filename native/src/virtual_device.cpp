#include <edgelab/virtual_device.h>

#include <utility>

namespace edgelab {
VirtualDevice::VirtualDevice(std::string device_id, double temperature_value,
                             std::string collected_at)
    : device_id_(std::move(device_id)), temperature_value_(temperature_value),
      collected_at_(std::move(collected_at)) {
    // 成员初始化列表会按照类中成员变量的声明顺序初始化。
    // 因此建议初始化列表顺序与 virtual_device.h 中的成员声明顺序保持一致。
    // std::move 表示 device_id 这个参数后续不再使用，可以把字符串资源转移给成员变量。
}

TelemetryReading VirtualDevice::collect_temperature() const {
    // const 成员函数承诺不会修改当前 VirtualDevice 对象。
    // 业务上，这里表示“采集一次温度”，返回一条新的遥测读数。
    return TelemetryReading{device_id_, "temperature", temperature_value_, "celsius",
                            collected_at_};
}

} // namespace edgelab