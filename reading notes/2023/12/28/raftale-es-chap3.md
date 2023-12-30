分片大小不能在运行过程中修改，但还是有个方法：re-index，这个后续再讲。

## nodes and cluster

副本一定不能放在同一机器上，防止单点故障。


## node roles

Node roles and their responsibilities：

1. master node：It's primary responsibility is cluster management

1. 负责创建、删除索引，集群管理工作，不参与CRUD操作

1. data node：Responsible for document persistence and retrieval

1. 负责查询、删除等相关的文档操作，与磁盘打交道比较多

1. Ingest node： responsible for the transformation of data via pipeline ingestion before indexing

1.

1. Maching learning node：handles machine learning jobs and requests
2. Transform node：handles transformations requests
3. Coordination node：this role is the default role. it takes care of incoming client's requests



角色配置：

当我们启动一个节点时，节点默认为master，data，ingest。我们可以根据我们的需求配置，比如一个集群20个节点，3个节点为master，15个为data，2个为ingest。

配置在`/config/elasticsearch.yml`文件中，`node.role=[master, data, ingest, ml]`。

不管是否配置， 每个节点都默认coordinator 角色。

如果`node.role=[]`，意味着该节点只有coordinator角色，好处是它只需要接受请求、收集结果、负责负载均衡。

# Inverted Index

倒排索引实际上就是建立了token和文档之间的映射关系，并记录了token的频次，通过token查询时，自然就能查询到相关文档。



# Relevancy

relevancy is a positive floating-point number that determines the ranking of the search results.

Elasticsearch默认使用了BM25相关性算法来评分结果。

# Relevancy algorithms

| similarity algorithm               | type            | descripinfo                                                  |
| ---------------------------------- | --------------- | ------------------------------------------------------------ |
| Okapi BM25 (default)               | BM25            | An enhanced TF/IDF algorithm that considers field length in addition to term and document frequencies |
| Divergence from Randomness (DFR)   | DFR             | Uses the DFR framework developed by its authors Amati and Rijsbergen |
| Divergence from Independence (DFI) | DFI             |                                                              |
| LM Dirichlet                       | LMDirichlet     |                                                              |
| LM Jelinek-Mercer                  | LMJelinekMercer |                                                              |
| Manual scripting                   |                 | creates a manual script                                      |
| Boolean similarity                 | boolean         | Does not consider ranking factors unless the query criteria is satisfied or not |

## Okapi BM25

Okapi BM25算法中，以下三点决定了结果的相关性：

1. TF：term-frequency

1. 搜索词在当前文档中出现的频次，频次越高，相关性越高
1. IDF：inverse document frequency

1. the inverse of document frequency，频次的倒数
2. 搜索词在整个索引中出现的频次，频次越高，说明该搜索词含常见，意味着其相关性就没那么高

1. field length norm

1. 当分数相同时，字段的长度越短说明相关性越高

# Routing algorithm

路由算法：

```
shard_number = hash(id) % number_of_shards
```

意味着改变分片数，路由会出现问题。如果实在要改变分片数，可以使用reindex 数据。

Reindexing意味着：

1. 创建一个新的索引
2. 将数据复制到新的索引

# Scaling

1. Scaling up：垂直扩容

1. 需要停机

1. Scaling out：水平扩容
