# 概念

## 消费者和消费者组



kafka消费者从属于消费者群组。一个群组里的消费者订阅的是同一个主题，每个消费者接受主题一部分分区的消息。

往群组里增加消费者是横向伸缩消费能力的主要方式，不过要注意不要让消费者的数量超过主题分区的数量，多余的消费者只会被限制。



消费者组之间互不影响。

## 消费者群组和分区再均衡

当添加了新的分区后，会发生分区重分配。

分区的所有权从一个消费者转移到另一个消费者，这样的行为被称为再均衡。

再均衡的两种策略：

1. eager rebalances：

1. 所有的消费者放弃已经分配的分区
2. 重新加入群组，得到新的分区分派，然后进行消费
3. 再均衡期间，消费者无法读取消息，会造成整个群组一小段时间的不可用。
4. ![img](https://cdn.nlark.com/yuque/0/2023/png/32473878/1700983773355-57900b73-b5f4-41e6-98a0-81d3f0551ce5.png)

1. cooperative rebalances：also called incremental rebalances

1. 只是重新分配一小部分的分区。
2. 首先，消费者群组leader通知所有消费者有一部分分区需要重新分配，这部分分区的消费者会停止消费
3. 然后，消费者群组leader会重新分配这些分区
4. 这个过程会持续几次，来完成预期的再均衡，它避免了eager rebalance期间的stop the world，提高了可用性。
5. ![img](https://cdn.nlark.com/yuque/0/2023/png/32473878/1700983782209-9891fca1-3441-40a3-9c7f-16a65a31d9bf.png)



消费者通过向被指向群组协调器的broker发送心跳来维持它们和群组的从属关系以及对分区的所有权关系。只要消费者以正常的时间间隔发送心跳，就被认为是活跃的。

消费者会在轮询消息或提交偏移量时发送心跳，如果消费者停止发送心跳的时间足够长，会话就会过期，群组协调器就会认为它已经死亡，就会触发一次再均衡。

如果一个消费者发生崩溃，并停止读取消息，群组协调器会等待几秒钟，确认它死亡了才会触发再均衡。



### 分配分区是如何工作的

当一个消费者想加入群组时，它会给group orordinator发送JoinGroup请求。



## static group membership

默认的，消费者在消费者群组的身份是暂时的，退出又进入时身份id将会变成新的。

除非你配置了一个独一无二的group.instance.id，它将会使消费者成为一个群组的static member。static member使得消费者退出后再加入还是使用之前的分区，从而不需要进行再平衡。

当你的应用想保存分配给消费者的分区的本地状态或者缓存时就比较有用，毕竟，重新构建缓存是耗时的，



# 创建Kafka消费者

读取消息之前，需要先创建KafkaConsumer对象。

KafkaConsumer的属性：

1. bootstrap.servers
2. key.deserializer
3. value.deserializer
4. group.id：指定消费者所属群组的名字



# 订阅主题



```java
public class KafkaConsumer<K, V> implements Consumer<K, V> {
  @Override
  public void subscribe(Collection<String> topics) {
    subscribe(topics, new NoOpConsumerRebalanceListener());
  }
    @Override
    public void subscribe(Pattern pattern) {
        subscribe(pattern, new NoOpConsumerRebalanceListener());
    }
}
consumer.subscribe(Collections.singletonList("customerCountries"));
```

订阅也是使用正则表达式，可以匹配多个主题，如果有人创建了新的主题，并且主题的名字与正则表达式匹配，那么会立即触发一次再均衡，消费者就可以读取新添加的主题。



# 轮询

消息轮询是消费者API的核心，

1. 消费者必须持续对Kafka进行轮询，否则会被认为已经死亡，它的分区会被移交给群组里的其他消费者。
2. poll方法返回一个记录列表，每条记录都包含了记录所属主题的信息、记录所在分区的信息、记录在分区里的偏移量，以及记录的键值对。
3. 在退出应用程序之前使用close方法关闭消费者。网络连接和socket也会随之关闭，并立即触发一次再均衡，而不是等待群组协调器发现它不再发送心跳并认定它已经死亡，因为那样需要更长的时间，导致整个群组在一段时间内无法读取消息。



轮询不只是获取数据那么简单：

1. 在第一次调用新消费者的poll()方法时，它会负责查找GroupCoordinator，然后加入群组，接受分配的分区。
2. 如果发生了再均衡，整个过程也是在轮询期间进行的。
3. 当然，心跳也是从轮询中发送出去的。



线程安全：按照规则，一个消费者使用一个线程。



# 消费者的配置

1. fetch.min.bytes：消费者从服务器获取记录的最小字节数，如果可用的数据量小于该参数，那么它会等到有足够的可用数据时才把它返回给消费者，这样可以降低消费者和broker的工作负载。如果没有很多可用数据，但消费者的CPU使用率却很高，那么就需要把该属性的值设置的比默认值大。如果消费者的数量比较到，把该属性的值设置的大一点可以降低broker的工作负载。
2. fetch.max.wait.ms：broker的等待时间，默认500ms。
3. fetch.max.bytes：消费者从服务器拉取记录的最大字节数，默认50MB。
4. max.poll.records：poll最大能返回的记录数
5. max.partition.fetch.bytes：指定服务器从每个分区返回给消费者的最大字节数，它的默认值是1MB。如果一个主题有20个分区和5个消费者，那么每个消费者需要至少20/5=4MB的可用内存来接受记录（一个消费者可用同时消费多个分区）。在为消费者分配内存时，可以为它们多分配一些，因为如果群组里有消费者崩溃，剩下的消费者需要处理更多的分区。

1. 还需要考虑消费者处理数据时间，如果消费者频繁调用poll()方法来避免会话过期和发生分区再均衡，如果单词调用poll()返回的数据太多，消费者需要更多的时间来处理，可能无法及时进行下一个轮询来避免会话过期。如果出现这种情况，max.partition.fetch.bytes可以设置的小些，或者绘画过期时间延长

1. session.timeout.ms：指定来消费者在被认为死亡之前可以与服务器断开连接的时间，默认是3s。如果消费者没有在指定的时间发送心跳给群组协调器，就被认为已经死亡，协调器就会触发再均衡，把它的分区分配给群组里的其他消费者。

1. hearbeat.interval.ms指定了poll方法向协调器发送心跳的频率，session.timeout.ms指定了消费者可以多久不发送心跳。所以一般需要同时修改这两个属性，heartbeat.interval.ms必须比session.timeout.ms小，一般是其1/3.
2. session.timeout.ms设置的比默认值小，可以更快的检测和恢复崩溃的节点，但是长时间的轮询或者垃圾回收会导致非预期的再均衡；
3. 设置的比默认值大，可以减少意外的再均衡，不过检测节点崩溃需要更长的时间

1. max.poll.interval.ms：无需轮询的时间间隔。
2. default.api.timeout.ms：
3. request.timeout.ms：
4. auto.offset.reset：该属性指定了消费者在读取一个没有偏移量的分区或者偏移量无效的情况下该如何处理

1. latest：从最新的记录开始读取数据
2. earliest：从起始位置读取分区的记录

1. enable.auto.commit：指定了消费者是否自动提交偏移量
2. partition.assignment.strategy：将分区分配给消费者的策略

1. Range：把主题的若干连续的分区分配给消费者
2. RoundRobin：把所有分区逐个分配给消费者
3. Sticky：
4. Cooperative sticky
5. 也可以自定义

1. client.id：标识从客户端发送过来的消息
2. client.rack：一般来说，拉取消息都是从leader副本的，但如果是多中心，从最近的副本拉取更为合理。
3. group.instance.id：上面说到的static group membership
4. max.poll.records：控制单次调用call()方法能够返回的记录数量
5. receive.buffer.bytes/send.buffer.bytes：socket在读写数据时用到的TCP缓冲区也可以设置大小，设置为-1使用操作系统的默认值，如果生产者和消费者与broker属于不同的数据中心，可以适当增大这些值。



# 提交和偏移量

每次调用poll方法，它总是返回由生产者写入kafka但还没有被消费者读取过的记录，我们因此可以追踪哪些记录是被群组里的哪个消费者读取的。

Kafka不会像其他JMS队列那样需要得到消费者的确认，相反，消费者可以使用kafka来追踪消息在分区里的位置（偏移量，offset）。

我们把更新分区当前位置的操作叫做提交。



消费者提交偏移量的方式：往一个叫做_consumer_offset的特殊主题发送消息，消息里包含每个分区的偏移量。如果发生了再均衡，每个消费者可能分配到新的分区，通过偏移量就可以继续之前的工作。



偏移量的记录跟消息，偏移量可能面临的两个问题：

1. 再均衡前的偏移量小于已经执行的任务时，均衡后会从偏移量开始消费，就会导致一部分消息重复消费


1. 再均衡前的偏移量大于已经执行的任务时，均衡后会从偏移量开始消费，就会导致一部分消息不会被消费，产生丢失




这意味着提交偏移量的测量会对消息消费产生很大的影响。

## 自动提交

最简单的方式消费者自动提交偏移量。如果enable.auto.commit = true, 并且auto.commit.interval.ms设置为5s，那么每隔5s，消费者会自动把从poll()方法接受到的最大偏移量提交上去。自动提交也是在轮询里进行的。



如果在最近一次提交之后的3s发生了再均衡，那么这3s内到达的消息会被重复处理。减少提交间隔时间可以减少重复消息，但不能完全避免；

在使用自动提交时，当提交偏移量的时间到了，下一次poll时就会把上一次poll的最后的偏移量提交上去，所以再次poll时一定要确保上一次的poll消息执行完毕。close()也会进行自动提交。

自动提交如果不是poll时提交，很可能丢消息。

自动提交无法避免重复消息消费。

## 提交当前偏移量

消费者API提供了另一种提交偏移量的方式，让开发者自行决定何时提交偏移量，而不是基于时间间隔。

把auto.commit.offset设为false，让应用程序决定何时提交偏移量，使用commitSync()提交偏移量最简单也最为可靠。

commitSync()：提交由poll()方法返回的最新偏移量。所以在调用commitSync()之前要确保处理完了所有的记录，否则会有丢失消息的风险。

如果在处理消息的过程中发生了再平衡，消息还是会被重复消费。



```java
Duration timeout = Duration.ofMillis(100);
while(true) {
    ConsumerRecords<String, String> records = consumer.poll(timeout);
    for(ConsumerRecord<String, String> record : records) {
        process(record);
    }
    try {
        consumer.commitSync();
    } catch(Exception e) {
        log.error("commit failed", e);
    }
}
```

只要没有发生不可恢复的错误，commitSync会一直尝试直到提交成，如果提交失败，我们也只能把异常记录到错误日志里。

## 异步提交

手动提交由一个不足之处是：当broker响应提交请求时应用是阻塞的。这回限制应用的吞吐量。

我们可以通过降低提交频率来提高吞吐，但代价是重平衡时会增加重复消费消息的数量。

```java
Duration timeout = Duration.ofMillis(100);
while(true) {
    ConsumerRecords<String, String> records = consumer.poll(timeout);
    for(ConsumerRecord<String, String> record : records) {
        process(record);
    }
    consumer.commitAsync();
}
```

commitAsync不会进行重试，如果commitAsync进行重试可能造成旧的提交覆盖新的提交，这样又会产生重复消费。

commitAsync支持回调，回调一般用来记录提交错误或者生成度量指标，如果要用来重试，需要谨慎考虑提交顺序，我们可以使用一个单调递增的序列号来维护异步提交的顺序，相当于版本号标记。



其实即使commitAsync不重试，如果发生重平衡，还是可能会产生重复的消息的，这是在性能和再平衡后重复消费之间的权衡。

## 同步和异步组合提交

一般情况下，针对偶尔的提交失败，不进行重试不会有太大问题，因为如果提交失败是因为临时问题引起的，那么后续的提交总会有成功的。

但如果这是发生在关闭消费者或者再均衡前的最后一次提交，就要确保能够提交成功。

因此，在消费者关闭前一般会组合使用commitAsync()和commitSync()

```java
Duration timeout = Duration.ofMillis(100);
try {
    while(true) {
        ConsumerRecords<String, String> records = consumer.poll(timeout);
        for(ConsumerRecord<String, String> record : records) {
            process(record);
        }
        consumer.commitAsync();
	} 
} catch(Exception e) {
    log.error(e);
} finally {
    try {
        consumer.commitSync();
    } finally {
        consumer.close();
    }
}
```

## 提交特定的偏移量

提交偏移量的频率与处理消息批次的频率是一样的，但如果想要更频繁的提交，想要在批次的中间提交。

幸运的是，消费者API允许在调用comimtSync()和commitAsync()时传递一个分区和偏移量的map来进行提交，因为消费者可能不只是读取一个分区，所以需要跟踪所有分区的偏移量。

下面是提交特定偏移量的例子，每处理1000条记录就提交一次偏移量。

```java
private Map<TopicPartition, OffsetAndMetadata> currentOffsets = new HashMap<>();
int count = 0;
...
 while(true) {
    ConsumerRecords<String, String> records = consumer.poll(timeout);
    for(ConsumerRecord<String, String> record : records) {
        process(record);
        TopicPartition key = new TopicPartition(record.topic(), record.partition());
        OffsetAndMetadata value = new OffsetAndMetadata(record.offset() + 1, "no metadata");
        currentOffsets.put(key, value);
        if(count % 1000 == 0) {
            consumer.commitAsync(currentOffsets, null);
        }
        count++;
    }
} 
```



# 再均衡监听器

消费者在退出和进行分区再平衡前会做一些清理工作。

在为消费者分配新分区或移除旧分区时，可以通过消费者API执行一些应用程序代码，在调用subscribe()方法时传进去一个ConsumerReblanceListener实例就可以了。

ConsumerReblanceListener有两个需要实现的方法：

```java
public interface ConsumerRebalanceListener {
  // 再均衡开始之前和消费者停止读取消息之后被调用
  void onPartitionsRevoked(Collection<TopicPartition> partitions);
	// 重新分配分区之后和消费者开始读取消息之前
  void onPartitionsAssigned(Collection<TopicPartition> partitions);
  
}
```

如何在失去分区所有权之前通过onPartitionsRevoked方法来提交偏移量。

```java
private Map<TopicPartition, OffsetAndMetadata> currentOffsets = new HashMap<>();

private class HandleRebalance implements ConsumerRebalanceListener { 
  public void onPartitionsAssigned(Collection<TopicPartition> partitions) {

  }
  public void onPartitionsRevoked(Collection<TopicPartition>
                                  partitions) {
    System.out.println("Lost partitions in rebalance.
    Committing current
    offsets:" + currentOffsets);
    consumer.commitSync(currentOffsets);
  }
}
try {
  consumer.subscribe(topics, new HandleRebalance()); 
  while (true) {
    ConsumerRecords<String, String> records =
    consumer.poll(100);
    for (ConsumerRecord<String, String> record : records)
    {
      System.out.println("topic = %s, partition = %s, offset = %d,
                         customer = %s, country = %s\n",
                         record.topic(), record.partition(), record.offset(),
                         record.key(), record.value());
      currentOffsets.put(new TopicPartition(record.topic(),
                                            record.partition()), new
                         OffsetAndMetadata(record.offset()+1, "no metadata"));
    }
    consumer.commitAsync(currentOffsets, null);
  }
} catch (WakeupException e) {

  //
} catch (Exception e) {
  log.error("Unexpected error", e);
} finally {
  try {
    consumer.commitSync(currentOffsets);
  } finally {
    consumer.close();
    System.out.println("Closed consumer and we are done");
  } 
}
```



# 从特定偏移量处开始处理记录

poll()方法是从各个分区的最新偏移量处开始处理消息，但有时我们需要从特定的偏移量处开始读取消息。

1. seekToBegining(Collection<TopicPartition> tp)：从分区的起始位置开始读取消息
2. seekToEnd(Collection<TopicPartition> tp)：从分区的末尾开始读取消息

```java
Long oneHourEarlier = Instant.now().atZone(ZoneId.systemDefault())
	.minusHours(1).toEpochSecond();
Map<TopicPartition, Long> partitionTimestampMap = consumer.assignment()
	.stream()
	.collect(Collectors.toMap(tp->tp, tp->oneHourEarlier));

// get the offsets that were current at these timestamps.
// the method sends a request to the broker
Map<TopicPartition, OffsetAndTimestamp> offsetMap = 
	consumer.offsetForTimes(partitionTimestampMap);

for(Map.entry<TopicPartition, OffsetAndTimestamp> entry : offsetMap.entrySet()) {
	// reset the offset on each partition to the offset
    consumer.seek(entry.getKey(), entry.getValue().offset());
 }
```
