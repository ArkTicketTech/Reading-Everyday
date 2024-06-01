复习下MySQL

buffer pool用于提高性能，一个buffer poll只允许单线程访问。

缓存页大小为16K。

管理：
1. LRU：缓存读管理
2. FREE：空闲页
3. FLUSH：管理脏页（被修改的页）

Redo log：非缓冲池，专门存储redo log，然后每秒罗盘。


checkpoint技术：
主要是在恢复redo log时能知道从哪个点开始恢复（标记点是通过LSN）


Insert buffer：二级非唯一索引的插入缓存，目的是避免对二级索引的直接罗盘，因为随机性比较大。
change buffer实际上是将insert buffer进行了增删改的细分。

binlog的作用：
1. 数据恢复
2. 从库同步

三种格式：
1. statement：记录SQL语句
2. row：记录行记录变更情况
3. mixed：混合

存储结构：表空间 -> 段 -> 区 -> 页 -> row
页是Innodb管理的最小单位


添加和删除索引会阻塞写操作。


