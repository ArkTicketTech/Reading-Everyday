## Chapter 7 Transaction事务

### 快照隔离级别与可重复读

异常现象：nonrepeatable read/read skew

采用快照隔离的例子：PostgreSQL, MySQL的InnoDB, Oracle, SQL Server

实现机制：MVCC(Multi-Version Concurrency Control)

2PL(two-phase locking):

* 可以保证串行化
* 性能差且无法拓展

弱隔离（非串行化）级别：

* 性能不错
* 容易引发各种边界条件
  * 更新丢失
  * 写倾斜
  * 幻读

最近新的算法：SSI(Serializable Snapshot Isolation)

To be continued...