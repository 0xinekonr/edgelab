# 进度跟踪方案

最后更新：2026-06-16

## 目的

本文档定义 EdgeLab 在长周期开发中的进度跟踪方式。它偏操作性：后续会话应根据实际进展持续更新这些文件。

## 信息优先级

当信息冲突时，按以下顺序判断：

1. 当前线程中的用户最新指令。
2. `AGENTS.md`
3. `docs/PROJECT_STATE.md`
4. `docs/CURRENT_PLAN.md`
5. `docs/HANDOFF.md`
6. `docs/ROADMAP.md`
7. `docs/ARCHITECTURE.md`
8. `docs/DECISIONS.md`
9. 更早的聊天历史

## 状态词

统一使用这些状态：

- `待开始`：已计划，尚未开始
- `进行中`：正在推进
- `阻塞`：被外部条件或用户输入阻塞
- `待验证`：实现或指导已完成，等待验证
- `已完成`：已验证并完成文档更新

## 每次会话检查清单

开始时：

- 阅读 `AGENTS.md`。
- 阅读 `PROJECT_STATE.md`、`CURRENT_PLAN.md`、`HANDOFF.md`。
- 运行 `git status --short`。
- 识别用户已有改动并保留。
- 简短重述当前目标。

过程中：

- 当前状态变化时更新 `PROJECT_STATE.md`。
- 当前功能块检查项变化时更新 `CURRENT_PLAN.md`。
- 重要设计决策写入 `DECISIONS.md`。
- `00-learning-log.md` 只记录学习解释，不记录流水账状态。
- 测试力度与风险匹配。

结束时：

- 在 `HANDOFF.md` 记录执行过的命令和验证结果。
- 在 `PROJECT_STATE.md` 更新下一步。
- 如果阶段或功能块完成，在 `ROADMAP.md` 标记。
- 留下清晰的恢复提示。

## 当前进度看板

| 区域 | 状态 | 负责人 | 备注 |
| --- | --- | --- | --- |
| 项目记忆文档 | 已完成 | Codex | 2026-06-08 初始化，已改为中文 |
| Phase 2 模拟器配置 | 已完成 | 用户 / Codex 指导 | 当前已有 `SimulationConfig` |
| header guard 注释空格修正 | 进行中 | 用户 | 现有 `simulation_config.h` 修改 |
| 命令行参数解析 | 已完成 | 用户手写，Codex 静态检查 | 已提交 |
| Java / C++ HTTP 上报 | 待开始 | 用户手写，Codex 指导 | 下一功能块 |

## 验证矩阵

native 侧聚焦检查：

| 变更类型 | 最小验证 |
| --- | --- |
| formatter 变更 | `telemetry_formatter_test` |
| 设备状态变更 | `virtual_device_test` |
| 温度 profile 变更 | `temperature_profile_test` 或 `random_temperature_profile_test` |
| 模拟器配置变更 | `simulation_config_test` |
| CLI parser 变更 | `command_line_parser_test`、`device_simulator --help`、一次示例模拟器运行 |
| CMake 变更 | 构建受影响 target，并运行对应聚焦测试 |

backend 侧聚焦检查：

| 变更类型 | 最小验证 |
| --- | --- |
| Controller / DTO / validation | `mvn test` 或聚焦后端测试 |
| 应用配置 | 后端启动或相关测试 |
| 后续持久化 / 缓存 | 聚焦集成测试 |

## 提交和 push 节奏

提交：

- 一个完整功能块或学习单元对应一次提交。
- 如果文档解释的是同一功能块，和代码一起提交。
- 不为几行琐碎代码频繁提交，除非是隔离格式化或清理噪音。

push：

- 几个有意义的提交后，或一个阶段块完成后 push。
- PR 前或需要远程备份时 push。

## 文档所有权

Codex 可以直接更新：

- `AGENTS.md`
- `docs/*.md`
- `.codex/config.toml`

核心源码和项目构建文件由用户手动维护。Codex 只提供方案、代码草稿、静态检查、验证建议和文档更新。

若未来需要 Codex 直接修改核心源码或构建文件，用户必须在当前线程明确说出“本次允许 Codex 修改源码”。
