## 认识事务

## 概念

InnoDB的事务完全符合ACID的特性

1. 原子性(Atomicity)：原子
2. 一致性(Consistency)：数据库从一种状态转变为下一种一致的状态，实际上就是某种现实约束。
3. 隔离型(isolation)：并发场景下事务之间不会相互影响
4. 持久性(durability)：事务一旦提交，其结果是永久性的，这里的永久性指的是数据库本身，而不是因为外部的破坏。

## 分类

1. 扁平事务：就是我们普遍认为的事务，要么全部成功，要么全部失败
2. 带有保存点的扁平事务：指定保存点，回滚到保存点，可设置多个保存点
3. 链事务：在提交一个事务后，释放不必要的数据对象，将必要的处理上下文传给下一个要开始的事务。
4. 嵌套事务：

1. 任何子事务都在顶层事务提交后才真正的提交；
2. MySQL并不原生支持，可以通过带有保存点的事务来模拟串形的嵌套事务，但有很多局限性

1. 分布式事务：分布式环境下的扁平事务

# 事务的实现
redo恢复提交事务修改的页操作；

undo回滚行记录到某个特定版本。

## redo
### 基本概念

1. redo log buffer
2. redo log file

redo log保证了事务的持久性，当事务提交时，先将该事务的所有日志写入到redo log file进行持久化。

写到redo log file其实只是写到了操作系统的文件系统缓存中，为了确保redo log写入磁盘，还需要进行一次fsync操作。

innodb_flush_log_at_trx_commit用来控制redo log file 刷新到磁盘的策略，

1. 0：不进行写入redo log file操作
2. 1：每1秒进行一次redo log file的sync操作。
3. 2：事务提交时将redo log 写入file中，但仅写入文件系统的缓存中，不进行fsync操作。

1. MySQL宕机并不会导致数据丢失，操作系统的宕机才会导致数据丢失



默认值为1，只有设置为1才保证ACID的D。



redo log是Innodb引擎层的。

redo log是物理格式，记录的是对每个页的修改。



### log block

InnoDB中，redo log都是以512字节存储的，这意味着redo log buffer, redo log file都是以块的方式存储的，称之为redo log block，由于redo log block的大小和磁盘区大小一样，都是512字节，因此redo log file的写入可以保证原子性，不需要double write技术。

redo log block除了日志本身之外，还有log block header 和 log block tailer。

log buffer 是由log block组成，在内部log buffer就好似一个数组。



### log group

log group为重做日志组，其中有多个重做日志文件。

log group 是一个逻辑上的概念，由多个redo log file 组成，redo log file存储的就是之前在log buffer中保存的log block，因此其也是根据块的方式进行物理存储的管理，每个块的大小与log block一样，同样为512字节。

在innodb运行过程中，log buffer根据一定的规则将内存中的log block刷新到磁盘，这个规则具体为：

1. 当事务提交时
2. 当log buffer中有一半的内存空间已经被使用时
3. log checkpoint时

log file的写入并不是都是顺序的，因为除了追加，还要更新头部前2K的信息，它们是：

1. log file header
2. checkpoint1
3. null
4. checkpoint2

checkpoint写在前面的目的是为了避免因介质失败而导致无法找到可用的checkpoint的情况。

### redo log 格式

1. redo_log_type：类型
2. space：表空间的ID
3. page_no：页的偏移量
4. redo log body

### LSN

log sequence number，日志序列号，占用8字节，单调递增。

LSN表示的含义有：

1. redo log写入的总量：单位为字节。比如一个事务写了100字节的redo log，那么LSN就加100。
2. checkpoint的位置：每个页的头部有一个值FILE_PAGE_LSN，记录了该页的LSN。页中的LSN表示该页刷新时LSN的大小。因为重做日志记录的是每个页的日志，因此页中的LSN用来判断页是否需要进行恢复操作。例如，页P1的LSN为10000，数据库启动时，InnoDB检测到写入redo log中的LSN为13000，并且该事务已经提交，那么数据库需要进行恢复操作，将redo log应用到P1页中。
3. 页的版本：
