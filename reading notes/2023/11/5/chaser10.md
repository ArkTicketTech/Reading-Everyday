#### BASE原则
1. 基本可用 系统能够基本运行、一致提供服务
2. 软状态 不要求系统能够一直保持强一致
3. 最终一致性 系统需要在某一个时刻后达到一致性要求

#### 原子提交协议
分布式事务中包含的两个最基础的角色就是 
- 协调者
- 参与者

##### 1PC
一个协调者 不断向所有发送提交或放弃事务的请求，直到所有参与者确认已执行玩相应的操作

**有什么问题？**
**一个节点放弃事务，协调者无法感知  执行和放弃事务单纯由协调者控制**

##### 2PC
1. **prepare阶段**  任意一个参与者发起分布式事务（一般由协调者发起），并执行本地事务但不提交，从working进入prepared状态 向协调者发送prepare消息
2. 收到参与者发来的prepare消息 协调者状态转变为preparing并询问所有节点是否可以提交
3. 收到询问消息的参与者，执行本地事务但不提交，进入prepared状态，然后投赞同票 如果本地事务没有执行成功 投反对票

1. **commit 阶段** 协调者收集所有投票 如果所有投票都是赞同票，表示没有故障发生，然后给所有参与者发送提交消息，要求参与者提交本地事务
2. 如果有一个反对票 就要放弃掉，发送放弃消息给所有参与者要求参与者回滚本地事务
3. 投了赞同票的参与者 阻塞等待协调者发送来的消息，如果接收到的提交消息 就提交本地事务反之就回滚

缺陷
1. 同步阻塞 参与者要锁定占有的本地资源 直到协调者发来提交或者回滚消息
2. 单点故障 协调者会出事 协调者出事了 参与者将一直阻塞收不到消息从而一直占用资源
3. 数据不一致  协调者给参与者发送的提交消息会因为网络原因阻塞导致没有提交本地事务成功

##### 3PC
1. cancommit阶段 协调者询问所有协调者是否可以执行事务
2. precommit阶段 协调者收到所有协调者的YES那么要求所有参与者执行事务，执行完了给协调者回复ack，如果有一个**NO或者等待超时**那么将发送中止事务消息给所有参与者
3. docommit阶段 协调者收到了所有ACK，向所有参与者发送提交事务信号，如果没有收到所有的ack那么就发送中止信号给所有参与者。**如果参与者超时没有收到消息就提交事务**

最后参与者超时没有收到消息是提交事务而不是回滚 是基于概率，因为他收到了前两次的消息了 所以提交的概率很大

3PC较之2PC多了超时控制 还有一轮预执行


### 共识算法
分布式系统想要状态保持一致，最重要的是对多个事件，操作序列的顺序进行共识
共识算法需要保证
1. safety 保证结果是对的，不会出现错误情况 一次执行**只有一个最终决议**，被多数接受的结果就是决议
2. liveness 决议要在有限的时间中产生

#### paxos
**基础概念**
1. value 提案值，一项操作，例如将xx key更新为xx value
2. number 提案编号，全局唯一，单调递增
3. proposal 集群需要达成共识的提案，提案=value+编号
paxos中三种角色
1. proposer 生成提案编号n和值v，然后向acceptors广播提案，接收acceptors的回复，如果有过半acceptors同意提案则选定提案，提案选定后通知所有的learner是学习最终选定的提案值，类似于raft中的leader
2. acceptor 接收proposer的提案并参与提案决策过程。
3. learner 不参与决策过程，只学习最终选定的提案值

一个节点可以充当多种角色，一个节点可以既是proposer又是acceptor，还可以是learner  还能允许有多个proposer 

*一个Acceptor必须接受它收到的第一个提案*
*只有当过半acceptors接受一个提案，才能最终定下该提案*

paxos基本命题
p1 在一次paxos流程中，如果一个值为v的提案被选定了，那么后续任何被最终选定的带有更大编号的提案中的value中也必须是v

p2 在一次paxos流程中，如果一个值为v的提案被选定了，那么后续任何被acceptor接受的带有更大编号的提案中的value中也必须是v

p3 在一次paxos流程中，如果一个值为v的提案被选定了，那么后续任何被proposer提议的带有更大编号的提案中的value中也必须是v

为了实现以上命题需要满足如下约束
对于一个被proposer提议的提案中任意的v和n，存在一个数量超过半数acceptors的集合s，满足以下两个条件中任意一个
- s中任何一个acceptor都没有接受过编号小于n的提案
- s中所有的acceptors接受过的最大编号的提案的value为v

**算法流程**
1. proposer 获取一个proposal id n，为了保证这个id可比较大小以及唯一 使用时间戳+id生成
2. proposer向所有acceptors广播prepare n请求
3. acceptor比较n和minproposal，minproposal是n至少应该为多少，如果n > minproposal， minproposal = n 并且将acceptproposal 和acceptvalue，也就是以及accept并持久化到accpetor本地最大的提案
4. proposer收到过半回复，发现有acceptedvalue返回，则将所有回复中的最大的acceptedvalue作为本次提案的value
5. 进入propose阶段 广播accept(n,value)到所有节点
6. accpetor比较n和minproposal， 如果n>=minProposal，则acceptedProposal=minProposal=n，acceptedValue=value，本地持久化后，返回；否则，返回minProposal。
7. 提议者接收到过半数请求后，如果发现有返回值result >n，表示有更新的提议，跳转到1；否则value达成一致。

一次paxos流程
阶段1
proposer生成一个全局唯一且递增的id n，时间戳+机器id
向所有acceptors广播prepare请求
acceptor收到prepare请求中的id 判断id是否大于大于自己接受过的id 如果大于则做出两个承诺一个应答
1. 承诺不再接收id号小于等于n的prepare请求
2. 承诺不在接收id号小于n的accept请求
3. 应答自己已经accept的最大id+value 如果没有就返回空

阶段2
如果proposer在发送prepare请求收到了过半的回应，则在所有应答中挑选id最大的那个value，如果大家应答的都是空 那么proposer可以自己指定value
然后将自己的id n+value广播给所有的acceptors，acceptors判断是否会违背承诺，如果违背承诺也就是此时本地接收的id已经大于n了，表明有**更新的提议**则返回本地id
得到半数的acceptors应答后，并且没有返回比n更大的id，那么决议形成复制给learner

#### mutil-paxos
basic-paxos是对单个值进行共识，至此需要两轮 往返四次prepare，accept从爱能基于一项提案达成共识，不限制proposer数量容易导致活锁，也就是两个proposer交替prepare，完成阶段一后其他proposer提交了更高的id，永远没有accept的机会

mutil-paxos做了两点改进
1. 多paxos实例，针对每一个需要完成共识的单值都运行一次basic paxos
2. 选举单一的leader proposer：选举出一个leader proposer，所有提案只能由leader proposer发起，因为不存在提案竞争的情况，也可以省略prepare阶段了，两阶段变成一个阶段，大大减少网络io  可以通过一次basic paxos选举出来leader
3. 选出来的leader必须要把leader节点的日志补全
4. 如果出现脑裂就退化成basic paxos 那么还是需要二阶段