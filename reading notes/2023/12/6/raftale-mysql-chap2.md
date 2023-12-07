# 体系架构

InnoDB存储引擎有多个内存块，这些内存快组成了一个大的内存池。负责：

1. 维护所有进程/线程需要访问的多个内部数据结构
2. 缓存磁盘上的数据，方便快速地读取，同时在对磁盘文件的数据修改之前在这里缓存
3. redo log缓冲
4. ...


后台线程的主要作用：

1. 刷新内存池中的数据
2. 保证缓冲池中的内存缓存的最近的数据
3. 修改的数据文件刷新到磁盘文件
4. 数据异常下，InnoDB能恢复到正常状态



## 后台线程

InnoDB存储引擎是多线程的模型，后台有多个不同的后台线程，负责处理不同的任务

1. Master Thread：主要负责将缓冲池中的数据异步刷新到磁盘，保证数据的一致性，包括脏页的刷新、合并插入缓存、UNDO页的回收等。
2. IO Thread：InnoDB引擎大量使用了AIO（Async IO）来处理写IO请求，IO Thread的工作主要是负责这些IO请求的回调处理。
3. Purge Thread：事务被提交后，undolog可能不再需要，需要PurgeThread来回收已经使用并分配的undo页。
4. Page Cleaner Thread：刷新脏页

## 内存

### innodb_buffer_pool

InnoDB存储引擎是基于磁盘存储的，并将其中的记录按照页的方式进行管理。由于CPU速度和磁盘速度之间的鸿沟，基于磁盘的数据库系统通常使用缓冲池技术来提高数据库的整体性能。

缓冲池简单来说就是一块内存区域，通过内存的速度来弥补磁盘速度较慢对数据库性能的影响。

1. 读操作：在数据库中进行读取页的操作，首先将从磁盘读取到的页存放到缓冲池中，下一次再读取相同的页时，首先判断该页是否在缓冲池中，若在缓冲池中，称该页在缓冲池中被命中，直接读取该页。否则，读取磁盘上的页。
2. 写操作：首先修改缓冲池中的页，然后再以一定的频率刷新到磁盘上。刷新到磁盘的机制是一种称为checkpoint的机制。

缓冲池的大小直接影响数据库的整体性能，配置参数为innodb_buffer_pool_size来设置。



缓冲池缓存的数据页类型有：

1. 索引页
2. 数据页
3. undo页
4. 插入缓冲
5. 自适应哈希索引
6. InnoDB存储的锁信息
7. 数据字典信息




缓冲池实例可以配置多个，每个页根据哈希值平均分配到不同缓冲池实例中，这样做的好处是减少数据库内部的资源竞争，增加数据库的并发处理能力。

配置参数为innodb_buffer_pool_instances

\#观察每个缓冲池实例运行的状态

show variables like 'innodb_buffer_pool_instances'



buffer pool是一片连续的内存，其中默认的缓存页大小和磁盘默认的页大小是一样的，都是16KB，为了更好的管理Buffer Pool中的缓存页，每个缓存页都有一个对应的控制块，其中包含了该页所属的表空间编号、页号、缓冲页在Buffer Pool中的地址、链表节点信息、一些锁信息以及LSN信息等。




### LRU List

通常来说，缓冲池是通过LRU算法来对内存区域进行管理的，即最频繁使用的页在LRU列表的前端，最少使用的页在尾端，当缓冲池不能存放新读取到的页时，将首先释放LRU列表中尾端的页。

InnoDB存储引擎中，缓冲池页的大小默认为16KB，同样使用LRU算法对缓冲池进行管理，略有区别的是它对传统的LRU算法做了一些优化。

1. LRU列表加了midpoint位置，新读取到的页，不放到列表首部，而是放到midpoint位置；
2. midpoint位置默认在5/8处，midpoint之前的列表称为new列表，之后的列表称为old列表。

1. innodb_old_blocks_pct = 40 控制old区域占比为40%

1. 为什么不将新读取的页放到LRU的首部呢？因为某些SQl操作如索引或数据的扫描操作，这类操作需要访问表中的许多页，甚至是全部的页，而这些页通常来说仅在某次查询中需要，并不是活跃的热点数据。如果在midpoint位置的第一次和最后一次的间隔小于innodb_old_blocks_time，那么就将midpoint位置的页加入到LRU列表的热端。



执行`show engine innodb status`，其中有个参数：Buffer pool hit，表示缓冲池的命中率，如果小于95%说明可能有全表扫描引起的LRU列表被污染的问题。

还可以通过下面的语句来观察缓冲池的运行状态：

```sql
select * from information_schema.INNODB_BUFFER_POOL_STATS
```

### Free List




Free列表实际上就是把空闲的缓存页对应的控制块作为节点放到了一个链表中。

当数据库刚启动的时候，LRU列表是空的，这时页都放在Free列表中，当加载磁盘需要从缓冲池中分配页时，首先从Free列表中查找是否有可用的空闲页，若有就将该页从Free列表中删除，放入到LRU List中。



### Flush List

当LRU列表中的页被修改后，称该页为脏页（dirty page），即缓冲池中的页和磁盘上的页的数据产生了不一致。脏页即存在LRU列表，也存在Flush列表，LRU列表用来管理缓冲池页的可用性，Flush列表用来管理将页刷回磁盘，两者互不影响。



如何查看脏页：

```sql
select * from information_schema.innodb_buffer_page_lru where OLDEST_MODIFICATION > 0
```



### redo log缓冲

除了缓冲池外，还有redo log buffer。InnoDB将redo log先放到这个redo log buffer中，然后按照一定频率将其刷新到redo log file。一般情况下每1秒就会刷一次。



### 额外的内存池

没看懂



# checkpoint技术

buffer pool的设计目的是为了协调CPU速度与磁盘速度的鸿沟，因此页的操作首先都是在buffer pool完成的。

如果每次脏页的变化时都将脏页刷新到磁盘，那么性能开销会非常大，同时如果还没有刷新到磁盘前发生了宕机，那么数据就不能恢复了。

为了避免数据丢失，当前数据库普遍采用了Write Ahead Log策略，当事务提交时，先写redo log，再修改页。如果发生宕机，数据可以从redo log中进行恢复。

假如崩溃后要从redo log中恢复数据，肯定不是从redo log的头部开始恢复数据，如果redo log非常大，那么数据库恢复的时间可能非常长。事实上也不需要，因为脏页会在某个时刻刷盘。



checkpoint技术的目的主要就是在恢复redo log时可以从某个确定的点开始恢复。

当数据库发生宕机时，数据库不需要重做所有的日志，因为checkpoint之前的页都已经刷新回磁盘。

数据库只需对checkpoint后的redo log进行恢复。



此外，checkpoint还有两种场景：

1. 当缓冲池不够用时，LRU算法会溢出最近最少使用的页，若此页为脏页，那么需要强制执行checkpoint。
2. redo log实际上是循环使用的，当日志满之后，新的日志数据会不断覆盖旧的日志数据。如果某部分redo log对应的数据已经刷盘，那么恢复时并不需要该log，被覆盖时就没问题，但如果redo log的对应数据还没有刷盘，覆盖时就需要强制产生checkpoint，将缓冲池的页刷新到当前redo log的位置。刷新了checkpoint后，该位置的redo log被覆盖就没有任何问题了。



checkpoint有两种：

1. sharp checkpoint：数据库关闭时将所有的脏页都刷新回磁盘
2. fuzzy checkpoint：

1. Master Thread Checkpoint：以一定频率将一定比例的脏页刷新回磁盘
2. Flush_LRU_List checkpoint：InnoDB需要保证一定的空闲页（Free List）始终可供使用，空闲页数量由参数innodb_lru_scan_depth，默认1024.

1. 如果低于这个数，那么InnoDB会将LRU列表尾端的页移除来获得空闲页。
2. 如果移除的页中有脏页，那么就需要进行checkpoint。

1. Async/Sync Flush Checkpoint：指的是当redo log文件不可用的情况，需要强制将一些脏页刷新回磁盘。

1. 某位置redo log不可用就是指对应的脏页还没有刷新到磁盘
2. 保证了redo log的循环使用的可用性

1. Dirty Page too much Checkpoint：脏页数量太多，导致InnoDB存储引擎强制进行checkpoint，由参数innodb_max_dirty_pages_pct控制
