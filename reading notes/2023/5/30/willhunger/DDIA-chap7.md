#### 3. Preventing Lost Updates
写丢失：某个事务从数据库读某些值并做相关修改，然后写回新值（read-modify-write）。当前有两个事务在同样的数据对象上执行类似操作，由于隔离性，第二个写操作并不包括第一个事务修改后的值，会导致第一个事务的修改值可能丢失。

发生场景：
* 递增计数器，更新账户余额
* 对某负载对象的一部分内容执行修改，例如对 JSON 文档中一个列表添加新元素
* 两个用户同时编辑 wiki 页面，并将整个页面发送至数据库

##### Atomic write operation
原子操作避免在应用层进行 read-modify-write 操作。类似这样的 sql 在大多数数据库中都是并发安全的：
`UPDATE bar SET value = value + 1 WHERE key = 'foo'`

原子操作通常采用对读取对象加独占锁的方式来实现，在被更新提交之前，其它事务不能读取。该技术称为游标稳定性。另一种实现方式是强制所有的原子操作都在单线程上执行。

但某些 ORM 框架并不一定产生 read-modify-write SQL，导致数据库无法进行原子操作，产生难以发现的错误。

##### Explicit locking
使用 for update 指令让数据库对所有返回行进行加锁。

```SQL
BEGIN TRANSACTION;

SELECT * FROM figures  
WHERE name = 'robot' AND game_id = 222 FOR UPDATE;

-- Check whether move is valid, then update the position 
-- of the piece that was returned by the previous SELECT. 
UPDATE figures SET position = 'c4' WHERE id = 1234;

COMMIT;
```

##### Automatically detecting lost updates
先让这类事务并发执行，如果事务管理器检测到了更新丢失风险，则中止当前事务，并强制回退到安全的 read-modify-write  mode。

优点：数据库可以借助快照隔离级别来执行检查。PG 的可重复读，Oracle 的可串行化，SQL Server 的快照级别隔离等都支持自动检查更新丢失，并中止违规的事务。但 MySQL / InnoDB 并不支持。

更新丢失监测，可以建少应用层的相关工作。

##### Compare and  set
Compare and  set ：上次读取的数据没有发生变化才允许更新。

##### Conflict resolution and replication
对于多副本数据库，需要考虑多节点上的数据库副本，不同的节点可能会并发修改数据。

相关解决措施：

* Lock 和 compare and set 方式的前提是只有一个最新的数据副本。但是对与多主节点和无主节点的多副本数据库，由于其支持多个并发写，所以会出现多个最新的数据副本，加锁和原子比较方式不再适用。

* 允许冲突，保留多个冲突版本，并由应用层来解决冲突。

* 如果操作可交换（顺序无关，在不同副本上以不同的顺序执行时仍然可以得到相同的结果），则该原子操作在多副本情况下也可以工作。例如计数器递增或者向集合中添加元素。这也是Riak 2.0 的设计思路。
* last write win 冲突解决方式，大多数多副本数据库默认选择该方式。