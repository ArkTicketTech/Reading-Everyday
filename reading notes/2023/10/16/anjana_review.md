clustered index: storing `all row data` within the index 在索引中直接存储行数据

- MySQL的InnoDB存储引擎中，表的`主键`始终是`聚集索引`，`二级索引`引用`主键`（而不是堆文件位置）

nonclustered index: storing `only references to the data` with the index 仅存储索引中数据的引用

多列索引（级联索引）：

key: `表的多个列`/`文档的多个字段`组合成一个键

内存数据库更快的原因：

- 并不是它们不需要从磁盘读取
- 是因为它们避免使用`写磁盘的格式`对`内存数据结构编码`的开销

