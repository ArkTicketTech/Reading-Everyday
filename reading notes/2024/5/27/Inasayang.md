# grokking streaming systems real-time event processing pp. 0-20

![01](https://inasa.dev/img/grokkingstreamingsystems/01.png)

code repo :  <https://github.com/nwangtw/GrokkingStreamingSystems>

## Part 1.Getting started with streaming

### Ch 1.Welcome to Grokking Streaming Systems

- 流处理简介
- 区分流处理系统与其他系统

关键特征: 事件在可用时（或几乎是可用时）立即被处理。
大多数情况下，延迟从几毫秒到几秒不等，可以被认为是实时或近实时处理；因此，流处理也被称为实时处理。

从使用的角度来看，流处理通常用于分析不同类型的事件。因此，实时分析、流式分析和事件处理等术语也可能在不同场景中用于指代流处理系统。

通常情况下，流处理系统中处理的事件没有预定的结束时间

![ch1-01](https://inasa.dev/img/grokkingstreamingsystems/ch1/01.png)

- 应用程序和后端服务的设计目的是尽快为客户（人类用户或远程请求）提供服务。
- 批处理系统的设计目的是高效处理海量数据。
  - 在典型的批处理系统中，整个过程被分解为多个步骤或阶段。各阶段通过存储连接，这些存储用于保存中间数据。
  - 主要的限制：延迟
  - 批处理系统需要在定期的时间间隔内（例如每小时或每天）收集和存储数据作为批次，然后开始处理。在特定时间窗口内收集的任何事件都需要等到窗口结束才能处理。在某些情况下，这可能是不可接受的。
- 流处理系统旨在以低延迟处理海量数据

![ch1-02](https://inasa.dev/img/grokkingstreamingsystems/ch1/02.png)

典型的流处理系统架构看起来与批处理系统类似。整个过程被分解为多个称为组件的步骤，数据在组件之间持续流动，直到处理步骤完成。

![ch1-03](https://inasa.dev/img/grokkingstreamingsystems/ch1/03.png)

流处理系统与批处理系统的主要区别在于，流处理系统的组件是长时间运行的进程。它们持续运行并接受新的数据进行处理。每个事件在被前一个组件处理后，会立即被下一个组件处理。因此，流处理系统在接收到事件后不久就会生成最终结果。

![ch1-04](https://inasa.dev/img/grokkingstreamingsystems/ch1/04.png)

无论是批处理系统还是流处理系统，都具有多阶段架构(multi-stage architecture)。这种架构具有一些使其适用于数据处理用例的优势：

- 更灵活：开发人员可以根据需要添加或删除作业的阶段
- 更具扩展性：各阶段相互连接，但彼此独立。如果某个阶段在现有实例中成为整个过程的瓶颈，可以轻松增加更多实例以提高吞吐量
- 更易维护：复杂的过程可以由简单的操作组成，这些操作更容易实现和维护

批处理系统的一个主要优势是故障处理简单。如果发生问题，例如计算机崩溃或读写数据失败，可以简单地在另一台机器上重新调度并重新运行失败的步骤。
