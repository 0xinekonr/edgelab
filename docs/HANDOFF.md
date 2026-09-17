# 交接文档

最后更新：2026-09-15

## 当前交接入口

- 2026-09-15：用户报告离线、202、404、连接失败四个场景全部符合预期。Codex 静态确认 main 接入且成功区间为 [200, 300)，未重新执行联调。
- 当前会话继续完成 M01；尚未到创建 M02 会话的时间点。整体安排见 `TEACHING_MODULES.md`。
- 下一步：用户运行现有 native 六项测试及 backend Maven 测试，集中完成一次提交前回归；通过后 commit/push 和 PR 收尾。当前仍有未提交及部分暂存修改，不创建 M02 会话。
- main 中注释误写 post_url，实际函数为 post_json，可在提交前顺手修正；不影响运行。
- 已知教学实现限制：同步短连接、无完整超时、接收至关闭连接、未实现 chunked 响应解码；不作为生产 HTTP 客户端。
- 用户要求教学全部在对话中提供，文档仅记录笔记和状态。
- 本轮执行了文档读取、`git status --short`、`git branch --show-current` 和源码搜索；未运行构建与测试。分支仍是 `feature/phase-2-cpp-device-simulator`，有未提交及部分暂存的改动。
- 本轮新增教学模块文档，并同步 AGENTS、状态、计划、路线图、进度跟踪和决策。没有修改核心源码或构建文件。
- M01 验收完成后记录实际验证、commit / push / PR 状态，再按教学模块文档的提示启动独立 M02 会话。
- 下方旧摘要和命令记录保留为历史信息，当前进度以上述入口为准。

## 最新摘要

EdgeLab 当前处于 Phase 2：C++ 设备模拟器。

最近完成：

- `device_simulator` 命令行参数解析。
- 长期项目连续性文档。
- 学习笔记补充命令行解析、`argc/argv`、`std::stod/stoul/stoull`、`numeric_limits`、`static_cast`、include what you use。

用户明确希望后续：

- 加快功能推进节奏。
- 测试轻量化。
- 从 Java 开发者视角点明 C++ 独特知识点，不讲太多通用语法。
- C++ 核心代码继续由用户手写，Codex 做方案、检查和文档维护。

## 本轮涉及文件

最近提交：

```text
333f923 docs: add long-term project continuity notes
04c2597 feat(native): add simulator command line parsing
de20b4a feat(native): add simulator runtime configuration
```

本次文档更新涉及：

- `docs/00-learning-log.md`
- `docs/PROJECT_STATE.md`
- `docs/CURRENT_PLAN.md`
- `docs/PROGRESS_TRACKING.md`
- `docs/HANDOFF.md`

## 执行过的命令

```powershell
git status --short
git log --oneline -3
Get-Content docs\00-learning-log.md
Get-Content docs\PROJECT_STATE.md
Get-Content docs\HANDOFF.md
ctest -R command_line_parser_test --output-on-failure
.\device_simulator.exe --readings -1
```

说明：

- `ctest` 在当前 PowerShell 环境中不在 PATH 中。
- `device_simulator.exe --readings -1` 成功打印非负整数错误和 usage。

## 验证状态

当前功能块已由用户完成并提交。

已确认：

- `git status --short` 为空。
- 最近提交包含 native CLI parser 和长期文档。
- 负数 `--readings -1` 场景输出符合预期。

## 剩余工作

下一功能块：

让 C++ 模拟器向 Java 后端 `POST /api/v1/telemetry` 上报 telemetry JSON。

建议策略：

1. 先讲清 C++ 网络实现路线选择。
2. 为用户提供一版小而清晰的手写实现方案。
3. 不引入大量测试，优先端到端手动验证。
4. 验证后再考虑是否引入成熟 HTTP 库或抽象接口。

## 建议恢复提示

```text
继续 EdgeLab Phase 2。命令行参数解析已提交。请按 docs/CURRENT_PLAN.md 进入 C++ device_simulator 向 Java 后端 POST telemetry 的功能块，保持测试轻量化，并从 Java 开发者视角讲解 C++ 网络、错误处理和工程边界。
```
