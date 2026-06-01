#include <edgelab/random_temperature_profile.h>

#include <stdexcept>

namespace edgelab {
RandomTemperatureProfile::RandomTemperatureProfile(double min_delta, double max_delta,
                                                   unsigned int seed)
    : engine_(seed), distribution_(min_delta, max_delta) {
    if (min_delta > max_delta) {
        throw std::invalid_argument("min_delta must not be greater than max_delta");
    }
}

double RandomTemperatureProfile::next_delta() {
    return distribution_(engine_);
}
} // namespace edgelab