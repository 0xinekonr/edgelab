#ifndef EDGELAB_RANDOM_TEMPERATURE_PROFILE_H
#define EDGELAB_RANDOM_TEMPERATURE_PROFILE_H

#include <random>

namespace edgelab {
class RandomTemperatureProfile {
public:
    // min_delta 和 max_delta 表示每次温度变化的范围。
    // seed 用于固定随机数序列，让测试可重复。
    RandomTemperatureProfile(double min_delta, double max_delta, unsigned int seed);

    // 每次调用都会推进随机数引擎状态，因此不是 const 成员函数。
    [[nodiscard]] double next_delta();

private:
    std::mt19937 engine_;
    std::uniform_real_distribution<double> distribution_;
};
} // namespace edgelab

#endif // EDGELAB_RANDOM_TEMPERATURE_PROFILE_H