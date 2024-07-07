# Learning OpenTelemetry Setting Up and Operating a Modern Observability System pp. 61-68

## ch5.Instrumenting Applications

设置 OpenTelemetry 的过程分为两个步骤：安装软件开发工具包（SDK）和安装自动化工具。

SDK 是处理和导出遥测数据的 OpenTelemetry 客户端。自动化工具是使用 OpenTelemetry API 编写的生成遥测数据的代码。

安装 OpenTelemetry SDK：

- 安装 SDK 是第一步，选择适合你编程语言的 SDK 版本并进行安装
- 配置 SDK，使其能够处理和导出遥测数据。通常包括设置导出器（Exporter）和处理器（Processor）

安装自动化工具：

- 自动化工具是实际生成遥测数据的代码。可以手动编写，也可以使用现成的库和框架来简化这个过程
- 根据需要选择适当的自动化工具库，并在你的应用程序中集成

### Agents and Automated Setup

在所有编程语言中，需要安装两个部分：处理和导出遥测数据的 SDK，以及与应用程序使用的框架、数据库客户端和其他常见组件相匹配的所有自动化工具库。

这涉及到许多组件的安装和设置。理想情况下，希望能够将所有这些工作自动化。但在实际操作中，每种语言的自动化支持程度不同。有些语言提供完全自动化，无需编写任何代码；而有些语言则完全不提供自动化支持。

### Installing the SDK

### Registering Providers

调用 OpenTelemetry API，在默认情况下，什么也不会发生。这个 API 调用是一个空操作（no-op），意味着调用这个 API 是安全的，但什么也不会发生，也没有任何开销。

要让某些事情发生，需要将提供者（providers）注册到 API 中。提供者是 OpenTelemetry 自动化工具 API 的实现。这些提供者处理所有的 API 调用。TracerProvider 创建 tracers 和 spans。MeterProvider 创建 meters 和 instruments。LoggerProvider 创建 loggers。

### TracerProvider

TracerProvider 实现了 OpenTelemetry 的跟踪 API。它由采样器（samplers）、Span 处理器（SpanProcessors）和导出器（exporters）组成。

采样器（Samplers）决定 span 是被记录还是被丢弃。

选择的采样器在很大程度上取决于你发送遥测数据的跟踪分析工具所具备的功能，要结合成本，目的。如果不确定，最好完全不要进行采样。最好一开始就不进行任何采样，然后根据需要减少特定的成本或开销再添加采样。

### SpanProcessors

Span 处理器（SpanProcessors）允许你收集和修改 spans。它们会在 span 开始和结束时各拦截一次。

默认的处理器叫做 BatchProcessor。这个处理器会缓冲 span 数据并管理导出器插件。通常将 BatchProcessor 安装为处理管道中的最后一个 SpanProcessor。BatchProcessor 具有以下配置选项：

- MaxQueueSize：指定可以缓冲的最大 span 数量
- ScheduleDelayMillis：指定定期批处理和导出 span 数据的时间间隔，以毫秒为单位
- ExportTimeoutMillis：指定单次批处理导出操作的超时时间，以毫秒为单位
- MaxExportBatchSize：指定单次批处理操作中导出的最大 span 数量

大多数默认值是可以的。但如果遥测数据要导出到本地 Collector，建议将 scheduledDelayMillis 设置为一个更小的数值。

选择在 SDK 中使用处理器，顺序很重要。处理器按照注册顺序链在一起，并以线性方式运行。例如，修改遥测数据的处理器应在批处理它们的处理器之前。

导出器（Exporters）将这些 spans 从进程中导出并转化为可读取的格式。这些插件定义了遥测数据的格式和目的地。默认情况下，建议使用 OpenTelemetry Protocol (OTLP) 导出器。

- 协议（protocol）： OTLP 支持三种传输协议：gRPC、http/protobuf 和 http/json。我们推荐使用 http/protobuf，这也是默认选项
- 端点（endpoint）：
- 请求头（headers）
- 压缩（compression）
- 超时（timeout）： OTLP 导出器等待每个批处理导出的最长时间。默认值是 10 秒。
- 证书文件（certificate file）、客户端密钥文件（client key file）和客户端证书文件（client certificate file）
