**Multidimensional Analysis of NewSQL Database Systems**

**Abstract**

compare in terms of 

- supported data models
- transactionality features
- querying abilities

an overview of 

- scalability implementations
- functionality
- security assets and liabilities



**1 Introduction**

2011, NewSQL was coined by Matthew Aslett



**2 Related work and terms**

NewSQL: 

- the scalable performance of NoSQL system + the ACID guarantees
- the scalability of the NoSQL DBMSs + the relational model and transactional support of SQL DBMSs



**3 Methodology and criteria**

**Taxonomy 分类学**

1. DB-Engines Ranking score
2. Presence of the database in scientific papers in Google Scholar
3. Presence of the database in scientific papers in WebOfKnowledge

**Criteria**

Data Model

- Relational model
- Graph
- Document
- Time Series category: TSDBs(Time Series Databases): a series of timestamps and corresponding values 

Transactionality

Consistency levels 一致性级别和the types of replication of a database uses数据库使用的复制类型有关

同步协议：立即一致性/强一致性

异步协议/混合协议：临时一致性/弱一致性/最终一致性



referential integrity 参照完整性 

foreign keys 外键



Querying Capabilities



Scalability

vertical partitioning: disjoint sets of columns

horizontal partitioning: disjoint sets of rows: hash and range

Replication: replica control

Concurrency Control

In-memory capabilities



Security

Authentication 验证

Authorization 授权

Encryption 加密

server to server connections

data at rest 

client to server communication

Auditing 审计



Cost and infrastructural support



**4 Analysis**



5 Further investigation and expansion areas

5.1 Conclusions

There is no clear overview on how a migration can be easily made from a legacy system to a NewSQL database.



**5.2 Opportunities**

common terminology is not yet established or agreed upon.