# 生产者概览

不同的使用场景对生产者API对使用和配置不同。

1. 有的场景不允许消息丢失和重复，例如支付；
2. 有的场景则允许，例如网站点击统计。

我们从创建一个ProducerRecord开始，ProducerRecord对象包括：
```java
public class ProducerRecord<K, V> {

    // 主题
    private final String topic;
    // 可选，指定分区直接发送该分区，没有指定则根据key来选择一个分区。
    private final Integer partition;
    private final Headers headers;
    // key和value需要先序列化，才能在网络上传输。
    private final K key;
    private final V value;
    // 记录的时间戳
    private final Long timestamp;

}
```

服务器接收到消息后会返回一个响应，如果消息成功写入kafka，就返回一个RecordMetaData对象，它包含了主题和分区信息，以及记录在分区里的偏移量。如果写入失败，则会返回一个错误。生产者收到错误之后会尝试重新发送消息，几次之后还是失败就返回错误信息。



```java
public final class RecordMetadata {

    /**
     * Partition value for record without partition assigned
     */
    public static final int UNKNOWN_PARTITION = -1;

    // 记录在分区里的偏移量
    private final long offset;
    // The timestamp of the message.
    // If LogAppendTime is used for the topic, the timestamp will be the timestamp returned by the broker.
    // If CreateTime is used for the topic, the timestamp is the timestamp in the corresponding ProducerRecord if the
    // user provided one. Otherwise, it will be the producer local time when the producer record was handed to the
    // producer.
    // 记录在分区里的时间戳
    private final long timestamp;
    private final int serializedKeySize;
    private final int serializedValueSize;
    private final TopicPartition topicPartition;

}

public final class TopicPartition implements Serializable {
    private static final long serialVersionUID = -613627415771699627L;

    private int hash = 0;
    // 分区
    private final int partition;
    // 主题
    private final String topic;
}
```





# 创建Kafka生产者

1. bootstrap.servers：broker的地址，host:port。清单里不需要包含所有的broker地址，生产者会从给定的broker里查找其他broker的信息。建议至少提供两个broker的信息，一旦其中一个宕机，生产者仍然能够连接到集群上。
2. key.serializer：必须设置的。
3. value.serializer

```java
  public static Properties initConfig() {
    Properties properties = new Properties();
    // 集群的broker地址
    properties.put(ProducerConfig.BOOTSTRAP_SERVERS_CONFIG, "broker1:9092, borker2:9092");
    // org.apache.kafka.common.serialization.StringSerializer
    properties.put(ProducerConfig.KEY_SERIALIZER_CLASS_CONFIG, StringSerializer.class.getName());
    properties.put(ProducerConfig.VALUE_SERIALIZER_CLASS_CONFIG, StringSerializer.class.getName());
    return properties;
  }
```

发送消息的方式：

1. 发送并忘记：发送后不关心是否正常到达，大多数情况下，消息会正常送达，因为kafka是高可用的，生产者会自动尝试重发。但不能保证不丢消息。
2. 同步发送：返回一个Future对象，调用get()方法进行等待
3. 异步发送：指定一个回调函数，服务器在返回响应时调用该函数。

```java
public class KafkaProducer<K, V> implements Producer<K, V> {
  @Override
  public Future<RecordMetadata> send(ProducerRecord<K, V> record) {
    return send(record, null);
  }
  
  @Override
  public Future<RecordMetadata> send(ProducerRecord<K, V> record, Callback callback) {
    // intercept the record, which can be potentially modified; this method does not throw exceptions
    ProducerRecord<K, V> interceptedRecord = this.interceptors.onSend(record);
    return doSend(interceptedRecord, callback);
  }


}
```

生产者可以多线程发送。
