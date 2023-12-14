# 锁的算法

行锁的3种算法：

1. Record Lock ：单个行记录上的锁
2. Gap Lock：间隙锁，锁定一个范围，但不包含记录本身
3. Next-Key Lock：Gap Lock + Record Lock，锁定一个范围，并且锁定记录本身



Record Lock总是会锁住索引记录；

Next-Key Lock：在可重复读的隔离级别下，采用该锁定算法，对于行的查询都是采用这种锁定算法。如果有一个索引有`[10, 11, 13, 20]`，那么该索引可能被Next-Key Locking的区间为：`(-infinity, 10], (10, 11], (11, 13], (13, 20], (20, +infinity)`

当事务T1已经通过Next-Key Locking锁定了如下范围`(10, 11], (11, 13]`，当插入新的记录12时，则锁定的范围会变成`(10, 11], (11, 12], (12, 13]`。

锁定的不是单个值，而是一个范围，是谓词锁predict lock的一种改进。

谓词锁是指锁住满足某一查询条件的所有数据项

当查询的索引含有唯一属性时，InnoDB会对Next-Key Lock进行优化，将其降级为`Record Lock`。



若是辅助索引，锁定的还是范围。

```sql
create table z (
  a int, 
  b int,
  primary key(a),
  key(b)
);
insert into z values
(1, 1),
(3, 1),
(5, 3),
(7, 6),
(10, 8)
```

b是辅助索引，若执行下列的SQL语句：

```sql
select * from z where b = 3 for update
```

该SQL语句通过索引列b进行查询，因此其使用传统的Next-Key Locking技术加锁，并且由于有两个索引，其需要分别进行锁定。

对于聚簇索引，其仅对列a=5定索引加上Record Lock；

对于辅助索引，其加上的是Next-Key Lock，锁定的范围是(1, 3)，另外InnoDB还会对辅助索引下一个键值加上gap lock，即还有一个辅助索引范围为(3, 6)的锁。因此，如果新会话中运行下面的SQL语句，都会被阻塞：

```sql
select * from z where a = 5 lock in share mode;
insert into z values(4, 2)
insert into z values(6, 5)
```

`insert into z values(8, 6)`则不会进行阻塞。

从上面的例子中可以看出，gap lock的作用是为了阻止多个事务将记录插入到同一范围，避免幻读问题的产生。

例如在上面的例子中，会话A已经锁定了b=3的记录，若此时没有gap lock锁定(3, 6)，那么用户仍然可以插入b=3的记录（虽然b=3的记录被锁了，但是可以插入相同的数据），就会导致会话A种的用户再次执行同样的查询会返回不同的记录，这就是幻读。



用户可以通过以下两种方式来显式地关闭Gap Lock：

1. 事务隔离级别设置为读已提交
2. 将参数innodb_lock_unsafe_for_binlog设置为1

1. 不会启用gap lock

### 解决幻读问题

参考：[https://gaoooyh.github.io/2021-09-28-MySQL-InnoDB-RR(%E5%8F%AF%E9%87%8D%E5%A4%8D%E8%AF%BB)%E9%9A%94%E7%A6%BB%E7%BA%A7%E5%88%AB%E8%83%BD%E5%90%A6%E8%A7%A3%E5%86%B3%E5%B9%BB%E8%AF%BB](https://gaoooyh.github.io/2021-09-28-MySQL-InnoDB-RR(可重复读)隔离级别能否解决幻读)

幻读问题是指：在同一事务下，连续执行两次相同的SQL语句可能导致不同的结果，第二次SQL语句可能返回之前不存在的行。

幻读的例子：A事务读取B事务**已提交新增**的数据。

隔离级别：可重复读；读已提交可以复现幻读。

| 会话1                           | 会话2                                        |
| ------------------------------- | -------------------------------------------- |
| begin                           |                                              |
| select * from user where id > 2 | begin                                        |
|                                 | insert into user(`id`, `age`) values(11, 9); |
|                                 | commit                                       |
| select * from user where id > 2 |                                              |
| commit                          |                                              |

由于会话2插入了一个新的值，两次读取的数据就不一样了，会话1会比第一次读取多读到一条数据。

MVCC避免了这个幻读，无法复现。这是因为在可重复读下，普通的查询是快照读，是不会看到别的事务插入的数据的。for update/update/insert/delete这些语句都会查询最新版本的数据，被认为是当前读。



当前读下是会造成幻读问题的，比如下面的例子：

| 会话1                                   | 会话2                                        |
| --------------------------------------- | -------------------------------------------- |
| begin                                   |                                              |
| select * from user where id > 2         | begin                                        |
|                                         | insert into user(`id`, `age`) values(11, 9); |
|                                         | commit                                       |
| update user set age = 10 where id = 11; |                                              |
| select * from user where id > 2         |                                              |
| commit                                  |                                              |

会发现id=11的age不再是9，而是10，会话1的第二次select * from user where id > 2也查询出来了id=11的值。两次查询返回了之前不存在的行，这是因为前一句update更新了快照读。



Innodb在当前读下采用next-key locking的算法避免幻读，如果select * from t where a > 2 for update，那么就会锁住(2, +infinity)这个范围，这个范围的插入都是不被允许的，插入语句会阻塞，超时会报错：

| 会话1                                      | 会话2                                                |
| ------------------------------------------ | ---------------------------------------------------- |
| begin                                      |                                                      |
| select * from user where id > 2 for update | begin                                                |
|                                            | insert into user(`id`, `age`) values(11, 9); // 阻塞 |
|                                            | commit                                               |
| update user set age = 10 where id = 11;    |                                                      |
| select * from user where id > 2 for update |                                                      |
| commit                                     |                                                      |

```
Lock wait timeout exceeded; try restarting transaction
```



next-key locking锁的是索引，而不是数据本身，如果update的where条件没有用到索引列，那么就会全表扫描，就会对所有记录加上next-key锁（间隙锁 + 记录锁），相当于锁全表，需要等到事务提交后才会释放，这是非常危险的，所以切记update的更新条件一定是要走索引的。

这是例子：

```sql
drop table user;

CREATE TABLE `user` (
  `id` int unsigned NOT NULL AUTO_INCREMENT,
  `age` int not null,
  `money` int not null,
  PRIMARY KEY (`id`),
  key(`age`)
) ENGINE=InnoDB ;

insert into user (`id`, `age`, `money`) values
(1, 1, 3),
(3, 1, 5),
(5, 3, 7), 
(7, 6, 11), 
(10,8, 15)
```

| 会话1                                                        | 会话2                                                        |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| begin                                                        |                                                              |
| select * from user where money > 5 and money < 15 for update | begin                                                        |
|                                                              | insert into user(`id`,`age`,`money`) values(11, 9,20); //阻塞 |
|                                                              | commit                                                       |
| select * from user where money > 5 and money < 15 for update |                                                              |
| commit                                                       |                                                              |

money=20不在（5, 15）范围内，不应该阻塞的，但这里却是被阻塞了，当给money字段加上索引后，就不会阻塞了。



总结：

1. 如果事务中只有快照读，InnoDB下可重复读隔离级别下是不会出现幻读的；

1. 读已提交下还是会有的

1. 如果事务中有当前读，根据当前读的时机：

1. 如果会话A一开始就当前读，那就直接阻塞会话B的修改
2. 如果会话A在会话B插入新数据后使用update/for update/insert之类的当前读，后续select会出现幻读

1. innodb可重复读解决幻读的手段就是Next-Key Locking + 显式加排他锁（for update），由于幻读的定义并不是完整的，很多人对幻读的定义不一样，可重复读是否解决了幻读有争议，这里我们认为可重复读解决了幻读。
2. Next-Key Locking实际上就是间隙锁 + 记录锁。

# 锁问题

通过锁定机制可以实现事务的隔离性要求，使得事务可以并发地工作。锁提高了并发，但也带来了三种问题：

1. 脏读：读已提交解决
2. 不可重复读：可重复读解决
3. 丢失更新：

## 脏读

脏读：一个事务可以读到另一个事务中未提交的数据。读已提交及以上的隔离级别解决了脏读。

## 不可重复读

书中定义：

1. 不可重复读：同一个事务中多次读取同一数据集合，由于另一个事务的修改提交，导致两次读到的数据是不一样的；
2. 幻读：同一事务中，连续执行两次同样的SQL语句导致不同的结果。

不可重复读和幻读貌似没什么区别，网上的博客都是用幻读是由于新增数据导致的，不可重复读是修改已有的数据来区分的，但书中并没有如此区分。



InnoDB存储引擎中，通过使用next-key Lock算法来避免不可重复读问题，mysql的官方文档将不可重复读的问题定义为幻读问题。意思是不可重复读是幻读的一个子集，感觉没有精准的定义，就不纠结了。

next-key lock算法下，不仅是锁住扫描到的索引，还锁住这些索引覆盖的范围，在这个范围内的插入都是不允许的。

## 丢失更新

丢失更新是指一个事务的更新操作会被另一个事务的更新所覆盖：

1. 事务A将行记录r更新为v1，但是事务A并未提交；
2. 事务B将行记录r更新为v2，但是事务B未提交；
3. 事务A提交；
4. 事务B提交。

但MySQL数据库层面不会出现上面的更新丢失问题，因为即使是读未提交的事务隔离级别，至少都是行级别的锁粒度，在步骤2就被阻塞住了。这也是数据库原子性的体现。

但丢失更新的情况很可能在程序中出现，因为这本质上是并发问题，很可能出现两个用户同时编辑wiki页面，其中一个用户覆盖另一个用户修改的内容，read-modify-write产生竞态条件。

要避免更新丢失，就需要让操作变得串形化，如果读取是加上一个排他X锁，for update，就可以实现串形化，但这会影响并发性，所以锁的粒度需要控制好。

DDIA中对丢失更新解决方案有：

1. 原子写：串形化
2. 显式锁定：相当于加锁
3. 自动检测丢失的更新
4. 比较并设置：CAS

可串形化没有丢失更新问题。

# 阻塞

锁之间在竞争时，阻塞就会发生，阻塞时间并不是无限的，参数innodb_lock_wait_timeout控制阻塞最长等待的时间，默认50s。

innodb_rollback_on_timeout设定是否在等待超时时对进行的事务进行回滚操作，默认OFF，代表不回滚。

| 会话1                                                        | 会话2                                                        |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| begin                                                        |                                                              |
| select * from user where money > 5 and money < 15 for update | begin                                                        |
|                                                              | insert into user(`id`,`age`,`money`) values(11, 9,20); //阻塞 |
|                                                              | commit                                                       |
| select * from user where money > 5 and money < 15 for update |                                                              |
| commit                                                       |                                                              |

默认情况下，InnoDB不会回滚超时引发的错误异常，大部分情况下都不会对异常进行回滚。

# 死锁

## 概念

死锁是两个或两以上的事务在执行过程中，因争夺锁资源而造成一种互相等待的现象。

解决死锁问最简单的方式不要有等待，将任何的等待都转化为回滚，并且事务重新开始。

当前数据库普遍采用wait-for graph的方式来进行死锁检测，若检测出存在死锁，InnoDB选择回滚undo量最小的事务。

wait-for graph要求保存以下两种信息：

1. 锁的信息链表
2. 事务等待链表

## 死锁的概率

一个事务发生死锁的概率：

1. 系统中事务的数量（n）越多，发生死锁概率越大
2. 每个事务操作的数量（r）越多，发生死锁概率越大
3. 操作数据的集合R越小，发生死锁的概率越大

## 死锁示例

| 会话1                                                        | 会话2                                  |
| ------------------------------------------------------------ | -------------------------------------- |
| begin                                                        |                                        |
| select * from t where a = 1 for update                       | begin                                  |
|                                                              | select * from t where a = 2 for update |
| select * from t where a = 2 for update // Deadlock found when trying to get lock; try restarting transaction |                                        |
|                                                              | select * from t where a = 1 for update |
|                                                              |                                        |

上面发生了死锁，会话2执行第二个查询时，会话1事务立刻抛出死锁异常。

InnoDB发现死锁后，会马上回滚一个事务。

# 锁升级

有些数据库会存在锁升级，比如microsoft SQL Server，可能把表的1000个行锁升级为一个页锁，或者将页锁升级为表锁。

InnoDB不存在锁升级，因为其不是根据每个记录来产生行锁的，相反，其根据每个事务访问的页对锁进行管理，采用的是位图的方式，因此不管一个事务锁住一个记录还是多个记录，其开销同城都是一致的。
