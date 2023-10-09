本章将介绍对线程池进行配置和调优的一些高级选项，并分析在使用任务执行框架需要注意的各种危险，以及一些使用Executor的高级示例。

# 在任务和执行策略之间的隐性耦合

Executor框架，也就是线程池的使用场景不一定适合所有的任务，有些类型的任务需要明确地指定执行策略，包括：

1. 依赖性任务：如果提交给线程池的任务需要依赖其他的任务，那么就隐含地给执行策略带来了约束，可能产生活跃性问题
2. 使用线程封闭机制的任务：本来是单线程执行的任务，放开为多线程执行可能造成线程不安全
3. 对响应时间敏感的任务：线程池吞吐量不够时，响应时间敏感的任务则很容易对用户造成不满；
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

要想正确的设置的线程池的大小，必须具体分析。对于计算密集型任务，在拥有N_cpu个处理器的系统上，当线程池的大小设置为`N_cpu + 1`通常能实现最优的利用率。对于I/O密集型任务，需要线程数为![img](https://cdn.nlark.com/yuque/__latex/389d1a3395200243df1cf0efb4f4a89c.svg)。![img](https://cdn.nlark.com/yuque/__latex/4e315f1410f935ec95b7600c1f01d00d.svg)是CPU的利用率，可以通过visualVM监控到，![img](https://cdn.nlark.com/yuque/__latex/abb00a99af20cef0f23dd8bc67dc72ec.svg)是任务等待时间与计算时间的比值，可以看到等待时间越长远远大于计算时间时，需要线程数越多，反之需要线程数越小。

![img](https://cdn.nlark.com/yuque/__latex/6231be05df32019c331b1009565ac488.svg)

# 配置ThreadPoolExecutor

ThreadPoolExecutor为一些Executor提供了基本的实现，这些Executor是由Executors中的newCachedthreadPool, newFixedThreadPool等工厂方法返回的。ThreadPoolExecutor是一个灵活的、稳定的线程池，允许进行各种定制。

如果默认的执行策略不能满足，那么可以通过ThreadPoolExecutor的构造函数来示例化一个对象，并根据自己的需求来定制。

## 线程的创建与销毁

线程池的核心线程池大小，最大线程池大小以及存活时间等因素共同负责线程的创建与销毁，核心线程池在没有任务执行时也会保留下来，最大线程池大小只有在工作队列满了的情况下才会创建超出核心线程池大小的线程数量，存活时间是指超过核心线程池大小的线程在空闲时间下最多的存活时间。

## 管理队列任务

无限制创建线程将导致系统不稳定，固定大小的线程池有很好的控制性，但如果新请求的到达速率超过了线程池的处理速率，那么最终累积下来系统的响应性还是会降低，甚至在无界队列下耗尽资源。	`newFixedThreadPool`和`newSingleThreadExecutor`在默认情况下都使用的是一个无界的队列。

所以更稳妥的资源管理策略是**使用有界队列**，在使用有界的工作队列时，队列的大小和线程池的大小必须一起调节，如果线程池较小而队列较大，那么有助于**减少内存使用量，降低CPU的使用率**，同时还可以**减少上下文切换**，但付出的代价是可能会限制**吞吐量**。有界队列能避免资源耗尽的情况发生，但它又带来了新的问题，队列满了，新的任务该如何处理？有许多饱和策略可以解决这个问题。饱和策略在下一小节再讲。

对于非常大的或者无界的线程池，可以通过使用SynchonousQueue来避免任务排队，以及直接将任务从生产者移交给工作者线程。

先进先出的队列如果不满足要求，还可以使用优先队列PriorityBlockingQueue，这个队列将根据优先级来安排任务，默认优先级是自然顺序，或者可以自己实现Comparator。

只有当任务相互独立时，为线程池或工作队列设置界限才是合理的，如果任务之间存在依赖性，那么有界将可能导致线程饥饿死锁问题。

## 饱和策略

当有界的队列被填满后，饱和策略开始发挥作用。ThreadPoolExecutor的饱和策略可以通过调用setRejectedExecutionHandler来修改，JDK实现了几种不同的RejectedExecutionHandler实现，每种实现都包含了不同的饱和策略：AbortPolicy， DiscardPolicy，CallerRunPolicy，

```java
public class ThreadPoolExecutor extends AbstractExecutorService{ /**
     * A handler for rejected tasks that throws a
     * {@link RejectedExecutionException}.
     * This is the default handler for {@link ThreadPoolExecutor} and
     * {@link ScheduledThreadPoolExecutor}.
     */
  public static class AbortPolicy implements RejectedExecutionHandler {
    public AbortPolicy() { }

    public void rejectedExecution(Runnable r, ThreadPoolExecutor e) {
      throw new RejectedExecutionException("Task " + r.toString() +
                                           " rejected from " +
                                           e.toString());
    }
  }

  /**
     * A handler for rejected tasks that silently discards the
     * rejected task.
     */
  public static class DiscardPolicy implements RejectedExecutionHandler {
    public DiscardPolicy() { }

    /**
     * Does nothing, which has the effect of discarding task r.
     *
     * @param r the runnable task requested to be executed
     * @param e the executor attempting to execute this task
     */
    public void rejectedExecution(Runnable r, ThreadPoolExecutor e) {
    }
  }

  public static class DiscardOldestPolicy implements RejectedExecutionHandler {
    public DiscardOldestPolicy() { }

    /**
     * Obtains and ignores the next task that the executor
     * would otherwise execute, if one is immediately available,
     * and then retries execution of task r, unless the executor
     * is shut down, in which case task r is instead discarded.
     *
     * @param r the runnable task requested to be executed
     * @param e the executor attempting to execute this task
     */
    public void rejectedExecution(Runnable r, ThreadPoolExecutor e) {
      if (!e.isShutdown()) {
        e.getQueue().poll();
        e.execute(r);
      }
    }
  }
  public static class CallerRunsPolicy implements RejectedExecutionHandler {

    public CallerRunsPolicy() { }

    public void rejectedExecution(Runnable r, ThreadPoolExecutor e) {
      if (!e.isShutdown()) {
        r.run();
      }
    }
  }
}
```

中止策略AbortPolicy：默认的饱和策略，该策略将抛出未检查的RejectedExecutionException，调用者可以捕获这个异常，然后根据需求编写自己的处理代码。

抛弃策略DiscardPolicy：当新提交的任务无法保存到队列中等待执行时，该策略会**悄悄地抛弃掉该策略。**

抛弃最旧的策略DiscardOldestPolicy：当新提交的任务无法保存到队列中等待执行时，该策略会抛弃队列中下一个将被执行的任务（其实也就是最旧的未处理的任务，处于队头，所以是最旧的)，然后尝试提交新的任务。

调用者运行策略CallerRunPolicy：当新提交的任务无法保存到队列中等待执行时，该策略不会抛弃任务，也不会抛出异常，而是将任务回退给调用者执行，从而降低新任务的流量。回退的任务将由调用execute的主线程（tomcat中将是主线程http-nio-8081-exec-1, http-nio-8081-exec-1, ...），由于主线程执行任务需要一定的时间，因此大量的请求可能服务器的线程池处理不过来，那么请求的线程将会保存到TCP层的队列中，如果TCP过载，同样会开始抛弃请求。这种过载情况从线程池到工作队列 ，再到应用程序，再到TCP层，最终到达客户端，导致服务器在高负载情况下实现一种**平缓的性能降低**。



饱和策略是可以在创建Executor后进行修改的。



上面的饱和策略都是对任务的直接处理，比如抛出异常、悄悄丢弃、抛弃下一个要执行的任务、返回给主线程执行，但是没有阻塞生产者提交，如果要实现阻塞任务的到达率，可以使用信号量，信号量上界设置为线程池的大小加上可排队任务的数量。



```java
@ThreadSafe
public class BoundedExecutor {
    private final Executor exec;
    private final Semaphore semaphore;

    public BoundedExecutor(Executor exec, int bound) {
        this.exec = exec;
        this.semaphore = new Semaphore(bound);
    }

    public void submitTask(final Runnable command) throws InterruptedException {
        semaphore.acquire();
        try {
            exec.execute(new Runnable() {
                public void run() {
                    try {
                        commend.run();
                    } finally {
                        semaphore.release();
                    }
                }
            });
        } catch() {
            semaphore.release();
        }
    }
}
```

## 线程工厂

每当线程池需要创建一个线程时，都是通过线程工厂方法newThread来完成的。

```java
public class ThreadPoolExecutor {
  /**
   * Creates with given first task and thread from ThreadFactory.
   * @param firstTask the first task (null if none)
   */
  Worker(Runnable firstTask) {
    setState(-1); // inhibit interrupts until runWorker
    this.firstTask = firstTask;
    this.thread = getThreadFactory().newThread(this);
  }
}

public interface ThreadFactory {

    /**
     * Constructs a new {@code Thread}.  Implementations may also initialize
     * priority, name, daemon status, {@code ThreadGroup}, etc.
     *
     * @param r a runnable to be executed by new thread instance
     * @return constructed thread, or {@code null} if the request to
     *         create a thread is rejected
     */
    Thread newThread(Runnable r);
}
```

默认的线程工厂方法将创建一个新的、非守护的线程，并且不包含特殊的配置信息。

```java
public class Executor {
  public Thread newThread(Runnable r) {
    Thread t = new Thread(group, r,
                          namePrefix + threadNumber.getAndIncrement(),
                          0);
    if (t.isDaemon())
      t.setDaemon(false);
    if (t.getPriority() != Thread.NORM_PRIORITY)
      t.setPriority(Thread.NORM_PRIORITY);
    return t;
  }
}
```

许多情况下需要使用定制的线程工厂方法，例如，希望为线程池中的线程指定一个UncaughtExceptionHandler，或者实例化一个定制的Thread类用于执行调试信息的记录。或者修改线程的优先级或者守护状态等。

## 在调用构造函数后再定制ThreadPoolExecutor

在调用完ThreadPoolExecutor等构造函数后，仍然是可以通过设置函数来修改构造函数的参数。如果Executor是通过Executors等某个工厂方法创建的，那么可以将结果的类型转化为ThreadPoolExecutor，然后再设置构造参数。

```java
Executor exec = Executors.newCachedThreadPool();
if(exec instanceof ThreadPoolExecutor) {
    ((ThreadPoolExecutor) exec).setCorePoolSize(10);
} else {
    throw new AssertionError("oops, bad assumption");
}
```

# 扩展ThreadPoolExecutor

ThreadPoolExecutor是可扩展的，它提供了几个可以在子类化中改写的方法：beforeExecute、afterExecute，terminated，这些方法可以用于扩展ThreadPoolExecutor的行为。

1. 如果beforeExecute抛出一个异常，后续不会被执行；
2. 无论任务从run中正常返回，还是抛出异常而返回，afterExecute都会被执行；但如果任务任务完成后带有一个Error，afterExecute不会被执行；
3. beforeExecute、afterExecute中可以添加日志、计时、监视或统计信息收集的功能
4. 在线程池完成关闭操作时调用terminated，也就是在所有任务都已经完成并且所有工作者线程也已经关闭后，terminated可以用来释放Executor在其生命周期里分配的各种资源，此外还可以执行发送通知、记录日志或者收集finalize统计信息等操作。

## 示例：在线程池中添加统计信息

自定义线程池TimingThreadPool，重写beforeExecute、afterExecute、terminated，然后记录任务执行的平均时间。

```java
import java.util.Random;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicLong;
import java.util.logging.Logger;
class TimeTask implements Runnable {

  private final static Random random = new Random(20);
  final int id;
  public TimeTask(int id) {
    this.id = id;
  }

  @Override
  public String toString() {
    return "TimeTask{" +
    "id=" + id +
    '}';
  }

  @Override
  public void run() {
    for(int i = 0; i < 100; i++) {
      try {
        TimeUnit.MILLISECONDS.sleep(random.nextInt(60));
      } catch (InterruptedException e) {

      }
    }
  }
}
public class TimingThreadPool extends ThreadPoolExecutor  {

  private final ThreadLocal<Long> startTime = new ThreadLocal<>();
  private final Logger log = Logger.getLogger("TimingThreadPool");
  private final AtomicLong numTasks = new AtomicLong();
  private final AtomicLong totalTime = new AtomicLong();

  public TimingThreadPool() {
    super(2, 2, 1, TimeUnit.SECONDS, new LinkedBlockingQueue<>(10));
  }

  @Override
  protected void beforeExecute(Thread t, Runnable r) {
    super.beforeExecute(t, r);
    log.info(String.format("Thread %s: start %s", t, r));
    startTime.set(System.nanoTime());
  }


  @Override
  protected void afterExecute(Runnable r, Throwable t) {
    try {
      long endTime = System.nanoTime();
      long taskTime = endTime - startTime.get();
      numTasks.incrementAndGet();
      totalTime.addAndGet(taskTime);

      log.info(String.format("Thread %s: end %s, time = %dns", t, r, taskTime));
    } finally {
      super.afterExecute(r, t);
    }
  }

  @Override
  protected void terminated() {
    try {
      log.info(String.format("Terminated: avg time = %dns", totalTime.get() / numTasks.get()));
    } finally {
      super.terminated();
    }
  }

  public static void main(String[] args) {
    ThreadPoolExecutor executor = new TimingThreadPool();
    for(int i = 0; i < 5; i++) {
      executor.execute(new TimeTask(i));
    }
    executor.shutdown();
  }

}
```

# 递归算法的并行化

当串行循环的各个迭代操作彼此独立，并且每个迭代操作执行的工作量比管理一个新任务带来的开销更多，那么这个串行循环就适合并行化。

在一些递归设计中可以采用循环并行化的方法，递归算法中通常都会存在串行循环：

```java
public<T> void parallelRecursive(Executor exec, List<Node> nodes, Collection<T> results) {
    for(Node<T> node : nodes) {
        exec.execute(new Runnable() {
            public void run() {
                results.add(node.compute());
            }
        })
        parallelRecursive(exec, node.getChildren(), results);
    }
}

public Collection<T> getParallelResults(List<Node<T>> nodes) throws InterruptedException {
    ExecutorService exec = Executors.newCachedThreadPool();
    Queue<T> resultQueue = new ConcurrentLinkedQueue<>();
    parallelRecursive(exec, nodes, resultQueue);
    exec.shutdown();
    exec.awaitTermination(Long.MAX_VALUE, TimeUnit.SECONDS);
    return resultQueue;
}
```

## 示例：谜题框架

这项技术的一种强大应用就是解决一些谜题，这些谜题都需要找出一系列的操作从初始状态转换到目标状态，例如类似于搬箱子、Hi-Q，四色方柱和其他的棋牌谜题。

代码略

# 小结

本章将介绍对线程池进行配置和调优的一些高级选项，并分析在使用任务执行框架需要注意的各种危险，以及一些使用Executor的高级示例。

**只有当任务都是同类型并且相互独立时，线程池的性能才能到达最佳。**

如果任务依赖于其他任务，有界的线程池则可能产生线程饥饿死锁。



线程池的大小设置取决于被提交任务的类型以及所部署系统的特性，对I/O密集型任务来说，一般线程池的最优大小为：cpu的核数 * cpu的利用率 * (1 + 任务平均等待时间/任务计算时间)。等待时间远远大于计算时间时，需要的线程数越多，来减少任务的等待时间。



**ThreadPoolExecutor**实现了Executor任务框架，ThreadPoolExecutor是一个灵活的、稳定的线程池任务框架，并允许进行各种定制，包括核心线程池大小、最大线程大小、救急线程池的存活时间、工作队列、拒绝策略等。稳妥的资源管理策略是**使用有界队列，**不过要注意只有当任务相互独立时，为线程池或工作队列设置界限才是合理的，如果任务之间存在依赖性，那么有界将可能导致线程饥饿死锁问题。

当工作队列满时，需要有饱和策略来响应接下来的任务，常见的饱和策略有AbortPolicy、DiscardPolicy, DiscardOldestPolicy, CallerRunsPolicy，背压策略（阻塞生产者提交）可以使用Semaphore自己实现。

线程池创建线程都是通过线程工厂方法，线程工厂方法可以进行定制。



ThreadPoolExecutor还可以进行扩展，常见的扩展方法有：beforeExecute、afterExecute、terminated。用它们可以实现任务执行时间的监控。



当串行循环的各个迭代操作彼此独立，并且每个迭代操作执行的工作量比管理一个新任务带来的开销更多，那么这个串行循环就适合并行化。
