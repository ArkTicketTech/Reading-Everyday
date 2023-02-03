##  Chap 6. VACUUM Processing
https://www.interdb.jp/pg/pgsql06.html

Vaccum ：
* Concurrent Vaccum : with ShareUpdateExclusiveLock
* Full Vaccum : with AccessExclusiveLock
### 6.1 Concurrent Vaccum
流程如下：
1. 移除 dead tuples，对页内进行碎片整理，如果对应表上存在索引，还需要移出相应的 index tuples。
2. 冻结旧事务 id 及相应的系统表（pg_database and pg_class），删除不必要的 clog。
3. 更新对应表所关联的 FSM 和 VM，更新相应的统计数据。

#### 6.1.1 First Block
freeze processing and removes index tuples that point to dead tuples.
构造 dead tuples lists 存放于 maintenance_work_mem 内存中。
#### 6.1.2 Second Block
移出页内的 dead tuples，同时更新 FSM 和 VM，如下图所示。
> 块内的 line pointer 不会被删除，后续可以被重新使用。

![图 1](assets/img_20230203-012041168.png)  


#### 6.1.3 Third Block
清理索引，更新统计信息和系统表。
#### 6.1.4. Post-processing
Vaccum 完成后，更新和 vaccum 相关的系统表和统计信息，并删除不必要的 clog。
> vaccum 处理使用环形缓冲区（循环队列）。
