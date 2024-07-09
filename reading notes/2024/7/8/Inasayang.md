# Learning OpenTelemetry Setting Up and Operating a Modern Observability System pp. 61-79

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

#### SpanProcessors

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

### MeterProvider

MeterProvider 实现了 OpenTelemetry 的度量 API。由以下组件组成：

- 视图（Views）：定义如何聚合和处理度量数据
- 度量读取器（MetricReaders）：从 MeterProvider 中读取度量数据
- 度量生成器（MetricProducers）：生成度量数据
- 度量导出器（MetricExporters）：将度量数据导出到指定的目的地

#### MetricReaders

MetricReaders 相当于度量数据的 SpanProcessors。它们收集和缓冲度量数据，直到数据可以导出。默认的 MetricReader 是 PeriodicExportingMetricReader。

每个 MetricProducer 都需要注册到一个 MetricReader。

视图（Views）用于自定义 SDK 输出的度量数据。

### LoggerProvider

LoggerProvider 由 LogRecordProcessors 和 LogRecordExporters 组成。

### Shutting Down Providers

在关闭应用程序时，必须在应用程序终止之前刷新任何剩余的遥测数据。

### Attaching Resources

资源（Resources）是一组属性，用于定义收集遥测数据的环境。它们描述了服务、虚拟机、平台、区域、云提供商等所有你需要知道的信息，以便将生产问题与特定位置或服务相关联。如果你的遥测数据（如 spans、metrics 和 logs）告诉你发生了什么，那么资源则告诉你这些事情发生在哪里。

#### Resource Detectors

除了特定服务的资源外，大多数资源来自应用程序部署的环境，例如 Kubernetes、AWS、GCP、Azure 或 Linux。这些资源来自已知位置，通常有标准的获取方式。用于发现这些资源的插件称为资源检测器（resource detectors）。

大多数资源可以由本地 Collector 发现，并在遥测数据通过 Collector 时附加到来自应用程序的遥测数据上。

几乎所有的 OpenTelemetry SDK，不论语言，都包括资源检测器。访问某些资源需要 API 调用，这可能会减慢应用程序启动速度，因此推荐使用 Collector 方法。

#### Service Resources

有一组关键资源无法从环境中收集：描述你的服务的资源。包括：

- service.name：这类服务的名称，例如 frontend 或 payment-processor
- service.namespace：服务名称并不总是全局唯一的。服务命名空间可以帮助区分两种不同类型的 “frontend” 服务
- service.instance.id：描述此特定实例的唯一 ID，使用你用于生成唯一 ID 的任何格式
- service.version：版本号，使用你用于版本控制的任何格式

### Layering Spans and Metrics

度量不仅仅用于测量你的服务中有多少 CPU 使用量或垃圾收集暂停时间。有效地使用应用程序度量可以节省资金，并使你能够分析长期性能趋势。

一个好的做法是为API 端点创建直方图度量，尤其是高吞吐量的端点。直方图是一种特殊类型的度量流，由桶（buckets）和落入这些桶中的计数组成。你可以将它们视为捕获值分布的方法。

OpenTelemetry 支持标准的预定义直方图和指数桶直方图。后者非常有用，它们会自动调整所输入的测量值的规模和范围，并且可以相加。这意味着可以运行一百个 API 服务器实例，所有这些实例都创建指数直方图来跟踪吞吐量、错误率和延迟，然后将所有值相加，即使它们的规模和范围不同。

### Browser and Mobile Clients

用户交互的设备，如手机、笔记本电脑、触摸屏和汽车，是我们分布式系统中的关键组件。浏览器和移动客户端通常在内存低、网络连接差的受限环境中运行。没有客户端遥测数据，解决这些性能问题是困难的。同时，也难以理解产品功能或 GUI 的更改将如何影响用户体验。

在可观察性领域，客户端遥测数据通常被称为真实用户监控（RUM）。

### 次要的可观察性信号

其他一些信号，如分析（profiling）、会话（sessions）和事件（events）。这些是遥测数据的专门类型，用于真实用户监控（RUM）和持续分析（continuous profiling）等技术中（持续分析是一种从运行中的进程获取代码级遥测数据的方法）。在撰写本文时，这些信号在 OpenTelemetry 中还未稳定，但相关工作正在进行中。最终，OpenTelemetry 对信号的实际使用方式持中立态度；它关注的是如何以标准化的方式创建、收集和表达这些信号，包括统一它们。

RUM 是理解分布式系统的重要部分，但你需要将其与后端遥测数据连接起来，才能真正转变你的可观察性实践。

RUM（Real User Monitoring）即真实用户监控，是一种用于监测和分析实际用户在使用网站或应用程序时的行为和性能的技术。通过收集和分析用户的真实交互数据，RUM 可以帮助开发者和运营团队了解用户体验，识别性能瓶颈，发现潜在问题，并优化系统性能和用户界面。

RUM的主要功能和优势：

- 用户体验分析：RUM 可以提供关于页面加载时间、交互速度、错误率等的详细数据，帮助你了解用户在使用应用程序时的体验
- 性能监控：通过监控真实用户的行为，RUM 可以识别出哪些页面或功能存在性能问题，从而有针对性地进行优化
- 问题诊断：RUM 可以捕捉用户遇到的错误和异常，并提供详细的上下文信息，帮助开发者迅速找到问题根源
- 用户行为分析：RUM 可以追踪用户的点击、滚动、导航等操作，帮助你了解用户的使用习惯和行为路径，从而改进界面设计和用户流程

RUM的实现方式，RUM 通常通过在网页或应用程序中嵌入监控代码来实现，这些代码可以捕捉用户的交互数据并将其发送到后端服务器进行分析。以下是 RUM 的一些常见实现方式：

- 浏览器端监控：在网页中嵌入 JavaScript 代码，实时捕捉用户的交互数据，如页面加载时间、点击事件、滚动行为等
- 移动端监控：在移动应用中集成 RUM SDK，捕捉用户在应用中的操作和性能数据
- 数据传输和存储：将捕捉到的数据发送到后端服务器，进行存储和分析，生成详细的报告和图表
