##### Dynamic partitioning
如果分区的数据量超过了某个参数阈值，则拆分为两个分区，每个分区承担一般的数据量；如果大量数据被删除，则于相邻分区合并。（类似于 B-Tree 的合并和分裂）

使用该方式的数据库：HBase 和 RethinkDB。

优点：分区数量自动适配数据总量，适用于  key range partitioning ，hash-partitioning 两种分区方式。

缺点：一个空数据库，刚开始写入时，所有的写入操作全部在一个节点上进行，导致其它节点处于空闲状态。为了缓解该问题，HBase 和 MongoDB 支持在一个空数据库上配置一组初始分区（pre-spliting）。


##### Partitioning proportionally to nodes
上述两种情况，分区数量仅和数据量大小成正比，实际上和节点数无关。

使分区数和集群节点数成正比关系。每个节点具有固定数量的分区。当一个节点加入集群时，其随机选择固定数量的现有分区进行分裂，并拿走这些分区的一半数据，另外一半数据留在原节点。

使用该方式的数据库：Cassandra, Ketama。

局限：仅适用于采取 hash-partitioning 分区方式的数据库。