# building the blocks

重要的组成部分：documents, indices,  shards, replicas。

## removal of types

在版本5.x以前，Elasticsearch允许用户在单个索引中索引多种type的文档，也就是说，汽车索引可以包含cars、performancesCars、caritems等，虽然将与汽车相关的模型放在一个地方是个好办法，但是有一个限制。

数据库中表的列是相互独立的，但Elasticsearch并非如此，因为底层lucene支持索引级别的字段管理，也就说不同的type如果使用相同名称的字段，那么该字段的类型必须是一样的。


版本6.x开始索引只能支持一个type，7.x则移除了type。

Es中的Index就相当于关系型数据库中的表。


## Index

index是文档在逻辑上的集合，索引同样有分片（shards）和副本（replica），分片和副本就不再解释了。


分片大小一旦定义就不能改变，但副本个数可以修改。

分片可以分为主分片和副分片。



每个索引都有mappings、settings和aliases这样的属性

1. mapping：定义schema
2. settings：配置shards和replicas
3. aliases：可替换的名称

有些属性比如分片的数量，一旦确定就不能更改。如果创建索引模版，那么新建的索引将会跟随模版变化。




## shards and replicas

shards是Lucene的实例，支持创建倒排索引，管理查询。分片会创建多个segment 文件来存储文档。


replicas用来做冗余，但也能支持查询，这与kafka中的replicas似乎是不一样的，为什么有区别呢？



集群状态：

1. red：not all shards are assigned and ready(cluster being prepared state)

1. 一般是集群开始启动时的瞬时状态

1. yellow：shards are assigned and ready but replicas aren't assigned and ready
2. green：shards and replicas are all assigned and ready


获取集群状态：`GET _cluster/health`


节点崩溃后重平衡：


分片大小设置：

业界的最佳做法是调整单个分片的大小不超过 50 GB。但也有例外，GitHub 的索引分布在 128 个分片中，每个分片 120 GB。一般来说是保持在 25 GB 到 40 GB 之间，如果我们知道电影索引在某个时候最多可容纳 500 GB 数据，建议将此数据分布在分布在多个节点的 10 到 20 个分片中。


分片路由算法：

```
shard_number = hash(document_id) % number_of_primary_shards
```

因为路由算法是根据分片数量来的，所以分片数量不能修改。


如果节点崩溃了，挂掉的分片节点会自平衡到其他节点中。
