Redis的数据库跟MySQL实例下的数据库类似，Redis的操作都是在数据库环境下的。



# 服务中的数据库

Redis的所有数据库都保存在服务器状态redisServer的db数组中

```c
struct redisServer {
    // ...
    // 一个数组，保存着服务器中的所有数据库
    redisDb *db;
	// 服务器的数据库数量, 默认16个
    int dbnum;
    // 
}
```


# 切换数据库

每个redis客户端都有自己的目标数据库，每当客户端执行数据库写命令或者读命令的时候，目标数据库就会成为这些命令的操作对象。select命令可以切换目标数据库。

```c
# switch database 1
select 1
```

在服务器内部，客户端状态redisClient结构的db属性记录了客户端当前的目标数据库，这个属性是一个指向redisDb结构的指针：

```c
typedef struct redisClent {
    redisDb *db;
} redisClient;
```


执行flushdb这样的危险命令之前，最好先执行select命令。

# 数据库键空间

Redis是一个key-value pair数据库服务器，服务器中的每个数据库都由一个redisDb表示。

```c
typedef struct redisDb {
    // 数据库键空间，保存着数据库所有的键值对
    dict *dict;
} redisDb;
```

redisDb结构的dict字典保存了数据库中**所有的键值对**，我们称这个字典为键空间（key space）。

1. 键空间的键也就是数据库的键，每个键都是一个字符串对象；
2. 键空间的值也就是数据库的值，每个键的值可以是字符串对象、列表对象、哈希表对象、集合对象、有序集合对象。

我们现在写入三种对象：

```bash
set message "hello world"
rpush alphabet "a" "b" "c"
HSET book name "redis in action"
HSET book author ""
HSET book publisher ""
```

1. message是一个字符串键
2. alphabet是一个列表键
3. book是一个哈希表键


# 设置键的过期时间

expire命令设置过期时间，setnx也可以在设置字符串键的时候设置过期时间，但只适用于字符串键。

```bash
> set key value
# 过期时间为5s
> EXPIRE key 5
# 返回剩余有效时间，单位为秒
> TTL key
# 返回剩余有效时间，单位为浩渺
> PTTL key
```

数据库如何实现设置键的过期时间，如何实现删除



redisDb结构的expires字典保存了数据库中所有键的过期时间。

# Redis的过期键删除策略

Redis采用了两种策略结合：

1. 定期删除：定时器定时执行任务删除，对内存友好，对cpu不友好
2. 惰性删除：获取键的时候判断是否过期，过期则删除；对cpu友好，对内存不友好

1. 所有的读写redis数据库的命令在执行之前都要调用expireIfNeeded函数进行过期检查。
