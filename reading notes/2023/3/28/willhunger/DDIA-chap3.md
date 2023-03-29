#### 1. Hash Index
最简单的 hash index 就是在内存中维护一个 hash map，该哈希表的 key 在内存中，value 则指向磁盘文件地址的偏移量。

Bitcask（Riak 的默认存储引擎）也采用这样的做法，由于 Bitcask 默认将所有 key 放入内存中，故其可以提供高性能的读和写； value 直接存储在磁盘中即可，代价是至少一次磁盘 IO。

Bitcask 的适用场景：每个 key 的 value 频繁更新的场景。写操作多且不同的 key 少，每个 key 的写操作多，能将所有 key 保存在内存中。

**Compaction**
若只追加到一个文件中，如何避免耗尽磁盘空间？将日志分解为一定大小的段，当文件达到一定大小时就关闭它，并在这些段上进行 compaction 操作，compaction 操作会丢弃日志中重复的键，并且只保留每个键最近的更新。

另外，由于 compaction 往往能使段变得更小，可以在执行压缩的的时候将多个段合并在一起。由于段在写入后不会更新，因此可以将合并的段写入一个新的文件中。merging 和 compaction 的操作可以在后台线程中实现，一旦完成 compaction，后续的读操作直接读新的文件即可。

每个 segment 都维护了自己的内存 hash table，记录着 key 在文件中的偏移量。查找 key 时，首先检查最新 segment 的 hash map，若没找到则去找次新的 segment。由于 compaction 使得 segment 数量较少，查找过程不需要检查太多的 hash map。


实现问题：
1. File format：binary string 文件更加紧凑，由于 CSV 格式。
2. Deleting records：tombstone 删除标志。
3. crash recovery：
	* 数据库重启后，需要扫描所有的 segment 并重建内存 hash map；如果 segment 文件大，恢复耗时较久。
	* Bitcask 将所有 segment 的 hash map 的 snapshot 存放在磁盘上，在恢复时速度更快。
4. Partially written records：可能在 append record 到 segment 过程中崩溃了，Bitcask 文件包含了 checksum，可以监测到损坏的文件。
5. Concurrency Control：写入是以严格的先后顺序追加到日志中的，只有一个写线程；数据文件时仅可追加的，并且不可变，故可以同时被多个线程读取。

Append-only log 看起来似乎浪费空间，为何不以新值覆盖旧值，原因如下：
1. append 和 segment merging 主要是顺序写，比随机写速度更快；
2. segment 如果是 append-only 或 immutable 的话，concurrency 和 crash recovery 更加简单。
3. 合并 old segment 可以避免随时间推移而导致的数据文件出现碎片化问题。

Hash index 的局限性：
1. 哈希表全部放入内存，支持的 key 数量有限。可以将哈希表放入磁盘，但需要大量的随机 IO，速度慢，且需要处理哈希冲突。
2. 区间查询效率低，在碰到区间查询时，只能逐个查询。