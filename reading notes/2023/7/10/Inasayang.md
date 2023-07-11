sync标准库包中提供的并发同步技术

-   为了避免各种异常行为，最好不要复制sync标准库包中提供的类型的值



`sync.WaitGroup`（等待组）类型

-   每个`sync.WaitGroup`值在内部维护着一个计数，此计数的初始默认值为零
-   对于一个可寻址的`sync.WaitGroup`值`wg`
    -   `wg.Done()`和`wg.Add(-1)`是完全等价的
    -   如果一个`wg.Add(delta)`或者`wg.Done()`调用将`wg`维护的计数更改成一个负数，一个恐慌将产生
-   一个`*sync.WaitGroup`值的`Wait`方法可以在多个协程中调用，当对应的`sync.WaitGroup`值维护的计数降为0，这些协程都将得到一个（广播）通知而结束阻塞状态



`sync.Once`类型

-   每个`*sync.Once`值有一个`Do(f func())`方法。 此方法只有一个类型为`func()`的参数



`sync.Mutex`（互斥锁）和`sync.RWMutex`（读写锁）类型

-   一个`Mutex`值常称为一个互斥锁
-   一个`Mutex`值常称为一个互斥锁
-    一个（可寻址的）`Mutex`值`m`只有在未加锁状态时才能通过`m.Lock()`方法调用被成功加锁
-   一个`RWMutex`值常称为一个读写互斥锁，它的内部包含两个锁：一个写锁和一个读锁



Pp. 413-421