# 交接文档

最后更新：2026-06-08

## 最新摘要

已阅读上一条 Codex 线程，并将关键上下文整理为仓库内长期项目记忆。

EdgeLab 当前处于 Phase 2：C++ 设备模拟器。

上一线程最新编码目标是：

```text
为 device_simulator 增加命令行参数解析。
```

本轮请求完成了长期项目说明文档和进度跟踪结构初始化，并将说明文档统一改为中文。

2026-06-08 当前线程再次确认协作边界：

- 核心代码和构建文件由用户手动编写。
- Codex 在用户每次完成后进行静态检查。
- Codex 保持旧线程教学节奏：一次给完整功能包，讲解关键 C++ / Java 差异，用户完成后再做一次静态检查。

## 本轮涉及文件

创建或更新：

- `AGENTS.md`
- `docs/PROJECT_STATE.md`
- `docs/CURRENT_PLAN.md`
- `docs/PROGRESS_TRACKING.md`
- `docs/ROADMAP.md`
- `docs/ARCHITECTURE.md`
- `docs/DECISIONS.md`
- `docs/HANDOFF.md`
- `.codex/config.toml`

删除：

- `AGENT.md`

删除原因：

- `AGENTS.md` 是更常见的仓库级 agent 指南文件名。
- `AGENT.md` 只是上轮为了兼容用户原话中的单数写法而创建。
- 同时存在两个入口会造成歧义，所以现在只保留 `AGENTS.md`。

读取过：

- `README.md`
- `docs/00-learning-log.md`
- `docs/01-domain-overview.md`
- `docs/superpowers/plans/2026-05-20-edgelab-learning-development-plan.md`
- native CMake、模拟器、配置和测试文件
- backend Maven 配置和部分后端文件

## 执行过的命令

```powershell
git status --short
git branch --show-current
git diff -- native\apps\device-simulator\simulation_config.h
Get-ChildItem -Force
rg --files
rg --files docs .codex AGENT.md AGENTS.md
python C:\Users\axin\.codex\skills\long-project-continuity\scripts\init_project_continuity.py --root D:\playground\edgelab
py C:\Users\axin\.codex\skills\long-project-continuity\scripts\init_project_continuity.py --root D:\playground\edgelab
```

脚本说明：

- `python` 命令失败且无输出。
- `py` 命令未被系统识别。
- 因此连续性文件是手动初始化的。

## 验证状态

本轮主要是文档整理，没有运行 C++ / Java 构建测试。

已做的文档检查：

- 读取并检查 `docs/CURRENT_PLAN.md`。
- 读取并检查 `docs/PROGRESS_TRACKING.md`。
- 通过 `git status --short` 查看当前工作区状态。

当前已知工作区中仍有一个既有源码修改：

```text
M native/apps/device-simulator/simulation_config.h
```

这个修改只是 include guard 注释空格修正，不要回退。

## 剩余工作

立即继续的功能块：

1. 用户一次完成命令行参数解析功能包：`command_line_parser.h`、`command_line_parser.cpp`、`command_line_parser_test.cpp`、`CMakeLists.txt`、`main.cpp`。
2. Codex 读取 diff 和相关文件，做一次静态检查。
3. 静态检查重点：参数缺失、未知参数、数字解析、整数溢出、help 跳过校验、异常边界、CMake target 和 include 路径。
4. 检查通过后再运行聚焦验证，并更新学习笔记。

历史协作边界：

- 除非当前线程明确授权 Codex 直接改源码，否则不要修改核心代码。
- 文档可以由 Codex 直接维护。

## 建议恢复提示

```text
继续 EdgeLab Phase 2。先阅读 AGENTS.md 和 docs/HANDOFF.md，按 docs/CURRENT_PLAN.md 推进 device_simulator 命令行参数解析；完成后更新项目状态和交接文档。
```
