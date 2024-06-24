继续复习es，争取周三前复习完。

mapping：

不定义mapping时，第一次插入数据时会自动推断字段的type，大部分情况下是没问题的，但有时会推断错误，或者由于输入歧义导致理解偏差。
比如"age":"12"，会将age定义text字段（多字段keyword，使用age.keyword可以排序），那么就不能进行排序，或者排序过于昂贵。

core data types:
1. text
2. keyword
3. date
4. numeric
5. boolean
6. range
7. ip

advanced data types:
1. geopoint
2. object
3. nested
4. flattened
5. join
6. search as you type

还可以定义multiple data type


索引的机制与大多数数据库系统类似：
客户的写请求先到index buffer，lucene每秒将buffer中的documents进行收集，创建新的segment，然后写入file system buffer（有参数可以控制），最后进行刷盘。
