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

### 11. Phase 2：C++ 设备模拟器起步

Phase 2 开始进入 C++ 侧开发。当前目标不是立刻连接 Java 后端，而是先建立一个可编译、可测试、可运行的 C++ 工程骨架。

当前 C++ 模块位于：

```text
native/
```

主要结构：

```text
native/
  CMakeLists.txt
  apps/device-simulator/
  include/edgelab/
  src/
  tests/
```

目录含义：

- `apps/device-simulator/`：放可执行程序入口。当前设备模拟器从这里启动。
- `include/edgelab/`：放对外可见的头文件，类似 Java 中公共 API 或接口定义。
- `src/`：放 C++ 实现文件。
- `tests/`：放测试程序。

#### CMake 的作用

CMake 是 C++ 项目常用的构建配置工具。它本身不是编译器，而是根据 `CMakeLists.txt` 生成适合不同平台和工具链的构建文件。

本项目使用 CMake 的原因：

- Visual Studio 可以直接打开 CMake 项目。
- 后续 WSL/Linux 也可以使用同一套 CMake 配置。
- 避免把项目绑定到 Visual Studio 专有 `.sln` 工程。

当前设置使用 C++17：

```cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
```

含义：

- `CMAKE_CXX_STANDARD 17`：要求使用 C++17。
- `CMAKE_CXX_STANDARD_REQUIRED ON`：如果编译器不支持 C++17，就直接失败。
- `CMAKE_CXX_EXTENSIONS OFF`：尽量使用标准 C++，不依赖某个编译器的私有扩展。

#### Visual Studio 编码警告

Visual Studio 曾出现 `C4819` 警告，意思是文件中包含当前代码页无法表示的字符。原因通常是源码中有中文注释，但文件或编译器没有按 UTF-8 处理。

本项目在 CMake 中为 MSVC 增加：

```cmake
if(MSVC)
    add_compile_options(/utf-8)
endif()
```

含义：

- `MSVC` 表示当前编译器是 Microsoft Visual C++。
- `/utf-8` 告诉 MSVC 按 UTF-8 处理源码文件。

如果仍然出现编码警告，可以在 Visual Studio 中对文件使用：

```text
File -> Save As -> Save with Encoding -> UTF-8
```

#### 第一个 C++ 数据结构：TelemetryReading

当前用 `struct` 表示一条遥测读数。

`struct` 可以先理解为 Java 中只有字段的 DTO。区别是 C++ 的 `struct` 成员默认是 `public`，适合表达简单数据对象。

当前字段包括：

- `device_id`：设备 ID。
- `metric_code`：指标编码。
- `value`：采集值。
- `unit`：单位。
- `collected_at`：采集时间。

字段命名使用 `snake_case`，这是 C++ 中常见风格。Java 通常使用 `camelCase`，所以同一业务字段在 Java 和 C++ 中可能分别写成：

```text
Java: deviceId
C++ : device_id
JSON: deviceId
```

#### const 引用参数

格式化函数使用：

```cpp
std::string format_as_json(const TelemetryReading& reading);
```

其中：

- `std::string`：返回 C++ 标准库字符串。
- `TelemetryReading`：参数类型。
- `&`：引用传参，避免复制整个对象。
- `const`：函数承诺不会修改传入对象。

这是一种常见 C++ 企业代码习惯。只读取一个对象时，优先考虑 `const T&`，既表达意图，又避免不必要的拷贝。

#### 第一个 C++ 测试

当前测试程序是：

```text
telemetry_formatter_test
```

它不是 JUnit 或 GoogleTest，而是一个最小 C++ 可执行程序。测试通过时不输出内容，失败时打印 `Expected` 和 `Actual` 并返回非零退出码。

CTest 根据退出码判断测试结果：

- 返回 `0`：测试通过。
- 返回非 `0`：测试失败。

因此，`telemetry_formatter_test` 没有输出并不表示没运行；如果 CTest 显示 `100% tests passed`，就表示测试通过。

#### 第一个设备模拟器输出

当前 `device_simulator` 会创建一条固定遥测数据，然后调用 `format_as_json` 转成 JSON 并打印到控制台。

这一步的业务意义是：

1. C++ 侧已经能表达一条设备数据。
2. C++ 侧已经能生成 Java 后端可理解的 JSON 结构。
3. 后续只需要把“打印到控制台”替换为“发送 HTTP 请求”，就能完成第一次 Java + C++ 协作。
