# Staring into the Abyss: An Evaluation of Concurrency Control with One Thousand Cores

## 4. Design Choices & Optimization

### 4.2 General Optimizations

* Deadlock Detection：运用死锁检测算法检测死锁时，多个线程在 centralized data structure 更新 wait-for graph 时需要互斥，本文通过按照 core 实现了 queue per thread 来对数据结构划分，实现了更新 wait-for graph 的无锁化。
* Lock Thrashing：锁抖动限制了高争用场景中的可伸缩性。
* Waiting vs. Aborting：trade-off between performance and the transaction abort rate，实验表明，适当增加 timeout threshold 可以减少锁抖动。

