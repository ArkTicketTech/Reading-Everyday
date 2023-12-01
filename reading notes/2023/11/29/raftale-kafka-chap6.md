# 复制

每个主题被分为若干个分区，每个分区有多个副本。

1. leader副本：为了保证一致性，所有的生产者和消费者请求都经过leader副本。
2. follower副本：为了保持高可用，唯一的任务就是复制leader副本。

1. 持续请求得到最新消息的副本被称为同步的副本。只有同步副本才可能被选举为新的leader副本。
2. 如果follower副本没有在10s（该参数由replica.lag.time.max.ms 控制）内请求最新的数据，那就会被认为不是同步副本。

# 处理请求

所有的请求都包含一个标准的请求头

1. Request Type
2. Request version
3. Corrlelation ID：标识请求消息的ID，可以追踪整个链条
4. client id：标识客户端的ID


一旦请求被放到request queue，I/O 线程将会自动处理它们。最常见的客户端请求包括：

1. produce requests：Sent by producers and contain messages the clients write to Kafka brokers
2. fetch requests：Sent by consumers and follower replicas when they read messages from Kafka brokers
3. admin requests：Sent by admin clients when performing metadata operations such as creating and deleting topics

produce requests和fetch requests都会发送到leader replica，如果broker收到的请求中实际请求的分区不在这个broker，客户端将会收到错误。

客户端如何知道请求该发往哪个broker？客户端使用另一种称为元数据请求的请求类型，获取分区、主题等信息。元数据请求可以发送任何broker，每个broker都存储了元数据信息。通过元数据请求，客户端可以知道该发往哪个broker，并且客户端会缓存该信息，并定时的更新数据，更新时间由max.age.ms来控制。






## 生产请求

1. acks=0：生产者无需等待
2. acks=1：leader副本收到消息被认为写入成功
3. acks=all：所有同步副本收到消息才算写入成功

## 获取请求


kafka使用零copy向客户端发送消息，也就是说，kafka直接把消息从文件（Linux文件系统缓存）发送到网络通道，而不需要经过任何中间缓冲区。

有些数据库会在将数据发送到客户端之前，先把它们保存在本地缓存里。

客户端可以设置broker返回数据的上限和下限。

1. 设置下限可以减少CPU和网络开销
2. 设置上限可以防止内存耗尽



消费者只能请求已经写入所有同步副本的消息

## 其他请求

broker之间的通信

# 物理存储

kafka的基本存储单元是分区副本，分区无法在多个broker之间进行再细分，也无法在同一个broker的多个磁盘上进行再细分。

kafka使用了一个专门用于存储分区的目录清单，可通过log.dirs参数进行配置。

kafka是如何使用这些目录来进行配置的，如何管理的？

## 分层存储（Tiered Storage）

kafka如何对broker进行分区的分配。

基本的算法是轮询，先轮询leader副本的分区，再轮询follower副本的分区。

为broker分配分区并没有考虑可用空间和工作负载的问题，但在将分区分配到磁盘上会考虑分区数量，不过不考虑分区大小。所以要注意磁盘空间。

## 分区的分配

分配算法：round-robin 算法决定leaders在broker的位置。

round-robin 是一种轮询调度算法，非常简单，即每次轮询服务器 `(i + 1) mod n`，选出第i台服务器。


## 文件管理

kafka不会永久保留数据，也不会等待所有消费者都读取了消息才删除消息，kafka会为主题配置了数据保留期限，要么是保存期限，要么是保存的数据量大小。

因为在一个大文件里查找和删除消息是很费时的，也很容易出错，索引分区被分成若干个片段(segment)，默认是情况下，每个段包含1个G或者一周的数据。当kafka往分区里写数据时，如果segment的存储达到上限，就会关闭该文件，并打开一个新文件。

正在写入的segment被称为active segment，active segment永远不会删除。

## 文件格式

每个segment被存储在单独的数据文件中，在文件中存储了kafka的消息和偏移量。保存到磁盘的文件格式与生产者send和消费者consume的文件格式是一致的。

因为使用了相同的消息格式进行存储和网络传输，所以kafka使用了zero-copy的优化，可以直接将消息发送给消费者，不需要再保存在内存中。

## 索引

kafka允许消费者从任意偏移量读取消息，这意味着kafka需要快速定位到指定的偏移量。

为了帮助broker更快地定位到执行的偏移量，kafka为每个分区维护了一个索引，索引把偏移量映射到segment files and postitions within the file。

同样的, kafka还有二级索引来映射时间戳和消息偏移量，当用时间戳来搜索消息就可以保证很快。

索引也被分解为segment，但旧的数据被删除后，旧的索引也没有保留的必要

## compaction and How compaction works

因为是基于日志的存储结构，数据的压缩其实跟基于日志的数据是类似的，这部分内容在DDIA中讲的很详细了，同一个key，最终只会保留最新的数据。key为null的数据是无法进行compaction的。



开启compaction配置：log.cleaner.enabled



## Deleted events

kafka客户端有一个方法：deleteRecords

## When are topics compacted?
