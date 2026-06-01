#include <edgelab/random_temperature_profile.h>

#include <cstdlib>
#include <iostream>

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

    return 0;
}