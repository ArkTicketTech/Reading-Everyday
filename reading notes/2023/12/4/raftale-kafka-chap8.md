聚合event的流处理应用有时对重复消息很敏感，比如计算平均值时某个相同的事件被重复生产者两次，检查结果的用户通常无法检测到平均值不正确，因此需要以来kafka保证不会丢失消息，也不会产生重复消息的特性。不丢消息、不产生重复消息就是Exactly-Once的语义。

1. At-Least-Once：不会丢弃已经提交的消息，但可能产生重复消息
2. Exactly-Once：不丢消息、不产生重复消息

流处理应用一般是指：拉取消息的消费者会处理消息，然后生成一个新的消息继续投递。

本章我们将会讨论如何使用kafka的Exactly-Once语义以及它的局限性。

Exactly-Once Semantics是两个特性的结合：

1. 幂等生产者：避免因为生产者重试造成的重复消息
2. 事务性语义：在流处理中保证Exactly-Once处理。

# Idempotent Producer

Producer的幂等性是指的当发送同一条消息时，数据在broker端只会被持久化一次。但这个幂等性是有条件的：

1. 幂等生产者只能防止因为producer自身重试（无论这个重试是生产者、网络、broker异常导致的）导致的重复消息。除此之外，比如生产者程序上发送了两次重复的消息，是没有办法识别的。
2. 只能保证Producer在单个会话内不丢不重，如果Producer重启，broker会为producer产生一条新的producer ID。

参数enable.idempotence需要设置为true。

```java
Properties props = new Properties();
props.put(ProducerConfig.ENABLE_IDEMPOTENCE_CONFIG, "true");
props.put("acks", "all"); // 当 enable.idempotence 为 true，这里默认为 all
props.put("bootstrap.servers", "localhost:9092");
props.put("key.serializer", "org.apache.kafka.common.serialization.StringSerializer");
props.put("value.serializer", "org.apache.kafka.common.serialization.StringSerializer");

KafkaProducer producer = new KafkaProducer(props);

producer.send(new ProducerRecord(topic, "test");
```

## 如何实现幂等生产者


当启动幂等生产者时，每个消息会标识一个producer ID（PID）和一个序列号（sequence number）：

1. PID：标识客户端，每个Producer在初始化的时候都会被分配一个唯一的PID，客户端重启的时候会重新分配一个新的PID，所以幂等是在会话级别的。
2. Sequence Number：同一个PID下，每个消息都会标识一个sequence number，一般是从0开始自增的

PID + 主题topic + 分区partition + Sequence Number 唯一标识了每个消息，因为重试而产生的重复消息会被broker拒绝掉。

与分布式系统一样，考虑幂等生产者在故障条件下的行为是很有趣的。考虑两种情况：

1. producer restart：如果生产者重启，由于生产者初始化，broker会产生一条新的producer ID，同一条消息会被认为是两条不同的消息。
2. broker fail：broker挂掉后，controller会选举另一个broker上的follower replica成为新的leader replica。新的leader replica能被选出来，就说明它是同步副本，由于leader副本会在内存中保存最近的5条sequence number来对消息进行去重，同步副本也同步了这5条序列号，所以新的leader分区也能实现去重。

1. 如果挂掉的broker又恢复上线，内存中肯定没有数据了，是可以通过同步其他broker副本来获取最新的序列号。但是如果集群只有这一个broker呢？因此当broker shutdown或者每次创建segment的时候，会将生产者的状态进行快照snapshot然后保存到文件中，当broker重启后，就可以再将文件加载到内存中。

1. 如果broker崩溃了但是最新的snapshot有没有更新怎么办呢？Producer ID and sequence ID也是日志中消息格式的一部分，重启恢复状态也会从每个分区的最近的segment的消息中恢复，这样就可以恢复到最新的producer state，然后生成新的snapshot。

幂等生产者只能防止因为producer自身重试（无论这个重复是生产者、网络、broker异常导致的）导致的重复消息。除此之外，比如生产者程序上发送了两次重复的消息，是没有办法识别的。



**如何开启幂等性生产者**

配置enable.idempotence=true

# 事务

kafka的事务是为流处理开发的。生产者幂等只能保证单个生产者会话中单分区的幂等，幂等不能跨分区运作，而事务可以保证分区写入操作的原子性。

kafka通过事务来实现exactly-once语义。

## 事务案例

事务是在需要accuracy比较准确的。

事务适用于任何需要高精度的流处理应用，尤其是流处理包含聚合和join时。

1. 生产者发送多条消息可以封装在一个事务中来形成一个原子操作。
2. consume-process-produce模式：消息消费和处理后生产另一个消息 封装在一个事务中，形成一个原子操作，常常一个服务需要从上游接受消息，然后经过处理后送达到下游。

## 事务能解决什么问题？

当我们需要我们的消息只被精确的处理一次。

通过事务，Kafka可以保证：

1. 跨生产者会话的消息幂等发送：

1. 生产者实例崩溃下线：消费者拉取消息后，需要处理消息并提交消费偏移量，如果处理消息后，还没来得及提交偏移量，应用就崩溃了，心跳的缺失会发生再均衡，之前消费的分区就会被分配到新的消费者。因为还没来得及提交消费偏移量，所以之前消费的部分信息会被重复消费。
2. 僵尸生产者实例：应用拉取消息后，然后可能因为long GC与kafka断开连接，kafka会认为该应用已经死亡，新的相同事务ID的实例启动并建立连接，新的应用实例处理完消息后，如果旧应用又开始工作，就会造成消息重复消费。

1. 但如果有事务ID，新的实例的事务ID版本号高于旧的实例，旧的实例的send\commit\abort请求就会被拒绝。

1. 跨生产者会话的事务恢复：

## 事务是如何保证精准一次的

一个简单的流处理应用：

1. 读消息
2. 处理消息
3. 将结果写入另一个消息

Exactly-once处理意味着这三个步骤是原子性的，

处理消息生成结果并提交偏移量必须是原子性的，如果偏移量被提交但结果没有被正确生成，或者结果生成但偏移量没有被提交，都会破坏原子性。为了支持这种行为，kafk事务引入了atomic multipartition 写，该想法是：提交偏移量和生成结果都涉及向分区写入消息，如果我们有一个事务，能保证同时写入这两条消息的原子性，那么我们将获得exactly-once语义。


使用事务并执行多分区写入，我们需要使用事务型生产者。事务型生产者只是一个配置了`transactional.id`并且通过`initTransactions()`初始化的生产者。与broker为producer自动生成的producer.id不一样，transactional.id是生产者配置的一部分，在重启后应该保持不变。

实际上，transactional.id的主要作用就是在重启后能识别出来是同一个生产者。kafka brokers维护transactional.id和producer.id的映射，因此如果初始化时重新调用initTransactions()，将分配之前相同的produer.id。

要防止僵尸应用（即短暂冻结之后再恢复的应用）的重复，通常的方法是使用epoch，即`transactional.id`带有版本号，`initTransaction()`时递增事务的版本号，send、commit、abort 生产者的请求时，相同的`transactional.id`但低版本号的请求将会被FencedProducer拒绝掉。

这里僵尸应用的场景其实没有想通

事务是一个生产者的特性，创造一个事务型生产者，开启一段事务，将records写入多个分区，提交消费偏移量，提交或者终止事务。这些操作都是来自于生产者，但这远远不够.

消费者需要配置正确的隔离保证，否则我们无法获得预期的exaclty-once保证。通过设置隔离级别isolation.level，我们能够控制消息被事务写入消息的消费。

如果设置为读已提交read_commited，那么只有成功提交了的事务消息才能被消费者进行消费，读未提交read_uncommited的级别下消费者可能读取到还没有被提交的消息。

读未提交是kafka默认的隔离级别

这个其实跟MySQL的事务可以关联起来理解，读未提交可以看到其他未提交事务的执行结果，造成脏读。


为了保证消息能被有序的读取，读已提交模式不会返回那些还没有确认提交的消息。

Last Stable Offset，LSO端点之前的内容认为是事务已提交，但这个是不包括LSO的。

如果读已提交的消费区poll消息，只会返回LSO之前的数据。如果执行事务的时间太长，将会提高消费者的延迟，因此应该避免长事务。

## 事务没有解决的问题

kafka的exactly-once是针对于consume-process-produce的流处理任务链的，也就是事务是保证多个在kafka不同分区上的写操作，kakfa中的事务是不保证不在kafka上的写操作的。在其他context下，保证事务则需要额外的努力才行：

1. 流处理的副作用：调用api、写文件、发邮件这类的副作用很明显无法得到事务的保证。
2. 从kafka读取消息然后写入database：写入数据库和提交偏移量是无法得到kakfa事务的保证，如果要保证事务，可以依赖于数据库的事务，将偏移量也提交到数据库，与写入数据作为事务操作。如果发生了重复消息，偏移量作为唯一key可以去重。

1. 微服务 更新数据库和发布消息到kafka需要是原子事务的，一个通用的解决方法是out-box pattern：即将更新的消息发送到一个特定的主题中（outbox），然后一个独立的消息处理服务来读取消息并更新数据库。这样的好处是发送消息是事务型的，要么成功要么失败。

1. 发布/订阅模式：publish/subscribe模式中使用事务，读已提交的消费者不会看到未确定的消息，但不足以保证exactly-once



## 如何使用事务

事务是kafka协议的一个broker feature，有许多客户端支持事务。

最常见和最推荐使用事务来保证exactly-once的是kafak streams，毕竟事务是专门为这种场景设计的。



要开启事务，需要配置processing.guarantee为exactly_once。

```java
public interface Producer<K, V> extends Closeable {
	// 初始化事务
	void initTransactions();

	// 开启事务
	void beginTransaction() throws ProducerFencedException;

	// 在事务内提交已经消费的偏移量
	void sendOffsetsToTransaction(Map<TopicPartition, OffsetAndMetadata> offsets,
																ConsumerGroupMetadata groupMetadata) throws ProducerFencedException;

	// 提交事务
	void commitTransaction() throws ProducerFencedException;

	// 丢弃事务
	void abortTransaction() throws ProducerFencedException;
}
```



```java
public class ExactlyOnceMessageProcessor {
	public void test() {
		// 同一个应用中，消费者接受消息，处理后由生产者投递消息
		// 生产者配置
		Properties producerProps = new Properties();
		producerProps.put(ProducerConfig.BOOTSTRAP_SERVERS_CONFIG, "localhost:9092");
		producerProps.put(ProducerConfig.CLIENT_ID_CONFIG, "DemoProducer");
		// 配置事务ID
		producerProps.put(ProducerConfig.TRANSACTIONAL_ID_CONFIG, transactionalId);
		producer = new KafkaProducer<>(producerProps);

		// 消费者配置
		Properties consumerProps = new Properties();
		consumerProps.put(ConsumerConfig.BOOTSTRAP_SERVERS_CONFIG, "localhost:9092");
		consumerProps.put(ConsumerConfig.GROUP_ID_CONFIG, groupId);
		// 配置自动提交为false
		props.put(ConsumerConfig.ENABLE_AUTO_COMMIT_CONFIG, "false");
		// 配置隔离级别为读已提交
		consumerProps.put(ConsumerConfig.ISOLATION_LEVEL_CONFIG, "read_committed");
		consumer = new KafkaConsumer<>(consumerProps);
		// 事务型生产者初始化
		producer.initTransactions();
		// 消费者订阅主题
		consumer.subscribe(Collections.singleton(inputTopic));


		// 消费消息
		while (true) {
			try {
				ConsumerRecords<Integer, String> records = consumer.poll(Duration.ofMillis(200));
				if (records.count() > 0) {
					// 开始事务
					producer.beginTransaction();
					// 发送消息
					for (ConsumerRecord<Integer, String> record : records) {
						ProducerRecord<Integer, String> customizedRecord = transform(record);
						producer.send(customizedRecord);
					}
					Map<TopicPartition, OffsetAndMetadata> offsets = consumerOffsets();
					// 提交消费偏移量
					producer.sendOffsetsToTransaction(offsets, consumer.groupMetadata());
					// 提交事务
					producer.commitTransaction();
				}
			} catch (ProducerFencedException|InvalidProducerEpochException e) {
				throw new KafkaException(String.format(
					"The transactional.id %s is used by another process", transactionalId));
			} catch (KafkaException e) {
				// 终止事务
				producer.abortTransaction();
				// 
				resetToLastCommittedPositions(consumer);
			}
		}


	}
}
```
