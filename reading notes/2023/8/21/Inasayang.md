### CHAPTER 6: DESIGN A KEY-VALUE STORE

键可以是纯文本，也可以是散列值。出于性能考虑，短键效果更好。

-   Plain text key: “last_logged_in_at”
-   Hashed key: 253DDEC4

键值对中的值可以是字符串、列表、对象等。在 Amazon dynamo、Memcached、Redis 等键值存储中，值通常被视为不透明对象。

![](https://inasa.dev/image/systemdesign/06/1.png)

#### Understand the problem and establish design scope

没有完美的设计。每种设计都要在读写和内存使用的权衡中实现特定的平衡。另一个需要权衡的问题是一致性和可用性。

设计一种键值存储，它具有以下特点特性：

-   键值大小小于10KB
-   存储大量数据
-   高可用:  即使出现故障，系统也能快速响应
-   高扩展性：系统可进行扩展，以支持大型数据集
-   自动扩展：应根据流量自动添加/删除服务器
-   可调整的一致性
-   低延迟

#### Single server key-value store

开发驻留在单个服务器中的键值存储很容易。一种直观的方法是将键值对存储在哈希表中，这样就能将所有内容保存在内存中。尽管内存访问速度很快，但由于空间限制，可能无法将所有内容都存储在内存中。要在单个服务器中存储更多数据，有两种优化方法：

-   数据压缩
-   只将常用数据存储在内存中，其余数据存储在磁盘中

即使进行了这些优化，单台服务器的容量也会很快达到极限。要支持大数据，就需要分布式键值存储。



#### Distributed key-value store

分布式键值存储也叫分布式哈希表，它将键值对分布在许多服务器上。在设计分布式系统时，理解 CAP（一致性、可用性、分区容忍性）定理非常重要。

##### CAP theorem

CAP 定理指出，分布式系统不可能同时提供多于两个的 一致性、可用性和分区容错这三项保证中的两项以上。

-   Consistency
    -   一致性意味着所有客户端无论连接到哪个节点，都能在同一时间看到相同的数据
-   Availability
    -   可用性意味着，即使部分节点出现故障，任何请求数据的客户端都能得到响应
-   Partition Tolerance
    -   分区表示两个节点之间的通信中断。分区容错是指尽管出现网络分区，系统仍能继续运行

![](https://inasa.dev/image/systemdesign/06/2.png)

键值存储根据其支持的两个 CAP 特性进行分类：

-   CP (consistency and partition tolerance) systems
    -   CP 键值存储支持一致性和分区容错，但却牺牲了可用性
-   AP (availability and partition tolerance) systems
    -   AP 键值存储支持可用性和分区容错，但会牺牲一致性
-   CA (consistency and availability) systems
    -   CA 键值存储支持一致性和可用性，但牺牲了分区容错性。由于网络故障不可避免，分布式系统必须容忍网络分区。因此，CA 系统不可能存在于真实世界的应用中



 在分布式系统中，数据通常会被多次复制。假设数据复制在三个复制节点 n1、n2 和 n3 上

###### Ideal situation

在理想情况下，网络分区永远不会发生。写入 n1 的数据会自动复制到 n2 和 n3。这样就实现了一致性和可用性。

![](https://inasa.dev/image/systemdesign/06/3.png)

###### Real-world distributed systems

在分布式系统中，分区是无法避免的，当发生分区时，我们必须在一致性和可用性之间做出选择。

n3 出现故障，无法与 n1 和 n2 通信。如果客户向 n1 或 n2 写入数据，数据就无法传播到 n3。如果数据被写入 n3，但尚未传播到 n1 和 n2，那么 n1 和 n2 将拥有过时的数据。

![](https://inasa.dev/image/systemdesign/06/4.png)

如果选择一致性高于可用性（CP 系统），则必须阻止对 n1 和 n2 的所有写操作，以避免这三个服务器之间出现数据不一致，从而导致系统不可用。银行系统通常对一致性要求极高。例如，银行系统必须显示最新的余额信息。如果由于网络分区造成不一致，银行系统会在不一致解决之前返回错误。

但是，如果选择可用性而非一致性（AP 系统），那么系统将继续接受读取，即使它可能返回陈旧的数据。对于写入，n1 和 n2 将继续接受写入，当网络分区解决后，数据将同步到 n3。

选择适合使用情况的 CAP 保证是构建分布式键值存储的重要一步。可以与面试官讨论这个问题，并据此设计系统。

Pp. 87-92