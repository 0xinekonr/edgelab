#include <edgelab/telemetry_formatter.h>

#include <cstdlib>
#include <iostream>
#include <string>

namespace {
// C++ 没有像 JUnit 那样内置断言框架
// 这里先手写一个最小断言函数，后面再引入更正式的测试框架
void expect_equal(const std::string& actual, const std::string& expected) {
    if (actual != expected) {
        std::cerr << "Expected: " << expected << '\n' << "Actual:	" << actual << '\n';
        std::exit(1);
    }
}

} // namespace

int main() {
    const edgelab::TelemetryReading reading{"pump-001", "temperature", 72.5, "celsius",
                                            "2026-05-20T10:30:00Z"};

    const std::string actual = edgelab::format_as_json(reading);

    const std::string expected =
        R"({"deviceId":"pump-001","metricCode":"temperature","value":72.5,"unit":"celsius","collectedAt":"2026-05-20T10:30:00Z"})";

    expect_equal(actual, expected);

    auto escaped = reading; // auto 推导类型；这里复制结构体，不是 Java 引用赋值。
    escaped.device_id = "pump\"\\\n";
    expect_equal(edgelab::format_as_json(escaped),
        R"({"deviceId":"pump\"\\\u000a","metricCode":"temperature","value":72.5,"unit":"celsius","collectedAt":"2026-05-20T10:30:00Z"})");
}