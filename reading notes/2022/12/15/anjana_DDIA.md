## Chapter 7: Transactions事务

事务

* 将应用程序的多个读、写操作捆绑在一起成为`一个逻辑操作单元`
* 针对多个对象、多个操作

NoSQL：并不具有事务语义。leaving the database in a partially update state.

弱隔离（非串行化）级别：

* Read-Uncommited:
  * 只防止脏写，不防止脏读
* Read-Commited:
  * 防止脏读：读DB时，只能看到已成功提交的data
  * 防止脏写：写DB时，只会覆盖已成功提交的data