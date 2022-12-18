## Chapter 6: Partitioning数据分区

### partitions/sharding 分区/分片

MongoDB, Elasticsearch, SolrCloud: shard

HBase: region

Bigtable: tablet

Cassandra, Riak: vnode

Couchbase: vBucket

编程语言内置的hash函数可能并不适合分区：同一个key在不同的process中可能返回不同的value

* Java的Object.hashcode
* Ruby的Object#hash

### 分区方法

* 基于关键字区间的分区
* 哈希分区

### 二级索引的分区

* 基于分档的分区
  * 本地索引
  * 写入只更新一个分区
  * 分散scatter/聚集gather：读取二级索引时
* 基于词条的分区
  * 以待查找的关键字本身作为索引：基于索引值
  * 读取：单个分区
  * 写入：更新二级索引的多个分区

### 请求路由

服务发现

分布式数据系统：Zookeeper跟踪集群范围内的元数据

