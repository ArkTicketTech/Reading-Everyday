#### 2. SSTables and LSM-Trees
Sorted String Table 要求每个键在每个 segment 文件中仅出现一次（compaction 过程保证了），且每个 segment 中按照 key 排序，比上一节的 Hash Index 的优点如下：
1. 合并 segment 更高高效，因为可以使用 merge short。
	* 如果两个 segment 中出现了同样的 key，则保留最近的 segment key 即可。
2. 由于 key 有序，因此在文件中查找特定的 key 时，不需要在内存中保存所有 key 的索引。意味着索引可以是稀疏的，两个 key 之前可以相差数 kb，在两个 key 之前查找时，速度更快。
	* 如果是固定长度的 key 和 value，可以在 segment 文件中做二分查找。但是在通常情况下，key 和 value 都是变长的。
3. 由于读请求通常需要扫描范围内的多个 key-value 对，因此可以将这些记录在写入一个磁盘块之前对其进行压缩，并使用稀疏索引指向压缩块的头部。压缩带来的好处：节省磁盘空间，减少 IO 带宽占用。

##### Constructing and maintaing SSTables
在内存中维护有序的数据结构：红黑树或 AVL 数，支持按任意顺序插入键。
构造一个 SSTables 的基本工作流程：
1. 将写入数据加入到内存平衡数中，通常称其为 memtable。
2. 当 memtable 大小超过限制时，将其作为 SSTable 文件落盘，由于 memtable 维护了有序，因此写入磁盘时较高效。新的 SStable 成为了数据库数据的最新部分。当 SStable 在落盘时，可以新启动一个 memtable，将新的写入写至其中。
3. 当读请求来时，首先在 memtable 中查找 key，其次按照 SSTable 从新至旧来查找目标 Key。
4. merging and compaction 进程可以在后台合并 segment files，丢弃那些被覆盖和删除的值。

上述过程存在的问题：databases crash 时，memtable 还未落盘产生的丢失问题。
解决方法：在磁盘上单独保留写入的日志，每次写入前，都先写该日志（和 WAL 类似），日志仅用于崩溃后的恢复，并不需要按 key 排序。当 memtable 落盘成为 SSTable 以后，相应的日志可以丢弃。

##### Making an LSM-tree out of SStables
LevelDB 和 RocksDB 使用的上述工作原理，其主要给其他上层应用提供 key-value 数据存储。

类似的存储引擎还有 Cassandra 和 HBase，其受 Google Bigtable 启发，Google Bigtable 首次提出了 SStable 和 memtable。

Lucene 是 ElasticSearch 等全文搜索所采用的索引，其也采用了类似的思想来保存。


##### performance optimizations
LSM-Tree 来查找一个不存在的 key 时，很有可能要找遍 memtable 和所有的 SStables。可以使用 bloom filter 来优化。

SStable 压缩和合并有两种方法，分别为大小分级和分层压缩：
* LevelDB 和 RocksDB 采用分层压缩，将较新的和较小的 SStable 被连续合并到单独的旧的层次中，按数的形式来组织。
* HBase 采用大小分级，将较新的和较小的 SStable 被连续合并到 较旧的和较大的 SStables 中。
* Cassandra 支持两种。

数据存储有序，区间查询效率高；并且写入有序，支持高写入吞吐。