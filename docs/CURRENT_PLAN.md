# 当前计划：设备模拟器命令行参数解析

最后更新：2026-06-08

## 目标

把 `device_simulator` 从固定默认参数的模拟器，升级为可以通过命令行配置的工具。

目标命令示例：

```powershell
.\device_simulator.exe --device-id pump-002 --readings 10 --seed 7 --initial-temperature 68.5 --min-delta -0.2 --max-delta 0.8
```

## 本功能块的协作方式

沿用历史线程边界，并按 2026-06-08 当前线程再次确认：

- Codex 提供设计、代码片段、静态检查、验证建议和文档更新。
- 用户手动编写或修改核心 C++ / CMake 文件。
- Codex 不直接修改核心源码或构建文件；用户每完成一批代码后，Codex 读取 diff 和相关文件做一次静态检查。
- 若未来确实需要 Codex 直接改源码，必须由用户明确说出“本次允许 Codex 修改源码”。
- 继续保持旧线程的教学节奏：一次推进一个完整功能包，包含设计、核心代码、轻量测试、验证和学习点；讲解节奏放慢，但编码任务粒度放大。

## 预期文件

新增源码文件：

```text
native/apps/device-simulator/command_line_parser.h
native/apps/device-simulator/command_line_parser.cpp
native/tests/command_line_parser_test.cpp
```

更新已有文件：

```text
native/CMakeLists.txt
native/apps/device-simulator/main.cpp
docs/00-learning-log.md
docs/PROJECT_STATE.md
docs/HANDOFF.md
```

## 功能需求

支持参数：

- `--device-id <id>`
- `--readings <count>`
- `--seed <number>`
- `--initial-temperature <number>`
- `--min-delta <number>`
- `--max-delta <number>`
- `--collected-at <timestamp>`
- `-h`, `--help`

行为要求：

- 从 `default_simulation_config()` 开始构造默认配置。
- 只覆盖用户显式传入的配置项。
- 除非请求 help，否则解析完成后调用 `validate_config()`。
- 拒绝未知参数。
- 拒绝缺失参数值。
- 拒绝非法数字。
- 拒绝 `std::size_t` 和 `unsigned int` 的整数溢出。
- help 和错误场景都能打印统一 usage。

## 建议 API

命名空间：

```cpp
namespace edgelab::simulator
```

结果对象：

```cpp
struct CommandLineParseResult {
    SimulationConfig config;
    bool help_requested;
};
```

函数：

```cpp
[[nodiscard]] CommandLineParseResult parse_command_line(int argc, char* argv[]);
[[nodiscard]] std::string usage_text();
```

## 实现检查清单

1. `进行中` 用户手写 `command_line_parser.h` 的 API 声明。
2. `进行中` 用户手写 `command_line_parser.cpp`。
3. `进行中` 从 `argc` / `argv` 解析常用参数。
4. `进行中` 增加读取必填参数值的辅助函数。
5. `进行中` 增加数字解析，并验证整个字符串都被消费。
6. `进行中` 为 count 和 seed 增加溢出检查。
7. `进行中` 非 help 场景调用 `validate_config()`。
8. `进行中` 增加 `usage_text()`。
9. `进行中` 更新 `main.cpp`，入口改成 `int argc, char* argv[]`。
10. `进行中` 更新 `main.cpp`，处理 help 和异常。
11. `进行中` 更新 `native/CMakeLists.txt`，加入 parser 源文件和测试 target。
12. `进行中` 增加 `command_line_parser_test.cpp`。
13. `待开始` 运行聚焦验证。
14. `待开始` 更新学习笔记和进度文档。

## 聚焦测试用例

parser 最小测试：

- 能解析 `--device-id`、`--readings`、`--seed`。
- `--help` 可以不触发配置校验。
- 未知参数会失败。
- 缺失参数值会失败。
- 非法整数会失败。
- 非法浮点数会失败。

模拟器最小手动检查：

```powershell
.\device_simulator.exe --help
.\device_simulator.exe --device-id pump-002 --readings 3 --seed 7
```

预期：

- help 打印 usage 并正常退出。
- 示例运行打印初始读数 + 3 条额外读数。

## 需要写入学习笔记的 C++ 点

- `argc` / `argv` 和 Java `String[] args` 的对比。
- `char* argv[]` 与 C 风格字符串边界。
- 引用参数 `int& index`。
- `std::stod`、`std::stoull`、`std::stoul`。
- 通过 parsed character count 验证完整字符串解析。
- `std::numeric_limits`。
- `static_cast`。
- 程序启动阶段的异常处理。
- 为什么命令行解析属于 `apps/device-simulator`，不属于 `edgelab_core`。
