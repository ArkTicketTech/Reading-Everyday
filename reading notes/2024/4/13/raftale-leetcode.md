前缀和中，常利用Map来优化时间复杂度。

Map中存储的可以是：

1. HashMap<前缀和, 第一次的索引位置>
2. HashMap<前缀和, 出现总次数>
3. HashMap<前缀状态, 第一次的索引位置>

leetcode中比如：

1. 325: key存储的是nums的前缀和，value是前缀和第一次出现的索引位置
2. 525: key存储的是nums[0:i]中1和0的差值， value是key第一次出现的索引位置；
3. 523: key存储的是preSum % k，value是key第一次出现的索引位置。
4. 560: key是nums中的「前缀和」，value为key出现的次数。
5. 1248: key是nums中的「奇数个数」，value是key出现的次数。
6. 974: key是nums中「preSum%k」，value是key出现的次数。
7. 2588: key是「前缀异或」, value是key出现的次数。
