# grokking streaming systems real-time event processing pp. 140-151

## Streaming systems review and a glimpse ahead

- Windowed computations
- Joining data in real time
- Backpressure
- Stateless and stateful computations

### Windowed computations

![01](https://inasa.dev/img/grokkingstreamingsystems/ch6/01.png)

在流处理系统中，要将事件分割成事件集进行处理，需要使用窗口计算。窗口计算通常有其局限性。窗口操作符处理的是事件集而不是单个事件。

### Joining data in real time

![02](https://inasa.dev/img/grokkingstreamingsystems/ch6/02.png)

### Backpressure

系统可靠性

反压是大多数流处理框架支持的一种常见自我保护机制。通过反压，处理过程会暂时放慢速度，尝试让系统有机会从问题中恢复，例如临时的网络问题或突然的流量激增导致计算机过载。在某些情况下，丢弃事件可能比放慢速度更可取。反压是开发人员构建可靠系统的有用工具。

### Stateless and stateful computations

- 状态组件
