# Part II Distributed Data
多台机器提供数据密集型应用时，需要关注：
* Scalability: 单机负载大时，需要将负载分散到其它机器上。
* Fault tolerance / high availability: 当某台机器出现故障时，可使用冗余组件迅速接管。
* Latency: 对用户提供物理距离最近的数据中心进行服务，以降低延时。

## Scaling to Higher Load
shared-memory(scaling up)：垂直扩展机器的 CPU ，主存，磁盘等硬件能力。瓶颈是扩展能力有限，容错能力有限，限制在单一地理单元内。
shared-disk：多台机器有独立的 CPU 和 RAM，但是共享磁盘阵列。服务器和磁盘阵列之间通过高速网络连接。瓶颈时：资源竞争和锁开销限制进一步的扩展能力。

### Shareding-Nothing Architectures
shared-nothing(scaling out)：每台机器成为 node，每个 node 独立使用本地的 CPU, 内存和磁盘，节点之间的所有通信等任务通过网络来实现。

优点：
1. 无特殊硬件限制，用户可以自行选择 price/performance ratio 最好的机器；
2. 可以部署到不同的地理单元中，减少延时；
3. 部署简单。

应用开发者也许要关注 shared-nothing 结构，例如，当数据分布在多个 node 上，需要考虑背后的权衡设计，来达到最佳性能，数据库往往不能把这些复杂性屏蔽起来。