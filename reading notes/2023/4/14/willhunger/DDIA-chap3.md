##### Keeping everything in memory
in-memory databases

1. 特殊硬件（例如 battery-powered RAM），写日志至磁盘，写周期性 snapshot 至从盘，复制内存状态至其它机器。
2. in-memory databases 和 disk-based databases 最主要的区别是在于，前者不需要强制将数据写至磁盘中，避免磁盘IO的开销。
3. in-memory databases 的另外一个优势是：可以实现磁盘索引难以实现的数据模型。例如：Redis 的内存数据结构。
4. anti-caching aaproach 优化 in-memory databases。将不需要访问的数据写至磁盘，需要使用时才载入内存。