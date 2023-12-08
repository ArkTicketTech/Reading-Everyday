# Master Thread工作方式

Master Thread具有最高的线程优先级别。

其内部由多个循环组成：

1. 主循环
2. 后台循环
3. 刷新循环
4. 暂停循环



大多数的操作是在主循环，其中有两大部分的操作：每秒钟的操作和每10s的操作。

每秒的操作包括：

1. 日志刷新到磁盘，即使这个事务还没有提交（总是）：将redo log buffer中的内容刷新到redo log文件
2. merge insert buffer（可能）：判断当前1秒内发生的IO次数是否小于5次，小于则可以执行合并insert buffer的操作。

1. merge insert buffer是指：merge 到真正的辅助索引中。下面会讲。

1. 至少刷新100个的脏页到磁盘（可能）：判断当前缓冲池中的脏页的比例是否超过了配置文件参数，如果超过了，就将100个脏页写入到磁盘。
2. 如果当前没有用户活动，则切换到background loop（可能）



每10s的操作包括：

1. 刷新100个脏页到磁盘（可能）：10s之内的磁盘的IO操作是否小于200次，小于则将100个脏页刷新到磁盘
2. 合并至多5个insert buffer（总是）
3. 将insert buffer刷新到磁盘（总是）
4. 删除无用的undo页（总是）
5. 刷新100个或者10个脏页到磁盘（总是）：如果缓存池中脏页的比例超过70%（buf_get_modified_ratio_get），刷新100个脏页到磁盘；如果脏页的比例小雨70%，则刷10%的脏页到磁盘



background loop：

若当前没有用户活动（数据库空闲时）或者数据库关闭（shutdown），就会切换到这个循环。background loop会执行以下操作：

1. 删除无用的undo页（总是）
2. 合并20个插入缓冲（总是）
3. 跳回到主循环（总是）



上面说的并不是最终也不是当前的形态，在上面的基础上InnoDB还做了许多优化。

1. 100个脏页和5个insert buffer在写入密集的应用可能不够，因此后续用了innodb_io_capacity来控制。

1. merge insert buffer时，数量为innodb_io_capacity 的5%
2. 从缓冲区刷新脏页时，刷新数量为innodb_io_capacity

1. 刷新脏页的操作分离到一个单独的Page Cleaner Thread。
