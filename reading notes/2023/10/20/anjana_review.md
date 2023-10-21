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



