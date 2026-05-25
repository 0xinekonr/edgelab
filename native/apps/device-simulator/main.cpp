#include <edgelab/telemetry_formatter.h>

#include <iostream>

int main()
{
    // TelemetryReading 是我们定义的遥测数据结构，类似 Java 中的请求 DTO。
    // 这里用花括号初始化每个字段，顺序对应 telemetry_reading.h 中 struct 的字段声明顺序。
    const edgelab::TelemetryReading reading{
        "pump-001",
        "temperature",
        72.5,
        "celsius",
        "2026-05-20T10:30:00Z"
    };

    // format_as_json 接收 const TelemetryReading&
    // 这表示函数只读取 reading，不复制对象，也不会修改对象。
    const std::string payload = edgelab::format_as_json(reading);

    // std::cout 是标准输出，类似 Java 的 System.out。
    // '\n' 表示换行。这里优先用 '\n'，而不是 std::endl，因为 std::endl 还会强制刷新缓冲区。
    std::cout << payload << '\n';

    return 0;
}
