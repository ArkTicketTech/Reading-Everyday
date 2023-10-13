
```
select ...  一致读
select ... lock in share mode  s锁
select ... for update  x锁

update table ... where id = 1  x锁
delete from table where id = 1  x锁
```

- 加锁的对象是索引，加锁的基本单位是next-key lock
- 在使用记录锁或者间隙锁就能避免幻读现象的场景下退化成记录锁或间隙锁

#### 唯一索引
##### 等值查询
- 查询的记录存在时会退化成记录锁
- 查询不存在时则退化成间隙锁 间隙是他的右边界（第一条大于他的记录）到他的上一条记录
##### 范围查询
- 会对扫描到的索引加next-key

#### 非唯一索引
