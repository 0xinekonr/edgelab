#ifndef EDGELAB_DEVICE_SIMULATOR_SIMULATION_CONFIG_H
#define EDGELAB_DEVICE_SIMULATOR_SIMULATION_CONFIG_H

#include <cstddef>
#include <string>

namespace edgelab::simulator {

// SimulationConfig 是 device_simulator 的启动配置。
// 它只是数据聚合，没有复杂行为，所以用 struct。
struct SimulationConfig {

    std::string device_id;
    double initial_temperature;
    std::string collected_at;
    double min_temperature_delta;
    double max_temperature_delta;
    unsigned int random_seed;
    std::size_t additional_reading_count;
};

// 返回本地模拟器的默认配置。
// 后续可以把这些值改成命令行参数，配置文件或环境变量。
[[nodiscard]] SimulationConfig default_simulation_config();

// 校验配置是否合法。配置错误应在程序启动时尽早失败。
void validate_config(const SimulationConfig& config);

} // namespace edgelab::simulator

#endif // EDGELAB_DEVICE_SIMULATOR_SIMULATION_CONFIG_H
