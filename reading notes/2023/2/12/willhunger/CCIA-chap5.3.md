### 5.3.2 The happens-before relationship
happens-before 关系：指程序中基本构建块的操作顺序，指定了某个操作去影响另一个操作。
1. 在单线程情况下，指 sequenced-before；
	* 注意：逗号作用符，例如 `foo(++i, ++i);` 单一声明中是不可排序的，即没有  happens-before 关系。
2. 线程间的 happens-before 关系依赖于 synchronizes-with 关系。
	* happens-before 关系具有传递性
	* 线程间的 happens before 关系可以和 synchronizes-with 关系结合。例如：如果操作 A happens before B，且 B sequenced-before C，那么  A inter-thread happens before C。

strongly-happens-before 关系大致上和 happens-before 关系在大多数情况下是一致的，区别在于被标记为 memory_order_consume 的操作，不是 strongly-happens-before 关系。