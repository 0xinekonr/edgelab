# 项目状态

最后更新：2026-09-15

## 最新状态校准

- 2026-09-15：用户已接通 main，并报告离线打印、HTTP 202、错误路径 404、关闭后端连接失败四个场景均符合预期。
- Codex 静态检查确认 optional 端点、两个上报调用点和异常退出路径已接通；成功区间已修正为 [200, 300)。本轮未代替用户运行构建或联调。
- 教学模块：M01，基础工程与首次跨语言闭环，覆盖 Phase 0–3，功能闭环已验证，待提交前回归与 Git 收尾。
- 模块安排见 `TEACHING_MODULES.md`；M01 在当前会话收尾，M02–M10 各使用独立项目会话。
- 2026-09-11 静态读取：HTTP client、server URL 配置和 CLI 选项已有实现；`main.cpp` 尚未调用 HTTP client。
- 工作区存在用户修改和新增 HTTP 文件，并非干净状态。旧日期下的状态和验证记录只表示当时结果。
- HTTP 学习路线已选择 Windows Winsock 最小实现，不再待选。当前不支持完整生产 HTTP 能力。
- 本轮只核对代码入口与更新规划文档，没有重新构建、运行测试或 HTTP 联调。
- 下一步：用户运行现有 native/backend 回归，无需新增测试；通过后提交、push 并处理 PR，再开启独立 M02 会话。

## 当前目标

继续推进 EdgeLab Phase 2，把 C++ 设备模拟器从本地可运行工具升级为可以和 Java 后端协同的数据上报组件。

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
- 使用命令行参数覆盖模拟器配置，支持本地脚本化运行。

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
- 已完成 `device_simulator` 命令行参数解析功能。
- 已提交：
  - `feat(native): add simulator command line parsing`
  - `docs: add long-term project continuity notes`

## 正在推进

下一步进入 Java / C++ 协同链路：让 C++ 模拟器把生成的 telemetry JSON 上报到 Spring Boot 后端。

当前协作模式：

- 用户手动编写核心 C++ / CMake 文件。
- Codex 提供中文方案、参考代码、静态检查、验证建议和文档更新。
- 用户每完成一批代码后，Codex 读取 diff 和相关文件做一次静态检查。

命令行解析已支持：

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

2026-06-16 检查：`git status --short` 输出为空，工作区干净。

## 待确认问题

- 后续网关阶段何时替换为成熟 HTTP 库，届时按可靠性需求决定；当前采用 Winsock 教学实现。
- 为了保持“手写 C++ 学习”目标，下一步应避免一开始就完全依赖高级封装库。

## 已知风险

- Java / C++ 协同会引入进程调用、HTTP 通信、错误重试、超时处理等边界，需要保持小范围验证。
- 当前 C++ 测试是最小可执行程序，不如 GoogleTest 表达力强，但足够适合当前学习阶段。
- 旧线程已明确希望减少测试仪式感，因此测试应集中在关键逻辑和跨语言边界。
- 当前 shell 中 `python` 启动失败、`py` 不可用，因此连续性初始化脚本没有跑通，文档是手动创建的。

## 验证记录

- 2026-06-08：运行 `git status --short`，发现一个已有 header 修改。
- 2026-06-08：读取 native 和 backend 当前文件，用于对齐状态。
- 2026-06-08：尝试用 `python` 运行连续性初始化脚本，失败且无输出。
- 2026-06-08：尝试用 `py` 运行连续性初始化脚本，系统未识别该命令。
- 2026-06-08：将项目说明文档统一改为中文，删除重复 `AGENT.md`。
- 2026-06-16：检查 `git status --short`，工作区干净。
- 2026-06-16：确认最近提交包含命令行解析和长期项目文档。
- 2026-06-16：曾在当前 shell 尝试运行 `ctest -R command_line_parser_test --output-on-failure`，但 `ctest` 不在 PATH 中。
- 2026-06-16：运行 `device_simulator.exe --readings -1`，程序正确打印非负整数错误和 usage。

## 下一步

1. 设计 C++ 模拟器向 Java 后端上报 telemetry 的最小链路。
2. 保持学习目标：优先让用户手写关键 C++ 代码，而不是单纯调用黑盒库。
3. 轻量验证：后端启动、模拟器运行、后端收到请求。
4. 完成后再决定是否引入更成熟的 HTTP 客户端库或封装。
