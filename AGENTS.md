# EdgeLab Agent 指南

这是本仓库中 Codex 会话的长期工作约定。后续在调整计划、文档或代码前，先阅读本文档。

## 文件命名

本仓库只保留 `AGENTS.md` 作为唯一权威入口。

说明：上一次初始化时同时创建了 `AGENT.md` 和 `AGENTS.md`，其中 `AGENT.md` 只是因为用户原话写了单数形式而创建的兼容入口。为了避免两个入口造成歧义，现统一删除 `AGENT.md`，只保留更常见、也更适合 Codex 项目记忆的 `AGENTS.md`。

## 项目定位

EdgeLab 是一个 Java + C++ 综合练手项目，业务背景是工业 IoT 边缘网关与云端监控平台。

项目刻意同时使用两套技术栈：

- Java / Spring Boot 负责云端业务 API、参数校验、数据存储、告警、服务治理，以及后续分布式系统能力。
- C++ 负责设备侧和边缘侧模拟、协议解析、缓冲队列、高性能 native 模块，以及后续 Java 与 native 的协作。

## 当前分支和阶段

- 当前分支：`feature/phase-2-cpp-device-simulator`
- 当前阶段：Phase 2，C++ 设备模拟器
- 当前功能块：HTTP 上报已接通，用户已报告四场景联调通过，待提交前回归与 Git 收尾。
- 当前教学模块：M01，基础工程与首次跨语言闭环。模块边界见 `docs/TEACHING_MODULES.md`。

## 教学会话边界

- 当前会话完成 M01；之后 M02–M10 各在一个独立的 EdgeLab 项目会话中顺序推进。
- 模块开始前读取 `docs/TEACHING_MODULES.md`，模块内部保持完整功能包节奏，不按细小语法点拆会话。
- 模块完成后先验证、更新状态与交接、提醒 commit / push / PR，再创建或进入下一个模块会话；不提前批量创建后续会话。
- 新会话继承本仓库的手写代码、中文教学注释和轻量验证约定，并先确认分支及工作区。

## 协作边界

沿用历史线程中的规则：

- 核心源码和项目构建文件通常由用户手动编写或修改。
- Codex 可以直接维护说明文档、学习笔记、项目状态、计划和交接文档。
- Codex 只有在当前线程中得到用户明确授权时，才直接修改核心代码或构建文件。

本次项目连续性初始化中，Codex 已被授权创建和更新项目记忆文档。

## 每次会话开始前

实现前按顺序读取：

1. `AGENTS.md`
2. `docs/PROJECT_STATE.md`
3. `docs/HANDOFF.md`
4. `docs/CURRENT_PLAN.md`
5. `docs/PROGRESS_TRACKING.md`
6. `docs/ROADMAP.md`
7. `docs/ARCHITECTURE.md`
8. `docs/DECISIONS.md`
9. 相关源码、测试和本地配置

尽早运行：

```powershell
git status --short
```

保留用户已有改动，不回退无关变更。

## 开发节奏

- 教学步骤、参考代码和操作说明直接在对话中提供，不让用户跳到文档完成教学；文档仅作为笔记和进度留存。
- 以推进项目为主，不再围绕极小代码点频繁停顿。
- 一个完整功能块或学习单元对应一次提交。
- 测试保持轻量，但关键行为必须覆盖。
- 功能块结束时运行聚焦验证。
- push 或 PR 前运行更完整的验证。

## C++ 规范

- C++ 标准：C++17。
- 构建系统：CMake。
- 日常 IDE：可以使用 CLion。
- Windows 编译器/工具链：MSVC / Visual Studio Toolchain。
- C++ 源码格式：`native/.clang-format` 是 `.h` 和 `.cpp` 的唯一格式依据。
- CMake 格式：保持当前文件局部风格一致，clang-format 不负责 CMake。
- 头文件保护：统一使用 include guard，不使用 `#pragma once`。
- 命名：
  - 文件：`snake_case`
  - 命名空间：小写，例如 `edgelab`
  - 类型 / 类：`PascalCase`
  - 函数和变量：`snake_case`
  - 私有成员变量：尾随下划线，例如 `device_id_`
- 不写 `using namespace std;`。
- 优先使用标准库，不急于手写底层内存管理。
- 本项目是面向 Java 开发者学习 C++ 的长期练习项目。遇到 C++ 独有或容易误解的语法、工程机制和资源管理写法时，应在代码中写教学型注释，而不是只在聊天中解释。典型场景包括：
  - RAII、构造函数、析构函数。
  - 禁止拷贝、移动构造、移动赋值、`noexcept`。
  - 指针、引用、C API 资源封装、句柄生命周期。
  - `std::unique_ptr` 自定义 deleter、`std::exchange`、`std::optional` 等现代 C++ 工具。
  - C++ 编译 / 链接边界，例如 include 头文件与 link 系统库的区别。
  - 整数类型边界、显式类型转换、`std::numeric_limits`。
- 教学型注释必须同时覆盖三个问题：
  - “做了什么”：说明当前类、方法或关键代码块在整体流程中的职责。
  - “为什么这样做”：说明设计原因、资源安全、协议要求或工程权衡。
  - “Java 开发者需要注意什么”：解释与 Java 不同或 Java 中不存在的语法、生命周期和构建机制。
- 对网络、协议、并发、内存、C API 适配等底层模块，必须提供：
  - 类 / 结构体注释：整体职责、拥有的资源、生命周期和不可复制 / 可移动等约束。
  - 方法注释：输入、输出、失败方式，以及该方法在整体调用链中的作用。
  - 关键步骤注释：例如初始化平台 API、域名解析、建立连接、循环发送、循环接收、协议解析和资源释放。
- 对 Java 开发者可能陌生的 C++ 语法不得一带而过，例如 `constexpr`、`explicit`、`operator=`、`this` / `*this`、`= delete`、移动语义、`noexcept`、裸指针与所有权、模板参数和显式类型转换。
- 简单控制流、普通变量赋值、显而易见的 getter/setter 可以简写，但不能因此省略底层流程和 C++ 特有语义的说明。
- CLion / clang-tidy 提示按风险分级处理：
  - 优先采纳 `[[nodiscard]]`、生命周期、越界、未初始化、空指针、悬空引用等安全或语义提示。
  - 风格提示按项目规范判断。
  - 暂不追逐过早的 `noexcept`、复杂 move/copy 控制、以及按值基础类型参数的顶层 `const`。

## 设计约束

- `VirtualDevice::collect_temperature()` 是读取操作，不能修改设备状态。
- 设备状态变化必须显式表达，目前通过 `apply_temperature_delta()` 完成。
- 温度生成策略不放进 `VirtualDevice`：
  - `TemperatureProfile`：确定性序列，适合测试和可控模拟。
  - `RandomTemperatureProfile`：随机波动，使用固定 seed 保持可复现。
- `native/include/edgelab` 和 `native/src` 放可复用核心库代码。
- `native/apps/device-simulator` 放应用专属启动、配置和命令行解析代码。

## 验证方式

当前 native 侧轻量检查：

- `simulation_config_test`
- `random_temperature_profile_test`
- `temperature_profile_test`
- `virtual_device_test`
- `telemetry_formatter_test`
- `device_simulator`

当前 backend 侧检查：

- 在 `backend/` 下运行 `mvn test`

小步推进时只跑聚焦检查；功能块结束、准备 push 或 PR 前再跑更完整检查。

## 文档

长期项目状态保存在：

- `docs/PROJECT_STATE.md`
- `docs/CURRENT_PLAN.md`
- `docs/PROGRESS_TRACKING.md`
- `docs/ROADMAP.md`
- `docs/ARCHITECTURE.md`
- `docs/DECISIONS.md`
- `docs/HANDOFF.md`
- `docs/00-learning-log.md`

以后说明文档统一使用中文。若更新了项目方向、当前进度或剩余工作，结束前必须更新 `PROJECT_STATE.md` 和 `HANDOFF.md`；重要设计决策写入 `DECISIONS.md`。
