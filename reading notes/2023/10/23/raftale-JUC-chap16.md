Java内存模型简介
Java内存模型是通过各种操作来定义的，包括对变量的读/写操作，监视器的加锁/解锁，以及线程的启动和合并操作。JVM为程序中所有的操作定义了一个偏序关系，称之为Happens-Before。要想保证执行操作B的线程看到A的结果，无论A和B是否在一个线程中，那么A和B之间必须满足Happens-Before关系。如果两个操作之间缺乏Happens-Before关系，那么JVM可以对它们进行重排序。

Java内存模型下天然的Happens-Before的规则有：
1. Program Order Rule：如果控制流（非代码中）操作A在操作B之前，那么A happens-before B。
2. 监视器锁：在监视器锁上的解锁操作必须在同一监视器锁上的加锁操作之前执行
3. volatile变量规则：对volatile变量的写入操作必须在对该变量的读操作之前执行；
4. 线程启动规则：线程上对Thread.start的调用必须在该线程中执行任何操作之前执行
5. 线程结束规则：线程中的任何操作都先于发生于对此线程的终止检测，我们可以通过Thread.join()， isAlive()的返回值等手段检测线程是否已经终止运行。
6. 线程中断规则：对线程interrupt()方法的调用先发生于被中断线程的代码检测到中断事件的发生，可以通过Thread::interrupted()等方法检测到是否有中断发生。
7. 终结器规则：一个对象的初始化完成（构造函数执行完成）先发生于它的finalize()方法的开始。
8. 传递性： If A happens-before B, and B happens-before C, then A happens-before C.
   Java无须任何同步手段保障就能成立的happens-before有且只有这八条规则。
   在描述happens-before原则时，可以使用「后续（subsequent）的锁获取操作」、「后续的volatile变量读取操作」等表达术语。
