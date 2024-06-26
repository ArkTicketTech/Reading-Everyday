# compound query

1. boolean：各种条件的组合查询:
   - must: and
   - must_not: not and
   - should: or
   - filter: 不带评分的must
2. constant_score：指定某个字段的评分为常量
3. boosting：字段偏好
4. dis_max
5. function：自定义评分算法


为什么es是一个近实时而不是实时的系统？
1. lucene写入内存后，并不是可搜索的，需要通过refresh把内存的对象转为segment后。

translog就是es的wal，translog是实时顺序写入磁盘的，translog的膨胀问题其实跟mysql中的redo log日志问题一样，解决方法也是利用checkpoint来做标记保证合理的删除translog。





