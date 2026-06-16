#include <command_line_parser.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect_equal(const std::string& actual, const std::string& expected) {
    if (actual != expected) {
        std::cerr << "Expected: " << expected << ", actual: " << actual << '\n';
        std::exit(1);
    }
}

void expect_true(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

void expect_parses_common_options() {
    char app[] = "device_simulator";
    char device_id_option[] = "--device-id";
    char device_id[] = "pump-999";
    char readings_option[] = "--readings";
    char readings[] = "3";
    char seed_option[] = "--seed";
    char seed[] = "77";

    char* argv[] = {
        app,
        device_id_option,
        device_id,
        readings_option,
        readings,
        seed_option,
        seed
    };

    const edgelab::simulator::CommandLineParseResult result =
        edgelab::simulator::parse_command_line(7, argv);

    expect_equal(result.config.device_id, "pump-999");
    expect_true(result.config.additional_reading_count == 3, "Unexpected readings count");
    expect_true(result.config.random_seed == 77, "Unexpected random seed");
    expect_true(!result.help_requested, "Help should not be requested");
}

void expect_rejects_unknown_option() {
    char app[] = "device_simulator";
    char unknown_option[] = "--unknown";

    char* argv[] = {app, unknown_option};

    try {
        edgelab::simulator::parse_command_line(2, argv);
    } catch (const std::invalid_argument&) {
        return;
    }

    std::cerr << "Expected std::invalid_argument for unknown option\n";
    std::exit(1);
}

void expect_accepts_help() {
    char app[] = "device_simulator";
    char help_option[] = "--help";

    char* argv[] = {app, help_option};

    const edgelab::simulator::CommandLineParseResult result =
        edgelab::simulator::parse_command_line(2, argv);

    expect_true(result.help_requested, "Help should be requested");
}

} // namespace

int main() {
    expect_parses_common_options();
    expect_rejects_unknown_option();
    expect_accepts_help();

    return 0;
}