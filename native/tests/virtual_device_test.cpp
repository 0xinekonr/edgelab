#include <edgelab/virtual_device.h>

#include <cstdlib>
#include <iostream>
#include <string>

namespace {
void expect_equal(const std::string& actual, const std::string& expected) {
    if (actual != expected) {
        std::cerr << "Expected: " << expected << '\n' << "Actual: " << actual << '\n';
        std::exit(1);
    }
}

void expect_equal(double actual, double expected) {
    if (actual != expected) {
        std::cerr << "Expected: " << expected << '\n' << "Actual: " << actual << '\n';
        std::exit(1);
    }
}

} // namespace

int main() {
    edgelab::VirtualDevice device{"pump-001", 68.25, "2026-05-26T08:15:00Z"};

    const edgelab::TelemetryReading reading = device.collect_temperature();

    expect_equal(reading.device_id, "pump-001");
    expect_equal(reading.metric_code, "temperature");
    expect_equal(reading.value, 68.25);
    expect_equal(reading.unit, "celsius");
    expect_equal(reading.collected_at, "2026-05-26T08:15:00Z");

    return 0;
}