# Learning OpenTelemetry Setting Up and Operating a Modern Observability System pp. 99-106

## Metamonitoring

元监控,即监控收集器的性能,也很重要。收集器会暴露一些指标，例如由Memory Ballast扩展提供的otelcol_processor_refused_spans和otelcol_processor_refused_metric_points。通过这些指标得知限制器是否导致收集器拒绝新数据。通过计算queue_size和queue_capacity指标之间的差异，得知接收服务是否忙碌。

将收集器部署在容器中，作为 Kubernetes 或其他容器编排器的一部分。在容器内部，一个好的规则是为内存限制和内存球使用2的倍数。例如，将内存球设置为容器内存的40%，然后将限制设置为80%。通过将内存预分配给堆，这可以通过减少内存清理的频繁性来提高性能，并允许收集器向生产者发出信号，让他们减缓遥测数据的生成，而不会因为内存耗尽而崩溃或重启。

关于生产环境中部署收集器架构的建议

- 在每个 pod 中使用 sidecar 收集器作为遥测数据的第一站。将遥测数据从进程和 pod 中刷新到 sidecar 中可以减轻业务服务的内存压力，从而简化开发和部署。这也可以在迁移或逐出过程中实现 pod 的干净关闭，因为进程不会因忙碌的遥测端点而等待。
- 按信号类型分离收集器，以便它们可以独立扩展。您还可以根据使用模式为每个应用程序甚至每个服务创建池。日志、追踪和指标处理都有不同的资源消耗特性和限制。
- 建议在遥测数据的创建和配置之间清晰地分离。例如，在收集器上执行数据的修订和采样，而不是在进程中执行。在进程中硬编码配置使得在生产环境中调整变得更加困难，而调整收集器配置通常要容易得多。
