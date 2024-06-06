复习下redis：



Redis的使用场景：
1. 热点数据的缓存
2. 限时业务的运用：expire命令
3. 计数器相关问题：incrby原子递增
4. 分布式锁：setnx
5. 延时问题：监听设置了过期时间的key
6. 排行榜相关问题：SortedSet
7. 求交集、并集、差集场景：
8. 简单队列



5种基础类型，针对的是key-value中的value：
1. String：可以是字符串、整数或浮点数
2. Hash：
3. List：链表
4. Set：
5. ZSet：sortedSet

3种特殊类型：
1. hyberloglogs：基数统计，可用于在线用户数、共同好友等。
2. bitmap:
3. geospatial
