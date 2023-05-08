## Chap6 Partitioning
分区在不同系统的称呼不同，例如：
* MongoDB, Elasticsearch and SolrCloud  -> Shard
* HBase -> region
* Bigtable -> tablet
* Cassandra and Rick -> vnode
* CounchBase -> vBucket

分区的目的：提升可扩展性，数据分散在不同 node 的磁盘上，查询负载也可也分散到更多 node 上。

###  Partioning and Replication

Partitioning 通常和 Replication 一同使用，每个分区在多个节点上都存有副本，以提高系统容错性。

### Partitioning of Key-Value Data
分区的主要目标：将数据和查询负载均匀分布在所有节点上，期望获得可扩展性。

如果分区后存在数据倾斜，极端情况下，所有的负载都集中在一个分区节点上，这会导致该分区节点成为系统热点。

避免热点的最简单方法：将数据记录随机分配到所有节点。该方法能均匀分布数据，但是读取特定的数据时，并不清楚数据存储在哪个节点上，不得不查询所有节点。

#### 1. Partitioning by Key Range
为每个分区分配一段连续的 key 或者 key range with minimal and maximum value，这能最优化 range query。

使用该分区方式的应用：BigTable, HBase, RethinkDB, 2.4 版本之前的 MongoDB。

缺点：某些访问模式会导致热点，如果是时间戳为 key，则分区对应的是一段时间范围，会导致写入集中在某一分区，在写入时会导致该分区负载过高，而其它分区处于空闲状态。

避免该热点问题的方法：在时间戳前面加入一个前缀。

#### 2. Partitioning by Hash of Key
好的哈希函数可以处理数据倾斜并使其均匀分布。

使用该分区方式的应用：Cassandra 和 MongoDB 使用 MD5，Voldemort 使用 Fowler-Noll-Vo 函数。另外，编程语言的内置哈希函数可能存在问题，例如，同一个 key 可能在不同进程中返回不同的哈希值。

缺点：丧失了 range query 的能力。在 MongoDB 中，如果启用了基于哈希的分片模式，则区间查询会发送到所有的分区上，而 Riak, CouchBase 和 Voldemong 不支持关键字的区间查询。

Cassandra 则在分区策略上做了一个这种，Cassandra 的表可以声明为多个列的复合主键，复合主键的第一部分用于 Hash 分区，其它列用于组合索引来对 Cassandra SSTable 中的数据进行排序。因此，岂不支持在第一列上进行区间查询。

#### 3. Skewed Workloads and Relieving Hot Spots
基于哈希的分区方法可以减轻热点，但是无法避免。一个极端情况是：所有的读写操作都针对同一关键字，则所有请求都会被路由至同一个分区。

大多数系统无法消除这种高度复杂的倾斜，只能由应用层来减轻这种倾斜程度。

一个例子：如果某个关键字成为热点，在该关键字的开头或结尾添加一个随机数，将数据分配到不同的分区上。缺点：后续的读取需要读取多个分区。
因此，只针对少量热点关键字添加随机数才有意义，写入吞吐量低的绝大多数关键字，会带来额外开销，以及还需要额外的元数据来标注哪些关键字进行了特殊处理。

目前大多数数据系统无法自动处理这些倾斜的负载，仍需应用开发者自己根据负载进行权衡。