本章将介绍对线程池进行配置和调优的一些高级选项，并分析在使用任务执行框架需要注意的各种危险，以及一些使用Executor的高级示例。

# 在任务和执行策略之间的隐性耦合

有些类型的任务需要明确地指定执行策略，包括：

1. 依赖性任务：如果提交给线程池的任务需要依赖其他的任务，那么就隐含地给执行策略带来了约束，可能产生活跃性问题
2. 使用线程封闭机制的任务：
3. 对响应时间敏感的任务
4. 使用ThreadLocal的任务：在标准的Executor实现中，当执行需求较低时将回收空闲线程，而当需求增加时将添加新的线程，并且如果从任务中抛出了一个未检查异常，那么将用一个新的工作者线程来替代抛出异常的线程。所以只有当ThreadLocal的生命周期受限于任务的生命周期时，在线程池中的线程使用ThreadLocal才有意义，而在线程池中的线程中不应该使用ThreadLocal在任务之间传递值。

**只有当任务都是同类型并且相互独立时，线程池的性能才能到达最佳。**

1. 如果将运行时间较长与运行时间较短的任务混合在一起，那么除非线程池很大，否则将可能造成「拥塞」。
2. 如果提交的任务依赖于其他任务，那么除非线程池无限大，否则将可能造成死锁；

幸运的是，在基于网络的典型服务器应用程序中-网页服务器、邮件服务器以及文件服务器等，它们的请求都是同类型的并且相互独立的。

## 线程饥饿死锁

线程池中，如果任务依赖于其他任务，那么可能产生死锁。例如在单线程的Executor中，如果一个任务将另一个任务提交到同一个Executor，并且等待这个被提交任务的结果，那么就会引发死锁。第二个任务等待第一个任务的完成，但第一个任务又需要第二个任务的完成才能完成。在更大的线程池中，如果所有正在执行任务的线程都由于等待其他处于工作队列的任务而阻塞，那么就会发生线程饥饿死锁（Tread Starvation Deadlock）：线程池的中任务需要无限期地等待一些必须由池中其他任务才能提供的资源或者条件。



```java
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

public class ThreadDeadLock {
  ExecutorService exec = Executors.newSingleThreadExecutor();
  public class LoadFileTask implements Callable<String> {
    private final String fileName;

    public LoadFileTask(String fileName) {
      this.fileName = fileName;
    }

    public String call() throws Exception {
      // Here's where we would actually read the file
      return "";
    }
  }
  public class RenderPageTask implements Callable<String> {
    public String call() throws Exception {
      Future<String> header, footer;
      header = exec.submit(new LoadFileTask("header.html"));
      footer = exec.submit(new LoadFileTask("footer.html"));
      String page = renderBody();
      // Will deadlock -- task waiting for result of subtask
      return header.get() + page + footer.get();
    }
  }

  public String renderBody() {
    return "";
  }
  public void test() {
    // 线程提交了RenderPageTask的任务，但是还需要其他等待任务中的任务完成，造成了线程饥饿死锁，这种饥饿无法被jps检测到
    exec.submit(new RenderPageTask());
  }

  public static void main(String[] args) {
    ThreadDeadLock threadDeadLock = new ThreadDeadLock();
    threadDeadLock.test();
  }
}
```

## 运行时间较长的任务

如果任务阻塞的时间较长，即使不出现死锁，线程池的响应性也会很糟糕。执行时间较长的任务不仅会造成线程池阻塞，如果线程池的数量远小于稳定状态下执行时间较长的任务的数量，那么到最后可能所有的线程都在执行这些时间较长的任务，从而影响整体的响应性。

限定任务等待资源的时间可以缓解较长事务的影响，大多数可阻塞方法都定义了计时等待的版本，任务超时后可以选择放弃或是重新将任务放回队列。如果在线程池中总是充满了被阻塞的任务，那么也可能表明线程池的规模过小。

# 设置线程池的大小

线程池理想的大小取决于被提交任务的类型以及所部署系统的特性。

设置线程池的大小并不困难，要避免过大的线程和过小的线程设置，如果线程池过大，那么大量的线程池将在相对较少的CPU和内存资源上发生竞争，这不仅会导致更高的内存使用量，而且还可能耗尽资源；如果线程池过小，那么将导致许多空闲的处理器无法执行工作，降低吞吐率。

要想正确的设置的线程池的大小，必须具体分析。对于计算密集型任务，在拥有N_cpu个处理器的系统上，当线程池的大小设置为`N_cpu + 1`通常能实现最优的利用率。对于I/O密集型任务，需要线程数为![img](https://cdn.nlark.com/yuque/__latex/389d1a3395200243df1cf0efb4f4a89c.svg)。![img](https://cdn.nlark.com/yuque/__latex/4e315f1410f935ec95b7600c1f01d00d.svg)是CPU的利用率，可以通过visualVM监控到，![img](https://cdn.nlark.com/yuque/__latex/abb00a99af20cef0f23dd8bc67dc72ec.svg)是任务等待时间与计算时间的比值，可以看到等待时间越长、计算时间越短时，需要线程数越多。


# 配置ThreadPoolExecutor

ThreadPoolExecutor为一些Executor提供了基本的实现，这些Executor是由Executors中的newCachedthreadPool, newFixedThreadPool等工厂方法返回的。ThreadPoolExecutor是一个灵活的、稳定的线程池，允许进行各种定制。

如果默认的执行策略不能满足，那么可以通过ThreadPoolExecutor的构造函数来示例化一个对象，并根据自己的需求来定制。

## 线程的创建与销毁

线程池的核心线程池大小，最大线程池大小以及存活时间等因素共同负责线程的创建与销毁，核心线程池在没有任务执行时也会保留下来，最大线程池大小只有在工作队列满了的情况下才会创建超出核心线程池大小的线程数量，存活时间是指超过核心线程池大小的线程在空闲时间下最多的存活时间。
