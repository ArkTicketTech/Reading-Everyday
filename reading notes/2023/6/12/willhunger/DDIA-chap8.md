## Chap8 The Trouble with Distributed Systems

现实情况中的错误情况的复杂性更加严重，所有可能出错的事情一定会出错。

分布式系统与单机系统的区别显著，分布式系统更可能碰到各种故障。

### Faults and Partial Failures
单机软件通常以一种确定性的方式运行：要么工作，要么出错；不会出现模棱两可的现象。
计算机设计的一个选择：如果发生了某种内部错误，宁愿使计算机全部崩溃，而不是返回一个错误的结果，错误的结果往往更难处理。

当涉及到多节点分布式系统时，情况更加混乱，理想情况下的标准正确模型不再适用。

In my limited experience I’ve dealt with long-lived network partitions in a single data center (DC), PDU [power distribution unit] failures, switch failures, accidental power cycles of whole racks, whole-DC backbone failures, whole-DC power failures, and a hypoglycemic driver smashing his Ford pickup truck into a DC’s HVAC [heating, ven‐ tilation, and air conditioning] system. And I’m not even an ops guy.

分布式系统会出现系统的一部分工作正常，但其他某些部分出现难以预测的故障，即 "部分失效" 的情况。该问题的难点在于：部分失效不确定。例如，网络发送消息延迟导致的不确定。

#### 1. Cloud computing and supercomputing
两种大规模计算系统：
	* 高性能计算：计算密集型任务
	* 云计算：多租户数据中心， 通用计算机， 用 IP 以太网链接， 弹性／按需资源分配， 并按需计费。
	* 传统企业数据中心则位于以上两个极端中间。

高性能计算，通常对定期对任务状态进行快照，然后保存在持久存储上，当某节点出现故障，解决方案是简单停止整个集群的任务，等故障节点修复以后，从最近的快照检查点继续执行。因此，高性能计算更像是一个单节点系统，其将局部失效升级为整体失效。

互联网服务系统和高性能计算的不同之处：
* 互联网服务一般是在线的，为用户提供低延迟服务，停止集群来处理故障，是不可取的。
* 高性能计募通常采用专用硬件， 每个节点的可靠性很高， 节点间主要通过共享内存或者远程内存直接访问(RDMA)等技术进行通信。 而云计算中的节点多是由通用机器构建，故障率较高。
* 大型数据中心网络通常基于IP和以太网， 采用 Clos 拓扑结构提供等分带宽。 高性能计算则通常特定的网络拓扑结构， 例如多维网格和toruses , 它们可以为 HPC 特定工作负载提供了更好的性能。
* 系统越大， 其中局部组件失效的概率就越大。 在长时间运行期间， 失效， 修复，再失效可以看成一个反复的过程。不能将大量时间花费在错误修复而不是正常的任务执行。
* 如果系统可以容忍某些失败的节点， 而使整体继续工作， 则对系统运维帮助极大。例如滚动升级。
* 对千全球分散部署的多数据中心（使用户访问地理靠近的数据中心， 从而降低延迟）， 通信很可能经由广域网， 与本地网络相比， 速度更慢且更加不可靠。 而高性能计算通常假设所有节点位置靠近、 紧密连接。