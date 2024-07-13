# Learning OpenTelemetry Setting Up and Operating a Modern Observability System pp. 86-92

## ch6.Instrumenting Libraries

在大多数应用程序中，大部分资源使用发生在库代码中，而非应用程序代码中。应用程序代码本身很少消耗大量资源；相反，它指导库代码使用资源。问题在于，应用程序代码可能会错误地指导这些库。

为什么需要原生检测，而不是提供钩子编写插件?

- 依赖性问题: 插件没有随着库更新
- 限制和支持负担：插件将你的检测限制在认为允许用户执行任意代码的地方，插件需要在库的运行时中插入钩子，这增加了将来需要支持的范围。架构改进经常改变可用的钩子，这会破坏插件的兼容性。钩子越多，兼容性问题就越严重
- 性能开销：插件和钩子增加了一层间接性，可能会增加开销。提供的数据需要转换为检测使用的格式，会浪费内存

追踪是库可观测性的真正障碍。由于追踪需要跨越库边界传播上下文，只有在所有库都使用相同的追踪系统时才能正常工作。

Shared Libraries Checklist

- OpenTelemetry API默认是关闭的，只有在应用程序所有者开启时才会激活
- 避免将OpenTelemetry API封装在自定义API中
- OpenTelemetry 语义约定
- 使用现有的语义约定作为编写自己约定的指南
- 确保库仅引用API包

Shared Services Checklist

- 允许用户通过公开标准的 OpenTelemetry 配置选项和环境变量配置系统生成的遥测数据
- 虽然包括额外的导出器和插件是可以的，但仅提供通过 HTTP/Proto 的 OTLP 作为默认导出选项已经足够。用户可以使用 Collector 在下游拆分和转换此输出
- 如果提供虚拟机或容器镜像，考虑提供一个安装了本地 Collector 的版本，以便捕获机器指标和其他资源
