键值对集的分片

键值对是数据的一种最通用、泛化的表示，其他种类数据库都可以转化为键值对表示：

关系型数据库，primary key → row
文档型数据库，document id → document
图数据库，vertex id → vertex props, edge id → edge props
因此，接下来主要针对键值对集合的分区方式，则其他数据库在构建存储层时，可以首先转化为 KV 对，然后进行分区。
