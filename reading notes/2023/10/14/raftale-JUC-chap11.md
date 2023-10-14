## 锁分段

Lock striping

把一个竞争激烈的锁分解为两个锁时，这两个锁可能都存在激烈的竞争。

在某些情况下，可以将锁分解技术进一步扩展为对**一组独立对象上的锁**进行分解，这种情况被称为锁分段。

例如，在 ConcurrentHashMap 的实现中使用了一个包含 16 个锁的数组，每个锁保护所有hash buckets 的 1/16，其中第 N 个散列桶由第 N %16 个锁来保护。假设 key 能实现均匀的分布，那相当于能够在高并发请求下将锁的请求减少到 1/16.

锁分段的劣势在于：与采用单个锁来实现独占访问相比，要获取多个锁来实现独占访问将更加困难并且开销更高，在某些情况下需要加锁整个容器，比如扩展映射范围，以及重新计算键值的散列值要分布到更大的桶集合中时，就需要获取分段锁集合中所有的锁。

使用锁分段技术的 Map 实现：

```java
@ThreadSafe
public class StripedMap {
    private static final int N_LOCKS = 16;
    private final Node[] buckets;

    private final Object[] locks;

    private static class Node {
        // 链表来保存冲突的hash
        Node next;
        Object key;
        Object value;
    }

    public StripedMap(int numBuckets) {
        buckets = new Node[numBuckets];
        locks = new Object[N_LOCKS];
        for(int i = 0; i < N_LOCKS; i++) {
            // 锁初始化
            locks[i] = new Object();
        }
    }

    private final int hash(Object key) {
        return Math.abs(key.hashCode() % buckets.length);
    }

    public Object get(Object key) {
        int hash = hash(key);
        synchronized(locks[hash % N_LOCKS)) {
            for(Node m = buckets[hash]; m != null; m = m.next) {
                if(m.key.equals(key)) {
                    return m.value;
                }
            }
        }
    }

    public void clear() {
        for(int i = 0; i < buckets.length; i++) {
            synchronized(locks[i % N_LOCKS]) {
                buckets[i] = null;
            }
        }
    }
    

    
}



@ThreadSafe
public class StripedMap {
    // Synchronization policy: buckets[n] guarded by locks[n%N_LOCKS]
    private static final int N_LOCKS = 16;
    private final Node[] buckets;
    private final Object[] locks;

    private static class Node {
        Node next;
        Object key;
        Object value;
    }


    public void clear() {
        for (int i = 0; i < buckets.length; i++) {
            synchronized (locks[i % N_LOCKS]) {
                buckets[i] = null;
            }
        }
    }
}
```



## 避免热点域

锁分解和锁分段技术都可以提高可伸缩性，因为它们都能使不同的线程在不同的数据（或者同一数据的不同部分）上操作，而不会相互干扰。如果程序采用锁分段技术，那么一定要表现出在锁上的竞争频率高于在锁保护的数据上发生竞争的频率，意思就是锁保护的某些数据并不是每次都真正参与了竞争，如果不是，那么这些数据是不是可以通过锁分段给隔离出来，比如一个锁保护了两个独立变量 X 和 Y，并且线程 A 想访问 X，线程 B 想访问 Y，那么这两个线程不会在任何数据上发生竞争，即使它们会在同一把锁上发生竞争。

但是当每个操作都请求多个变量时，锁的粒度就很难降低，一些常见的优化措施，例如将一些反复计算的结果缓存起来，但这会引入一些热点域，而这些热点域往往会限制可伸缩性。

举个例子，当实现 HashMap 时，你需要考虑如何在 size 方法中计算 Map 中的元素数量。最简单的方法就是，每次调用都统计一次元素的数量，但它的时间复杂度每次都是 O(N)，一个常见的优化是引入一个 size 变量的计数器，每次 put 和 remove 都操作这个计数器。单线程下它的性能很好，但却限制了可伸缩性，因为每次修改 map 的操作都需要更新这个共享的计数器，即使使用锁分段技术，那么在对计数器的访问进行同步时，也会重新导致在使用独占锁时存在的可伸缩性问题。在这种情况下，size 计数器也被称为热点域，因为每个导致元素数量发生变化的操作都需要访问它。

为了避免这个问题，ConcurrentHashMap 的 size 将对每个分段都维护了一个独立的计数器。



## 一些替代独占锁的方法

放弃使用独占锁，使用一种友好并发的方法来管理共享状态，例如，使用并发容器，读-写锁，不可变对象以及原子变量。

## 监控 CPU 的利用率

当测试可伸缩性时，需要确保处理器得到充分利用。vmstat 可以查看处理器的忙碌状态。

如果所有的 cpu 利用率并不均匀，不均匀的利用率表示大多数计算都是由一小组线程完成的，并且应用程序没有利用到其他的处理器。

如果 CPU 没有得到充分利用，通常有以下几种原因：

1. 负载不充分
2. I/O 密集：可以通过 iostat 来判断某个应用程序是否是磁盘 IO 密集型的，或者检测网络的通信流量级别来判断它是否需要高带宽。
3. 外部限制：如果应用程序依赖于外部服务，例如数据库或者其他服务，那么性能瓶颈可能不在本服务中。
4. 锁竞争：使用分析工具；或者通过 thread dump 。



vmstat 命令输出中，有一栏信息是当前处于可运行状态但并没有允许的线程数量，如果 CPU 的利用率很高，并且总有可运行的线程在等待 CPU，那么当增加更多的处理器时，程序的性能可能会得到提升。

## 向对象池说不

在并发程序中，对象池的表现会更加糟糕。如果线程从对象池中请求一个对象，那么就需要通过某种同步来协调对对象池数据结构的访问，从而可能造成线程阻塞。通常，对象分配操作的开销比同步的开销低，所以不要使用对象池技术。

# 示例：比较 Map 的性能

在单线程环境下，ConcurrentHashMap 的性能比同步的 HashMap 的性能略好一些，但在并发环境下则要好很多。ConcurrentHashMap 的实现中假设，大多数常用的操作都是获取某个已经存在的值，因为它对各种 get 操作进行了优化从而提高最高的性能和并发性。

在同步 Map 的实现中，可伸缩性的最主要阻碍在于整个 Map 只有一个锁，因此每次只能有一个线程访问这个 Map。不同的是，ConcurrentHashMap 对于大多数读操作不会加锁，在写入操作以及其他需要锁的操作中使用了锁分段技术。因此，多个线程能并发地访问这个 Map 而不会发生阻塞。

## 减少上下文切换的开销

当任务在运行和阻塞这两个状态之间转换时，就相当于一次上下文切换。

`System.out.println("")`是通过synchronized同步保证线程安全的，这意味着并发下只能串行执行，极大可能造成线程阻塞，如果多个线程都通过它来打印日志，那么就会增加请求的时间，因为日志操作的时间包括了线程阻塞的时间和 I/O 时间，阻塞加 IO 也极大的增加了上下文切换的开销。

```java
public final class System {
  public static final PrintStream out = null;

}
public abstract class InputStream implements Closeable {
  public void println(String x) {
    synchronized (this) {
      print(x);
      newLine();
    }
  }
}
```



请求服务的时间不应该过长，因为请求服务时间过长，会导致服务质量差，更重要的长时间更容易存在更多的锁竞争。

通过将 I/O 操作从处理请求的线程中分离出来，可以缩短处理请求的平均服务时间；线程只需要将日志消息放入队列就返回，put 操作非常轻量不容易产生阻塞，后台进行 IO 记录日志的线程也不会有锁的竞争，
