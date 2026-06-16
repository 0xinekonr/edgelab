# 项目状态

最后更新：2026-06-08

## 当前目标

从上一条 Codex 线程的进度继续，把 C++ 设备模拟器从固定默认参数升级为可通过命令行配置的工具，同时把项目记忆沉淀到仓库文档中。

## 当前阶段

Phase 2：C++ 设备模拟器。

当前模拟器已经具备：

- 使用 `TelemetryReading` 表达遥测数据。
- 将遥测数据格式化为 Java 后端可理解的 JSON。
- 使用 `VirtualDevice` 表达虚拟设备和设备状态。
- 使用 `apply_temperature_delta()` 显式修改温度状态，采集动作本身不修改状态。
- 使用 `TemperatureProfile` 生成确定性温度变化序列。
- 使用 `RandomTemperatureProfile` 生成可复现的随机温度变化。
- 使用 `SimulationConfig` 收拢 `device_simulator` 的运行配置。

## 已完成工作

- Phase 0：项目骨架、Git 工作流和基础目录结构。
- Phase 1：Spring Boot 后端最小服务。
- 后端健康检查和遥测接收 API。
- 后端参数校验和基础统一错误响应。
- Phase 2 native CMake 工程。
- C++ 遥测格式化器和最小测试。
- `VirtualDevice`、`TemperatureProfile`、`RandomTemperatureProfile`、`SimulationConfig`。
- C++ 代码风格统一为 clang-format。
- 头文件统一使用 include guard。
- 根目录 `.gitignore` 已统一管理。
- 学习笔记已记录到随机温度 profile 和模拟器配置。
- 项目连续性文档已初始化，并统一为中文说明。
- 已删除重复的 `AGENT.md`，只保留 `AGENTS.md` 作为权威入口。

## 正在推进

为 `native/apps/device-simulator` 增加命令行参数解析。

当前协作模式：

- 用户手动编写核心 C++ / CMake 文件。
- Codex 提供中文方案、参考代码、静态检查、验证建议和文档更新。
- 用户每完成一批代码后，Codex 读取 diff 和相关文件做一次静态检查。

计划支持：

- `--device-id <id>`
- `--readings <count>`
- `--seed <number>`
- `--initial-temperature <number>`
- `--min-delta <number>`
- `--max-delta <number>`
- `--collected-at <timestamp>`
- `-h` / `--help`

预期行为：

- 默认值来自 `default_simulation_config()`。
- 命令行参数只覆盖用户显式传入的配置。
- `--help` 打印用法并正常退出。
- 未知参数、缺失参数值、非法数值等情况应尽早失败，并打印清晰错误和用法。

## 当前工作区说明

`git status --short` 当前有一个既有源码修改：

```text
M native/apps/device-simulator/simulation_config.h
```

该 diff 只是修正 include guard 末尾注释空格：

```cpp
#endif // EDGELAB_DEVICE_SIMULATOR_SIMULATION_CONFIG_H
```

这属于已有进度，不要回退。

## 待确认问题

- native 侧后续验证优先使用哪种方式：CLion target、Visual Studio CTest，还是命令行 CMake。
- 命令行参数解析完成后，下一块是直接进入 C++ HTTP 上报 Java 后端，还是先做一轮模拟器命令行体验整理。

## 已知风险

- 命令行解析容易在缺失值、非法数字、整数溢出、未知参数上出错，需要轻量但有效的测试覆盖。
- 当前 C++ 测试是最小可执行程序，不如 GoogleTest 表达力强，但足够适合当前学习阶段。
- 旧线程已明确希望减少测试仪式感，因此测试应集中在关键逻辑和跨语言边界。
- 当前 shell 中 `python` 启动失败、`py` 不可用，因此连续性初始化脚本没有跑通，文档是手动创建的。

## 验证记录

- 2026-06-08：运行 `git status --short`，发现一个已有 header 修改。
- 2026-06-08：读取 native 和 backend 当前文件，用于对齐状态。
- 2026-06-08：尝试用 `python` 运行连续性初始化脚本，失败且无输出。
- 2026-06-08：尝试用 `py` 运行连续性初始化脚本，系统未识别该命令。
- 2026-06-08：将项目说明文档统一改为中文，删除重复 `AGENT.md`。

## 下一步

1. 用户先手写 `native/apps/device-simulator/command_line_parser.h`。
2. Codex 读取该文件和相关 diff，做一次静态检查。
3. 静态检查通过后，再进入 `command_line_parser.cpp` 的第一版实现。
4. 后续按 `docs/CURRENT_PLAN.md` 的检查清单逐项推进。
5. 功能块完成后运行 parser、config、simulator 的聚焦验证。
6. 更新学习笔记、项目状态和交接文档。
