复习下ddia。

# chap1
设计一个系统软件需要考虑的有：
1. 可靠性
2. 可伸缩性
3. 可维护性

# chap2
数据模型：
1. 关系模型：SQL，平铺型，很难直观的展示1对多的关系
2. 文档模型：擅长一对多
3. 图模型：多对多



# chap3 
存储引擎两大类：
1. log-structured
2. page-oriented

索引是一种从原始数据中构建辅助查询的数据结构，本质上是以空间换时间的思想。

1. Hash indexes：hash table需要放入到内存中，对数据总量有限制，且不支持范围查询
2. SSTable：有序的哈希表，对哈希表的优化。
   - 合并时更高效；
   - 磁盘有序的情况下，就可以在内存中建立稀疏索引，提高内存的利用率。
   - 如何维护写操作：用简单的平衡查找树，比如AVL
3. LSM Tree：层级SSTable，对SSTable合并时的优化。
4. B-Tree：MySQL支持的，平衡查找树，可原地修改数据。
   - BTree读取更快，LSM写更快

列式存储：每一列的值存储在一起，按列分析数据时更高效。





