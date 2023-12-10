本章将分析构成MySQL和InnoDB存储引擎表的各种类型文件。

1. 参数文件：实际上就是配置文件，MySQL实例启动时会加载
2. 日志文件：错误日志、二进制日志、慢查询日志、查询日志等
3. socket文件：当用UNIX域套接字方式进行连接时需要的文件
4. pid文件：MySQL实例的进程ID文件
5. MySQL表结构文件：用来存放MySQL表结构定义的文件
6. 存储引擎文件：

# 参数文件

可以通过`mysql -help | grep my.cnf`来找到文件。

MySQL的参数分为两种：

1. 动态参数：在MySQL实例运行中可以修改

1. 可以通过Set命令修改，比如隔离级别
2. 修改的作用范围分为session 和global
3. 修改后，只对本次的实例的生命周期有效，并不会修改参数文件中的值，下次重启后，还是以参数文件的为准。

1. 静态参数：只读，运行时不能修改

# 日志文件

## 错误日志

```
show variables like 'log_error'
```

MySQL不能正常启动时，第一个要查找的就是错误日志。

## 慢查询日志

1. 记录慢SQL，执行时间超过long_query_time的SQL就是慢SQL，默认为10s。
2. 配置long_queries_not_using_indexes，当运行的SQL没有使用索引时，同样会将该SQL记录到慢查询日志中



## 查询日志

查询日志记录了所有对MySQL数据库请求的信息。



## 二进制日志

就是binlog，记录了对MySQL数据库执行更改的所有操作。

如果update并没有导致数据库的变化，也不会记录下来。

binlog的作用：

1. 恢复recovery：某些数据的恢复需要二进制日志
2. 复制replication：从库同步
3. 审计audit



配置参数`log-bin [=name]`可以启动binlog，如果不指定name，则默认binlog文件名为主机名，后缀名为二进制日志的序列号，所在路径为数据库所在目录（datadir），如：

```
show variables like 'datadir'
```



binlog默认不开启，需要手动指定参数开启，开启后会使性能下降1%。



以下配置参数影响binlog的信息和行为：

1. max_binlog_size：单个binlog文件的最大值，默认1G。超过了就后缀名+1，开启一个新的文件，并记录到.index文件。
2. binlog_cache_size：未提交的binlog会先记录在一个缓存中，等事务提交后再写入binlog文件中。这个缓存大小就由该参数哦那个纸
3. sync_binlog：默认情况，binlog并不是每次写的时候都同步磁盘（相当于磁盘缓冲），而是表示每写缓冲多少次就同步到磁盘。

1. 设置为1表示采用同步写磁盘的方式

1. binlog-do-db： 需要写入哪些库的日志
2. binlog-ignore-db：需要忽略哪些库的日志



如果是从库，不会有binlog，除非设置log-slave-update。



binlog_format设置记录binlog的格式。

1. statement：记录SQL语句，但复制时涉及到函数、自动时间， 会造成数据不一致
2. row：记录行记录的更改情况。

1. 对磁盘空间要求增加，相应的复制网络开销也增加

1. mixed：statement和row的权衡，默认用statement，需要用row的场景采用row





binlog文件的查看方式：

binlog其实是二进制文件，无法用cat/head/tail等命令来查看，要想看到binlog等内容，需要用专业的工具，比如mysql提供的mysqlbinlog，

# 套接字文件

在UNIX系统下本地连接MySQL可以采用UNIX域套接字方式，这种方式需要有一个socket文件。套接字文件可由参数socket控制，一般在/tmp目录下，名为mysql.sock：

```
show variables like 'socket'
```

# pid文件

当MySQL实例启动时，会将自己的进程ID写入一个文件中，该文件即pid文件。

```
show variables like 'pid_file'
```

# 表结构定义文件

MySQL数据的存储是根据表进行的，每个表都会有与之对应的文件。这个文件以fm为后缀名，记录了该表的表结构定义。

# InnoDB存储引擎文件

Innodb存储引擎还有自己独立的文件。

## 表空间文件

InnoDB将存储的数据按表空间进行存放。

在默认配置下就有一个初始大小为10MB，名为ibdata1的文件，该文件就是默认的表空间文件，用户可以通过参数innodb_data_file_path对其进行设置，设置后会讲表的数据都存放到该共享表空间中。

如果需要每个表都产生一个独立表空间，设置innodb_file_per_table。


## 重做日志文件

redo log和binlog的区别？

1. 所属不同：binlog是MySQL的，会记录所有与MySQL数据库有关的日志记录；redo log是Innodb的。
2. 记录的内容不同：binlog记录的是一个事务的具体操作内容；redo log记录的是关于每个页的更改的物理情况
3. 写入时间不同：binlog仅在事务提交之前进行提交；在事务进行的过程中，不断有redo log被写到redo log file中。




redo log写入磁盘是按照一定条件的：

1. Master Thread 每秒将redo log buffer 写入redo log files，不论事务是否已经提交
2. innodb_flush_log_at_trx_commit：表示在提交操作时，处理redo log的方式

1. 0代表：提交事务时不将redo log buffer写磁盘，而是等待master thread 每秒的刷新
2. 1代表：提交时将redo log buffer 同步写到磁盘
3. 2代表：提交时将redo log buffer异步写到磁盘

为了保证ACID中的持久性，必须将innodb_flush_log_at_trx_commit设置为1.
