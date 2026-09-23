#include "simulation_config.h"

#include <stdexcept>
#include <cmath>

namespace edgelab::simulator {

SimulationConfig default_simulation_config() {
    return SimulationConfig{
        "pump-001",
        72.5,
        "2026-05-20T10:30:00Z",
        "",
        -0.5,
        0.5,
        42,
        5
    };
}

void validate_config(const SimulationConfig& config) {
    // stod 接受 nan/inf，但遥测温度与随机区间必须是有限数。
    // NaN 的大小比较通常返回 false，单靠 min > max 无法发现它。
    if (!std::isfinite(config.initial_temperature) ||
        !std::isfinite(config.min_temperature_delta) ||
        !std::isfinite(config.max_temperature_delta)) {
        throw std::invalid_argument("Temperature values must be finite");
    }
    if (config.device_id.empty()) {
        throw std::invalid_argument("device_id must not be empty");
    }

    if (config.min_temperature_delta > config.max_temperature_delta) {
        throw std::invalid_argument(
            "min_temperature_delta must not be greater than max_temperature_delta"
            );
    }
}

} // namespace edgelab::simulator