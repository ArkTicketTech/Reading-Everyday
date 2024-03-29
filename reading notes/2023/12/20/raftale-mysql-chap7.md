# 事务的隔离级别

SQL标准定义的四个隔离级别为：

1. READ UNCOMMITTED
2. READ COMMITTED
3. REPEATABLE READ
4. SERIALIZABLE



REPEATABLE READ可重复读实际上不解决幻读的问题，

InnoDB实际上在可重复读级别下就通过next key locking解决了快照读的幻读，InnoDB默认隔离级别就是可重复读。但大多数互联网公司会将隔离级别选择为读已提交，试图减小锁的开销来提高并发性能。

SERIALIZABLE串形化的隔离级别下，InnoDB会对每个select语句自动加上Lock in share mode（共享锁）。

# 分布式事务

### MySQL分布式事务

Innodb提供了对XA事务的支持，并通过XA事务来支持分布式事务的实现。

XA是eXtended Architechture的缩写，代表跨异构技术实现两阶段提交的标准。XA 不是一个网络协议 —— 它只是一个用来与事务协调者连接的API。

XA事务允许不同数据库之间的分布式事务，参与全局事务的每个节点都需要支持XA事务。

XA事务由

1. 一个或多个资源管理器：提供访问事务资源的方法，通常一个数据库就是一个资源管理器
2. 一个事务管理器：也称为协调者，协调参与全局事务中的各个事务，需要和参与全局事务的所有资源管理器进行通信
3. 一个应用程序：定义事务的边界，指定全局事务中的操作

组成。



分布式事务使用两阶段提交（two phase commit）的方式：

1. 第一阶段：所有参与全局事务的节点都开始prepare，告诉事务管理器它们准备好提交了；
2. 第二阶段：我事务管理器告诉资源管理器执行rollback还是commit

2PC的特点是prepare后一旦系统承诺提交，那就必须提交，即使崩溃重启后也要尽一切手段提交。



如果协调者在第一阶段后，第二阶段前失效，参与者之间也不会相互沟通，不确定到底是提交还是中止，在2PC的框架下，只能等协调者恢复。协调者恢复将通过崩溃前记录的日志来决定，没有日志则直接中止。

也就是说，如果在第一阶段没有收到事务管理器的确认可以提交的信号 时崩溃，那么这个事务也就是中止了。

进阶阅读：三阶段提交、其他分布式事务。



### 内部XA事务

最为常见的XA事务存在于binlog和innodb之间，事务提交时，先写binlog，再写redo log，对上述两个操作的要求是原子的。

当事务提交时，innodb会先做一个prepare操作，将事务的xid写入，接着进行binlog的写入，此时如果是宕机了，MySQL会检查准备的UXID是否已经提交，没有，则重新提交。当然，binlog写入之前失败的话，事务就不算成功。



# 长事务

执行时间较长的事务

解决方法：长事务转化为小批量事务来进行处理
