3.1 共享锁和排他锁InnoDB实现了两种标准的行级别锁定，分别是共享（S）锁和排他（X）锁。共享锁允许读事务持有。如果一个事务对数据R添加共享锁后，其他事务就可以在数据 R上立即获得共享锁，这种情况就是锁兼容。但如果其他事务想要在事务R上添加排他锁则会处于等待状态，这种情况称为锁冲突。排他锁允许写事务持有。如果一个事务对数据R添加排他锁后，其他事务就不能立即获得共享锁和排他锁。
