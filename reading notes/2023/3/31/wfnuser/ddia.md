- ### Hash Indexes  
	- 哈希索引是最简单的KV索引之一；同时也被用于构建许多更为复杂的索引。  
	- 对于我们前面建立的 KV 存储，我们可以建立内存中的 hashmap 作为索引加速查询。  
		- 内存中的哈希表的Key和查询Key保持一致；Value则存对应记录在文件中的Offset。  
		- ![image.png](../assets/image_1680017828848_0.png)  
	- 事实上，Bitcask 就是基于这个简单的思想实现的；在内存中还加了一层对查询结果的缓存。  
		-  
		  > Bitcask offers high-performance reads and writes, subject to the requirement that all the keys fit in the available RAM, since the hash map is kept completely in memory.  

		-  
		  > A storage engine like Bitcask is well suited to situations where the value for each key is updated frequently.  

		- 在所有key都可以存储到内存中时性能非常好  
			- 对于写操作来说，追加写性能很好  
			- 对于读操作来说，内存查询到offset后可以直接对磁盘进行寻址而不需要遍历  
	- 截至目前为止，我们的写入（包括更新和删除）只是不断地 append； 如何避免磁盘空间的不够用呢？  
		- 我们首先对文件进行分片，文件大小达到一定尺寸之后；我们就会将文件变为只读，然后开始写入新的文件。  
		- 随后，我们就可以对多个分片进行压缩操作，即对于重复的键，仅保留最新的一条记录，写入新的文件中。最后，删除老文件即可减少占用的磁盘存储空间。  
		- ![image.png](../assets/image_1680018604654_0.png)  
		- 对于所有的分片中只有一个是活跃可追加的，其余都是不可变的。因此压缩操作和读取操作并不是互斥的；通常压缩是由一个独立的后台进程完成的。  
			-  
			  > Segments are never modified after they have been written, so the merged segment is written to a new file. The merging and compaction of frozen seg‐ ments can be done in a background thread, and while it is going on, we can still con‐ tinue to serve read and write requests as normal, using the old segment files. After the merging process is complete, we switch read requests to using the new merged seg‐ ment instead of the old segments—and then the old segment files can simply be deleted.  

		- ![image.png](../assets/image_1680018744024_0.png)  
	- ![log structure storage engine with hashmap](https://wei-ming.tw/static/77b810dcf665996bd187b758f033410b/d9199/log-structure-storage-with-hash.png){:height 328, :width 654}  
	- 将这个想法应用到生产环境下还需要处理许多问题：  
		- 文件格式：CSV并不是一个高效的存储格式。  
		- 记录删除：由于是append-only的存储结构，无法在文件中删除记录，一般采用标记删除实现。追加特殊的tombstone标记表示某个键值对已经被删除即可。  
		- 宕机恢复：内存中的哈希表在机器重启后会丢失；相比于全表扫描重建哈希表一种更快的方式是在每个文件中持久化对应的索引记录，重启时可更快的恢复。（同样是空间换时间的思想）  
		- 记录不完整：写入过程进行到一半的时候如果机器出现故障，会导致数据不完整。  
		- 并发控制  
	- 哈希索引 + 日志存储的优势：  
		- 顺序写入性能很好  
		- 由于大部分文件是不可变的。因此 crash recovery 和 concurrency 更简单。  
		- 压缩操作的特性使得数据文件不容易碎片化  
	- 哈希索引的限制  
		- 对Key的数量限制很高。 因为 hash table 需要放在内存中； on-disk hash map 性能比较差。  
		- 不支持 range queries  
	- 如何解决上述限制呢？  
	- 有个小疑问：内存中的哈希索引在合并的时候会被修改吧？似乎书中没有提及这个问题。  