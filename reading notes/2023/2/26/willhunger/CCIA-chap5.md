#### 5.3.6 原子操作对非原子的操作排序
Fences ，memory_order_release/memor_order_consume 也可以为非原子操作排序；这样，可以使用非原子操作，去代替 memory_order_relax 操作。

#### 5.3.7 非原子操作排序
非原子操作的排序，可以使用原子操作操作排序，但是需要保证非原子操作 sequence brfore 和 happens before 关系。
If a non-atomic operation is sequenced before an atomic operation, and that atomic operation happens before an operation in another thread, the non-atomic operation also happens before that operation in the other thread.