主从延迟的原因：
1. 从库机器性能不足；
2. 从库压力过大
3. 长事务执行
4. 主库DDL

B+树：
1. 回表
2. 索引下推
3. 最左前缀匹配原则

事务不生效的场景：
1. 同一类中调用事务注解方法不生效
2. @Transactional注解作用在非public方法上
3. 异常在注解方法内被捕获


长事务解决方法：
1. 避免远程调用
2. 同步变异步
3. 避免一次性处理大量数据
4. 减少事务粒度


慢SQL优化：
1. 最好不要枚举字段：变更麻烦；变更后可能导致代码不兼容。
2. join时字符集不同时不能走索引
3. In的集合数量不宜过多，超过eq_range_index_dive_limit设置可能导致全表扫描
4. 避免隐式类型转换
5. 避免左模糊查询
6. where条件不宜有函数
7. ....
8. 


