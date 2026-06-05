#include <simulation_config.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
void expect_equal(const std::string& actual, const std::string& expected) {
    if (actual != expected) {
        std::cerr << "Expected: " << expected << '\n' << "Actual: " << actual << '\n';
        std::exit(1);
    }
}

void expect_true(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

void expect_invalid_argument_for_empty_device_id() {
    edgelab::simulator::SimulationConfig config = edgelab::simulator::default_simulation_config();
    config.device_id = "";

    try {
        edgelab::simulator::validate_config(config);
    } catch (const std::invalid_argument&) {
        return;
    }

    std::cerr << "Expected std::invalid_argument for empty device_id\n";
    std::exit(1);
}

void expect_invalid_argument_for_invalid_delta_range() {
    edgelab::simulator::SimulationConfig config =
        edgelab::simulator::default_simulation_config();
    config.min_temperature_delta = 1.0;
    config.max_temperature_delta = -1.0;

    try {
        edgelab::simulator::validate_config(config);
    } catch (const std::invalid_argument&) {
        return;
    }

    std::cerr << "Expected std::invalid_argument for invalid delta range\n";
    std::exit(1);
}

} // namespace

int main() {
    const edgelab::simulator::SimulationConfig config =
        edgelab::simulator::default_simulation_config();

    expect_equal(config.device_id, "pump-001");
    expect_true(config.additional_reading_count == 5, "Unexpected additional reading count");

    edgelab::simulator::validate_config(config);

    expect_invalid_argument_for_empty_device_id();
    expect_invalid_argument_for_invalid_delta_range();

    return 0;
}