#include "simulation_config.h"

#include <stdexcept>

namespace edgelab::simulator {

SimulationConfig default_simulation_config() {
    return SimulationConfig{
        "pump-001",
        72.5,
        "2026-05-20T10:30:00Z",
        -0.5,
        0.5,
        42,
        5
    };
}

void validate_config(const SimulationConfig& config) {
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