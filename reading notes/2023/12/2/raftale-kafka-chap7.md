可靠性是系统的一个属性，不是某一个组件的事情。

kakfa在数据传递可靠性方面具备很大的灵活性，有一些场景需要严格的数据一致性，有些场景则需要性能和简洁性。kafka被设计为高度可配置的，它的客户端API可以满足不同程序的可靠性需求。

# 可靠性保证

1. kafka可以保证分区消息的顺序；
2. 只要当消息被写入分区的所有同步副本时（不一定要写入磁盘），才会被认为已提交的。
3. 只要还有一个副本是活跃的，那么已经提交的消息就不会丢失
4. 消费者只能读取已经提交的消息

# 复制

Kafka的复制机制是分区的多副本架构是kafka可靠性保证的核心。

分区leader是同步副本，对于follower来说，它需要满足以下条件才被认为是同步的

1. 与zookeeper之间有一个活跃的会话，也就是说，在过去的6s内向zookeeper发送过心跳
2. 在过去的10s内从首领那里获取过消息
3. 在过去的10s内从首领那里获取过最新的消息

# broker配置

## 复制系数

主题级别的配置参数：replication.factor

broker级别的配置参数：default.replication.factor。

复制系数决定了分区副本的数量。

复制系数需要对以下特性作出权衡。

1. 可用性
2. 持久性Durability
3. 吞吐量：副本越多，broker之间要进行复制的流量越多
4. End-to-end latency：消息必须同步到所有副本才能被消费，副本越多，消息消费的延迟就会变高。
5. cost：副本越多，存储和网络成本就越高



建议复制系数设置为3。

副本的物理分布也很重要。

## Unclean Leader Election

当一个分区的leader不再可用时，其中一个同步副本（in-sync replica）被选举为新的leader，这个leader选举就是clean的，因为同步副本能保证数据始终是最新的。

但如果选举时没有可用的同步副本，又该如何？

1. 如果不同步的副本不能被提升为新的leader，那么分区在旧leader恢复之前是不可用的；
2. 如果不同步的副本可以被提升为新首领，那么在这个副本变为不同步之后写入旧首领的消息会全部丢失，造成数据不一致。

unclean.leader.election参数为true就表明了允许不同步的副本成为leader，这是broker级别（实际上是集群范围内）的配置，默认为false。

可用性和数据一致性 需要有权衡。

## Minimum In-Sync Replicas

min.insync.replicas参数配置最小同步的副本，如果低于该值，生产者的请求将不可用。



## Keeping Replicas In Sync

一个副本变为不同步副本的两种方式：

1. 失去与kakfka的连接：zookeeper.session.timeout.ms，该设置时间内没有发送心跳broker就会被认为死亡，2.5版本默认18s。
2. 在指定的时间内无法跟上leader的最新消息：该时间由replica.lag.time.max.ms控制。2.5版本默认为30s，这个时间也会影响消费者的消费延迟，因为消息只有被写入所有的同步副本后，才被允许消费。

# Using Producer in a Reliable System

即使我们配置了broker的可靠性，如果没有配置好生产者的配置，也不一定可靠。

比如说：

1. 我们配置了三个副本，unclean leader election 配置false，acks = 1。如果leader replica确认了消息，但马上崩溃了，此时还没来得及同步到跟随着副本上，如果副本作为新的leader被选举出来，那么对于生产者来说，就是丢失了信息。
2. 还是三个副本的配置，unclean leader election 配置false，但acks配置为all。如果写消息的时候leader崩溃了，kafka会响应「Leader Not Available」，如果生产者没有进行重试，那么消息就会丢失。这是因为生产者没有正确处理错误，也可能造成消息丢失。

上面的例子证明了需要注意两件重要的事情来保证消息可靠性：

1. 需要正确的acks配置来满足可靠性的要求
2. 需要正确的处理错误

## Send Acknowledgements

1. acks = 0：消息投递出去就返回成功的响应，即使kafka集群不可用、分区下线等，但网络错误和序列化失败还是会返回失败
2. acks = 1：只要leader副本确认消息
3. acks = all：leader需要等待所有的同步副本返回响应。最少同步副本的数量由min.insync.replicas控制。

## Configuring Producer Retries

生产者需要处理的错误有两种：

1. 生产者可以自动处理的错误
2. 需要开发者手动处理的错误



broker返回的错误分为两种：

1. 可以通过重试解决的
2. 无法通过重试解决的

如果broker返回的是「Leader_Not_Avaliable」，生产者就可以尝试重新发送消息，它是一个可重试的错误。

INVALID_CONFIG则是不可重试的错误。

可重试的错误最好是保持重试。重试次数建议使用默认的MAX_INT，即无限次重试，但并不会真的无限等待，delivery.timeout.ms设置了send最大的超时时间。

重试消息可能导致消息重复，使用enable.idempotence=true将导致生产者在记录中包含额外的信息，broker将使用这些信息来保证消息的精准一次，第八章将详细讨论。



## Additional Error Handling

开发者需要处理的错误：

1. 不可重试的broker错误，例如消息大小错误、认证错误等
2. 在消息发送到broker之前的错误，例如序列化错误
3. 生产者达到重试次数上限或者消息占用内存达到上限时发生的错误
4. 超时

如何处理错误需要看具体情况，可以直接丢弃、记录、保存到本地磁盘、回调等。

# 在可靠的系统里使用消费者

消费者消费到的消息一定是同步到所有同步副本中的消息。

消费者每次请求一批消息后，会拿批次中最近的offset作为下一次的起始偏移量，这保证了不丢消息的有序性。

消费者组中可能有多个消费者，为了使消费者彼此之间知道应该消费的消息偏移量，因此消费者需要提交他的消费偏移量。所以必须要保证消费完消息才能提交偏移量，否则就可能丢消息。

## 消费者可靠的配置

1. group.id：消费者群组id
2. auto.offset.reset：没有偏移量可提交时，比如消费者第一次启动或者请求的偏移量在broker上不存在时，消费者会做什么。

1. earliest：消费者会从分区的开始位置读取消息
2. latest：消费者会从分区的末尾开始读取消息

1. enable.auto.commit：是否自动提交偏移量。
2. auto.commit.interval.ms：选择自动提交偏移量后，提交的间隔时间
