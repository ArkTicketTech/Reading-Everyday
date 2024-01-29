Append Only File


```bash
redis> SET msg "hello"
redis> SAD fruits "apple" "banana" "cherry" 
(integer) 3
redis> RPUSH numbers 128 256 512 
(integer) 3
```

RDB和AOF的区别在于：

1. RDB持久化保存数据库状态的方法是将msg、fruits、numbers三个键的键值对保存到RDB文件中，
2. 而AOF持久化保存数据库状态的方法则是将服务器执行的SET、SADD、RPUSH 三个命令保存到AOF文件中。



# AOF持久化的实现

可以分为

1. 命令追加Append
2. 文件写入：写入文件系统缓存
3. 文件同步：落盘

三步

## 命令追加

服务器执行一个写命令后，会将写命令追加到服务器状态的aof_buf缓冲区的末尾：

```c
struct redisServer {
    // AOF缓冲区
    sds aof_buf;
}
```



## AOF文件的写入与同步



1.2 AOF文件的写入与同步

Redis的服务器进程就是一个事件循环(loop)，伪代码：

```python
def eventLoop():
  while True:
    # 处理文件事件，接收命令请求以及发送命令回复
    # 处理命令请求时可能会有新内容被追加到aof_buf 缓冲区中
    processFileEvents ()
    # 处理时间事件 
    processTimeEvents ()
    # 考虑是否要将 aof_buf 中的内容写入和保存到AOF 文件里面 
    flushAppendonlyFile ()
```

flushAppendonlyFile是否同步由配置appendfsync参数决定：

1. always：每次写入都同步到磁盘
2. everysec：如果距离上次同步超过1秒，就执行落盘，可以理解为每秒执行落盘。默认选项
3. no：写入文件系统缓存，由操作系统自己决定

在效率和安全权衡后，一般都是选择everysec。



# AOF的载入与还原



# AOF重写

重写的目的是为了避免文件占用不断膨胀，因此需要将冗余的信息进行压缩。

原理就是从数据库中读取现在的键值，然后用一条命令去做记录。



Redis重写期间，不会阻塞请求，但新的请求可能导致新的写入与重写的文件数据不一致。

为了解决不一致问题：

1. Redis读取的数据库是副本
2. Redis设置了一个AOF重写缓冲区，每当redis执行新的写入请求时，也会同时将这个写命令发送到AOF重写缓冲区，当AOF重写完之后，再将重写缓冲区的写命令追加到AOF文件末尾。

子进程重写完成后，父进程会将命令追加到新AOF的末尾，就算是有重复的，等待下一轮重写。

举个例子 ， 下图展示了一个AOF文件后台重写的执行过程 :

1. 当子进程开始重写时，服务器进程(父进程)的数据库中只有k1 一个键，当子进程完成AOF文件重写之后，服务器进程的数据库中已经多出了k2、k3、k4 三个新键。
2. 在子进程向服务器进程发送信号之后，服务器进程会将保存在AOF 重写缓冲区里面记录的k2、k3、k4三个键的命令追加到新AOF文件的末尾，然后用新AOF文件替换旧的AOF文件，完成AOF文件后台重写操作。
