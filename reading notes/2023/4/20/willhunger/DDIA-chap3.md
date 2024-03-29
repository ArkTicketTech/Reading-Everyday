#### 2. Sort Order in Column Storage
 排序的好处：
 1. 减少扫描文件数量
 2. 数据有序后，排序键的相同值，可以进行压缩以减少存储空间。基于第一个排序键的压缩效果很好，后续第二个第三个排序键的压缩效果可能会因为变得混乱而无法压缩；总的来说，对前几列排序是能获得收益的。

##### Serval different sort orders
Vertica C-Store 所使用的方式：将不同的数据副本按照不同规则排序，将冗余副本存储在其它机器上，选择最适合 query 的副本进行查询。

这种列存多副本排序的方法，和行存中的多个二级索引类似。


#### 3. Writing to Column-Oriented Storage
列存情况下，如果需要采用原地更新的写入，在传统 B-Tree 存储引擎中，如果需要将一个行插入到一个 sorted table，则可能需要重写所有的 column file，因为所有的列都需要更新一次。

为了避免上述问题，列存使用 LSM-trees 来组织。


#### 4. Aggregation: Data Cubes and Materialized Views
物化视图是查询结果的实际副本，会被写入磁盘，后续查询直接查询预计算的副本即可。

当底层数据发生变化时，物化视图也会随之更新，数据库可自动执行更新，这种更新方式会影响写入性能。因此 OLTP 型数据库很少使用物化视图，OLAP 型数仓使用更有意义。

优点：查询速度快，因为预计算。
缺点：缺乏灵活性，不如查源表的灵活性强。
