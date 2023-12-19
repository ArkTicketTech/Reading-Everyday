group commit
https://keithlan.github.io/2018/07/24/mysql_group_commit/
https://www.cnblogs.com/autopenguin/p/15429410.html
若事务为非只读事务，由于每次提交事务都会顺序写入redo log，所以每次都需要进行一次fsync操作保证redo log落盘。然而磁盘的fsync性能有限，为了提高fsync的效率，group commit使得一次fsync可以确保多个事务的redo log被写入文件。
事务提交的操作其实被拆分为两个阶段：
1. 将redo log写入文件系统缓存；
2. 调用fsync将文件系统缓存的redo log写入磁盘。
   步骤2相对1来说是一个较慢的过程，所以多个事务的redo log可以先写入缓存，然后通过一次fsync刷新到磁盘，对于写入频繁的操作，group commit的效果就非常明显。
   但是InnoDB1.2版本之前，在开启binlog日志后，InnoDB的group commit功能会失效。导致失效的原因是redo log 和binlog的两阶段提交，两阶段提交可以保证事务和binlog的一致性。
   不保证一致性有什么后果？
   两阶段提交：
1. 当事务提交时InnoDB进行prepare操作；
   a. redo log状态为prepare写入page cache；
   b. fsync写入磁盘。
   c. 并告知执行器执行完成了，随时可以提交事务
2. 写入binlog并落盘：
   a. binlog写入page cache
   b. fsync写入磁盘
3. 执行器调用InnoDB引擎的提交事务接口，InnoDB把刚刚写入的 redo log 改成提交（commit）状态，更新完成。

两阶段提交如何保证数据不会丢失？上面三个阶段系统崩溃的场景有两个：
1. 第一阶段后，第二阶段前系统崩溃了：
   a. 此时由于binlog还没写 ，redo log也还没有提交，所以崩溃恢复的时候，这个事务会回滚。
2. 第二阶段binlog写成功后，第三阶段前系统崩溃了：
   a. 此时redo log的事务状态为prepare，如果对应的binlog是完整存在的，那就将redo log改为commit状态，重新提交事务，就保证了数据不会丢。
   事实上一旦步骤2写binlog完成，就确保了事务的提交。

回到正题，为什么两阶段提交破坏了group commit？
因为要保证两阶段提交下的 binlog的写入顺序和redo log顺序一致。

为什么要保证binlog的写入顺序和redo log顺序一致呢？

本来事务的开始顺序： T1 -> T2 ->T3 ，由于事务并发执行，非常可能产生 事务的提交结束顺序是：  T2 -> T3 ->T1  ，binlog的写入顺序： T1 -> T2 -> T3。这样事务与binlog的写入顺序就不一致了，如果通过online backup建立副本，并且是在T3commit后、T1 commit之前开始备份，发现事务T3已经完成了提交，那么就跳过了T1（因为T1的位置100小于T3的300）。而两阶段提交不仅保证了事务和写binlog的原子性，还通过加锁保证了整个事务的串形性。


所以使用了prepare_commit_mutex这个锁，启动这个锁意味着事务提交的两个阶段（也就是上面说的两阶段提交的prepare）
1. 将redo log写入文件系统缓存；
2. 调用fsync将文件系统缓存的redo log写入磁盘。
   是原子性的，所以这里是串形执行的，binlog也不是组提交的，所以redo log也无法组提交，
   这里可以简单理解为redo log实现了组提交，但binlog没有实现组提交。但这里还要细品，有点没想通。这本书里讲的两阶段提交是先落盘binlog再落盘redo log，但其他地方是反着来的。

MySQL5.6提出了Binary Log Group Commit（BLGC）实现了组提交。
BLGC将事务提交的过程划分为几个步骤来完成，每个阶段都有一个队列，每个队列都有一把锁，第一个进入队列的事务会成为leader，后续的为follower，leader负责整个队列的操作，完成操作后发出通知。


1. Flush阶段:
   a. 首先获取队列中的事务组；
   b. 将Redo log中prepare阶段的数据刷盘（Flush Redo log）；因为是顺序写，其实很快。
   c. 将Binlog数据写入文件，当然此时只是写入文件系统的缓冲，并不能保证数据库崩溃时Binlog不丢失 （Write Binlog步骤）；
   d. Flush阶段队列的作用是用于支撑Redo log的组提交；
   e. 如果在这一步完成后数据库崩溃，由于协调者Binlog中不保证有该组事务的记录，所以MySQL可能会在重启后回滚该组事务。
2. Sync阶段:
   a. 这里为了增加一组事务中的事务数量，提高刷盘收益，MySQL使用两个参数控制获取队列事务组的时机，分别如下。
   ■ Binlog_group_commit_sync_delay=N：在等待N μs后，开始binlog刷盘
   ■ Binlog_group_commit_sync_no_delay_count=N：如果队列中的事务数达到N个，就忽视Binlog_group_commit_sync_delay的设置，直接开始binlog刷盘
   a. Sync阶段队列的作用是用于支持Binlog的组提交；
   b. 如果在这一步完成后数据库崩溃，由于协调者Binlog中已经有了事务记录，MySQL会在重启后通过Flush 阶段中Redo log刷盘的数据继续进行事务的提交。
3. Commit阶段：
   a. 首先获取队列中的事务组；
   b. 依次将Redo log中已经prepare的事务在引擎层提交
   c. Commit阶段不用刷盘，Flush阶段中的Redo log刷盘已经足够保证数据库崩溃时的数据安全了
   d. Commit阶段队列的作用是承接Sync阶段的事务，完成最后的引擎提交，使得Sync可以尽早的处理下一组事务，最大化组提交的效率。

group commit为何保证了redo log和binlog的顺序？
binlog写入page cache提前到了redo log fsync前，flush阶段多个事务作为一组一同进入队列，完成redo log和bin log的写入。


当提交的事务越多，group commit带来的性能提升就越大。
