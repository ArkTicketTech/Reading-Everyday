# 对象

在前面的数个章节里， 我们陆续介绍了 Redis 用到的所有主要数据结构， 比如简单动态字符串（SDS）、双端链表、字典、压缩列表、整数集合， 等等。

Redis 并没有直接使用这些数据结构来实现键值对数据库， 而是基于这些数据结构创建了一个对象系统， 这个系统包含字符串对象、列表对象、哈希对象、集合对象和有序集合对象这五种类型的对象， 每种对象都用到了至少一种我们前面所介绍的数据结构。

通过这五种不同类型的对象， Redis 可以在执行命令之前， 根据对象的类型来判断一个对象是否可以执行给定的命令。 使用对象的另一个好处是， 我们可以针对不同的使用场景， 为对象设置多种不同的数据结构实现， 从而优化对象在不同场景下的使用效率。

除此之外， Redis 的对象系统还实现了基于引用计数技术的内存回收机制： 当程序不再使用某个对象的时候， 这个对象所占用的内存就会被自动释放； 另外， Redis 还通过引用计数技术实现了对象共享机制， 这一机制可以在适当的条件下， 通过让多个数据库键共享同一个对象来节约内存。

最后， Redis 的对象带有访问时间记录信息， 该信息可以用于计算数据库键的空转时长， 在服务器启用了 maxmemory 功能的情况下， 空转时长较大的那些键可能会优先被服务器删除。

本章接下来将逐一介绍以上提到的 Redis 对象系统的各个特性。

## 对象的类型与编码

Redis 使用对象来表示数据库中的键和值， 每次当我们在 Redis 的数据库中新创建一个键值对时， 我们至少会创建两个对象， 一个对象用作键值对的键（键对象）， 另一个对象用作键值对的值（值对象）。

比如`set msg "hello world"`，Redis会创建一个键值对，键是一个包含了字符串值msg的对象，值是一个包含了字符串值`"hello world"`的对象。

Redis 中的每个对象都由一个 redisObject 结构表示， 该结构中和保存数据有关的三个属性分别是 type 属性、 encoding 属性和 ptr 属性：

```c
typedef struct redisObject {

    // 对象的类型
    unsigned type:4;

    // 对象的编码
    unsigned encoding:4;

    // 指向底层实现数据结构的指针
    void *ptr;

    // ...

} robj;
```

### 类型

type记录了对象的类型，键总是一个字符串对象，值可以是字符串对象、列表对象、哈希对象、集合对象或是有序集合对象。

type命令返回的是值的对象类型：

1.

```bash
> set msg "hello world"
ok
> type msg
string
> rpush numbers 1 3 5
(integer) 3
> type numbers
list
> HMSET profile name tom age 24 career engineer
ok
> type profile
hash
```

type输出：

1. REDIS_STRING：string
2. REDIS_LIST：list
3. REDIS_HASH：hash
4. REDIS_SET：set
5. REDIS_ZSET：有序集合，zset



### 编码与底层实现

对象的ptr指针指向对象的底层实现数据结构，encoding属性记录了对象所使用的编码，编码也就是这个对象底层使用了什么数据结构。


```bash
> object encoding msg
"embstr"
> object encoding numbers
"listpack"
```


通过encoding属性设定对象所使用的编码，而不是为特定类型的对象关联一种固定的编码，极大的提升了Redis的灵活性和效率。比如：在列表对象包含的元素比较少时，Redis使用压缩列表作为列表对象的底层实现。

1. 压缩列表比双端链表更节约内存
2. 对象包含元素过多，使用压缩列表将不再具有优势

# 字符串对象

字符串对象的编码可以是int, raw, embstr。
