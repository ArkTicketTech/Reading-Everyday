compare/exchange操作 可以指定两个内存序，允许失败和成功的内存序不同。可能成功时使用 memory_order_acq_rel，失败时使用 memory_order_relaxed。由于失败的 compare/exchange操作不会进行存储，因此失败时不能使用 meory_order_release或memory_order_acq_rel，因此，不保证这些值能作为失败的顺序，也不提供比成功内存序更加严格的失败内存序。

* 如果没有制定失败的内存序，那么其和成功的语序一样。除了 release 部分的顺序，memory_order_release 变成 memory_order_relaxed，并且 memory_order_acq_rel 变成 memory_order_acquire。
* 如果都不指定，则默认的内存序为 memory_order_seq_cst。
以下两次调用等价：
```
std::atomic<bool> b;  
bool expected; b.compare_exchange_weak(expected,true,

memory_order_acq_rel,memory_order_acquire); b.compare_exchange_weak(expected,true,memory_order_acq_rel);
```
