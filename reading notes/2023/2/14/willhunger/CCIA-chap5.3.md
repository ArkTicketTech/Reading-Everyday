#### 5.3.3 原子操作的内存序
三种内存模型：
* sequentially consistent：memory_order_seq_cst
* acquire-release ordering：memory_order_consume, memory_order_acquire, memory_order_release, memory_order_acq_rel
* relaxed ordering : memory_order_relaxed

##### SEQUENTIALLY CONSISTENT ORDERING
* 顺序一致性，程序中的行为从任意角度去看，序列都保持一定顺序。
* 从同步的角度看，顺序一致的功能比排序约束更强大，使用顺序一致的原子操作，都会存储值后再加载。
* 简单直观，但是需要对所有线程进行全局同步，开销大，多处理器设备上在处理时需要在信息交换上消耗大量时间。

##### NON-SEQUENTIALLY CONSISTENT MEMORY ORDERINGS
* 在其它内存序上，不同的线程看到相同操作，不一定有相同的顺序；操作在其它线程上没有明确的顺序限制，不同的 CPU 及其内部缓冲区，都可能在同样的存储空间中存储不同的值，线程无法保证一致性。
* In the absence of other ordering constraints, the only requirement is that all threads agree on the modification order of each individual variable. Operations on distinct variables can appear in different orders on different threads, provided the values seen are consistent with any additional ordering constraints imposed.