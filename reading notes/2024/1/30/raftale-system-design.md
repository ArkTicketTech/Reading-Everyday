# Single server setup



## Database


1. releational database：MySQL, Oracle, PostgreSQL
2. non-relational database: CouchDB, Neo4j, Cassandra, HBase, Amazon DynamoDB

1. four categories

1. key-value stores: redis
2. graph stores: Neo4j
3. column stores:
4. document stores:

1. join operations are generally not supported

when to choose non-relational databases?

1. 需要低延迟
2. 数据非结构化
3. 只需要序列化和反序列化数据
4. 需要存储大量的数据

# Vertical scaling vs horizontal scaling

vertical scaling: scale up, add more power(CPU, RAM, etc) to your servers

horizontal scaling: scale out, add more servers



垂直扩容的缺点很明显，垂直有上限，水平扩容更有扩展性。

# Load Balancer


# Database replication

一般是主从架构，主节点负责读写，副本只负责读。


# Cache






1. 何时使用缓存：当数据读取频繁但修改不频繁时可以考虑使用
2. 过期策略：缓存使用规范中提到，一定要设置过期时间，过期时间不宜过长也不易过短
3. 一致性：
4. 单点故障
5. 回收策略：LRU



# Content delivery network(CDN)

CDN是一个地理分散的服务的服务器网络，用于存储静态内容，比如一些图片、视频、JS文件等。



使用CDN需要考虑：

1. 成本：不经常使用的数据没有收益，徒增成本
2. 合适的过期时间
3. CDN发生故障
4. 无效文件



CDN相当于是将一些静态文件（一般是不变的大文件）放到离用户近的地方，加速用户访问。








# Stateless web tier

尽量保持web服务的无状态性，无状态更健壮、更适合水平扩展，状态存储到数据库中。





# Data Centers


多数据中心的问题：

1. 流量路由：根据用户所在地区路由到最近的数据中心
2. 数据同步：来自不同地区的用户可能使用不同的本地数据库或缓存，用户的流量可能到另外一个数据中心，比如故障转移。常见的策略是在多个数据库中心之间复制数据。
3. 测试和部署



# Message queue

解耦使得消息队列称为构建可扩展和可靠应用程序的首选架构。

# Loging, metrics, automation





# Database scaling



## Horizontal scaling

分库分表



# Millions of users and beyond

扩容是一个迭代的过程，总结一下支撑高性能的百万用户的系统应该有的特质：

1. web服务无状态
2. 每一层构建冗余
3. 尽可能的缓存数据
4. 数据多中心
5. CDN托管静态资产
6. 数据库分库分表
7. 微服务拆分
8. CI/CD等工具自动化生产流程，提高效率
9. 可观测性：方便定位排查问题
