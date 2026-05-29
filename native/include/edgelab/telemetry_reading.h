#ifndef EDGELAB_TELEMETRY_READING_H
#define EDGELAB_TELEMETRY_READING_H

#include <string>

namespace edgelab {

// struct 用来定义一组相关数据，类似 Java 中只保存字段的 DTO。
// C++ 的 struct 成员默认是 public，适合表达“遥测读数”这种简单数据对象。
struct TelemetryReading {
    // std::string 是 C++ 标准库字符串类型；std:: 表示它来自标准库命名空间。
    // 业务含义：设备唯一标识，例如 pump-001。
    std::string device_id;

    // 业务含义：指标编码，例如 temperature、pressure、voltage。
    std::string metric_code;

    // double 是双精度浮点数，用来保存 72.5 这类带小数的采集值。
    double value;

    // 业务含义：单位，例如 celsius、kpa、volt。
    std::string unit;

    // 当前阶段先用字符串保存采集时间；后续再学习 std::chrono 时间类型。
    std::string collected_at;
};

} // namespace edgelab

#endif // EDGELAB_TELEMETRY_READING_H
