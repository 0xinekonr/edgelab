#include <edgelab/telemetry_formatter.h>

#include <sstream>

namespace edgelab
{

std::string format_as_json(const TelemetryReading& reading)
{
    // std::ostringstream 是“字符串输出流”，可以像写控制台输出一样拼接字符串。
    // 这里先手写 JSON，是为了学习 C++ 字符串拼接和流式输出。
    // 真实生产项目通常会使用成熟 JSON 库，后续再引入更合适。
    std::ostringstream json;

    // << 是流插入运算符，把右侧内容写入左侧的输出流。
    // reading.device_id 使用点号访问 struct 字段；指针访问字段时才会使用 ->。
    json << "{"
         << "\"deviceId\":\"" << reading.device_id << "\","
         << "\"metricCode\":\"" << reading.metric_code << "\","
         << "\"value\":" << reading.value << ","
         << "\"unit\":\"" << reading.unit << "\","
         << "\"collectedAt\":\"" << reading.collected_at << "\""
         << "}";

    // str() 会把 ostringstream 中累计写入的内容取出来，形成 std::string。
    return json.str();
}

} // namespace edgelab
