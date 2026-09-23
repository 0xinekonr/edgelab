#include <edgelab/temperature_profile.h>

#include <cstdlib>
#include <iostream>

namespace {

void expect_equal(double actual, double expected) {
    if (actual != expected) {
        std::cerr << "Expected: " << expected << '\n' << "Actual: " << actual << '\n';
        std::exit(1);
    }
}

} // namespace

int main() {
    edgelab::TemperatureProfile profile{{0.3, -0.1}};

    expect_equal(profile.next_delta(), 0.3);
    expect_equal(profile.next_delta(), -0.1);
    expect_equal(profile.next_delta(), 0.3);

    return 0;
}