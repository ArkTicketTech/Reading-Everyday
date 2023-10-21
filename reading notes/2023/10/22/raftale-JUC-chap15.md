JUC包的许多类中，例如Semaphore和ConcurrentLinkedQueue，都提供了比synchronized机制更高的性能和可伸缩性。本章将介绍这种性能提升的主要来源：原子变量和非阻塞的同步机制。

非阻塞算法用底层的原子机器指令（CAS）代替锁来确保数据在并发访问中的一致性。非阻塞算法被广泛地用于在操作系统和JVM中实现线程/进程调度机制、垃圾回收机制以及锁和其他并发数据结构。

非阻塞方法在可伸缩性和活跃性有着巨大的优点：

1. 能够在粒度更细的层次上进行协调，能极大的减少调度开销；
2. 不存在死锁和活跃性问题；

# 锁的劣势

独占锁在并发场景始终面临着一系列劣势：

1. 独占锁，竞争失败的线程会被挂起并等待恢复（现代智能JVM不一定会挂起，可能会选择自旋等待），竞争激烈的情况下这种上下文切换开销非常大。
2. volatile具有可见性，但不支持原子性操作；
3. 自增操作不是原子性的，加锁可以保证原子操作；
4. 线程在等待锁的过程中不能做其他事情；

锁始终是开销较大的操作，幸好的是现代处理器提供了一种粒度更细的技术，类似于volatile变量的机制，同时还要支持原子的更新操作。

# 硬件对并发的支持

独占锁是一种悲观的技术，对于细粒度的操作，还有一种更高效的方法：乐观锁。借助冲突检测机制来判断在更新过程中是否存在来自其他线程的干扰，如果存在，这个操作将失败，并且可以重试，当然也可以不重试。

在针对多处理器操作而设计的处理器提供了一些特殊指令，用于管理对共享数据的并发访问，几乎所有的现代处理器都包含了某种形式的原子读-改-写指令，例如compare-and-swap或者load-linked/storeConditional，操作系统和JVM就是使用这些指令来实现锁和并发的数据结构。

## 比较与交换

在大多数处理器架构中采用的方法是实现一个比较并交换（CAS）指令。CAS包含了三个操作数，需要读写的内存位置V、进行比较的值A，和预期写入的新值B，当且仅当V的值等于A时，CAS才会通过原子方式用新值来更新V的值，否则不会执行任何操作。该语义的模拟代码如下：

```java
public class SimulatedCAS {
    private int value;
    public synchronized int get() {
        return value;
    }
    public synchronized int compareAndSwap(int expectedValue, int newValue) {
        int oldValue = value;
        if(oldValue = expectedValue) {
            value = newValue;
        }
    }
    public synchronized boolean compareAndSet(int expectedValue, int newValue) {
        return expectedValue == compareAndSet(expectedValue, newValue);
    }
}
```

当多个线程尝试使用CAS同时更新同一个变量时，只有其中一个线程能更新变量的值，其他的线程将失败。然而，失败的线程并不会被挂起， 而只是失败返回，可以再次尝试、或者执行一些恢复操作、或者什么也不执行（因为它已经知道值被修改了）。
