几种弱隔离级别

如果两个事务修改的数据没有交集，则可以安全的并发；否则，就会出现竞态条件。一旦出现并发 BUG，通常很难复现和修复。单客户端的并发已经足够麻烦，多客户端并发访问更加剧了并发问题。

数据库试图通过事务隔离（transaction isolation）来给用户提供一种隔离保证，从而降低应用侧的编程复杂度。最强的隔离性，可串行化（Serializability），可以对用户提供一种保证：任意时刻，可以认为只有一个事务在运行。

初学者对几种隔离级别的递进关系通常难以理解，往往是找不到一个合适的角度。我的经验是，从实现的角度对几种隔离级别进行理解，会简单一些。如 ANSI SQL 定义的四种隔离级别：读未提交（Read Uncommited）、读已提交（Read Commited）、可重复读（Repeatable Read）和可串行化（Serializability），可以从使用锁实现事务的角度来理解。
