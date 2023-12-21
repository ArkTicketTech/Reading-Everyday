## 事务控制语句

1. start transaction/ begin：显式的开启一个事务
2. commit：commit work，提交事务
3. rollback：回滚
4. savepoint identifier：创建保存点
5. release savepoint identifier：删除保存点
6. rollback to identifier：回滚到保存点
7. set transaction： 设置隔离级别



## 隐式提交的SQL语句

以下SQL会产生隐式的提交操作：

1. DDL语句
2. 修改MySQL架构的操作：create user，grant等
3. 管理语句：analyze table等

DDL语句无法回滚。
