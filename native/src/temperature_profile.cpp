#include <edgelab/temperature_profile.h>

#include <stdexcept>
#include <utility>

namespace edgelab {
TemperatureProfile::TemperatureProfile(std::vector<double> deltas)
    : deltas_(std::move(deltas)) {
    if (deltas_.empty()) {
        throw std::invalid_argument("temperature profile must contain at least one delta");
    }
}

double TemperatureProfile::next_delta() {
    const double delta = deltas_[next_index_];

    next_index_ = (next_index_ + 1) % deltas_.size();

    return delta;
}
} // namespace edgelab