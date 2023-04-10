对于密集写的应用，性能瓶颈很可能在于数据库写入磁盘的速率。写放大的性能成本：存储引擎写入磁盘的次数越多，可用磁盘带宽减少。

通常 LSM-Tree 的写放大发生机率效率 B-Tree：
* LSM-Tree 顺序写 compact SSTable，而 B-Tree 通常情况下随机重写整个页。
* B-Tree 存在磁盘页碎片，而 LSM-Tree 定期写 SSTable 可以消除碎片，存储开销小。

另外，在 SSD 的某些固件上也使用了 log-structed 算法将随机写转换为底层存储芯片上的顺序写。因此，可能数据库存储引擎的写入模式受影响并不明明显。此外，更低的写放大和更少的碎片对于 SSD 来说更加收益，原因是**增加可用内存带宽**。


##### Downside of LSM-Trees
**LSM compaction 时会干扰读写操作**，原因是磁盘资源有限（例如磁盘带宽）。

LSM-Tree 的 compaction 时间并不确定，某些时候 query 的响应时间可能较久；B-Tree 的响应延迟的可预测性更强。

高写吞吐时，磁盘有限的写带宽在 initial write (logging and flushing a memtable to disk) 和 compaction threads running in the background 之间共享。

特别的，write throughout 高但 compaction 阈值未配置好时，compaction 可能跟不上后续的写操作。这种情况下，磁盘上 unmerged segment 持续增加，直到耗尽磁盘空间，读操作由于需要检查更多的 unmerged segment 文件，速度亦会减慢。通常情况下，及时 compaction 操作速度更不上，以 SSTable 为基础的存储引擎不会限制写操作的速率，所以，需要额外的系统来监控这种行为。

**B-Tree 更加适应于强事务语义**：
* B-Tree 特点是每个键都唯一对应于索引中的特定位置，而 LSM-Tree 多个段中可能有相同键的副本。
* 大部分关系型数据库中，事务隔离通常需要给一定范围内的键加锁，在 B-Tree 中，这只需要在树结构中能实现。