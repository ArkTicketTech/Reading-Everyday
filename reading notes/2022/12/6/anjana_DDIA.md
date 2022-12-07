### Index

* clustered index: storing `all row data` within the index 在索引中直接存储行数据
* nonclustered index: storing `only references to the data` with the index 仅存储索引中数据的引用

### View

* materialized view： 查询结果的实际副本，并被写到磁盘。
* standard(virtual) view：编写查询的快捷方式。从虚拟存储中读取时，SQL引擎将其动态拓展到视图的底层查询，然后处理拓展查询。

OLTP(Online Transaction Processing)

* log-structured school: BitCask, SSTable, LSM-Tree, LevelDB, Cassandra, HBase, Lucene
* update-in-place school: B-Tree(RDB, NoSQL)