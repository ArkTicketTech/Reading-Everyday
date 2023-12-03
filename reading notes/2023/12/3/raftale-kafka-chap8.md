精准一次是两个特性的结合：
1. 幂等生产者：避免因为生产者重试造成的重复消息
2. 事务性语义：它在流处理中保证精准一次语义。
   Idempotent Producer
   Producer的幂等性是指的当发送同一条消息时，数据在broker端只会被持久化一次。但这个幂等性是有条件的：
1. 幂等生产者只能防止因为producer自身重试（无论这个重试是生产者、网络、broker异常导致的）导致的重复消息。除此之外，比如生产者程序上发送了两次重复的消息，是没有办法识别的。
2. 只能保证Producer在单个会话内不丢不重，如果Producer重启，broker会为producer产生一条新的producer ID。

参数enable.idempotence需要设置为true。
Properties props = new Properties();
props.put(ProducerConfig.ENABLE_IDEMPOTENCE_CONFIG, "true");
props.put("acks", "all"); // 当 enable.idempotence 为 true，这里默认为 all
props.put("bootstrap.servers", "localhost:9092");
props.put("key.serializer", "org.apache.kafka.common.serialization.StringSerializer");
props.put("value.serializer", "org.apache.kafka.common.serialization.StringSerializer");

KafkaProducer producer = new KafkaProducer(props);

producer.send(new ProducerRecord(topic, "test");
How Does the Idempotent Producer Work?

当启动幂等生产者时，每个消息会标识一个producer ID（PID）和一个序列号（sequence numbers）：
1. PID：标识客户端，每个Producer在初始化的时候都会被分配一个唯一的PID，客户端重启的时候会重新分配一个新的PID，所以幂等是在会话级别的。
2. sequence numbers：同一个PID下，每个消息都会标识一个sequence number，一般是从0开始自增的
   加上消息和分区，这些唯一标识了每个消息。当broker接受到了重复的消息，broker会拒绝掉。
   与分布式系统一样，考虑幂等生产者在故障条件下的行为是很有趣的。考虑两种情况：生产者重新启动和代理失败。
   Producer restart
   如果生产者重启，由于生产者初始化，broker会产生一条新的producer ID，同一条消息会被认为是两条不同的消息。
   PID是会话级别的
   broker fail
   幂等生产者只能防止因为producer自身重试（无论这个重复是生产者、网络、broker异常导致的）导致的重复消息。除此之外，比如生产者程序上发送了两次重复的消息，是没有办法识别的
