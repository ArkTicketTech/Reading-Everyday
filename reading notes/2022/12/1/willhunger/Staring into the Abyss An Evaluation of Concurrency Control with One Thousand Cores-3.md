# Staring into the Abyss: An Evaluation of Concurrency Control with One Thousand Cores

## 4. Design Choices & Optimization

#### 4.1 General Optimizations

* Memory Allocation：malloc 速度慢，采用每个线程管理自己的内存池来进行优化。
* Lock Table：不采用中心化的 lock table 和 timestamp manager，细粒度锁 tuple。
* Mutexes：2PL 的主要开销是死锁检测；T/O 算法的主要开销是申请分配唯一的 timestamp。

