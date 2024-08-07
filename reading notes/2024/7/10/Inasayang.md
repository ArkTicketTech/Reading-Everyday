# Learning OpenTelemetry Setting Up and Operating a Modern Observability System pp. 81-86

## ch6.Instrumenting Libraries

在大多数应用程序中，大部分资源使用发生在库代码中，而非应用程序代码中。应用程序代码本身很少消耗大量资源；相反，它指导库代码使用资源。问题在于，应用程序代码可能会错误地指导这些库。

为什么需要原生检测，而不是提供钩子编写插件?

- 依赖性问题: 插件没有随着库更新
- 限制和支持负担：插件将你的检测限制在认为允许用户执行任意代码的地方，插件需要在库的运行时中插入钩子，这增加了将来需要支持的范围。架构改进经常改变可用的钩子，这会破坏插件的兼容性。钩子越多，兼容性问题就越严重
- 性能开销：插件和钩子增加了一层间接性，可能会增加开销。提供的数据需要转换为检测使用的格式，会浪费内存
