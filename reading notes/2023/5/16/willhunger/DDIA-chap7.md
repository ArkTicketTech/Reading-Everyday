### The Slippery Concept of Transcation
起源：IBM 推出的第一个数据 System R，现阶段的主流关系型数据库 MySQL，PostgreSQL，Orcale，SQL Server 等系统实现的事务和 System R 非常相似。

NoSQL 的新起，新一代的数据库放弃了事务支持，或者将其重新定义，实现了事务的弱保证。

事务和可扩展性并非互斥。
本节分析了事务的优势和局限性，理解事务设计的权衡之道，考虑极端情况下，事务所提供的保证。