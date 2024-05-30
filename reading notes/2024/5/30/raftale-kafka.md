消费者位移
Consumer 的位移数据作为一条条普通的 Kafka 消息，提交到 __consumer_offsets 中，__consumer_offsets 的主要作用是保存 Kafka 消费者的位移信息。
消息的key和value
1. key：<group_id, topic, partition>
2. value：位移及一些元数据
   

Coordinator专门为Consumer Group服务，负责为Group执行Rebalance以及提供位移管理和组成员管理等。
具体来讲，Consumer 端应用程序在提交位移时，其实是向 Coordinator 所在的 Broker 提交位移。同样地，当 Consumer 应用启动时，也是向 Coordinator 所在的 Broker 发送各种请求，然后由 Coordinator 负责执行消费者组的注册、成员管理记录等元数据管理操作。


如何避免消费者断连？
1. session.timout.ms 决定了Consumer发送心跳的超时时间。
2. heartbeat.interval.ms：consumer发送心跳的频率
3. max.poll.interval.ms：限定了consumer端应用程序两次调用poll方法的最大时间间隔，如果超过该时间consumer就会被要求离开group，重新rebalance。
4. 所以第一个是要避免非必要的rebalance，一般设置session.timout.ms是heartbeat.interval.ms的3倍，给发送心跳时间一点冗余；
5. 第二个是要避免消费者处理消息时间过长，
6. 第三个是避免consumer的长时间full GC

位移提交：
1. enable.auto.commit ：自动提交和手动提交
2. auto.commit.interval.ms：自动提交间隔时间，默认5s
3. 手动提交分为同步提交和异步提交，同步提交会重试直到提交成功，异步提交不会重试。

提交失败异常

不可恢复的异常：CommitFailedException
poll了，提交还没完成就发生了rebalance，提交就会遇到该异常。
产生的原因可能是： 当消息处理的总时间超过预设的max.poll.interval.ms参数值。
所以要尽可能保证每次消费尽可能的快，比如每次拉取消息条数不要过多，每条消息处理要尽可能快。
当然，也可以增加max.poll.interval.ms参数值。
