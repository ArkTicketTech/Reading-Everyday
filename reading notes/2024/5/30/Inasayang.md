# grokking streaming systems real-time event processing pp. 81-108

## ch4.Stream graph

- stream fan-out
- stream fan-in
- graph and DAG (directed acyclic graph)

需要一个系统，其中包含多个基于规则的分析器操作符，用于评估交易并对风险进行评分。最后，需要一个分类器来结合每个分析器的所有评分并做出决策。分析器应用规则来评估交易的风险。所有的风险评分最终会被结合成一个结果。

![ch04-01](https://inasa.dev/img/grokkingstreamingsystems/ch4/01.png)

卡网络位于商店和银行之间。当交易进入卡网络时，逻辑处理会提供尽可能多的信息给支付银行。这有助于银行决定是否支付某笔交易。

- 实体店、在线业务，甚至移动设备都可以接受信用卡支付。
- 卡网络在收集尽可能多的信息后，将交易路由到正确的银行，以帮助银行决定是否支付费用。欺诈检测系统就在这里生成风险评分。
- 银行根据卡网络收集的信息来决定是否允许交易通过。

### Streaming isn’t always a straight line

可以像收费站系统那样一字长蛇阵的构建这个系统。

虽然可行的，但不理想。未来会添加新的分析器，列表会变长，端到端的延迟也会增加。当有很多分析器时，也会导致维护这个作业可能会变得更加困难。

![ch04-02](https://inasa.dev/img/grokkingstreamingsystems/ch4/02.png)

所有三个分析器都连接到交易源，并独立运行。评分聚合器收集它们的结果，并汇总评分以做出最终决策。添加更多分析器时，端到端的延迟不会增加。

![ch04-03](https://inasa.dev/img/grokkingstreamingsystems/ch4/03.png)

欺诈评分的范围是 0 到 3。评分为 0 表示任何分析器都没有检测到欺诈，评分为 3 表示所有分析器都检测到了欺诈。每个分析器都会为评分加一分。可以将评分为 2 或更高的交易视为潜在的欺诈交易。

![ch04-04](https://inasa.dev/img/grokkingstreamingsystems/ch4/04.png)

交易源在事件进入信用卡系统的 API 网关时拉取事件。它会创建相同交易的 3 个不同实例并将它们分发给各个分析器。

- 平均票据分析器 会查看交易的消费金额。如果金额超出客户的正常消费历史，它会将欺诈评分加 1。
- 窗口交易计数分析器 会检查在指定时间窗口内来自同一账户的交易。一个人在短时间内多次使用同一张卡是不太可能的。如果看到这种行为，它会将欺诈评分加 1。
- 窗口接近分析器 会检查在指定时间窗口内来自同一账户的交易。在一个地点刷卡后，随后又在 200 英里外刷卡是不太可能的。如果发现这种行为，它会将欺诈评分加 1。

评分聚合器会等待每个上游分析器在一个小时间窗口内的结果，然后汇总总的欺诈评分。在这个窗口过期后，它会将评分写入数据库。

### Upstream and downstream components

对于每个组件，直接位于前面的组件（或组件群）是其上游组件，直接位于后面的组件是其下游组件。事件从上游组件流向下游组件。

### Stream fan-out and fan-in

交易源组件有三个与之连接的下游组件。这被称为流分发（stream fan-out）。同样地，评分聚合器有三个上游组件（我们也可以说这三个分析器有相同的下游组件）。这被称为流汇聚（stream fan-in）。

### Graph, directed graph, and DAG

大多数流处理作业都可以表示为有向无环图 (DAG)。

在有向图中，一个循环意味着事件可以回到同一个组件中再次处理。由于额外的复杂性和风险，这种情况需要非常小心地处理。

![ch04-05](https://inasa.dev/img/grokkingstreamingsystems/ch4/05.png)

![ch04-06](https://inasa.dev/img/grokkingstreamingsystems/ch4/06.png)

当一个组件发出新事件时，实例执行器会将该事件复制并放入所有连接的输出队列中。

在当前的解决方案中，每个事件都会被多次复制。由于这些不同的队列，不同的分析器能够异步处理每个事件。这些大的事件通过网络传输，并由分析器加载和处理。此外，一些分析器不需要或无法处理某些事件，但这些事件仍然会被传输和处理。结果是，内存和网络资源的使用效率不高，在事件流量较高时，这一点尤为重要。

为了改进这种情况，可以采用以下方法：

- 事件压缩：在将事件传输到分析器之前，可以对事件进行压缩，以减少网络传输的数据量。
- 事件过滤：在传输事件之前，过滤掉不需要处理的事件，以减少传输和处理的开销。
- 事件共享内存：使用共享内存机制，让多个分析器访问同一个事件实例，而不是复制多个副本。
- 按需加载：只有在需要时才加载事件的某些部分，以减少内存消耗。

在流分叉（stream fan-out）中，不同的输出队列不需要彼此相同。

- 发出的事件可以被推送到某些输出队列，但可以跳过其他队列。
- 指向不同下游组件的不同输出队列中的事件可以具有不同的数据结构。

![ch04-07](https://inasa.dev/img/grokkingstreamingsystems/ch4/07.png)

![ch04-08](https://inasa.dev/img/grokkingstreamingsystems/ch4/08.png)

![ch04-09](https://inasa.dev/img/grokkingstreamingsystems/ch4/09.png)

![ch04-10](https://inasa.dev/img/grokkingstreamingsystems/ch4/10.png)

### another stream fan-in: Join

在数据库中，join操作用于组合来自多个表的列。

然而，相对于数据库表，流更加动态。事件被连续接受和处理，匹配来自多个连续数据源的字段需要更多的考虑。