Amdahl定律通过系统中并行化和串行化的比重来描述多处理器系统能获得的运算加速能力，摩尔定律则用于描述处理器晶体管数量与运行效率之间的发展关系。这两个定律的更替代表了近年来硬件发展从追求处理器频率到多核心并行处理的发展过程。

每秒事务处理数（TPS）代表着一秒内服务端平均能响应的请求总数。TPS一定程度上代表了程序的并发能力。

本章将讲述虚拟机如何实现多线程、多线程之间由于竞争和共享数据而导致的一系列问题和解决方案。

# 硬件的效率与一致性

现代计算机系统加入了一层基于高速缓存的存储交互，解决了处理器与内存速度之间的矛盾。但也带来了新的问题：缓存一致性。多路处理器系统中，每个处理器都有自己的高速缓存，而它们又共享同一主内存，这种系统称为共享内存多核系统。为了解决缓存一致性的问题，需要根据协议来读写，这类协议有：MSI、MESI、MOSI等。


内存模型：特定的操作系统下，对特定的内存或者高速缓存进行**读写访问过程的抽象**。不同的架构有不同的内存模型，JVM也有自己的内存模型。

除了高速缓存之外，为了使处理器内部的运算单元能尽量被充分利用，处理器可能会进行重排序。JVM的JIT也有指令重排序的优化。

# JAVA内存模型

## 主内存与工作内存

Java内存模型的主要目的是定义程序中各种变量的访问规则，即关注在虚拟机中把变量存储到内存和从内存中取出变量值这样的底层细节。此处的变量不是指java中的变量，而是JVM中的，它包括了实例字段、静态字段和构成数组对象的元素等共享的变量。

Java内存模型规定了所有的变量都存储在主内存，每条线程还有自己的工作内存。线程的工作内存中保持了被该线程使用的变量的主内存副本，线程对变量的所有操作都必须在工作内存中进行，它不能直接读写主内存中的数据。不同线程之间也无法直接访问其他线程的工作内存。

## 内存间交互操作

主内存和工作内存之间具体的交互协议，是依靠定义的8中原子操作来完成的。

1. lock
2. unlock
3. read: 把一个变量的值从主内存传输到工作内存，以便随后的load使用
4. load：read后的变量放到工作内存
5. use：它把工作内存的变量值传递给执行引擎
6. assign：执行引擎的赋值操作
7. store：工作内存的变量值传递给主内存，以便随后的write使用
8. write：将变量放入到主内存中


## 对于volatile型变量的特殊规则

volatile保证了变量的可见性，可见性是指此变量的值一旦被当前修改，当前线程能立刻通知其他线程下次读取不再走缓存而是走主存。注意这里只是下次的读取从主存中获取，但已经读取到工作内存中的值并不会被修改。所以volatile只保证了可见性的语义，并没有保证原子性的语义，比如race++，它不是一个原子操作，而是一个复合操作：

1. 从主存中读取race的值；
2. 对值进行+1；
3. 写回race的值到主存中

如果有两个线程同时执行race++，这就是一个read-modify-write的竞态条件，所以必然是线程不安全的。

因此volatile的使用场景一般是：

1. 运算结果不依赖变量的当前值，或者能确保只有单一的线程能修改变量的值；
2. 变量不需要与其他的状态变量构成不变性条件。

内存屏障：https://bbs.huaweicloud.com/blogs/344101



volatile的另一个保证是禁止指令重排序优化。

```java
Map configurations;
char[] configText;
volatile boolean initialized = false;
// 线程A执行
configurations = new HashMap<>();
configText = readConfigFile(fileName);
processConfigOptions(configurations,configText);
initialized = true;

// 线程B执行
while(!initialized) {
    sleep();
}
doSomethingWithConfig();
```

如果volatile没有禁止重排序，那么`initialized = true`很有可能因为重排序在线程A中被提前执行，从而导致线程B续的doSomethingWithConfig出错。

这个例子说明的是，如果volatile仅仅只是保证可见性，那多线程执行下，重排序还是会导致出错，所以禁止重排序保证了底层代码的执行语义与程序代码提供的语义是一致的，也被称为「线程内表现为串行的语义」（within-thread as if serial Semantics）.

volatile的性能优于内置锁，也比普通变量的读写差的不多。
