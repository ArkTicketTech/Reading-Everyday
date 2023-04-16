### Transaction Processing or Analytics?

事务并不需要有四个特性（原子性、一致性、隔离性、持久性），相反，事务的四个特性指的是 low-latency reads and writes（和周期运行的批处理任务相对）。

| Property     | OLTP                                            | OLAP                             |
| ------------ | ----------------------------------------------- | -------------------------------- |
| 主要读模式   | 读取量小，通过 key 查                           | 读取量大，通常有 agg 操作        |
| 主要写模式   | Radom access, low latency writes for user input | Bulk import(ETL) or event strean |
| 使用者       | web 用户                                        | 内部分析，用于分析决策           |
| 数据代表什么 | 特定时间点的最新数据                            | 历史事件数据                     |
| 数据量       | GB 级别                                         | TB 级别                                 |

#### 1. Data Warehousing
相关产品：
* Teredata，Vertiea，SAP HANA，ParAccel，Amazon RedShift。
* SQL on Hadoop：Apache Hive，Spark SQL，Cloudera Impala，Facebook Presto，Apache Tajo，Apache Drill，其中一些受 Google Dremel 而来。

#### 2. Stars and Snowflakes: Schmeas for Analytics
##### Stars schema
星型模型：由事实表和维度表租成。事实表通过外键和维度表关联。其中，事实表中的每一行都代表了一个时间，维度通常代表事件的对象(who)、 什么(what)、 地点(where)、 时间(when)、 方法(how)以及原因(why)。

##### Snowflake schema
和星型模型类似，但维度表被分拆为多个字表。