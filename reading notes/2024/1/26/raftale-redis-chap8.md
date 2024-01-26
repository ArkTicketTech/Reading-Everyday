# 有序集合对象

有序集合的编码可以是ziplist或者skiplist

## 1. ziplist

每个元素使用两个紧挨在一起的压缩列表节点来保存，第一个元素是保存元素的成员（member），第二个元素是保存元素的分值。

有序性体现在：集合内的元素按照分值从小到大排序。


使用ziplist的前提是：

1. 有序集合保存的元素数量小于128个;
2. 且有序集合保存的所有元素成员的长度都小于64字节;

否则就会使用skiplist

## 2. skiplist

skiplist编码的有序集合对象使用zset结构作为底层实现：

```c
typedef struct zset {
    ziplist *zsl;
    dict *dict;
} zset;
```

zsl跳跃表按分值从小到大保存了所有集合元素，zrank、zrange就是利用这个特性，时间复杂度为O(N)。

dict创建了一个从成员到分值的映射，zscore就是利用这一特性实现的，dict前面说到其实就是一个map，查找时间复杂度O(1)。

zsl和dict都不可或缺，缺少dict，zscore的时间复杂度就为O(N)；缺少zsl，zrange/zrank就需要对dict进行排序，时间复杂度就为O(NlogN)，额外内存为O(N).


命令：

1. zadd：添加元素
2. zcard：获取元素数量
3. zcount：获取指定分数区间的元素数量
4. zrange：获取指定索引区间的元素
5. zrevrange：从表尾开始获取指定索引区间的元素
6. zscore：获取指定元素的分数
7. zrank：从表头开始遍历直到找到给定的元素，返回沿途经过节点的数量，不包含该元素本身。
8. zrevrank：反过来从表尾找
9. zrem：移除指定元素

```bash
> ZADD price 8.5 apple 5.0 banana 6.0 cherry
(integer) 3
> ZCARD price
(integer) 3
> zcount price 5 6
(integer) 2
> Zrange price 0 2
1) "banana"
2) "cherry"
3) "apple"
> zrevrange price 0 2
1) "apple"
2) "cherry"
3) "banana"
> zscore price apple
"8.5"
> zrank price cherry
(integer) 1
> zrevrank price cherry
(integer) 1
> zrem price apple
(integer) 1
> zrange price 0 2
1) "banana"
2) "cherry"
```
