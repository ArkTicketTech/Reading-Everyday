# 原子提交

原子性涉及了硬件和软件，常见的机械硬盘一般可以保证512字节的原子写。

单机事务：常见的方法是使用日志或者WAL这类技术，简单来说，先将操作的元数据写入一个单独的日志文件，同时还有表示操作是否完成的标记。

分布式事务的原子性是通过原子提交协议实现的，原子提交协议满足三个特性：

1. Aggrement：
2. Validity：
3. Termination：



原子提交协议实际上解决了分布式共识问题的一个子类：即对事务的提交或中止达成共识。



## 两阶段提交

Two-Phase Commit， 2PC

两阶段提交的基本思想是，既然仅仅发送一个请求不足以知道其他节点是否成功提交，那么最直接的想法就是再增加一轮请求，先检查每个节点上的状态是否能够满足事务正确性，再进行事务操作。

两阶段提交包含两个角色：

1. 协调者：负责协调算法的各个阶段
2. 参与者：参与到事务中执行事务操作



两个阶段：

1. 准备/投票阶段：协调者发送准备消息，并等待参与者响应；参与者响应请求，执行操作，并记录操作日志，执行成功则返回「是」请求，执行失败则返回「否」请求
2. 提交阶段：协调者向所有参与者发送提交或中止消息，并等待参与者响应；提交或中止事务，记录操作日志，并返回响应，协调者确认响应。



无论是协调者还是参与者，都有可能在任何阶段发生故障。

1. 第一阶段：参与者在回复协调者之前发生了故障；
    1. 解决方法：协调者超时等待，超时默认中止，并在稍后重试；

1. 第一阶段，协调者发送准备请求后立即发生故障，那么参与者将阻塞，直到协调者恢复后才知道本次事务要提交还是中止。

    1. 后果：参与者的数据库还锁定了事务相关资源，可能会导致系统停顿；

    1. 只有恢复协调者，所以两阶段提交是一种阻塞提交算法

1. 第二阶段，协调者是发送了一部分提交消息，此时发生了网络分区，导致部分参与者没有接受到提交消息。





## 三阶段提交

两阶段提交中，协调者发生故障会导致阻塞。

三阶段就是为了解决阻塞而设计的。

它相当于是将准备阶段（两阶段中的第一阶段）拆成了两份，增加了一轮简单的询问环节。


第一阶段canCommit：

1. 协调者向参与者发送prepare消息，询问参与者是否准备好执行事务，并等待响应；
2. 参与者判断是否具备执行事务的条件，并不执行操作；如果具备条件，则返回「是」，反之返回「否」

第二阶段preCommit：

1. 协调者向参与者发送预提交消息，询问参与者是否可以执行并提交事务，并等待响应
2. 参与者接受到消息后，执行事务操作，并记录操作日志，执行成功则返回「是」
3. 协调者将投票结果发送给所有参与者

第三阶段doCommit：

1. 协调者向所有参与者发送提交消息
2. 参与者收到消息后提交事务，并返回响应
3. 参与者响应有为「否」时，协调者发送中止消息



超时机制：

1. canCommit阶段超时中止事务
2. preCommit阶段超时默认执行事务



三阶段的优势在于：

1. 三阶段提交是非阻塞协议，即使协调者发生故障，参与者仍然会选择新的协调者来推进事务的执行
2. 减少了事务资源锁定范围：增加canCommit阶段，排除了一些因为网络或者自身原因的非必要的失败。
3. 降低了同步阻塞：超时机制

三阶段并没有完全解决二阶段的问题，它增加的可用性是以正确性为代价的。3PC很容易受到网络分区的影响。



不足在于：

1. 增加了一轮消息，增加了复杂度和协商效率
2. 超时机制可能带来数据不一致



3PC应用场景比较少。



## Paxos提交算法



## 基于Quorum的提交



## Sega事务

完整实现分布式事务原子性的代价是比较高的，通常需要多轮消息才能决议出整个系统的行为，此外事务操作涉及到一些资源占用，过长的等待会影响系统吞吐量。

有一类特殊的事务，称为长活事务，持续时间通常以小时或者天为单位。

这类事务的原子性保证使用Sega来实现，默认系统可用性较高。

核心原理就是将事务拆分成很多子事务，每个子事务都有一个补偿事务，若需要回滚则执行补偿事务。



# 并发控制



并发控制是一种隔离并发事务以保证数据正确性的机制。

并发控制分为三类：

1. 悲观并发控制：加锁
2. 乐观并发控制：CAS + 重试
3. 多版本并发控制：快照隔离

## 两阶段锁

Two-Pahse Locking，2PL。

悲观并发控制。

两阶段锁的策略：通过先获取锁再访问数据。

锁类型：读写锁，读不排斥写，写排斥读和写。

两阶段锁可能造成死锁，解决死锁的方法有：

1. 避免死锁：有序加锁；行级锁
2. 死锁预防：发现获取的资源被锁住就尝试中止占有资源的事务，被中止的事务稍后进行重试
   1. 等待-死亡：一个事务申请另一个事务已经获取的锁时，如果持有锁的事务年轻，那么申请锁的事务等待；如果持有锁的事务年老，那么申请锁的事务退避并且死亡
   2. 伤害-死亡：一个事务申请另一个事务已经获取的锁时，如果持有锁的事务年轻，那么申请锁的事务伤害持有锁的事务，请求它死亡；如果持有锁的事务年老，那么申请锁的事务等待

1. 死锁检测

