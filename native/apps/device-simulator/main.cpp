#include <edgelab/telemetry_formatter.h>
#include <edgelab/virtual_device.h>

#include <iostream>
#include <string>

int main()
{
    // VirtualDevice 表示一台虚拟工业设备。
    // 这里的 pump-001 可以理解为一台水泵的设备编号。
    const edgelab::VirtualDevice device{ "pump-001", 72.5 };

    // 设备采集一次温度，返回一条 TelemetryReading。
    const edgelab::TelemetryReading reading = device.collect_temperature();

    // 把内部 C++ 数据结构转换成 Java 后端 API 能理解的 JSON。
    // format_as_json 接收 const TelemetryReading&
    // 这表示函数只读取 reading，不复制对象，也不会修改对象。
    const std::string payload = edgelab::format_as_json(reading);

    // std::cout 是标准输出，类似 Java 的 System.out。
    // '\n' 表示换行。这里优先用 '\n'，而不是 std::endl，因为 std::endl 还会强制刷新缓冲区。
    std::cout << payload << '\n';

    return 0;
}
