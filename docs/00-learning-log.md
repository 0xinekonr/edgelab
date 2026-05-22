# EdgeLab 学习笔记

## 2026-05-20

### 1. 项目定位

EdgeLab 是一个用于练习 Java + C++ 协同开发的工业 IoT 边缘网关与云端监控平台。

业务场景可以理解为：工厂、机房、仓库、电站等场景中存在大量设备，例如水泵、电机、温湿度传感器、电表、压力传感器。这些设备会持续产生运行数据，例如温度、压力、电压、转速、故障码。边缘侧程序负责采集、解析、缓存、过滤和上报这些数据；云端系统负责设备管理、数据接收、告警判断、历史查询和后续统计分析。

这个项目中，Java 更适合承担云端业务系统，C++ 更适合承担靠近设备侧的高性能、低资源占用、协议解析和数据处理工作。

### 2. 当前开发环境决策

本项目尽量贴近 Java 和 C++ 各自常见的开发习惯。

Java 侧：

- 使用 Windows 侧开发。
- 使用 IntelliJ IDEA。
- 使用 JDK 17。
- 使用 Spring Boot 3.x。
- 平时在 Windows 中开发，部署和中间件环境后续再逐步接触 Linux/WSL。

C++ 侧：

- 优先使用 Visual Studio，方便新手学习断点调试、变量查看、调用栈和工程管理。
- 使用 CMake 管理 C++ 工程，避免绑定到 Visual Studio 专有项目格式。
- 代码尽量使用标准 C++，后续可以在 WSL/Linux 中验证可移植性。

基础设施：

- MySQL 8.4.8 运行在 WSL。
- Redis 8.6.3 运行在 WSL。

### 3. 项目目录结构

仓库根目录：

```text
D:\playground\edgelab
```

当前规划结构：

```text
edgelab/
  docs/
  backend/
  native/
  deploy/
```

目录含义：

- `docs/`：学习笔记、业务说明、架构设计和开发记录。
- `backend/`：Java/Spring Boot 云端后端服务。
- `native/`：C++ 设备模拟器、边缘网关、协议解析和后续高性能模块。
- `deploy/`：后续放 Docker Compose、部署脚本和环境配置。

`backend/` 是 Spring Boot Maven 工程根目录。之前误初始化成了 `backend/backend/`，已经修正为：

```text
edgelab/
  backend/
    pom.xml
    src/
```

### 4. Maven 命名

后端工程使用以下 Maven 标识：

```xml
<groupId>com.edgelab</groupId>
<artifactId>edgelab-backend</artifactId>
<name>edgelab-backend</name>
```

相比 `backend` 这种泛名，`edgelab-backend` 更适合企业项目。原因是后续在构建产物、日志、部署单元、服务注册、CI/CD 和监控系统中都更容易识别它属于哪个项目、哪个服务。

### 5. Git 工作流

项目已经初始化 Git，并完成了第一次远程 push。

当前采用的工作流是：

- `main` 分支保持稳定。
- 每个阶段使用独立 feature 分支。
- 小步提交，提交信息表达清楚本次变更。
- 阶段完成后推送 feature 分支，并通过 PR 合并到 `main`。

当前阶段分支：

```text
feature/phase-1-backend-device-api
```

这种流程比“本地全部开发完后一次性上传”更接近企业开发方式，也便于复盘每个阶段的演进过程。

### 6. 第一个 Spring Boot 服务

后端服务已经初始化完成，并成功验证：

```text
GET /actuator/health
```

返回：

```json
{
  "status": "UP"
}
```

`Actuator` 是 Spring Boot 常用的生产级监控组件。健康检查接口在后续 Docker、负载均衡、服务网关、监控系统中都会用到。

当前基础依赖包括：

- Spring Web：提供 HTTP API 开发能力。
- Validation：提供请求参数校验能力。
- Spring Boot Actuator：提供健康检查和运行状态接口。
- Spring Boot Test：提供自动化测试能力。

### 7. `/api/v1` 接口风格

当前遥测接口使用：

```text
POST /api/v1/telemetry
```

含义：

- `/api`：表示这是给程序调用的接口，不是网页页面。
- `/v1`：表示接口版本是第 1 版。
- `/telemetry`：表示业务资源是遥测数据。

公共 API 和企业内部 API 经常使用类似风格。原因是接口一旦被前端、移动端、边缘网关、第三方系统或其他微服务调用，就不能随意改变请求和响应格式。如果未来接口结构发生较大变化，可以新增：

```text
/api/v2/telemetry
```

这样旧客户端继续使用 `/api/v1/telemetry`，新客户端迁移到 `/api/v2/telemetry`，可以降低升级风险。

### 8. 遥测数据的业务含义

遥测数据，英文是 telemetry，指设备自动采集并上报的运行数据。

示例请求：

```json
{
  "deviceId": "pump-001",
  "metricCode": "temperature",
  "value": 72.5,
  "unit": "celsius",
  "collectedAt": "2026-05-20T10:30:00Z"
}
```

字段说明：

- `deviceId`：设备唯一标识，例如 `pump-001`。
- `metricCode`：指标编码，例如 `temperature`、`pressure`、`voltage`。
- `value`：采集到的数值。
- `unit`：单位，例如 `celsius`、`kpa`、`volt`。
- `collectedAt`：设备采集该数据的时间。

当前实现只做接收和确认，后续会逐步加入设备合法性校验、数据库存储、告警规则、消息队列和统计分析。

### 9. 第一个测试踩坑：JSON 字符串必须使用双引号

测试中曾经出现：

```text
Status expected:<202> but was:<400>
```

原因是测试请求体中写了非法 JSON：

```json
{
  "deviceId": 'pump-001'
}
```

JSON 标准要求字符串必须使用双引号：

```json
{
  "deviceId": "pump-001"
}
```

MockMvc 会通过 Spring MVC 和 Jackson 按照 JSON 规范解析请求体。非法 JSON 会在进入 Controller 之前就失败，因此返回 `400 Bad Request`。

这个问题说明：自动化测试不仅能测试业务逻辑，也能帮助发现请求格式、序列化和反序列化问题。

### 10. 当前编码规范

本项目后续保持以下习惯：

- 尽量先写测试，再写实现。
- Controller 保持轻量，只处理 HTTP 请求和响应。
- 业务逻辑放到 Service 中。
- 请求和响应使用独立 DTO 或 record 表达。
- 注释只写必要内容，重点解释业务含义、关键配置、重要框架机制或不明显的实现原因。
- 每一步变更尽量小而清晰，便于测试、提交和回滚。
