### 4.4 简化代码

#### 4.4.3  std::experimental::future
std::experiment 命名空间中提供了 std::experimental::future ：
* 为了给 future 添加持续性，只需在成员函数后添加 then() 即可。比如：给定 future fut，添加持续性的调用即为 fut.then(continuation)。
* std::experimental::when_all : 可以将需要等待的 future 传入 when_all 函数中，函数会返回新的 future，即当传入的 future 状态都为就绪时，新 future 的状态就会置为就绪，这个 future 可以和 fut.then 持续性 合起来处理其他任务。
* std::experimental::when_any : 将 future 收集在一起，当 future 有一个为就绪时，任务即为完成。

#### 4.4.7 锁存器和栅栏
1. std::experimental::latch：一种同步对象，当计数器减为 0 时，就处于就绪态。
2. std::experimental::barrier：当每个线程完成其处理任务时，都会到达栅栏 处，并且通过调用栅栏对象的arrive_and_wait成员函数，等待小组的其他线程。当最后一个线程抵达时，所 有线程将被释放，栅栏重置。组中的线程可以继续接下来的任务，或是处理下一个数据项，或是进入下一个 处理阶段。
3. std::experimental::flex_barrier：比 std::experimental::barrier 多了额外的构造函数，需要传入一个完成的函数和线程数量，当所有线程都到达了栅栏处，那么这个传入的函数就由其中一个线程运行。