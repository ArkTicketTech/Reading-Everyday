# 11.30

## Chapter2 数据模型与查询语言

1、网络、层次模型——>关系模型(RDBMS+SQL)

NoSQL:Not only SQL

* 比RDB更好的拓展需求：very large datasets, very high write throughput
* free and open source software over commercial database products
* specialized query operations that are not well supported by the rational model
* for a more dynamic and expressive data model

2、ORM(Objected-Reational Mapping frameworks): ActiveRecord, Hibernate

3、Document-oriented Database: MongoDB, RethinkDB, CouchDB, Espresso

无模式（读时模式）：数据的结构是隐式的，只有在读取时才解释。类似于：动态（运行时）类型检查

写时模式（rational database）：模式是显式的，确保数据写入时都必须遵循。类似于：静态（编译时）类型检查

4、property graph: Neo4j, Titan, InfiniteGraph