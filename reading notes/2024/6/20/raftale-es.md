复习下ES

针对非结构性数据，非结构数据是自由化无组织的格式，因为其模式太自由，所以不容易搜索。
比如mysql就需要定义schema， ES基于Lucence构建， 写入数据前可以不预定义数据格式，也可以预先定义以更好的支持精确查询。

架构：
Elasticsearch是构建在Lucene上的，Lucene是一个高性能的，全文搜索的library。

索引index是文档在逻辑上的集合，索引同样有分片和副本。
分片大小一旦确定，不能轻易变更，因为分片路由是根据分片数来决定的。


节点角色：
1. master：创建、删除索引，集群管理等
2. data node：负责CRUD
3. ingest：数据前置转换
4. Coordination：默认，每个节点都可以处理请求

节点默认为master、data、ingest、Coordination。

倒排索引：实际上就是建立了token和文档之间的映射关系，并记录token的频次，通过token可以查询到文档。

相关性算法：默认Okapi BM25算法。

