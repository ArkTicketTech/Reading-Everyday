### 4. 文档数据库与关系数据库的融合
#### 数据查询语言
声明式查询语言，命令式查询语言：
* 执行顺序
* 查询结果的顺序
* 并发执行

MapReduce 查询
map和reduce函数对于可执行的操作有所限制。 它们必须是纯函数， 这意味着只能使用传递进去的数据作为输入， 而不能执行额外的数据库查询， 也不能有任何副作用。