# grokking streaming systems real-time event processing pp. 53-80

## ch3.Parallelization and data grouping

- Parallelization (并行化)
- Data Parallelism (数据并行) 和 Task Parallelism (任务并行)
- Event Grouping (事件分组)

![ch03-01](https://inasa.dev/img/grokkingstreamingsystems/ch3/01.png)

![ch03-02](https://inasa.dev/img/grokkingstreamingsystems/ch3/02.png)

水平扩展

需要注意的是，现代 CPU 内部有指令流水线，以显著提高处理性能。

![ch03-03](https://inasa.dev/img/grokkingstreamingsystems/ch3/03.png)

在流处理方面，数据执行独立性(data execution independence)意味着无论数据元素的计算或执行顺序如何，最终结果都是相同的。例如，在将队列中的每个元素乘以 4 的情况下，无论是同时进行还是一个接一个地进行，它们的结果都是相同的。这种独立性允许使用数据并行。

![ch03-04](https://inasa.dev/img/grokkingstreamingsystems/ch3/04.png)

除了数据并行性之外，还有另一种并行化类型：任务并行性，也称为功能并行性。与数据并行性涉及在不同数据上运行相同任务相比，任务并行性则侧重于在相同数据上运行不同任务。

传感器读取器和车辆计数器组件不断运行以处理传入的事件。当车辆计数器组件正在处理（计数）一个事件时，传感器读取器组件同时获取一个不同的、新的事件。换句话说，这两个不同的任务是并发工作的。这意味着一个事件从传感器读取器发出，然后由车辆计数器组件处理。

### Data parallelism vs. task parallelism

- 数据并行性表示相同的任务同时在不同的事件集上执行。
- 任务并行性表示不同的任务同时执行。

数据并行性广泛应用于分布式系统中，以实现水平扩展。在这些系统中，通过增加更多的计算机来提高并行性相对容易。相反，对于任务并行性，通常需要手动干预将现有过程分解为多个步骤以增加并行性。

流处理系统是数据并行性和任务并行性的结合体。在流处理系统中，数据并行性指的是创建每个组件的多个实例，而任务并行性指的是将整个过程分解为不同的组件来解决问题。

- 并行化：同时进行许多相同的事情。
- 并发：同时进行许多不同的事情。

![ch03-05](https://inasa.dev/img/grokkingstreamingsystems/ch3/05.png)

- 在一个实例内，处理顺序保证与原始顺序（传入队列的顺序）相同。
- 在不同实例之间，处理顺序不保证。有可能后来的事件被处理和/或完成的时间早于另一个较早到达的事件，如果这两个事件是由不同实例处理的。

### Event grouping

伪随机路由在许多情况下是可以接受的，但有时可能希望预测性地将事件路由到特定的下游实例。这个将事件定向到实例的概念称为事件分组。

所有事件被划分为不同的组，每个组被分配给一个特定的实例进行处理。有几种事件分组策略。最常用的两种是：

- 洗牌分组（Shuffle grouping）——事件被伪随机分布到下游组件，
- 字段分组（Fields grouping）——事件根据事件中指定字段的值被预测性地路由到相同的下游实例。

#### Shuffle grouping

![ch03-06](https://inasa.dev/img/grokkingstreamingsystems/ch3/06.png)

洗牌分组简单来说就是将数据元素从一个组件随机分布到下游操作符。它允许将负载相对均匀地分配给下游操作符。

在许多框架中，循环分配（Round Robin）是实现洗牌分组的方法。在这种分组策略中，下游实例（即传入队列）以相等的份额和循环顺序被选择。相比基于随机数的洗牌分组，这种分布可以更加均匀，计算也更加高效。

#### Fields grouping

![ch03-07](https://inasa.dev/img/grokkingstreamingsystems/ch3/07.png)

字段分组通过根据流事件元素中的字段（通常由开发人员指定）来决定将数据路由到哪里。在许多情况下，字段分组也被称为按组或按键分组。

![ch03-08](https://inasa.dev/img/grokkingstreamingsystems/ch3/08.png)

为了确保相同的车辆事件始终分配到相同的组（路由到相同的实例），通常使用哈希。

将键的哈希除以并行度，并返回余数来决定事件将分配到哪个下游操作符实例。

事件调度器是流处理系统中的一个组件，位于组件执行器之间，执行事件分组过程。它不断地从其指定的传入队列中拉取事件，并根据分组策略返回的键将事件放置到其指定的传出队列中。

![ch03-09](https://inasa.dev/img/grokkingstreamingsystems/ch3/09.png)

事件调度器负责从上游组件执行器接受事件，应用分组策略，并将事件发送到下游组件。
