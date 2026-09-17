# 当前计划：C++ 模拟器上报 Java 后端

最后更新：2026-09-15

教学归属：M01 收尾；完成验收后进入独立 M02 会话。总安排见 `TEACHING_MODULES.md`。

## 目标

让 `device_simulator` 把生成的 telemetry JSON 上报到 Spring Boot 后端的 `POST /api/v1/telemetry` 接口，形成第一条 Java / C++ 协同链路。

目标运行方式示例：

```powershell
.\device_simulator.exe --device-id pump-002 --readings 3 --seed 7 --server-url http://localhost:8080/api/v1/telemetry
```

## 本功能块的协作方式

沿用历史线程边界，并按 2026-06-08 当前线程再次确认：

- Codex 提供设计、代码片段、静态检查、验证建议和文档更新。
- 用户手动编写或修改核心 C++ / CMake 文件。
- Codex 不直接修改核心源码或构建文件；用户每完成一批代码后，Codex 读取 diff 和相关文件做一次静态检查。
- 若未来确实需要 Codex 直接改源码，必须由用户明确说出“本次允许 Codex 修改源码”。
- 继续保持旧线程的教学节奏：一次推进一个完整功能包，包含设计、核心代码、轻量测试、验证和学习点；讲解节奏放慢，但编码任务粒度放大。

## 前置状态

已完成：

```text
feat(native): add simulator command line parsing
docs: add long-term project continuity notes
```

模拟器当前已经可以：

- 生成 telemetry JSON。
- 通过命令行参数控制设备 ID、读数数量、随机种子、初始温度和温度波动范围。
- 对非法命令行参数打印错误和 usage。

## 功能需求

新增能力：

- 为模拟器增加后端上报地址配置，例如 `--server-url <url>`。
- 支持“只打印到控制台”和“打印并上报”两种模式。
- C++ 侧发送 HTTP POST，请求体为当前已有 JSON。
- 后端继续复用现有 `/api/v1/telemetry`。
- 上报失败时给出清晰错误信息，不做复杂重试。

行为要求：

- 默认仍可本地打印，避免必须启动后端才能演示模拟器。
- 指定 server URL 后，对每条 telemetry 执行一次 POST。
- 上报逻辑和设备状态逻辑分离，不能塞进 `VirtualDevice`。
- 先实现最小可理解版本，再考虑抽象 HTTP client。

## 学习重点

- C++ 和 Java 通过进程 / HTTP 协同，而不是一开始就 JNI。
- 为什么 HTTP 是跨语言协作中最常见、最稳妥的第一步。
- C++ 网络编程的选择：标准库缺口、OS API、第三方库。
- 请求失败、超时、状态码和异常边界。
- 如何让模拟器保持可测试、可替换、不过度耦合。

## 实现检查清单

1. `已完成` 确认采用 Windows Winsock 最小 HTTP 教学实现。
2. `已完成` `SimulationConfig` 增加 server URL。
3. `已完成` CLI 支持 `--server-url`。
4. `已完成` 最小 Winsock HTTP client；完整超时和响应解码仍为明确限制。
5. `已完成` main 编排：生成 telemetry、打印、按配置上报；成功区间 [200, 300)。
6. `已完成` 用户于 2026-09-15 报告离线、202、404、连接失败符合预期；Codex 未重复执行。
7. `已完成` 更新教学笔记和项目状态，后续教学直接在对话中提供。
8. `待验证` 用户运行现有 native/backend 提交前回归。
9. `待开始` commit、push、PR 收尾及 M02 独立会话交接。

## 轻量验证

本功能块不做大量测试，优先手动验证端到端链路：

```powershell
# backend 侧确认服务启动
curl http://localhost:8080/actuator/health

# native 侧运行模拟器
.\device_simulator.exe --device-id pump-002 --readings 1 --server-url http://localhost:8080/api/v1/telemetry
```

验收标准：

- 后端返回 `202 Accepted`。
- C++ 侧能展示发送失败时的错误。
- 不因为后端未启动而崩溃到无解释状态。
