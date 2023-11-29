如何退出
如何安全干净的退出？
如果要关闭消费者并且希望立即退出，即使消费者可能正在等待长时间的poll，可以通过另一个线程调用consumer.wakeup()。如果你在主线程中运行消费者循环，则可以从shutdownHook执行此操作。
consumer.wakeup() 是唯一的可以安全地从不同线程调用的消费者方法。
调用wakeup()将导致poll()退出并抛出WakeupException，WakeupException不需要被处理，但在退出线程之前，必须调用consumer.close()。
代码：
https://github.com/gwenshap/kafka-examples/blob/master/SimpleMovingAvg/src/main/java/com/shapira/examples/newconsumer/simplemovingavg/SimpleMovingAvgNewConsumer.java

1. ShutdownHook 运行在一个独立的线程中，利用wakeup打断消费者poll
2. 如果poll loop 足够短，可以用原子变量在每个迭代中检测。

# 反序列化器

生成消息使用的反序列化器与读取消息使用的反序列器应该是一一对应的。

Avro和Schema注册表进行序列化和反序列化的优势是：AvroSerializer可以保证写入主题的数据和主题的schema是兼容的。

尽量不要使用自定义的反序列化器。

# 独立消费者- 为什么以及怎么使用没有群组的消费者

如果只有一个消费者的时候，就不需要订阅主题，而是为自己分配分区。但一个消费者无法同时做订阅和分配分区这件事。

如何为自己分配分区？

```java
List<PartitionInfo> partitionInfos = consumer.partitionsFor("topic");

if(partitionInfos != null) {
    fort(PartitionInfo partition : partitionInfos) {
    	partitions.add(new TopicPartition(partition.topic(), partition.partition()));
    }
	consumer.assign(partitions);
	while(true) {
        ConumserRecords<String, String> records = consumer.poll(timeout);
         for (ConsumerRecord<String, String> record : records) {
             dosomething();
         }
        consumer.commitSync();
    }

}
```

除了不会发生再均衡，也不需要手动查找分区，但如果主题增加了新的分区，消费者并不会收到通知。所以要么周期性调用consumer.partitionsFor()方法来检查是否有新分区加入，要么在添加新分区后重启应用程序。
