我们平时在写 SQL 的时候，where 条件中会使用括号，也会出现多层 and、or 嵌套的情况，特别是使用各种 ORM 框架时，框架生成的 SQL 语句括号嵌套一层又一层，层峦叠嶂，非常壮观。

MySQL 中多层 where 条件会形成一棵树状结构，每多一个层级，都需要额外的逻辑处理，执行效率上会有一点影响，所以在语法分析阶段，就会对 where 条件的树状结构层级进行简化，可以合并的层级就合并。

上面说的树状结构，不是二叉树或多叉树实现的那种树结构，而是每一层的 Item_cond_and 或者 Item_cond_or 都包含一个子条件数组，而数组中的每个元素可能又是包含子条件数组的 Item_cond_and 或者 Item_cond_or，这样多层就会组成类似树状的结构。
