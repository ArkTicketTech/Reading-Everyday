#### 5.3.4 释放队列与同步
一个操作链，存储操作标记为 memory_order_release， memory_order_acq_rel 或memory_order_seq_cst，加载标记为 memory_order_consum， memory_order_acquire或 memory_order_sqy_cst，并且操作链上的每一加载操作都会读取之前操作写入的值，因为操作链上构成了一个释放序列（release sequence），并且初始化存储同步(对应 memory_order_acquire 或 memory_order_seq_cst)或是前序依赖(对应memory_order_consume )的最终加载，这样，操作链上的任何原子“读-改-写”操作可以拥有任意个内存序(甚至是memory_order_relaxed)。

#### 5.3.5 Fences
Fences 操作会对内存序列进行约束，使其无法对任何数据进行修改，因此可以和 memory_order_relaxed 序的原子操作一起使用。
Fences 操作属于全局操作，执行 Fences 操作可以影响到在线程中的其它原子操作。由于自由操作可以使用编译器或硬件方式来对独立变量重排，但 Fences 操作可以限制这种自由。