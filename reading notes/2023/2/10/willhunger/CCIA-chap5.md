### 5.3 同步操作和强制排序
原子类型通过 happens-before 和 synchronizes-with 两个操作来确定访问顺序。

#### 5.3.1 The synchronizes-with relationship
常见的同步操作场景：
* 假设原子写操作 W 对变量 x 进行操作，读取的是 W 操作写入值；
* 或者是 W 操作以后，读取同一线程上的原子些操作对 X 写入的值；
* 任意线程对 x 的一系列 `读-改-写` （read-modify-write）操作，例如 fetch_add() 或 compare_exchange_weak() 操作
