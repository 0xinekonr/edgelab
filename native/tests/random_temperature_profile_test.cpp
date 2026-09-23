#include <edgelab/random_temperature_profile.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>

namespace {

void expect_between(double actual, double min_value, double max_value) {
    if (actual < min_value || actual > max_value) {
        std::cerr << "Expected value between " << min_value << " and " << max_value
                  << ", actual: " << actual << '\n';
        std::exit(1);
    }
}

} // namespace

int main() {
    edgelab::RandomTemperatureProfile profile{-0.5, 0.5, 42};

    for (int i = 0; i < 10; ++i) {
        expect_between(profile.next_delta(), -0.5, 0.5);
    }

    bool rejected = false;
    try {
        edgelab::RandomTemperatureProfile invalid{1.0, -1.0, 42};
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    if (!rejected) {
        std::cerr << "Expected invalid range to be rejected\n";
        return EXIT_FAILURE;
    }

    return 0;
}