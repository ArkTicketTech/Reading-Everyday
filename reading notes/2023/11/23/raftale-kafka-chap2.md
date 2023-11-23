1. 操作系统：Linux
2. Java环境：Java8+
3. Zookeeper：保存集群的元数据信息和消费者信息，高版本的kafka自带raft协议
4. Kafka broker



# broker配置

## 常规配置

1. broker.id：broker标识符
2. port：端口
3. zookeeper.connect：保存元信息的zookeeper地址，hostname:port/path
4. log.dirs：日志存放的磁盘目录
5. num.recovery.threads.per.data.dir：服务器启动和关闭时的处理线程
6. auto.create.topics.enable：

1. 默认情况下，Kafka会在如下集中情形下自动创建主题：

1. 当一个生产者开始往主题写入消息时
2. 当一个消费者开始从主题读取消息时
3. 当任意一个客户端向主题发送元数据请求时

1. 也可以手动创建或者通过其他配置系统来创建，需要设置为false



## 主题的默认配置

1. num.partitions：指定新创建的主题包含多少分区
2. log.retention.ms：数据被保留多久，默认一周。
3. log.retention.bytes：通过保留的消息字节数来判断消息是否过期
4. log.segment.bytes：日志片段的大小
5. log.segment.ms：日志片段关闭的时间
6. message.max.bytes：限制单个消息的大小


# 硬件的选择

1. 磁盘吞吐量：优先固态硬盘，影响生产者生产速度
2. 磁盘容量：根据实际情况
3. 内存：影响消费者读取速度，如果kafka能单独部署，利用系统的page cache，能显著提升性能
4. 网络：网络吞吐量决定了kafka能处理的最大数据流量
5. CPU：对计算能力要求相对较低



# kafka集群

## 需要多少个broker

1. 从数据存储考虑，需要多少磁盘空间以及单个broker能存储多少空间，还有数据复制系数是多少
2. 集群处理请求的能力

## broker配置

要把一个broker加入到集群里，只需要修改两个配置参数：

1. broker必须配置zookeeper.connect
2. 每个broker都必须为broker.id参数设置唯一的值
