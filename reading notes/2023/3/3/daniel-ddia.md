分布式的 Actor 框架
Actor 模型是一种基于消息传递的并发编程模型。Actor 通常是由状态（State）、行为（Behavior）和信箱（MailBox，可以认为是一个消息队列）三部分组成：

状态：Actor 中包含的状态信息。
行为：Actor 中对状态的计算逻辑。
信箱：Actor 接受到的消息缓存地。
由于 Actor 和外界交互都是通过消息，因此本身可以并行的，且不需要加锁。

分布式的 Actor 框架，本质上是将消息队列和 actor 编程模型集成到一块。自然，在 Actor 滚动升级是，也需要考虑前后向兼容问题。
