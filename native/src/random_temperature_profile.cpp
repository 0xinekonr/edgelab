#include <edgelab/random_temperature_profile.h>

#include <stdexcept>
#include <cmath>

namespace edgelab {
RandomTemperatureProfile::RandomTemperatureProfile(double min_delta, double max_delta,
                                                   unsigned int seed)
    : engine_(seed) {
    // 成员先于函数体构造，所以先使用标准库合法的默认分布。
    // 此类可以被独立调用，不能依赖 CLI 已做过检查。
    if (!std::isfinite(min_delta) || !std::isfinite(max_delta) || min_delta > max_delta
        || !std::isfinite(max_delta - min_delta)) {
        throw std::invalid_argument("Invalid temperature delta range");
    }
    // 校验通过后才构造目标分布，并赋给已经存在的成员对象。
    distribution_ = std::uniform_real_distribution<double>{min_delta, max_delta};
}

double RandomTemperatureProfile::next_delta() {
    return distribution_(engine_);
}
} // namespace edgelab