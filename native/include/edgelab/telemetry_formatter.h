#ifndef EDGELAB_TELEMETRY_FORMATTER_H
#define EDGELAB_TELEMETRY_FORMATTER_H

#include <edgelab/telemetry_reading.h>

#include <string>

namespace edgelab {

// 函数声明：告诉编译器“有一个名为 format_as_json 的函数”，具体实现放在 .cpp 文件中。
//
// std::string:
//   返回值类型，表示这个函数会返回一个 C++ 标准库字符串。
//
// const TelemetryReading& reading:
//   const 表示函数不会修改 reading；
//   & 表示按引用传参，避免复制整个 TelemetryReading 对象；
//   这相当于“只借用这条遥测数据来读取”，是 C++ 中常见的高质量代码习惯。
//
// 业务含义：
//   把 C++ 设备模拟器内部的遥测读数，转换成 Java 后端 /api/v1/telemetry 能接收的 JSON。
std::string format_as_json(const TelemetryReading& reading);

} // namespace edgelab

#endif // EDGELAB_TELEMETRY_FORMATTER_H
