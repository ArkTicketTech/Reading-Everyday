#### 5. Other Index Structures
二级索引同样也可以根据 key-value 来构建。其和主键索引的区别在于其键不是唯一的，其实现方式有两种：
* 使得索引中的每个值成为匹配行标识符的列表（类似于全文索引中的 position list）。
* 追加一个 row identifier 使得每个键变得唯一。

##### Storing values within the index
heap file：索引 key 和 value 不存储在一起，value 无序存放在文件中（maybe append only，也可能维护 deleted rows 列表用于后续的 overwrite）。
heap file 的优势：多个二级索引存在时，避免 duplicating data。
更新 heap file 中 value 的两种情况：
1. when the size of new value is leas than old value，在 heap file 中的特定位置直接 overwtire 即可。
2. 相反，可能需要将 new value 的值写至 heap file 的其它空闲区域中，然后更新该 key 的所以索引的指向。

cluster index：将 row 和 index 存储在一起。在 MySQL innodb 中，表的主键始终是聚集缩影，二级索引则指向主键。在 SQLServer 中，可以为每个表指定一个聚集索引。 

在 cluster index 和 noncluster index 之间的一个 compromise ：covering index or index with included columns，索引中存储表的特定列，这种情况下，可以只通过访问索引则可完成某些简单查询。

索引带来的问题：
* 额外的存储，增加了写入开销
* 需要额外的工作来保证事务，避免应用程序感知 inconsistencies。

##### multi-column indexes
concatenated index ：
* 将多个字段有序组织成一个键。
* 级联索引存在局限性，例如，其无法查找满足第一个字段的所有行。

multi-dimensional index：
* 更适用于时空数据。例如查询经纬度范围内的数据。标准 B-Tree 和 LSM-Tree 无法高效处理这种查询。
* 一种方法是：适用 space-filling curve 将二维位置转换为某个数字，然后使用常规的 B-Tree 进行索引组织。例如：PostGIS 使用 PostgreSQL's Generalized Search Tree indexing 实现了 R-Tree。

##### Full-text search and fuzzy indexes
Lucene 全文索引：
* Lucene 使用 SSTable-like 结构存储 tern dictionary，内存中的索引是键中字符的有限状态机，类似于 Trie Tree。这个自动机可以转换为 Levenshtein automaton，其支持在给定编辑距离内高效搜索单词。