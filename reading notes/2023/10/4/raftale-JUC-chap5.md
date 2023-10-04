# 阻塞方法与中断方法

当某个方法抛出`InterruptedException`时，表明该方法是一个**阻塞方法**，如果这个方法被中断，那么它将努力提前结束阻塞状态，并且将中断标记重置。该方法包括`sleep, wait, join, LockSupport.part, Blocking.take(), Blocking.put()`等。

Thread提供了`interrupt`**中断方法**，用于中断线程。每个线程都有一个boolean类型的属性，表示线程的中断状态，当中断线程时将设置这个状态。中断是一种协作机制，一个线程不能强制其他线程停止正在执行的操作而去执行其他的操作。当线程A中断B时，A仅仅要求B在执行到某个可以暂停的地方停止正在执行的操作，前提是如果线程B愿意停止下来。

当在代码里调用了一个抛出InterruptedException的方法时，你自己的方法也就变成了一个阻塞方法，并且必须要处理对中断的响应。一般有两种选择：

1. 传递InterruptedException：不捕获异常，继续向上抛；或者捕获该异常，然后在执行某个简单的清理工作后再次抛出这个异常。
2. 恢复中断：有时候不能抛出InterruptedException异常，例如当代码是Runnable的一部份时，Runnable本身不能抛出该异常，所以必须捕获InterruptedException，并调用`Thread.currentThread().interrupt()`恢复中断标记的状态。

# 同步工具类

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

另一种形式的栅栏为Exchanger，他是一种两方栅栏，各方在栅栏位置交换数据。
