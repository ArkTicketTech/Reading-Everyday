# Multi-Paxos

Basic-Paxos只选出一个提案，存在多条日志记录时，只需要多次运行Basic-Paxos来实现状态机复制。

如果每一条日志都通过一个Paxos实例来达成共识，那么每次至少两轮通信。

所以优化后的被称为Multi-Paxos，Multi-Paxos尝试高效地实现状态机复制。

优化点：

1. 引入了Leader角色：只能由Leader发起提案，减少了活锁的概率。
2. 优化Prepare阶段：在没有提案冲突的情况下，优化Prepare阶段，优化成一个阶段。



太复杂了，不深究了，先看Raft，以后有精力再说吧。

# Raft算法

## 系统模型

1. 服务器可能宕机、停止运行，过段时间再恢复，但节点的行为是非恶意的，不会篡改数据
2. 消息可能丢失、延迟、乱序或者重复；
3. 可能有网络分区，并在一段时间之后回复



Raft算法和Multi-Paxos一样是基于领导者的共识算法。

使用领导者算法的优势是：只有一个领导者，逻辑简单；

难点：当领导者发生改变时，系统可能处于不一致的状态，状态机日志也会堆积一些没有批准的日志，因此，当下一任领导者上线时，必须对状态机日志进行清理。



## 基本概念

Raft算法中的服务器在任意时间只能处于以下三种状态之一：

1. 领导者（Leader）：领导者负责处理所有客户端请求和日志复制，同一时刻最多只能有一个正常工作的领导者。
2. 跟随者（Follower）：跟随者完全被动地处理请求，即跟随者不主动发送RPC请求，只响应收到的RPC请求，服务器在大多数情况下处于此状态。
3. 候选者（Candidate）：候选者用来选举出新的领导者，候选者是处于领导者和跟随者之间的暂时状态。

系统正常运行时，只会有一个领导者，其余都是跟随者。



Raft算法选出领导者意味着进入一个新的任期（Term），实际上任期就是一个逻辑时间。

Raft将分布式系统的时间划分为一个一个不同的任期来解决时序问题，每个任期都由一个单调递增的数字来表示任期号。

任期通常分为两部分：领导者选举和正常运行的部分



有些任期内没有选出领导者，如Term3，这时会立即进入下一个任期，再次尝试选出一个领导者。



每个节点需要维护一个持久化的currentTerm变量，表示当前已知的最新任期号。

任期对于Raft来说非常重要，它能帮助Raft识别过期的信息。

最后，Raft中节点通信主要通过两个RPC调用实现：

1. RequestVote RPC：用于领导者选举
2. AppendEntries RPC：被领导者用来复制日志和发送心跳

## 领导者选举

领导者维持领导地位的方法是：向集群中其他跟随者节点周期性的发送心跳包，即空的AppendEntries消息。

如果一个跟随者节点再选举超市时间（一般在100～500ms）内没有收到任期更大的RPC请求，则该节点认为集群中没有领导者，于是开始新一轮选举。

新一轮选举开始后存在两种可能性，

1. 领导者真的挂了，那么所有的节点都不会收到心跳，于是参与投票选出新的领导者
2. 领导者没有挂，只是该节点当前尚未接受到心跳，后续继续接受到心跳，由于领导者并没有真正挂掉，所以新一轮选举不会成功。



选举流程：





如果旧的领导者如果重新上线，它的任期编号肯定没有新的领导者大。