## Insert Buffer

由于B+树的特性，进行插入操作时，如果二级索引不是有序的，就意味着插入需要离散的访问索引页，很可能插入的索引页并没有在缓冲池中，并且由于随机读取磁盘的存在导致了插入操作性能下降。



InnoDB存储引擎开创性地设计了Insert Buffer，避免了插入式的磁盘IO。

1. 对于非聚簇索引的插入或更新操作，不是每一次直接插入到索引页中，而是先判断插入的非聚簇索引页是否在缓冲池中，则直接插入；
2. 若不在，则先放入到一个Insert Buffer对象中，假装非聚族索引已经插入到叶子结点
3. 然后再以一定频率进行Insert Buffer与索引叶子结点进行合并，或者未来读取索引叶子结点再与Insert buffer进行合并。
4. 这样做的好处是能将多个插入合并到一个操作中，然后再进行页的落盘，避免IO消耗。



使用Insert Buffer需要满足两个条件：

1. 索引是二级索引
2. 索引不是唯一的



但需要考虑一种极端情况，如果还没有落盘数据库就发生了宕机，如何恢复？

因为还是在buffer中，写buffer就会写redo log，所以崩溃了也可以从redo log中恢复。

但需要注意的是，如果buffer中插入的数据很多，恢复的时间也会很长

二级索引不能是唯一的，因为在插入缓存时，数据库不会去查找索引页来判断记录的唯一性，如果去查了，就会随机读取磁盘，导致insert buffer失去意义。

但InnoDB存在一个问题是：在写密集的情况下，插入缓存会占用过多的缓冲池内存innodb_buffer_pool，默认最大可以占到1/2的缓冲池内存。



### change buffer

1.0版本后开始引入了Change Buffer，可以将其视为Insert Buffer的升级。InnoDB可以对DML操作 - Insert有Insert Buffer，delete有Delete Buffer，update有Purge buffer。

对一条记录进行update操作可能分为两个过程，

1. 将记录标记为已删除：Delete Buffer
2. 真正将记录进行删除：Purge Buffer



配置参数innodb_change_buffering，可配置参数有：

1. inserts
2. deletes
3. purges
4. changes：启用inserts和deletes
5. all：启用所有
6. none：都不启用

默认为all

innodb_change_buffer_max_size：控制Change Buffer最大使用那次的百分比



### insert buffer的内部实现

Insert Buffer是一颗B+树，这棵B+树放在共享表空间中，因此试图通过独立表空间恢复表中数据时，可能触发CHECK TABLE失败，因为表的二级索引的数据可能还在Insert Buffer中，也就是共享表空间中。

Insert buffer非叶子结点存放的是查询的search key，由space、marker、offset组成。

1. space：待插入记录所在的表空间id。每个表都有唯一的space id。
2. marker：兼容老版本的insert buffer
3. offset：页所在的偏移量

构造叶子结点：


metadata保存每个记录进入Insert Buffer的顺序。

为了保证每次merge insert buffer页必须成功，还需要有一个特殊的页用来标记每个辅助索引页的可用空间，这个页的类型为Insert Buffer Bitmap，每个Insert Buffer Bitmap用来追踪16384个辅助索引页。

### merge Insert buffer

Insert buffer中的记录何时进行合并到真正的辅助索引中？

1. 辅助索引页被读取到缓冲池中
2. Insert Buffer Bitmap页追踪到该辅助索引页已无可用空间时
3. Master Thread

## 两次写

Insert buffer带来了性能的提升，但如果innodb存储引擎正在写入某个页列表中，而这个页只写了一部分，比如一个16kb的页，只写了前4kb，然后就发生了宕机，这种情况被称为部分写失效。

InnoDB的页是16k，操作系统的页一般是4K，也就是InnoDB的页写到磁盘需要分四次写。

只有一部分页写成，说明这个页本身就已经发生了损坏，而redo log是对页的物理操作，如偏移量800，写入aaa记录，破坏的页是无法进行redo log恢复的。这部分数据肯定就丢了，为了避免这种情况，在应用redo log日志前，用户需要一个页的副本，当写入失效发生时，先通过页的副本来还原该页，再进行重做。这就是doublewrite。


doubulewrite由两部分组成：

1. 内存中的double write buffer
2. 物理磁盘上共享表空间中连续的128个页。

在对缓冲池的脏页进行刷新时，并不直接写磁盘，而是通过memcpy函数将脏页先复制到内存中的doublewrite buffer，之后通过doublewirte buffer先写入共享表空间（8.0后变成了双写文件）的物理磁盘上，然后马上调用fsync函数，同步磁盘，因为doublewrite是连续的、顺序的，开销不大。



这部分要理解还是挺难的，要基础够才行，以后再研究吧。



## 自适应哈希索引

一般来说，生产环境下，B+树的高度一般为3～4层，故需要3～4次查询。

但如果提前将某个索引的热点的key的页所在的位置保存到哈希表中，那就只需要一次查询。这就是自适应哈希索引的思想。

存储引擎会监控对表上各索引页的查询，如果观察到建立哈希索引可以带来速度提升，则建立哈希索引，并且系统自优化，无需人为调整。

哈希索引只能适用等值的查询。

key就是经常访问到的索引键值，value就是该索引键值匹配到的完整记录所在页面的位置。



## 异步IO

为了提高磁盘操作性能，InnoDB等存储引擎都采用异步IO（AIO）的方式来处理磁盘操作。

如果有可以并发的IO请求，AIO可以异步请求，还可以对多个IO进行合并（可以合并的话）。

## 刷新邻接页

Flush Neighbor Page

刷新一个脏页时，InnoDB会检测该页所在区（extent）的所在页，如果是脏页，那么一起进行刷新。这样做的好处是：可以通过AIO将多个IO写入操作合并为一个IO操作。

机械硬盘下有很好的性能提升，SSD的磁盘可以关闭该特性。



# 启动、关闭与恢复

运维MySQL的一些东西
