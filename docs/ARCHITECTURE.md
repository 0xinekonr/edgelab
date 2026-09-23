# 架构说明

最后更新：2026-06-08

## 系统形态

EdgeLab 模拟一条工业 IoT 数据链路：

```text
设备 / 模拟器 -> 边缘侧 C++ 模块 -> Java 云端后端 -> 存储、缓存、告警、统计、后续分布式服务
```

早期阶段刻意保持小而可观察：

- C++ 先把 JSON 打印到控制台。
- Java 先通过简单 REST 接口接收遥测数据。
- 基础设施在核心数据链路清晰后再引入。

## 仓库结构

```text
backend/
  Spring Boot 后端服务

native/
  C++ 设备模拟器、边缘网关、协议和 native 模块

docs/
  学习笔记、项目记忆、架构、路线图、决策和交接文档

deploy/
  后续 Docker Compose 和部署相关内容
```

## Java 后端

当前职责：

- 云端 API 服务。
- 通过 `POST /api/v1/telemetry` 接收遥测数据。
- 校验请求 payload。
- 返回结构化 API 响应和错误响应。

后续增长方向：

- 设备管理。
- MySQL 持久化。
- Redis 状态缓存。
- Kafka 或其他消息队列。
- 告警规则。
- Spring Cloud 服务治理。
- 实时统计。

## Native C++ 模块

当前职责：

- 模拟一台工业设备。
- 生成温度遥测数据。
- 将遥测数据格式化为 Java DTO 兼容的 JSON。
- 保持状态变化显式、可测试。

核心库代码：

```text
native/include/edgelab/
native/src/
```

包含可复用领域 / 运行时代码：

- `TelemetryReading`
- `format_as_json`
- `VirtualDevice`
- `TemperatureProfile`
- `RandomTemperatureProfile`

应用专属代码：

```text
native/apps/device-simulator/
```

包含可执行程序自己的启动和配置逻辑：

- `main.cpp`
- `SimulationConfig`
- 计划中的 `command_line_parser`

## 设计原则

- 核心领域代码保持可复用，不依赖具体应用启动细节。
- 命令行解析属于应用层，不放进共享的 `edgelab_core`。
- 采集动作保持只读：采集遥测数据不应修改设备状态。
- 状态变化通过 `apply_temperature_delta()` 等函数显式表达。
- 随机行为使用固定 seed 保持可复现。
- 在解析、协议、校验、跨语言契约等位置写小而聚焦的测试。

## 当前 native target 结构

重要 CMake target：

- `edgelab_core`
- `device_simulator_support`
- `device_simulator`
- `telemetry_formatter_test`
- `virtual_device_test`
- `temperature_profile_test`
- `random_temperature_profile_test`
- `simulation_config_test`

下一步计划 target：

- `command_line_parser_test`

## Java / C++ 契约

遥测 JSON 结构：

```json
{
  "deviceId": "pump-001",
  "metricCode": "temperature",
  "value": 72.5,
  "unit": "celsius",
  "collectedAt": "2026-05-20T10:30:00Z"
}
```

C++ 内部命名使用 `snake_case`；JSON 和 Java 请求 DTO 字段使用 Java 常见的 `camelCase`。
