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
2. object：json对象，但内部对象flattened化存储
3. nested：真正效果的json对象
4. flattened：对象下所有的值都被认为keyword，可以直接查询出来。
5. join：类似外键，可以在文档间建立父子关系，
6. search as you type
7. ...


还可以定义multiple data type。

索引的机制与大多数数据库系统类似：
客户的写请求先到index buffer，lucene每秒将buffer中的documents进行收集，创建新的segment，然后写入file system buffer（有refresh参数可以控制），最后进行刷盘。
如果不是实时的写入file system buffer，可能就会出现刚写的数据查询不到的情况。配置实时会影响性能，所以说es是一个近实时的系统。refresh可以针对某一个索引。

由于每秒都会生成段，段不会断膨胀，所以后台会合并段，每三个segment合并成一个新的segment。
