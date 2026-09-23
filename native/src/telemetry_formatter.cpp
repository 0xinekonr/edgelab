#include <edgelab/telemetry_formatter.h>

#include <sstream>
#include <cmath>
#include <stdexcept>

namespace {
/** 转义 JSON 字符串内容，不包含外围双引号；输入按 UTF-8 字节保留。 */
std::string escape_json(const std::string& value) {
    constexpr char hex[] = "0123456789abcdef";
    std::string result;
    // 使用 unsigned char，避免高位字节被当成负数。
    for (unsigned char ch : value) {
        if (ch == '"' || ch == '\\') {
            result += '\\';
            result += static_cast<char>(ch);
        } else if (ch < 0x20) {
            // JSON 禁止直接嵌入控制字符，将它编码为 \u00XX。
            result += "\\u00";
            result += hex[ch >> 4]; // 高四位，对应第一个十六进制数字。
            result += hex[ch & 0xf]; // 低四位，对应第二个十六进制数字。
        } else {
            result += static_cast<char>(ch);
        }
    }
    return result;
}
}

namespace edgelab {

std::string format_as_json(const TelemetryReading& reading) {
    if (!std::isfinite(reading.value)) {
        throw std::invalid_argument("Telemetry value must be finite");
    }
    // std::ostringstream 是“字符串输出流”，可以像写控制台输出一样拼接字符串。
    // 这里先手写 JSON，是为了学习 C++ 字符串拼接和流式输出。
    // 真实生产项目通常会使用成熟 JSON 库，后续再引入更合适。
    std::ostringstream json;

    // << 是流插入运算符，把右侧内容写入左侧的输出流。
    // reading.device_id 使用点号访问 struct 字段；指针访问字段时才会使用 ->。
    json << "{"
        << "\"deviceId\":\"" << escape_json(reading.device_id) << "\","
        << "\"metricCode\":\"" << escape_json(reading.metric_code) << "\","
        << "\"value\":" << reading.value << ","
        << "\"unit\":\"" << escape_json(reading.unit) << "\","
        << "\"collectedAt\":\"" << escape_json(reading.collected_at) << "\""
        << "}";

    // str() 会把 ostringstream 中累计写入的内容取出来，形成 std::string。
    return json.str();
}

} // namespace edgelab
