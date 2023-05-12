在MySQL中，执行计划的实现是基于 JOIN 和 QEP_TAB 这两个对象。其中JOIN类表示一个查询语句块的优化和执行，每个select查询语句（即Query_block对象）在处理的时候，都会被当做JOIN对象，其定义在 sql/sql_optimizer.h 。

QEP_TAB 是 Query Execution Plan Table 的缩写，这里的表Table对象主要包含物化表、临时表、派生表、常量表等。 JOIN::optimize() 是优化执行器的统一入口，在这里会把一个查询语句块 Query_block 最终优化成 QEP_TAB 。

在 MySQL-8.0.22版本之后 ，又引入访问方式 AccessPath 和执行迭代器 Iterator 对象，再结合JOIN和QEP_TAB对象，最终得到整个解析计划的执行路径。
