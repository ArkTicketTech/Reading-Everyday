# 发送消息到Kafka

```java
public void send() {
  ProducerRecord<String, String> record = new ProducerRecord<>(
    message.getTopic(),
    null,
    message.getCreatedDate().getTime(),
    message.getKey(),
    message.getContent());
  try {
    producer.send(record);       // 异步发送
    producer.send(record).get(); // 同步发送
  
    producer.send(record, new DemoProducerCallback()); // 异步回调, 对异常结果进行处理
  } catch(Exception e) {
    e.printStackTrace();
  }
}

public class DemoProducerCallback implements Callback {

  @Override
  public void onCompletion(RecordMetadata metadata, Exception exception) {
    if (exception != null) {
      exception.printStackTrace();
    }
  }
}
```

消息会发送到缓冲区，然后使用单独的线程发送到服务器。

# 生产者配置

1. client.id
2. akcs：必须要有多少个分区副本收到消息，生产者才会认为消息是写入成功的。

1. acks = 0：生产者不会等待服务器的响应，适合高吞吐量，但消息可能丢失
2. acks = 1：领导者节点收到消息就成功响应，领导者节点没有收到消息会进行重试，但还是可能丢消息，当领导者节点崩溃的时候
3. acks = all：所有复制节点全部收到消息才成功响应。最安全的模式，但是发送一个消息的延迟较高
4. acks的设置意味着在可靠性和延时之间的权衡。

1. message delivery time:

1. ![img](https://cdn.nlark.com/yuque/0/2023/png/32473878/1700897174731-b534099f-2bc5-4517-ad47-d5a94174c27f.png)
2. max.block.ms：send()或者paritionsFor()时的阻塞时间，当send buffer满了的时候，send是可能阻塞的，超过该时间抛出异常。
3. linger.ms：生产者在发送批次之前等待更多的消息加入批次的时间，在批次填满或者linger.ms达到上限时把批次发送出去，因为批量发送，能够提升吞吐量
4. request.timeout.ms：生产者等待服务器的超时时间，不包括重试，超时会重试，或者回调。
5. retires：失败后重试次数，并不是所有的错误都会重试，比如消息体太大就不会进行重试。
6. retry.backoff.ms：重试之前会等待一段时间，默认100ms
7. delivery.timeout.ms：真正发送到得到响应的时间，包括重试。如果重试时超过了这个时间，会执行回调函数。

1. buffer.memory：生产者内存缓冲区的大小，如果消息发送速度超过了发送到服务器的速度，缓冲区就会被占满，这个时候，send()方法要么被阻塞，要么抛出异常，取决于max.block.ms。
2. compression.type：默认消息发送时不进行压缩，该参数设置压缩算法
3. batch.size：多个消息需要被发送到一个分区时，会放在同一个批次，该参数指定了一个批次可以使用的内存大小。
4. max.in.flight.requests.per.connection：生产者在收到服务器响应之前可以发送多少个消息，设置为1可以保证消息是按照发送的顺序写入服务的，即使发生了重试。如果设置为2，可能会有重试的消息后写入的场景，就保证不了顺序性。
5. max.request.size：控制生产者发送请求的大小，该参数是指单个请求大小的最大值
6. receive.buffer.bytes and send.buffer.bytes：TCP socket接受和发送数据包的缓冲区大小
7. enable.idempotence：设置为true时，生产者会为每个发送的记录设置一个唯一ID，保证不会重复发送消息

# 序列化器

自定义序列化器容易出现兼容性问题，最好的使用通用的序列化器Avro。

# 分区

消息的key有两个用途：

1. 作为消息的附加信息
2. 决定消息被写到主题的哪个分区



如果key为null，并且使用了默认的分区器，那么记录将被随机分配；

如果key不为null，并且使用了默认的分区器，那么记录将会按照key散列后分配；



同一个key的消息必然被分配到同一个分区。



只有不改变主题分区数量的情况下，key与分区的映射才能保持不变。



也可以自定义分区策略，实现partitioner类。

```java
public class BananaPartitioner implements Partitioner {

  @Override
  public int partition(String topic, Object key, byte[] keyBytes, Object value, byte[] valueBytes,
      Cluster cluster) {
    if ((keyBytes == null) || !(key instanceof String)) {
      throw new InvalidRecordException("We expect all messages to have customer name as key");
    }
    // Get the list of partitions for this topic
    List<PartitionInfo> partitions = cluster.partitionsForTopic(topic);
    int numPartitions = partitions.size();

    if (((String) key).equals("Banana"))
      return numPartitions; // Banana
//
    return Math.abs(Utils.murmur2(keyBytes)) % (numPartitions - 1);
  }

  @Override
  public void close() {

  }

  @Override
  public void configure(Map<String, ?> configs) {

  }
}
```



# headers

Records还包含了headers，Record headers可以添加一些metadata，header经常加一些数据来源，路由和追踪数据就可以基于header，而不用解析message（也许消息被压缩或者加密了）。

```java
ProducerRecord<String, String> record =
        new ProducerRecord<>("CustomerCountry", "Precision Products", "France");
record.headers().add("privacy-level","YOLO".getBytes(StandardCharsets.UTF_8));
```

api

```java
@Override
public Headers add(String key, byte[] value) throws IllegalStateException {
    return add(new RecordHeader(key, value));
}
```



# interceptors

kafka支持不用拦截，可以保证在不修改代码的情况下做些监控和追踪。

甚至可以拦截增加标准的header，以便更好的进行追踪。

```java
import java.util.Map;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicLong;
import org.apache.kafka.clients.producer.ProducerInterceptor;
import org.apache.kafka.clients.producer.ProducerRecord;
import org.apache.kafka.clients.producer.RecordMetadata;

public class CountingProducerInterceptor implements ProducerInterceptor {

  ScheduledExecutorService executorService =
      Executors.newSingleThreadScheduledExecutor();
  static AtomicLong numSent = new AtomicLong(0);
  static AtomicLong numAcked = new AtomicLong(0);
  @Override
  public ProducerRecord onSend(ProducerRecord record) {
    numSent.incrementAndGet();
    return record;
  }

  @Override
  public void onAcknowledgement(RecordMetadata metadata, Exception exception) {
    numAcked.incrementAndGet();
  }

  @Override
  public void close() {
    executorService.shutdownNow();
  }

  @Override
  public void configure(Map<String, ?> configs) {
    Long windowSize = Long.valueOf((String) configs.get("counting.interceptor.window.size.ms"));
    executorService.scheduleAtFixedRate(CountingProducerInterceptor::run, windowSize, windowSize,
        TimeUnit.MILLISECONDS);
  }
  
  public static void run() {
    System.out.println(numSent.getAndSet(0));
    System.out.println(numAcked.getAndSet(0));
  }
}
```



# Quotas and Throttling

kafka可以限制生产者和消费者的速度。

quota.producer.default=2M 
