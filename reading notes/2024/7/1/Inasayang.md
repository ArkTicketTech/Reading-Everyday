# Learning OpenTelemetry Setting Up and Operating a Modern Observability System pp. 41-

## ch4.The OpenTelemetry Architecture

OpenTelemetry 包含三类组件：安装在应用程序中的instrumentation、用于基础设施（如 Kubernetes）的exporters，以及将所有这些遥测数据发送到存储系统的pipeline components。

### Application Telemetry

遥测数据最重要的来源是应用程序。这意味着要使 OpenTelemetry 正常工作，必须在每个应用程序中安装它。无论是通过使用代理自动安装，还是通过编写代码手动安装，所安装的组件都是相同的。

### Library Instrumentation

最重要的遥测数据来自开源软件（OSS）库，例如框架、HTTP 和 RPC 客户端以及数据库客户端。这些库在大多数应用程序中承担了大量工作，并且通常来自这些库的遥测数据足以覆盖应用程序执行的几乎所有工作。

### The OpenTelemetry API

### The OpenTelemetry SDK

### Infrastructure Telemetry

### Telemetry Pipelines
