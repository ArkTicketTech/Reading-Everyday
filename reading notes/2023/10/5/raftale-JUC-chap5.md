# 同步工具类

同步工具类：用于协调多个相互协作的线程控制流

阻塞队列是一种独特的类，它不仅能作为保存对象的容器，还能协调生产者和消费者等线程之间的控制流，因为take和put等方法将阻塞，直到队列达到期望的状态。

同步工具类可以是任何一个对象，只要它根据其自身的状态来协调线程的控制流。阻塞队列可以作为同步同居类，其他类型的同步工具类还包括信号量（Semaphore）、栅栏（Barrier）以及闭锁（Latch）。

如果这些还无法满足需求，第14章有如何创建自己的同步工具类。

同步工具类是一组用于多线程编程的工具，它们提供了一些用于同步和通信的方法和类。通过使用同步工具类，我们可以实现线程的互斥访问、线程的等待和唤醒、线程的执行顺序控制等功能。

## 闭锁latch

latch是一种同步工具类，可以延迟线程的进度直到其达到终止状态。latch的作用相当于一扇门，latch单词的意思就是门闩，在latch到达结束状态之前，这扇门一直是关闭的，并且没有任何线程能通过，当达到结束状态后，这扇门会打开并允许阻塞的线程通过。当latch到达结束状态后，将一直会保持打开状态。latch可以用来确保某些活动直到其他活动都完成后才继续执行。例如：

1. 确保某个计算在其需要的所有资源都被初始化之后才继续执行
2. 确保某个服务在其依赖的所有其他服务都已经启动之后才启动
3. 等待直到某个操作的所有参与者都就绪后再继续执行



CountDownLatch是一种灵活的闭锁实现，它包括一个计数器，这个计数器表示需要等待的事件数量。

`countDonw()`递减计数器，表示一个事件已经发生了，而`await()`阻塞直到计数器减为0（或者等待中的线程中断、等待超时）。

现在有一个这样的需求，我们要测试n个线程并发执行某个任务所需要的总时间，实现如下：

```java
package net.jcip.examples;

import java.util.concurrent.*;

/**
 * TestHarness
 * <p/>
 * Using CountDownLatch for starting and stopping threads in timing tests
 *
 * @author Brian Goetz and Tim Peierls
 */
public class TestHarness {
  public long timeTasks(int nThreads, final Runnable task)
  throws InterruptedException {
    final CountDownLatch startGate = new CountDownLatch(1);
    final CountDownLatch endGate = new CountDownLatch(nThreads);

    for (int i = 0; i < nThreads; i++) {
      Thread t = new Thread() {
        public void run() {
          try {
            // 阻塞线程
            startGate.await();
            try {
              task.run();
            } finally {
              endGate.countDown();
            }
          } catch (InterruptedException ignored) {
          }
        }
      };
      t.start();
    }

    long start = System.nanoTime();
    // 计数器变为0，结束阻塞
    startGate.countDown();
    // 所有线程都执行完后结束阻塞，然后计算总时间
    endGate.await();
    long end = System.nanoTime();
    return end - start;
  }
}
```

## FutureTask

FutureTask也可以用作闭锁，FutureTask实现了Future语义，表现一种抽象的可生成结果的计算。

```java
public class FutureTask<V> implements RunnableFuture<V> {

    /** The underlying callable; nulled out after running */
    private Callable<V> callable;
    /** The result to return or exception to throw from get() */
    private Object outcome; // non-volatile, protected by state reads/writes
    /** The thread running the callable; CASed during run() */
    private volatile Thread runner;
    /** Treiber stack of waiting threads */
    private volatile WaitNode waiters;

    public FutureTask(Callable<V> callable) {
        if (callable == null)
            throw new NullPointerException();
        this.callable = callable;
        this.state = NEW;       // ensure visibility of callable
    }

    public FutureTask(Runnable runnable, V result) {
        this.callable = Executors.callable(runnable, result);
        this.state = NEW;       // ensure visibility of callable
    }
}
```

FutureTask表示的计算是通过Callable来实现的，相当于一种可生成结果的Runnable，并且可以处于以下三种状态：

1. Wating to run
2. running
3. completed

Future.get()的行为取决于任务的状态，如果任务已经完成，那么get会立即返回结果，否则get将阻塞直到任务进入完成状态，然后返回结果或抛出异常。FutureTask将计算结果从执行计算的线程传递到获取这个结果的线程，而FutureTask的规范确保了这种传递过程能实现结果的安全发布。

## 信号量

计数信号量（Counting Semaphore）用来

1. 控制同时访问某个特定资源的操作数量；
2. 同时执行某个指定操作的数量
3. 实现某种资源池
4. 对容器施加边界



Semaphore管理着一组虚拟的许可（permit），许可的数量可通过构造函数指定。在执行操作时可以首先获取许可（acquire），并在使用之后释放许可（release）。如果没有许可，那么获取许可将阻塞直到有许可。这非常类似令牌许可算法。

Semaphore可以实现资源池，例如数据库连接池。同时也可以将任何一种容器变成有界阻塞容器，容器的最大值就是信号量的计数值。

## 栅栏

栅栏（Barrier）类似于闭锁，它能阻塞一组线程直到某个事件发生。栅栏与闭锁的关键区别在于，所有线程必须同时到达栅栏位置，才能继续执行。**闭锁用于等待事件，而栅栏用于等待其他线程**。栅栏用于实现一些协议，例如价格家庭决定要在某个地方集合。

CyclicBarrier可以使一定数量的参与方反复地在栅栏位置汇集，它在并行迭代算法中非常有用：这种算法通常将一个问题拆分为一系列相互独立的子问题。当线程到达栅栏位置时将调用await方法，这个方法将阻塞直到所有的线程都到达栅栏位置。如果线程都到达了栅栏位置，栅栏位置将打开，此时所有线程都被释放，而栅栏将被重置以便下次使用。如果对await的调用超时，或者阻塞被中断，那么所有阻塞的await调用都将终止并抛出BrokenBarrierException。如果成功通过栅栏，awiat将为每个线程返回一个唯一的到达索引号。CyclicBarrier还可以使你将一个栅栏操作传递给构造函数，这是一个Runnable，当成功通过栅栏时会执行它。

```java
public class CyclicBarrier {
  /** The lock for guarding barrier entry */
  private final ReentrantLock lock = new ReentrantLock();
  /** Condition to wait on until tripped */
  private final Condition trip = lock.newCondition();
  /** The number of parties */
  private final int parties;
  /** The command to run when tripped */
  private final Runnable barrierCommand;
  /** The current generation */
  private Generation generation = new Generation();

  public CyclicBarrier(int parties, Runnable barrierAction) {
    if (parties <= 0) throw new IllegalArgumentException();
    this.parties = parties;
    this.count = parties;
    this.barrierCommand = barrierAction;
  }
  public int await() throws InterruptedException, BrokenBarrierException {
    try {
      return dowait(false, 0L);
    } catch (TimeoutException toe) {
      throw new Error(toe); // cannot happen
    }
  }

  /**
     * Main barrier code, covering the various policies.
     */
  private int dowait(boolean timed, long nanos)
  throws InterruptedException, BrokenBarrierException, TimeoutException {
    final ReentrantLock lock = this.lock;
    lock.lock();
    try {
      final Generation g = generation;
      // ...

      int index = --count;
      // 都达到栅栏位置，执行Runnalbe任务
      if (index == 0) {  // tripped
        boolean ranAction = false;
        try {
          final Runnable command = barrierCommand;
          if (command != null)
            command.run();
          ranAction = true;
          nextGeneration();
          return 0;
        } finally {
          if (!ranAction)
            breakBarrier();
        }

      }

      // loop until tripped, broken, interrupted, or timed out
      for (;;) {
        // ...
        if (g != generation)
          return index;
        // ...
      }
    } finally {
      lock.unlock();
    }
  }
}
```

下面介绍了计算细胞的自动化模拟，如Convey life game，这是个非常有趣的游戏，用于模拟生命的繁衍和凋亡，它的游戏规则为，令一个细胞或者空的位置周围的细胞为n个：

1. n > 3时，它将会死亡；
2. n <= 1时，它将会死亡；
3. 2 <= n <= 3，它将继续存活
4. n = 3，细胞将会繁衍在这个空位置

周围3细胞新生，2、3细胞存活：B3/S23。

```java
public class CellularAutomata {
  // 细胞的生存空间
  private final Board mainBoard;
  private final CyclicBarrier barrier;
  // 并行执行的线程
  private final Worker[] worker;

  public CellularAutomata(Board board) {
    this.mainBoard = board;
    int count = Runtime.getRuntime().availableProcessors();
    // 任务会在barrier打开后执行
    this.barrier = new CyclicBarrier(count, new Runnable() {
      public void run() {
        mainBoard.commitNewValues();
      }
    });
    this.workers = new Worker[count];
    for(int i = 0; i < count; i++) {
      // 将生存空间划分为互不干扰的空间，然后并行执行
      workers[i] = new Worker(mainBoard.getSubBoard(count, i));
    }
  }

  public class Worker implements Runnable {
    private final Board board;

    public Worker(Board board) {
      this.board = board;
    }

    public void run() {
      while(!board.hasConverged()) {
        for(int x = 0; x < board.getMaxX(); x++) {
          for(int y = 0; y < board.getMaxY(); y++) {
            board.setNewValue(x, y, computeValue(x, y));
          }
        }
      	// 第k次计算完成后等待所有线程执行完毕后再继续进行第k+1次计算
        try {
          barrier.await();
        } catch (InterruptedException ex) {
          return;
        } catch(BrokenBarrierException ex) {
          return;
        }
      }
    }

    private int computeValue(int x, int y) {
      // Compute the new value that goes in (x,y)
      return 0;
    }
  }

  public void start() {
    for(int i = 0; i < workers.length; i++) {
      new Thread(workers[i]).start();
    }
    mainBoard.waitForConvergence();
  }

  interface Board {
    int getMaxX();
    int getMaxY();
    int getValue(int x, int y);
    int setNewValue(int x, int y, int value);
    void commitNewValues();
    boolean hasConverged();
    void waitForConvergence();
    Board getSubBoard(int numPartitions, int index);
  }
}
```

另一种形式的栅栏为Exchanger，他是一种两方栅栏（Two-Party），各方在栅栏位置交换数据。



# 构建高效且可伸缩的结果缓存

几乎所有的服务器都会使用某种形式的缓存，缓存能够降低延迟，提高吞吐量，但却需要消耗更多的内存。

我们将开发一个高效且可伸缩性的缓存组件，用于改进一个高计算开销的函数。我们首先从HashMap开始，然后分析它的并发性缺陷，并讨论如何修改它们，再测试它们的性能和安全性：

1. 单纯的HashMap：但HashMap线程不安全
2. 同步加锁的HashMap：线程安全，但同步阻塞导致性能开销大，是hashMap的5倍
3. 使用并发容器类ConcurrentHashMap：线程安全，性能好，但
4. 使用FutureTask：线程安全，性能高
5. 使用



首先是单纯的HashMap，但HashMap不是线程安全的，当有多个线程并发插入数据时，可能产生数据覆盖。

```java
import java.util.HashMap;
import java.util.Map;
import org.apache.commons.lang3.concurrent.Computable;

public class MemoizerOfHashMap<A, V> extends Memoizer<A, V> {
  private final Map<A, V> cache = new HashMap<>();
  private final Computable<A, V> c;

  public MemoizerOfHashMap(Computable<A, V> c) {
    this.c = c;
  }

  public V compute(A arg) throws InterruptedException {
    V result = cache.get(arg);
    if (result == null) {
      result = c.compute(arg);
      cache.put(arg, result);
    }
    return result;
  }

  @Override
  protected int getCacheSize() {
    return cache.size();
  }
}
```



要确保线程安全，最直观的是使用同步加锁，一次只保证一个线程执行。

```java
import java.util.HashMap;
import java.util.Map;
import org.apache.commons.lang3.concurrent.Computable;

public class MemoizerOfSynHashMap<A, V> extends Memoizer<A, V> {

  private final Map<A, V> cache = new HashMap<>();
  private final Computable<A, V> c;

  public MemoizerOfSynHashMap(Computable<A, V> c) {
    this.c = c;
  }

  public synchronized V compute(A arg) throws InterruptedException {
    V result = cache.get(arg);
    if (result == null) {
      result = c.compute(arg);
      cache.put(arg, result);
    }
    return result;
  }

  @Override
  protected int getCacheSize() {
    return cache.size();
  }
}
```

compute在多个线程下只允许串行执行，如果某个计算非常耗时，那将阻塞所有其他的查询，将导致系统不可用。解决HashMap线程不安全的手段还可以使用并发容器ConcurrentHashMap来替代。

```java
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import org.apache.commons.lang3.concurrent.Computable;

public class MemoizerOfConcurrentMap<A, V> extends Memoizer<A, V> {

  private final Map<A, V> cache = new ConcurrentHashMap<>();
  private final Computable<A, V> c;

  public MemoizerOfConcurrentMap(Computable<A, V> c) {
    this.c = c;
  }

  public V compute(A arg) throws InterruptedException {
    V result = cache.get(arg);
    if (result == null) {
      result = c.compute(arg);
      cache.put(arg, result);
    }
    return result;
  }

  @Override
  protected int getCacheSize() {
    return cache.size();
  }
}
```

ConcurrentHashMap只是保证了内部状态的线程安全性，但是compute是一个复合操作「先检查结果是否存在缓存，不存在就计算，拿到结果后再放入缓存」，这里是没有保证原子性的， 可能出现同一个参数有多个线程都同时compute计算，当这个计算开销很大时，系统仍然可能阻塞住。所以我们希望通过某种方式来表示「线程A正在计算某个值」，当另一个线程来查询时，它能知道最高效的方法是等待线程A计算结束，然后再去查询缓存结果。FutureTask可以实现这个功能，它缓存的不是结果，而是任务，它将复合操作变成了「先检查任务是否在缓存中，不存在则放入缓存」，然后计算，结果会自动保存在计算任务中，然后只需要通过FutureTask.get的阻塞特性来等待计算完成，计算完成后再返回结果。

```java
import java.util.concurrent.Callable;
import java.util.concurrent.CancellationException;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.FutureTask;
import org.apache.commons.lang3.concurrent.Computable;

public class MemoizerOfFutureTask<A, V> extends Memoizer<A, V> {

  private final ConcurrentMap<A, Future<V>> cache = new ConcurrentHashMap<>();
  private final Computable<A, V> c;

  public MemoizerOfFutureTask(Computable<A, V> c) {
    this.c = c;
  }

  public V compute(final A arg) throws InterruptedException {
    Future<V> f = cache.get(arg);
    if (f == null) {
      Callable<V> eval = new Callable<V>() {
        public V call() throws InterruptedException {
          return c.compute(arg);
        }
      };
      FutureTask<V> ft = new FutureTask<V>(eval);
      f = ft;
      cache.put(arg, ft);
      ft.run();
    }
    try {
      return f.get();
    } catch (ExecutionException e) {
      throw LaunderThrowable.launderThrowable(e.getCause());
    }
  }

  @Override
  protected int getCacheSize() {
    return cache.size();
  }
}
```

可以看到，这种实现将复合操作的粒度减小了，之前复合操作中非常耗时的计算被剥离出来了，用了粒度更小的复合操作来做替代。利用FutureTask的阻塞特性解决了如何知道「线程A正在计算某个值」的问题，但compute中if代码块的非原子操作操作 变成了 「先检查任务是否在缓存中，不存在就将任务放入缓存」，因此多个线程仍有可能在同一事件调用compute来计算相同的值，当两者都没在缓存中找到期望的值时都会进行计算。


问题的根因是复合操作的原子性没有得到保证，如果我们将这个复合操作放到ConcurrentHashMap的原子方法`putIfAbsent(key, value)`中，JDK1.8后的`putIfAbsent`对哈希冲突的有锁，所以相同的key计算在同一时间就只会执行一次。

putIfAbsent: return the previous value associated with the specified key, or null if there was no mapping for the key


```java
public class MemoizerOfFutureTaskWithPutIfAbsent<A, V> extends Memoizer<A, V> {

  private final ConcurrentMap<A, Future<V>> cache = new ConcurrentHashMap<>();
  private final Computable<A, V> c;

  public MemoizerOfFutureTaskWithPutIfAbsent(Computable<A, V> c) {
    this.c = c;
  }
  @Override
  public int getCacheSize() {
    return cache.size();
  }

  public V compute(final A arg) throws InterruptedException {
    while (true) {
      Future<V> f = cache.get(arg);
      if (f == null) {
        Callable<V> eval = new Callable<V>() {
          public V call() throws InterruptedException {
            return c.compute(arg);
          }
        };
        FutureTask<V> ft = new FutureTask<V>(eval);
        // JDK1.8后，哈希冲突时还是阻塞锁
        f = cache.putIfAbsent(arg, ft);
        if (f == null) {
          f = ft;
          ft.run();
        }
      }
      try {
        return f.get();
      } catch (CancellationException e) {
        // 如果计算被取消，那么将把Future从缓存中移除，然后外层的while(true)保证重新进行计算
        cache.remove(arg, f);
      } catch (ExecutionException e) {
        throw LaunderThrowable.launderThrowable(e.getCause());
      }
    }
  }
}
```

测试代码：

```java
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import org.apache.commons.lang3.concurrent.Computable;

public class MemoizerTest {


  private final static String HASHMAP = "HASHMAP";
  private final static String SYN_HASHMAP = "SYN_HASHMAP";
  private final static String CONCURRENT_MAP = "CONCURRENT_MAP";

  private final static String FUTURE_TASK = "FUTURE_TASK";

  private final static String FUTURE_TASK_PUT_IF_ABSENT = "FUTURE_TASK_PUT_IF_ABSENT";


  private static final Computable<Integer, String> task = new Computable<>() {
    @Override
    public String compute(Integer arg) throws InterruptedException {
      return arg.toString();
    }
  };


  /**
   HashMap size: 6511865
   耗时：1841ms, 有时候会卡住

   Synchronized HashMap size: 5000000
   耗时：5910ms

   Concurrent HashMap size: 5000000
   耗时：1695ms

   Future_Task concurrent map size: 5000000
   耗时：1691ms

   PutIfAbsent concurrent map size: 5000000
   耗时：1545ms
   */
  public static void main(String[] args) {
    String type = FUTURE_TASK_PUT_IF_ABSENT;
    final Memoizer<Integer, String> memoizer;
    if (type.equals(HASHMAP)) {
      memoizer = new MemoizerOfHashMap<>(task);
    } else if (type.equals(SYN_HASHMAP)) {
      memoizer = new MemoizerOfSynHashMap<>(task);
    } else if (type.equals(CONCURRENT_MAP)) {
      memoizer = new MemoizerOfConcurrentMap<>(task);
    } else if (type.equals(FUTURE_TASK)) {
      memoizer = new MemoizerOfFutureTask<>(task);
    } else {
      // FUTURE_TASK_PUT_IF_ABSENT
      memoizer = new MemoizerOfFutureTaskWithPutIfAbsent<>(task);
    }


    int threadNumber = 10;
    ExecutorService executorService = Executors.newFixedThreadPool(threadNumber);
    CountDownLatch door = new CountDownLatch(1);
    CountDownLatch end = new CountDownLatch(threadNumber);

    long start = System.currentTimeMillis();
    for (int i = 0; i < threadNumber; i++) {
      executorService.submit(() -> {
        try {
          door.await();
          for (int k = 0; k < 500_0000; k++) {
            String value = memoizer.compute(k);
          }
        } catch (InterruptedException e) {
          throw new RuntimeException(e);
        } finally {
          end.countDown();
        }
      });
    }
    // 开门放行
    door.countDown();

    try {
      end.await();
    } catch (InterruptedException e) {
      throw new RuntimeException(e);
    }

    int size = memoizer.getCacheSize();
    if (type.equals(HASHMAP)) {
      System.out.println("HashMap size: " + size);
    } else if (type.equals(SYN_HASHMAP)) {
      System.out.println("Synchronized HashMap size: " + size);
    } else if (type.equals(CONCURRENT_MAP)) {
      System.out.println("Concurrent HashMap size: " + size);
    } else if (type.equals(FUTURE_TASK)) {
      System.out.println("Future_Task concurrent map size: " + size);
    } else {
      // FUTURE_TASK_PUT_IF_ABSENT
      System.out.println("PutIfAbsent concurrent map size: " + size);
    }

    System.out.println("耗时：" + (System.currentTimeMillis() - start) + "ms");
    executorService.shutdown();
  }

}
```



在apache-commons-lang中，有这个缓存器的实现，与最后一种方法是一样的：

```java
package org.apache.commons.lang3.concurrent;

import java.util.concurrent.Callable;
import java.util.concurrent.CancellationException;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.FutureTask;

public class Memoizer<I, O> implements Computable<I, O> {

  private final ConcurrentMap<I, Future<O>> cache = new ConcurrentHashMap<>();
  private final Computable<I, O> computable;
  private final boolean recalculate;

  public Memoizer(final Computable<I, O> computable) {
    this(computable, false);
  }

  public Memoizer(final Computable<I, O> computable, final boolean recalculate) {
    this.computable = computable;
    this.recalculate = recalculate;
  }

  @Override
  public O compute(final I arg) throws InterruptedException {
    while (true) {
      Future<O> future = cache.get(arg);
      if (future == null) {
        final Callable<O> eval = () -> computable.compute(arg);
        final FutureTask<O> futureTask = new FutureTask<>(eval);
        future = cache.putIfAbsent(arg, futureTask);
        if (future == null) {
          future = futureTask;
          futureTask.run();
        }
      }
      try {
        return future.get();
      } catch (final CancellationException e) {
        cache.remove(arg, future);
      } catch (final ExecutionException e) {
        if (recalculate) {
          cache.remove(arg, future);
        }

        throw launderException(e.getCause());
      }
    }
  }

  private RuntimeException launderException(final Throwable throwable) {
    if (throwable instanceof RuntimeException) {
      return (RuntimeException) throwable;
    } else if (throwable instanceof Error) {
      throw (Error) throwable;
    } else {
      throw new IllegalStateException("Unchecked exception", throwable);
    }
  }
}
```



# 总结



同步容器类：同步阻塞的容器类



同步容器类内部是线程安全的，也支持**客户端加锁**以便客户端在容器上进行一些**复合操作**。但内部**迭代器的设计并没有考虑线程安全性**，迭代器本身也是反复调用hasNext和next的**复合操作**来迭代对象，如果在迭代期间对容器加锁，可能产生死锁、容器规模很大时将长时间的阻塞，极大的降低了吞吐量和CPU的利用率，克隆一个大副本的开销也导致在克隆副本上进行迭代也不被考虑，还有很多隐藏的迭代器（equals, contains）都说明了在迭代器上保证线程安全是不现实的，目前迭代器的应对策略时「**及时失败**告警」。



并发容器：专门针对并发访问、经过性能优化的容器类，如`ConcurrentHashMap, CopyOnWriteList, ConcurrentLinedQueue,  ConcurrentSkipListMap`等。

CopyOnWriteList：写时复制一份新的数组；共享读



阻塞队列通过take和put阻塞方法简化了生产者-消费者的代码实现。



Thread提供了中断方法interrupt()，当阻塞方法被中断时会提前结束阻塞状态，并且将中断标记重置。



同步工具类：用于协调多个相互协作的线程控制流。

1. Latch：CountDownLatch，等待事件数满足设置阈值后才能执行，只能执行一次
2. FutureTask：任务未执行完成时进行阻塞
3. Semaphore：有许可才能执行操作
4. Barrier：Cyclicbarrier, 等待线程数满足设置阈值后才能执行，能反复进行触发执行



构建高效且可伸缩的结果缓存：

1. HashMap实现缓存，但HashMap线程不安全，不能在多线程环境下使用；
2. 同步方法串行保证方法原子性，但性能太差，Synchroinzed method
3. ConcurrentHashMap实现缓存，但「先检查是否存在缓存，计算，将结果放入缓存」不是原子性操作，存在重复计算的可能；
4. FutureTask(task)通过粒度更小的「先检查是否存在缓存，不存在缓存就将任务放入缓存」 复合操作，任务计算后结果会自动保存在任务中，通过FutureTask.get实现等待计算结果。
5. ConcurrentHashMap.putIfAbsent：「先检查是否存在缓存，不存在缓存就将任务放入缓存」 复合操作依然不是原子性的，依然存在重复计算的可能，用ConcurrentHashMap.putIfAbsent原子方法来使这个复合操作原子化。
