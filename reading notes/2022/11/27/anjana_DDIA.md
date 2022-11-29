# 11.27

“费曼学习法”，还是要自己总结输出啊。

## Chapter 3 数据存储与检索

### storage engines

存储引擎：

* log-structured ～: 日志结构
  * 仅支持追加式更新的log

* page-oriented ～: 面向页 B-tree

the hash table's limitations:

* the hash table must fit in memory：哈希表必须全部放入内存。
* it is difficult to make an on-disk hash map perform well
  * a lot of random access I/O
  * expensive to grow when it becomes full
  * hash collisions require fiddly logic
* Range queries are not efficient：只能采用逐个查找的方式查询每一个键。

### SSTable

SSTable

* Sorted String Table 排序字符串表
* 按key-value对的顺序**按键key**排序

### LSM-Tree

Log-Structured Merge-Tree(LSM-Tree) 日志结构的合并树

LSM存储引擎：基于合并和压缩排序文件原理的存储引擎。

Lucene

* an indexing engine for full-text search used by Elasticsearch and Solr 索引引擎
* key: 单词（词条） 
* value: 所有包含该单词的文档ID的列表（倒排表）

Bloom filters: `memory-efficient` data structure for approximating the content of a set: if a key does not appear in the database.

### B-Tree

* fixed-size blocks/pages

* read/write one page at a time

* individual keys(a leaf page):

  * inline

  * references to the pages where the values can be found

* an additional data structure on a disk: a write-ahead log(WAL,redo log) 预写日志/重做日志（仅支持追加）

分支因子：B-Tree中一个页所包含的子页引用数量

B-Tree: faster for reads

* 先写WAL
* 后修改树本身的页（还可能发生页分裂）

LSM-Tree: faster for writes

### Index

* clustered index: storing `all row data` within the index
* nonclustered index: storing `only references to the data` with the index

### View

* materialized view
* standard(virtual) view
