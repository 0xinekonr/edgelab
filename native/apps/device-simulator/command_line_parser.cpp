#include "command_line_parser.h"

#include <limits>
#include <stdexcept>
#include <cstddef>

namespace {

std::string require_value(int argc, char* argv[], int& index, const std::string& option) {
    if (index + 1 >= argc) {
        throw std::invalid_argument(option + " requires a value");
    }

    ++index;
    return argv[index];
}

double parse_double_value(const std::string& option, const std::string& value) {
    std::size_t parsed_char_count = 0;
    const double parsed_value = std::stod(value, &parsed_char_count);

    if (parsed_char_count != value.size()) {
        throw std::invalid_argument(option + " must be a valid number");
    }

    return parsed_value;
}

std::size_t parse_size_value(const std::string& option, const std::string& value) {
    if (!value.empty() && value.front() == '-') {
        throw std::invalid_argument(option + " must be a non-negative integer");
    }

    std::size_t parsed_char_count = 0;
    const unsigned long long parsed_value = std::stoull(value, &parsed_char_count);

    if (parsed_char_count != value.size()) {
        throw std::invalid_argument(option + " must be a non-negative integer");
    }

    if (parsed_value > std::numeric_limits<std::size_t>::max()) {
        throw std::invalid_argument(option + " is too large");
    }

    return static_cast<std::size_t>(parsed_value);
}

unsigned int parse_unsigned_int_value(const std::string& option, const std::string& value) {
    if (!value.empty() && value.front() == '-') {
        throw std::invalid_argument(option + " must be a non-negative integer");
    }

    std::size_t parsed_char_count = 0;
    const unsigned long long parsed_value = std::stoull(value, &parsed_char_count);

    if (parsed_char_count != value.size()) {
        throw std::invalid_argument(option + " must be a non-negative integer");
    }

    if (parsed_value > std::numeric_limits<unsigned int>::max()) {
        throw std::invalid_argument(option + " is too large");
    }

    return static_cast<unsigned int>(parsed_value);
}

} // namespace

namespace edgelab::simulator {

CommandLineParseResult parse_command_line(int argc, char* argv[]) {
    SimulationConfig config = default_simulation_config();
    bool help_requested = false;

    for (int i = 1; i < argc; ++i) {
        const std::string option = argv[i];

        if (option == "--help" || option == "-h") {
            help_requested = true;
        } else if (option == "--device-id") {
            config.device_id = require_value(argc, argv, i, option);
        } else if (option == "--readings") {
            config.additional_reading_count =
                parse_size_value(option, require_value(argc, argv, i, option));
        } else if (option == "--seed") {
            config.random_seed =
                parse_unsigned_int_value(option, require_value(argc, argv, i, option));
        } else if (option == "--initial-temperature") {
            config.initial_temperature =
                parse_double_value(option, require_value(argc, argv, i, option));
        } else if (option == "--min-delta") {
            config.min_temperature_delta =
                parse_double_value(option, require_value(argc, argv, i, option));
        } else if (option == "--max-delta") {
            config.max_temperature_delta =
                parse_double_value(option, require_value(argc, argv, i, option));
        } else if (option == "--collected-at") {
            config.collected_at = require_value(argc, argv, i, option);
        } else if (option == "--server-url") {
            config.server_url = require_value(argc, argv, i, option);
        } else {
            throw std::invalid_argument("Unknown option: " + option);
        }
    }

    if (!help_requested) {
        validate_config(config);
    }

    return CommandLineParseResult{config, help_requested};
}

std::string usage_text() {
    return "Usage: device_simulator [options]\n"
        "\n"
        "Options:\n"
        "  --device-id <id>                 Device identifier, e.g. pump-001\n"
        "  --readings <count>               Number of generated readings after the first one\n"
        "  --seed <number>                  Random seed for repeatable simulation\n"
        "  --initial-temperature <number>   Initial temperature value\n"
        "  --min-delta <number>             Minimum random temperature delta\n"
        "  --max-delta <number>             Maximum random temperature delta\n"
        "  --collected-at <timestamp>       Timestamp used in generated telemetry\n"
        "  --server-url <url>               HTTP endpoint used to post generated telemetry\n"
        "  -h, --help                       Show this help message\n";
}

} // namespace edgelab::simulator