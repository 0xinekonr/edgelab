#ifndef EDGELAB_TEMPERATURE_PROFILE_H
#define EDGELAB_TEMPERATURE_PROFILE_H

#include <cstddef>
#include <vector>

namespace edgelab {
class TemperatureProfile {
public:
    // explicit 防止 std::vector<double> 被意外转换成 TemperatureProfile。
    // 这里用 vector 保存一组温度变化量，例如 0.3、-0.1。
    explicit TemperatureProfile(std::vector<double> deltas);

    // next_delta 会推进内部下标，因此不是 const 成员函数。
    // [[nodiscard]] 表示调用方不应该忽略返回的变化量。
    [[nodiscard]] double next_delta();

private:
    std::vector<double> deltas_;

    // std::size_t 是 C++ 中常用来表示容器大小和下标的无符号整数类型。
    std::size_t next_index_{0};
};

} // namespace edgelab

#endif // EDGELAB_TEMPERATURE_PROFILE_H
