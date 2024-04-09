## 多主复制

只有一个写节点无法适用于大量写负载的系统。

一个想法是增加多个主节点来分担写负载，由于网络延迟，可能会有数据冲突。

一些常见的解决数据冲突方法：

1. 由客户端解决冲突
2. 最后写入胜利
3. 因果关系跟踪



此外还有一种无冲突复制数据类型（CRDT）的数据结构，能够根据一定规则自动解决冲突，副本之间不需要额外的协调和冲突处理。

多主系统的缺点在于它的复杂性，一般用于多中心数据系统。

## 无主复制

另一种复制技术没有主节点，以Dynamo架构为代表。

无主复制的思想是：客户端不仅向一个节点发送写请求，而是将请求发送到多个节点，在某些情况下甚至会发送给所有节点。

无主复制直接去掉了领导者，只要能够满足写入数量的节点可用，系统仍然被认为是正常的。但无主复制面临的冲突也增多，可能产生多个节点的数据不一致。解决方法是读取也跟写请求一样读取所有的节点的数据，来根据版本决定使用哪个值。



如果要对于不一致的数据要进行数据修复，方法有：

1. 读修复：读取时修复
2. 反墒过程：后台进程修复
    1. 使用merkle tree减少数据传输量



关于多少个数量的节点是满足无主复制的要求，Quorum机制是一种算法。

DDIA第五章也讨论了，可以对比阅读