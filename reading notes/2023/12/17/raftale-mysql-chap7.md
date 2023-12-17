### 恢复

InnoDB在启动时都会尝试进行恢复操作。因为redo log记录的是物理日志，因此恢复的速度比binlog要快很多，并且InnoDB也通过顺序读取、并行应用做了一定的优化。

由于checkpoint表示已经刷新到磁盘页上的LSN，因为恢复过程中仅需恢复checkpoint开始的日志部分。例如下图，当数据库的checkpoint的LSN为10000时发生宕机，恢复操作仅恢复LSN10000～13000范围内的日志。

InnoDB存储的redo log是物理日志，例如对于Insert操作，其记录的是每个页的变化，对于下面的表：

```sql
create table t(
  a int,
  b int,
  primary key(a),
  key(b)
  );
  insert into t values(1, 2);
```

其redo log大致为：

```
page(2, 3), offset 32, value 1, 2 # 聚簇索引
page(2, 4) offset 64, value 2 # 二级索引
```

redo log是幂等的。

## undo

### 基本概念

事务回滚通过undo，undo存放在数据库内部的一个特殊段中，这个段称为undo 段。undo段位于共享表空间内。

undo回滚的原理是：执行相反的语句，比如回滚insert语句，通过执行delete，对于update，执行一个相反的update。

undo的另一个作用是MVCC，当用户读取一条记录时，若该记录已经被其他事物占有，则通过undo读取之前的版本信息。

undo 也会产生redo log，因为undo log也需要持久化的性质。

### undo存储管理

事务提交后并不能马上删除undo log及undo log所在的页。这是因为可能还有其他事务需要undo log来得到行记录之前的版本（比如可重复读）。故事务提交时将undo log放入一个链表中，是否可以最终删除undo log及undo log所在页由purge线程来判断。

### undo log格式

undo log分为：

1. insert undo log：insert操作中产生的undo log，insert只对事务本身可见，对其他事务不可见，故该undo log可以在事务提交后直接删除，不需要进行purge操作；
2. update undo log：delete和update产生的undo log，可能需要提供MVCC机制，不能在事务提交之后就进行删除，提交后放入undo log链表，等待purge线程进行最后的删除。


### 查看undo信息

delete操作并不直接删除记录，而是将记录标记为已删除，最终的删除在purge操作中；

update主键的操作分为两步：

1. 将原主键记录标记为已删除；
2. 插入一条新的记录；
