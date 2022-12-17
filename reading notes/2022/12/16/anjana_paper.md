## No compromises: distributed transactions with consistency, availability, and performance.

被标题吸引的论文🌚，就如同最近对世界上是否真有完美的人的疑惑（以前不相信有，现在开始怀疑）。

### Abstract

以前system designers对分布式系统事务的妥协

* avoid transactions completely 完全避免事务
* weaken consistency guarantees 弱一致性保证
* single-machine transactions 单机事务：require programmers to partition data 需要程序员对数据分区

FaRM:

* `main memory` distributed computing platform
* provide distributed transactions
* 不妥协的关键：
  * the design of new transaction, replication, and recovery protocols
  * commodity networks with RDMA: Remote Directed Memory Access 远程的直接内存访问DMA（在两个或多个计算机进行通讯时使用DMA）
  * NVM: non-volatile DRAM 非易失性存储

### 1. Introduction

Three principles to address these CPU bottlenecks:

* reducing message counts: 
  * `vertical Paxos` with primary-backup replication
  * `unreplicated coordinators` that communicate directly with primaries and backups.
* using one-sided RDMA reads and writes instead of messages
* exploiting parallelism effectively

Four phase commit protocol (optimistic concurrency control 乐观并发控制):

* lock: eliminate the messages to backups
* validation
* commit backup
* commit primary

`reduces CPU overhead` by using one-sided RDMA operations

* uses no remote CPU
* use no foreground CPU at backups
* lazily truncating logs to update objects in-place: use CPU in the background

## 2. Hardware trends

Lithium-ion n. 锂离子

> **不间断电源**（**UPS**，即 **Uninterruptible Power Supply**）是在电网异常（如停电、欠压、干扰或浪涌（涌浪电流））的情况下不间断的为电器负载设备提供后备交流电源，维持电器正常运作的设备。

`A distributed UPS effectively makes DRAM durable.`

当power failure发生时，distributed UPS 使用`电池能源`将`内存内容`保存到commodity SSD。主要通过两个方面提升了性能：

* 避免 synchronous write同步写入SSD
* 延长了SSD寿命：因为只有failure发生的时候才写入

## 7. Related Work

Spanner: 2f+1 replicas

FaRM: f+1 replicas