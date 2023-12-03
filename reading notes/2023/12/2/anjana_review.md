Redis的内存淘汰策略？

ChatGPT

Redis 使用内存淘汰（Eviction）策略来管理内存，以防止内存使用超过配置的最大内存限制。以下是一些常见的 Redis 内存淘汰策略：

1. **No Eviction（不淘汰）：**

   - 如果 Redis 的内存使用超过最大内存限制，所有写操作（包括新写入和更新操作）将会失败，直到有足够的空间为止。这是默认的内存淘汰策略。

   ```
   confCopy code
   maxmemory-policy noeviction
   ```

2. **All Keys LRU（Least Recently Used，最近最少使用）：**

   - Redis 将所有的键按最近最少使用的原则排序，当内存不足时，删除最近最少使用的键。

   ```
   confCopy code
   maxmemory-policy allkeys-lru
   ```

3. **Volatile LRU：**

   - 类似于 All Keys LRU，但只对设置了过期时间的键进行淘汰。在这种策略下，过期时间越早的键越容易被淘汰。

   ```
   confCopy code
   maxmemory-policy volatile-lru
   ```

4. **All Keys Random（随机淘汰）：**

   - Redis 随机选择一个键进行淘汰。

   ```
   confCopy code
   maxmemory-policy allkeys-random
   ```

5. **Volatile Random：**

   - 类似于 All Keys Random，但只对设置了过期时间的键进行淘汰。

   ```
   confCopy code
   maxmemory-policy volatile-random
   ```

6. **All Keys LFU（Least Frequently Used，最不经常使用）：**

   - Redis 将所有的键按最不经常使用的原则排序，当内存不足时，删除最不经常使用的键。

   ```
   confCopy code
   maxmemory-policy allkeys-lfu
   ```

7. **Volatile LFU：**

   - 类似于 All Keys LFU，但只对设置了过期时间的键进行淘汰。

   ```
   confCopy code
   maxmemory-policy volatile-lfu
   ```

8. **TTL（Time to Live，过期时间）：**

   - Redis 根据键的剩余生存时间（TTL）来淘汰键，越快过期的键越容易被淘汰。

   ```
   confCopy code
   maxmemory-policy ttl
   ```

可以通过在 Redis 配置文件中设置 `maxmemory-policy` 参数来选择相应的内存淘汰策略。选择合适的策略取决于具体的业务需求和数据访问模式。