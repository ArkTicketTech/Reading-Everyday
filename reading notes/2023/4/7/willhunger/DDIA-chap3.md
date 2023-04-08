#### 4. Comparing B-Trees and LSM-Trees
通常情况下，LSM-Tree 写操作更快，B-Tree 上的读操作更快。
LSM-Tree 上的读操作更慢时因为需要在不同几种数据结构和 SSTables。

具体的性能对比还是取决于特定的 workload。

##### Advantage LSM-Tree 
B-Tree index write 操作至少有两次写数据：一次为 WAL，一次为 tree page（还有可能发生分裂）。即使 tree page 中只有几个字节更改，也必须承担写整个页的开销。此外，某些存储引擎会对相同的页覆盖两次，以避免在电源故障的情况下出现不一致的页。

Log-Structed index 在进行 compaction 和 merging SSTables 也可能会写入多次。在数据库中，一次数据库写入请求导致多次磁盘写，成为写放大。特别的，SSD 需要关注写放大指标，因为其只能承受有限次擦除覆盖。