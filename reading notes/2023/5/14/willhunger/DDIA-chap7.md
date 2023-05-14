## Chap7 Transactions

数据库存储环境中，可能出现以下错误情况，例如：
* 可用性问题：数据库随时可能失效，应用程序可能随时崩溃，应用和数据库节点之间、不同数据库节点之间的链接可能随时会终端。
* 并发带来的问题：多个客户端并发写数据；客户端读到了一些无意义或部分更新的数据；客户端由于边界条件竞争带来的奇怪问题。

事务定义：A transaction is a way for an application to group several reads and writes together into a logical unit. Conceptually, all the reads and writes in a transaction are executed as one operation: either the entire transaction succeeds (commit) or it fails (abort, rollback). If it fails, the application can safely retry. With transactions, error handling becomes much simpler for an application, because it doesn’t need to worry about partial failure—i.e., the case where some operations succeed and some fail (for whatever reason).

有了事务，应用程序可以不用考虑某些内部潜在的错误以及复杂的并发问题，可以直接将这些棘手难题交由数据库处理。即提供了安全性保证。
> 但某些数据库会为了更好的性能和可用性，会弱化事务处理或完全放弃事务。


本章内容：分析可能出错的场景，探讨数据库防范这些问题的方法和算法。