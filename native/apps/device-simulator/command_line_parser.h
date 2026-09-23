#ifndef EDGELAB_DEVICE_SIMULATOR_COMMAND_LINE_PARSER_H
#define EDGELAB_DEVICE_SIMULATOR_COMMAND_LINE_PARSER_H

#include "simulation_config.h"

#include <string>

namespace edgelab::simulator {

// C++ 里常用 struct 表达“只有数据、几乎没有行为”的结果对象。
// 这里类似 Java 里的简单 DTO/record， 但 C++ 默认是值语义：对象可以直接返回、复制、移动。
struct CommandLineParseResult {
    SimulationConfig config;
    bool help_requested;
};

// argc/argv 是 C/C++ 程序接收命令行参数的传统入口格式。
// argc 表示参数数量， argv 是参数字符串数组。argv[0] 通常是程序名。
[[nodiscard]] CommandLineParseResult parse_command_line(int argc, char* argv[]);

// 单独提供 usage_text()，让 --help 和异常错误输出复用同一份帮助文本。
[[nodiscard]] std::string usage_text();

} // namespace edgelab::simulator

#endif // EDGELAB_DEVICE_SIMULATOR_COMMAND_LINE_PARSER_H
