Redis没有直接使用C语言传统的字符串表示，而是自己构建了一种名为简单动态字符串（Simple dynamic string， SDS）的抽象类型。

set msg "hello world"

键值对的键是一个字符串对象，底层是SDS，值是一个字符串/列表对象。

除了用来保存数据库中的字符串值之外，SDS还被用作缓冲区：AOF模块中的AOF缓冲区，以及客户端状态中的输入缓冲区。

# SDS

定义：

```c
struct sdshdr {
    // 记录buf数组中已使用字节的数量，等于SDS所保存字符串的长度
	int len;
	// 记录buf数组中未使用字节的数量
    int free;
	// 字节数组，用于保存字符串
	char buf[];
}
```

## SDS与C字符串的区别

1. 常数复杂度获取字符串长度：C字符串并不记录自身的长度信息，获取长度时间复杂度为O(N)
2. 杜绝缓冲区溢出：扩展字符串前sds会计算空间够不够，不够就进行扩展，C字符串则不会，所以会有缓冲区溢出。
3. 减少修改字符串带来的内存重分配次数：通过未使用空间实现了空间预分配和惰性空间释放两种优化策略

1. 空间预分配用于字符串扩张时
2. 惰性空间释放用于缩短字符串时所需的内存重分配操作

1. SDS的API都是二进制安全的，数据写入时什么样，读取就什么样。

1. C不能包含空字符，否则会被认为是字符串结尾，所以C字符串只能保存文本数据；
2. SDS的buf可以保存任何二进制数据，它使用len来判断字符串的结尾

1. 兼容部分C字符串函数：SDS的尾部还是空字符串结尾



# LinkedList

Redis构建了自己的链表实现，可以用于构建列表键，当一个列表键包含了数量比较多的元素，又或者列表中包含的元素都是比较长的字符串时，Redis就会使用链表作为列表键的底层实现。

每个链表节点都使用一个adlist.h/listNode结构来表示：

```c
typedef struct listNode {

    // 前置节点
    struct listNode *prev;

    // 后置节点
    struct listNode *next;

    // 节点的值
    void *value;

} listNode;
```

虽然仅仅使用多个 listNode 结构就可以组成链表， 但使用 adlist.h/list 来持有链表的话， 操作起来会更方便

```c
typedef struct list {

    // 表头节点
    listNode *head;

    // 表尾节点
    listNode *tail;

    // 链表所包含的节点数量
    unsigned long len;

    // 节点值复制函数
    void *(*dup)(void *ptr);

    // 节点值释放函数
    void (*free)(void *ptr);

    // 节点值对比函数
    int (*match)(void *ptr, void *key);

} list;
```

list 结构为链表提供了表头指针 head 、表尾指针 tail ， 以及链表长度计数器 len ， 而 dup 、 free 和 match 成员则是用于实现多态链表所需的类型特定函数：

- dup 函数用于复制链表节点所保存的值；
- free 函数用于释放链表节点所保存的值；
- match 函数则用于对比链表节点所保存的值和另一个输入值是否相等。



图 3-2 是由一个 list 结构和三个 listNode 结构组成的链表：


Redis 的链表实现的特性可以总结如下：

- 双端： 链表节点带有 prev 和 next 指针， 获取某个节点的前置节点和后置节点的复杂度都是 O(1) 。
- 无环： 表头节点的 prev 指针和表尾节点的 next 指针都指向 NULL ， 对链表的访问以 NULL 为终点。
- 带表头指针和表尾指针： 通过 list 结构的 head 指针和 tail 指针， 程序获取链表的表头节点和表尾节点的复杂度为 O(1) 。
- 带链表长度计数器： 程序使用 list 结构的 len 属性来对 list 持有的链表节点进行计数， 程序获取链表中节点数量的复杂度为 O(1) 。
- 多态： 链表节点使用 void* 指针来保存节点值， 并且可以通过 list 结构的 dup 、 free 、 match 三个属性为节点值设置类型特定函数， 所以链表可以用于保存各种不同类型的值。

# 字典

Redis的数据库就是使用字典来作为底层实现的，对数据库的crud也是构建在字典的操作之上的。字典， 又称符号表（symbol table）、关联数组（associative array）或者映射（**map**）， 是一种用于**保存键值对（key-value pair）的抽象数据结构**。

set msg "hello world"

这个键值对就是保存在代表数据库的字典里面的。

## 字典的实现

### 哈希表

Redis的字典使用哈希表作为底层实现，一个哈希表里面可以有多个哈希表节点，而每个哈希表节点就保存了字典中的一个键值对。

Redis 字典所使用的哈希表由 dict.h/dictht 结构定义：

```c
typedef struct dictht {
  // 哈希表数组
  dictEntry **table;
  // 哈希表大小
  unsigned long size;
  // 哈希表大小掩码，用于计算索引值
  // 总是等于 size - 1
  unsigned long sizemask;
  // 该哈希表已有节点的数量
  unsigned long used;
} dictht;
```

table是一个数组，每个dictEntry结构保存着一个键值对。

### 哈希表节点

哈希表节点使用dictEntry结构表示，每个dictEntry都保存着一个键值对

```c
typedef struct dictEntry {
    // 键
    void *key;

    // 值
    union {
        void *val;
        uint64_t u64;
        int64_t s64;
    } v;

    // 指向下个哈希表节点，形成链表
    struct dictEntry *next;
} dictEntry;
```

next存储key的哈希值相同的数据。

### 字典

Redis的字段由dict.h/dict结构表示，dictht就是上面讲到的哈希表，一个字典中存储了两个哈希表。其目的是为了rehash。

```c
typedef struct dict {

    // 类型特定函数
    dictType *type;

    // 私有数据
    void *privdata;

    // 哈希表
    dictht ht[2];

    // rehash 索引
    // 当 rehash 不在进行时，值为 -1
    int rehashidx; /* rehashing not in progress if rehashidx == -1 */

} dict;
```

ht 属性是一个包含两个项的数组， 数组中的每个项都是一个 dictht 哈希表， 一般情况下， 字典只使用 ht[0] 哈希表， ht[1] 哈希表只会在对 ht[0] 哈希表进行 rehash 时使用。下图展示了一个普通状态下（没有进行 rehash）的字典：


## 哈希算法

当要将一个新的键值对添加到字典里面时， 程序需要先根据键值对的键计算出哈希值和索引值， 然后再根据索引值， 将包含新键值对的哈希表节点放到哈希表数组的指定索引上面。

```java
# 使用字典设置的哈希函数，计算键 key 的哈希值
hash = dict->type->hashFunction(key);

# 使用哈希表的 sizemask 属性和哈希值，计算出索引值
# 根据情况不同， ht[x] 可以是 ht[0] 或者 ht[1]
index = hash & dict->ht[x].sizemask;
```

Redis目前使用的哈希算法是MurmurHash2，可以保证即使输入的键是有规律的，也能有一个很好的随机分布性。



## 解决键冲突

当有两个或以上数量的键被分配到了哈希表数组的同一个索引上面时， 我们称这些键发生了冲突（collision）。

Redis 的哈希表使用链地址法（separate chaining）来解决键冲突： 每个哈希表节点都有一个 next 指针， 多个哈希表节点可以用 next 指针构成一个单向链表， 被分配到同一个索引上的多个节点可以用这个单向链表连接起来， 这就解决了键冲突的问题。

因为 dictEntry 节点组成的链表没有指向链表表尾的指针， 所以为了速度考虑， 程序总是将新节点添加到链表的表头位置（复杂度为 O(1)）， 排在其他已有节点的前面。

## rehash

随着操作的不断执行， 哈希表保存的键值对会逐渐地增多或者减少， 为了让哈希表的负载因子（load factor）维持在一个合理的范围之内， 当哈希表保存的键值对数量太多或者太少时， 程序需要对哈希表的大小进行相应的扩展或者收缩。

负载因子：当前容量占到整体容量的某个百分比时，就会进行扩容缩容，这个百分比就是负载因子。

扩展和收缩哈希表的工作可以通过执行 rehash （重新散列）操作来完成， Redis 对字典的哈希表执行 rehash 的步骤如下：

1. 为字典的 ht[1] 哈希表分配空间， 这个哈希表的空间大小取决于要执行的操作， 以及 ht[0] 当前包含的键值对数量 （也即是 ht[0].used 属性的值）：

- 如果执行的是扩展操作， 那么 ht[1] 的大小为第一个大于等于 ht[0].used * 2 的 2^n （2 的 n 次方幂）；
- 如果执行的是收缩操作， 那么 ht[1] 的大小为第一个大于等于 ht[0].used 的 2^n 。

1. 将保存在 ht[0] 中的所有键值对 rehash 到 ht[1] 上面： rehash 指的是重新计算键的哈希值和索引值， 然后将键值对放置到 ht[1] 哈希表的指定位置上。
2. 当 ht[0] 包含的所有键值对都迁移到了 ht[1] 之后 （ht[0] 变为空表）， 释放 ht[0] ， 将 ht[1] 设置为 ht[0] ， 并在 ht[1] 新创建一个空白哈希表， 为下一次 rehash 做准备。

当以下条件中的任意一个被满足时， 程序会自动开始对哈希表执行扩展操作：

1. 服务器目前没有在执行 BGSAVE 命令或者 BGREWRITEAOF 命令， 并且哈希表的负载因子大于等于 1 ；
2. 服务器目前正在执行 BGSAVE 命令或者 BGREWRITEAOF 命令， 并且哈希表的负载因子大于等于 5 ；

另一方面， 当哈希表的负载因子小于 0.1 时， 程序自动开始对哈希表执行收缩操作。

# SkipList

跳跃表（skiplist）是一种有序数据结构， 它通过在每个节点中维持多个指向其他节点的指针， 从而达到快速访问节点的目的。

跳跃表支持平均 O(log N) 最坏 O(N) 复杂度的节点查找， 还可以通过顺序性操作来批量处理节点。

跳表的本质实际上就是在底层数据节点之上构建多层不同的稀疏索引，加速节点的查询定位，跟平衡树的功能类似。

在大部分情况下， 跳跃表的效率可以和平衡树相媲美， 并且因为跳跃表的实现比平衡树要来得更为简单， 所以有不少程序都使用跳跃表来代替平衡树

其实就是有了红黑树的平衡特性，支持范围查询，提高查询效率。如果一个有序集合包含的元素数量比较多， 又或者有序集合中元素的成员（member）是比较长的字符串时， Redis 就会使用跳跃表来作为有序集合键的底层实现。



Redis 只在两个地方用到了跳跃表， 一个是实现有序集合键， 另一个是在集群节点中用作内部数据结构。

## 跳跃表节点

```c
typedef struct zskiplistNode {

  // 后退指针
  struct zskiplistNode *backward;

  // 分值
  double score;

  // 成员对象
  robj *obj;

  // 层
  struct zskiplistLevel {

    // 前进指针
    struct zskiplistNode *forward;

    // 跨度
    unsigned int span;

  } level[];

} zskiplistNode;
```

1. 后退指针：指向前一个节点，方便回溯
2. 分值：作为排序的依据，分值相同时按照成员对象的字典序大小
3. 层数：数组，跳跃表特性
4. 成员对象：保存的对象



但通过使用一个 zskiplist 结构来持有这些节点， 程序可以更方便地对整个跳跃表进行处理， 比如快速访问跳跃表的表头节点和表尾节点， 又或者快速地获取跳跃表节点的数量（也即是跳跃表的长度）等信息。

## 跳跃表

```c
typedef struct zskiplist {
  // 表头节点和表尾节点
  struct zskiplistNode *header, *tail;

  // 表中节点的数量
  unsigned long length;

  // 表中层数最大的节点的层数
  int level;
} zskiplist;
```

书中讲的没咋看懂，跳表具体实现后续再研究。

# 整数集合

整数集合（intset）是集合键的底层实现之一： 当一个集合只包含整数值元素， 并且这个集合的元素数量不多时， Redis 就会使用整数集合作为集合键的底层实现。

举个例子， 如果我们创建一个只包含五个元素的集合键， 并且集合中的所有元素都是整数值， 那么这个集合键的底层实现就会是整数集合：

redis> SADD numbers 1 3 5 7 9 (integer) 5

redis> OBJECT ENCODING numbers "intset"

在这一章， 我们将对整数集合及其相关操作的实现原理进行介绍。





## 实现

整数集合（intset）是 Redis 用于保存整数值的集合抽象数据结构， 它可以保存类型为 int16_t 、 int32_t 或者 int64_t 的整数值， 并且保证集合中不会出现重复元素。

```java
typedef struct intset {

    // 编码方式
    uint32_t encoding;

    // 集合包含的元素数量
    uint32_t length;

    // 保存元素的数组
    int8_t contents[];

} intset;
```

contents 数组是整数集合的底层实现： 整数集合的每个元素都是 contents 数组的一个数组项（item）， 各个项在数组中按值的大小从小到大有序地排列， 并且数组中不包含任何重复项。


## 升级

每当我们要将一个新元素添加到整数集合里面， 并且新元素的类型比整数集合现有所有元素的类型都要长时， 整数集合需要先进行升级（upgrade）， 然后才能将新元素添加到整数集合里面。

升级整数集合并添加新元素共分为三步进行：

1. 根据新元素的类型， 扩展整数集合底层数组的空间大小， 并为新元素分配空间。
2. 将底层数组现有的所有元素都转换成与新元素相同的类型， 并将类型转换后的元素放置到正确的位上， 而且在放置元素的过程中， 需要继续维持底层数组的有序性质不变。
3. 将新元素添加到底层数组里面。



因为每次向整数集合添加新元素都可能会引起升级， 而每次升级都需要对底层数组中已有的所有元素进行类型转换， 所以向整数集合添加新元素的时间复杂度为 O(N) 。

# 压缩列表

压缩列表（ziplist）是列表键和哈希键的底层实现之一。

redis5.0 引入了listpack，是对ziplist的改进版本，在存储与结构上都比ziplist要更为节省和精简

```c
struct listpack<T> {
    int32 total_bytes; // 占用的总字节数
    int16 size;        // 元素个数
    T[] entries;       // 元素列表
    int8 end;          // 同ziplist的zlend一样，恒为0xFF
}
# entrie 机构如下
struct lpentry {
    int<var> encoding;
    optional byte[] content;
    int<var> length;
}
```

压缩列表是为了节约内存而开发的。