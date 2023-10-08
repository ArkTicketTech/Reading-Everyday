Java没有提供任何机制来安全的终止线程。但它提供了中断，这是一种协作机制，能够使一个线程终止另一个线程的当前工作。

# 任务取消

如果外部代码能在某个操作正常完成之前将其置入「完成状态」，那么这个操作就可以称为可取消的（Cancellable）。取消操作的原因有很多：

1. 用户取消请求；
2. 有时间限制的操作；
3. 应用程序事件；
4. 错误；
5. 关闭。

Java中没有一种安全的抢占式方法来停止线程，只有一些**协作式的机制**，使请求取消的任务和代码都遵循一种友好协商的协议。

其中一种协作机制就是「已请求取消」（Cancellation Requested）标记，任务将定期查看该标记，如果设置了这个标记，那任务就可以提前进行结束。下面的例子就使用了这个技术：

```java
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;

@ThreadSafe
public class PrimeGenerator implements Runnable {

  private static final ExecutorService exec = Executors.newCachedThreadPool();

  @GuardedBy("this") //只有持有了this锁才能进行访问
  private final List<BigInteger> primes = new ArrayList<>();
  private volatile boolean cancelled;

  static List<BigInteger> aSecondOfPrimes() throws InterruptedException {
    PrimeGenerator generator = new PrimeGenerator();
    System.out.println("start task...");
    exec.execute(generator);
    try {
      TimeUnit.SECONDS.sleep(1);
      System.out.println("sleep finished...");
    } finally {
      System.out.println("cancel...");
      generator.cancel();
    }

    return generator.get();
  }

  public static void main(String[] args) throws InterruptedException {
    List<BigInteger> bigIntegers = aSecondOfPrimes();
    System.out.println(bigIntegers.get(bigIntegers.size() - 1));
    exec.shutdown();
    System.out.println("-----");
  }

  public void run() {
    BigInteger p = BigInteger.ONE;
    while (!cancelled) {
      p = p.nextProbablePrime();
      synchronized (this) {
        primes.add(p);
      }
    }
    System.out.println("finish...");
  }

  public void cancel() {
    cancelled = true;
  }

  public synchronized List<BigInteger> get() {
    return new ArrayList<>(primes);
  }
}
```

这是一种简单的取消策略，客户代码通过识别标记来自己取消任务执行。

## 中断

简单凭借标记来取消任务会有一个问题，如果某个任务需要执行非常长的时间，或者任务本身在阻塞中（比如Blocking.put/take），那么取消机制会面临无法取消的问题，因为它可能无法执行到取消的代码中。所以Java中一些阻塞的库方法（如sleep, join, wait, Blocking.put/take, ...）都会检查线程的中断状态，并且在中断状态为true时提前响应，它们响应中断时执行的操作包括：清除中断标记，抛出InterruptedException异常，表示阻塞操作由于中断而提前结束，那么客户代码就知道已经提前中断了，然后选择自己的处理。

线程中断是一种协作机制，表示线程可以通过这种机制来通知另一个线程中止当前的任务。每个线程都有一个boolean类型的中断状态，这个状态在本地方法中被管理。当中断线程时，这个线程的中断状态将被设置为true。Thread类中包含了中断线程以及查询线程中断状态的方法。

1. interrupt(): 设置目标线程中断状态为true；
2. isInterrupted(): 返回目标线程的中断状态；
3. static interrupted(): 返回**当前**线程的中断状态，并清除**当前**线程的中断状态，即设置为false。注意这里是当前线程，而不是目标线程。

```java
public class Thread {
  // 中断线程
  public void interrupt() {
    if (this != Thread.currentThread()) {
      checkAccess();

      // thread may be blocked in an I/O operation
      synchronized (blockerLock) {
        Interruptible b = blocker;
        if (b != null) {
          interrupt0();  // set interrupt status
          b.interrupt(this);
          return;
        }
      }
    }
    // set interrupt status
    interrupt0();
  }
  // 返回目标线程的中断状态
  public boolean isInterrupted() {
    return isInterrupted(false);
  }
  // 清除当前线程的中断状态，并返回它之前的值
  public static boolean interrupted() {
    return currentThread().isInterrupted(true);
  }
  private native boolean isInterrupted(boolean ClearInterrupted);
  private native void interrupt0();
}

public interface Interruptible {
  public void interrupt(Thread t);
}
```

中断状态的设置并不意味着目标线程会立即停止目标线程正在进行的工作，它只是传递了请求中断的请求，目标线程甚至可以选择忽视，如果目标线程选择终止，它需要进行额外的取消处理。

中断机制解决了任务取消时简单标记无法处理阻塞方法的取消，使得阻塞方法可以通过抛异常提前返回。

```java
public class PrimeProducer extends Thread {

  private final BlockingQueue<BigInteger> queue;
  private volatile boolean cancelled = false;

  PrimeProducer(BlockingQueue<BigInteger> queue) {
    this.queue = queue;
  }

  public void run() {
    try {
      BigInteger p = BigInteger.ONE;
//      while (!Thread.currentThread().isInterrupted()) { // 查询中断提高响应
      while (!cancelled) {
        queue.put(p = p.nextProbablePrime());
      }
      System.out.println("task finish ...");
    } catch (InterruptedException consumed) {
      /* Allow thread to exit */
      System.out.println("producer interrupted, task finish...");
    }
  }

  public void cancel() {
    this.cancelled = true;
  }

  public static void main(String[] args) throws InterruptedException {
    int size = 10;
    BlockingQueue<BigInteger> primes = new ArrayBlockingQueue<>(size);
    PrimeProducer producerThread = new PrimeProducer(primes);
    producerThread.start();
    while (primes.size() != size) {

    }
    // 等待阻塞队列满载后，取消生生产者任务，会发现
//      primeProducer.cancel();
    // 向producerThread线程发送中断通知
    producerThread.interrupt();

    producerThread.join();
    System.out.println("任务结束");

  }
}
```

## 中断策略

正如任务中应该包含取消策略一样，线程同样应该包含中断策略。

最合理的中断策略是某种形式的线程级取消操作或者服务级取消操作：尽快退出，在必要时进行清理，通过某个所有者该线程已经退出。



线程拥有者的概念，我觉得可以理解为，创建线程的一方。比如线程池就是线程池中工作线程的拥有者

## 响应中断

响应InterruptedException中断异常无非两种方式：

1. 继续传递异常
2. 恢复中断状态，因为抛出InterruptedException时中断状态已经被清除掉了，所以需要恢复中断状态，这样调用栈上层代码能够对其中断状态进行判断，然后做适当的业务处理，比如退出任务。

恢复中断状态一般是调用interrupt再次进行中断，同时恢复中断状态也是有策略的，如果过早的设置中断状态，就可能引起无限循环。

有些场景下，某个方法需要循环调用可中断的阻塞方法，并在发现中断后重新尝试，但是该方法自身不支持取消（即不支持抛出InterruptedException）。需要注意的是，大多数可中断的阻塞方法都会在入口检查处检查中断状态，并且当发现该状态已经被设置为true时会立刻抛出`InterruptedException`，从Blocking.take()的代码可以看出，中断状态的检查在函数一开始就进行了。

```java
public class ArrayBlockingQueue {
  final ReentrantLock lock;
  public E take() throws InterruptedException {
    final ReentrantLock lock = this.lock;
    lock.lockInterruptibly();
    try {
      while (count == 0)
      notEmpty.await();
      return dequeue();
    } finally {
      lock.unlock();
    }
  } 
}

public class ReentrantLock {
  public void lockInterruptibly() throws InterruptedException {
    sync.acquireInterruptibly(1);
  }
}

public class Sync {
  public final void acquireInterruptibly(int arg)
  throws InterruptedException {
    // 返回中断状态，然后清除中断状态
    if (Thread.interrupted())
      throw new InterruptedException();
    if (!tryAcquire(arg))
      doAcquireInterruptibly(arg);
  }
}
```

这就意味着，恢复中断状态不能在捕获到`InterruptedException`时，如果在捕获到就恢复状态，那必然就是个死循环。

```java
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.TimeUnit;

class Task {
  private static int count = 0;

  private final int id = count++;

  @Override
  public String toString() {
    return "Task{" +
    "id=" + id +
    '}';
  }
}
public class NoncancelableTask implements Runnable{

  private BlockingQueue<Task> queue;

  NoncancelableTask(BlockingQueue<Task> queue) {
    this.queue = queue;
  }

  // 调用可中断阻塞方法，
  public Task getNextTask(BlockingQueue<Task> queue) {
    while(true) {
      try {
        return queue.take();
      } catch (InterruptedException e) {
        System.out.println("线程被中断后会重置中断状态-------" +  Thread.currentThread().isInterrupted());
        Thread.currentThread().interrupt();
        System.out.println("恢复中断状态后-------" +  Thread.currentThread().isInterrupted());

      }
    }
  }


  @Override
  public void run() {
    Task task;
    while(!Thread.interrupted()) {
      task = getNextTask(queue);
      if (task != null) {
        System.out.println("获取任务成功：" + task);
      } else {
        System.out.println("中断任务成功，任务获取结束---");
      }
    }
  }

  public static void main(String[] args) throws InterruptedException {
    BlockingQueue<Task> queue = new ArrayBlockingQueue<>(10);
    NoncancelableTask noncancelableTask = new NoncancelableTask(queue);

    for(int i = 0; i < 10; i++) {
      TimeUnit.MILLISECONDS.sleep(100);
      queue.put(new Task());
    }

    System.out.println("生产任务结束---");
    Thread thread = new Thread(noncancelableTask);
    thread.start();

    TimeUnit.SECONDS.sleep(2);
    System.out.println("获取任务take阻塞中---");
    System.out.println("中断任务");
    thread.interrupt();
  }
}
```

为了避免死循环，不应该在捕获InterruptedException就进行打断，而是使用一个局部变量来标记，然后在退出函数时重新恢复状态。

```java
public Task getNextTask(BlockingQueue<Task> queue) {
    boolean interrupted = false;
    try {
        // 这里似乎不需要while(true)也可以
      while (true) {
        try {
          return queue.take();
        } catch (InterruptedException e) {
          interrupted = true;
          // fall through and retry
          System.out.println("线程被中断后会重置中断状态-------" +  Thread.currentThread().isInterrupted());
          return null;
        }
      }
    } finally {
      if (interrupted) {
        // 如果这里不恢复中断状态，任务（run）就不会检测到中断状态，继续请求getNextTask就会陷入新的阻塞，但此时已经没有打断的请求了
        Thread.currentThread().interrupt();
        System.out.println("finally 恢复中断状态 => " + Thread.currentThread().isInterrupted());
      }
    }
  }
```

如果代码不会调用可中断的阻塞方法，那么仍然可以通过在任务代码中轮询当前线程的中断状态来响应中断。正如上面的getNextTask不支持抛出InterruptedException异常，所以外层在不停的轮询当前线程的中断状态来响应中断。

在取消过程中可能除了涉及中断状态之外的其他状态，例如一个由ThreadPoolExecutor拥有的工作线程检测到中断时，它会检查线程池是否正在关闭，如果是，它会在结束之前执行一些线程池清理工作，否则它可能创建一个新线程来将线程池恢复到合理的规模。

## 示例：计时运行

如何实现一个「在另一个独立线程运行的方法」在指定时间内返回，实现中需要面临两个问题：

1. 「在另一个独立线程运行的方法」运行期间出现异常不能被调用线程捕获；
2. 如何保证任务在指定时限前返回。

首先是第一个问题，如果在指定时限内抛出了一个Unchecked Exception，根据Java的异常处理原理原则，这个线程的异常不会被显式的处理，因为在Java中，线程是独立执行的代码片断，线程的问题应该由线程自己来解决，而不要委托到外部。所以多线程程序中，线程都不允许抛出未捕获Checked exception，线程需要处理自己的Checked Exception，对于Unchecked Exception，JVM会调用dispatchUncaughtException方法来寻找异常处理器(UncaughtExceptionHandler)处理异常，一旦发现该线程即刻终止，但不会影响其他线程。

```java
@FunctionalInterface
public interface Runnable {
	// 没有抛出异常，一旦线程抛出异常就不能被捕获，只能被JVM捕获
  public abstract void run();
}

public class Thread implements Runnable {
  /**
     * Dispatch an uncaught exception to the handler. This method is
     * intended to be called only by the JVM.
     */
  private void dispatchUncaughtException(Throwable e) {
    getUncaughtExceptionHandler().uncaughtException(this, e);
  }
}


public class ThreadGroup implements Thread.UncaughtExceptionHandler {

  public void uncaughtException(Thread t, Throwable e) {
    if (parent != null) {
      parent.uncaughtException(t, e);
    } else {
      Thread.UncaughtExceptionHandler ueh =
      Thread.getDefaultUncaughtExceptionHandler();
      if (ueh != null) {
        ueh.uncaughtException(t, e);
      } else if (!(e instanceof ThreadDeath)) {
        System.err.print("Exception in thread \""
                         + t.getName() + "\" ");
        e.printStackTrace(System.err);
      }
    }
  }
}
```

所以为了能让异常在调用线程中感知到，那就不能让任务由一个新的线程执行，而是让调用线程自己执行并处理异常。为了实现定时的效果，创建了一个外部线程，该线程使用定时器创建了一个定时取消任务，在超时后中断调用线程，这就解决了 「独立线程从任务中抛出未检查异常无法被调用线程检测到」和 「定时执行任务」的问题。

```java
package com.raftale.snippets.concurrency.chap7;

import java.math.BigInteger;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

class PrimeGeneratorScheduledProducer implements Runnable {

  private final List<BigInteger> primes = new ArrayList<>();
  @Override
  public void run() {
    BigInteger p = BigInteger.ONE;
    while (!Thread.interrupted()) {
      p = p.nextProbablePrime();
      synchronized (this) {
        primes.add(p);
      }
      if (primes.size() == 10) {
        throw new RuntimeException("超过10个，任务取消");
      }
    }
    System.out.println("线程被中断，任务取消");
  }

  public List<BigInteger> get() {
    return new ArrayList<>(primes);
  }
}

public class PrimeGeneratorScheduledConsumer {
  private static final ScheduledExecutorService cancelExec = new ScheduledThreadPoolExecutor(1);

  static void timedRun(Runnable r, long timeout, TimeUnit unit) {
    Thread taskThread = Thread.currentThread();
    System.out.println(taskThread.getName() + " 线程开始执行timedRun任务");
    // cancelExec会开启一个新的线程，也就是外部线程
    cancelExec.schedule(new Runnable() {
      @Override
      public void run() {
        taskThread.interrupt();
      }
    }, timeout, unit);
    // 直接运行，不会开启一个新的线程，会由当前Main线程运行
    r.run();

  }

  public static void main(String[] args) throws InterruptedException {
    // 任务
    PrimeGeneratorScheduledProducer primeTask = new PrimeGeneratorScheduledProducer();
    Thread t1 = new Thread(() -> {
      try {
        // 如果开一个新的独立线程t2执行任务，t1线程是无法捕获到t2抛出的异常的
//        new Thread(primeTask, "t2").start();
        // t1线程执行当前任务，然后内部由外部线程安排定时中断，就可以实现t1线程捕获到t1线程抛出的异常
        timedRun(primeTask, 1, TimeUnit.SECONDS);

        try {
          System.out.println(Thread.currentThread().getName() + " 线程还需要执行其他的任务");
          Thread.sleep(2);
        } catch (InterruptedException e) {
          System.out.println(
              Thread.currentThread().getName() + " 线程执行的其他不应当被中断的任务被中断");
          throw new InterruptedException();
        }
      } catch (RuntimeException e) {
        System.out.println(Thread.currentThread().getName() + " 线程捕获到timedRun任务异常：" + e);
        cancelExec.shutdown();
      } catch (InterruptedException e) {
        System.out.println("线程被中断"  + e);
        Thread.currentThread().interrupt();
      }
    }, "t1");
    t1.start();

    t1.join();
    List<BigInteger> primes = primeTask.get();
    System.out.println("任务结束，最终结果：" + primes);
  }
}
```



但这种实现方法会有副作用，它破坏了以下规则：在中断线程之前，应该了解它的中断策略。由于timedRun可以从任意一个线程中调用，因此它无法知道这个**调用线程**的中断策略。如果任务在超时之前完成，那么中断timedRun所在调用线程的取消任务还是会运行。我们不知道这种情况下调用线程还会运行什么代码，也许执行完timedRun后还有其他代码的运行（比如上面的线程t1执行完timedRun后，还需要执行其他的任务），那么taskThread.interrupt()中断线程就会导致这些其他的任务被中途中断，我们不知道这会带来什么意外的结果。但是如果任务不及时响应中断，那么timedRun就会超时返回，也并不符合该函数的预期。所以上面这种实现并没有真正解决问题。



为了能让定时任务真正的定时执行的同时又不影响调用线程的其他任务，一个思想是将该定时任务又独立的放到另一个独立的线程中，也就是在timedRun中，我们再创建一个新的线程`t_new`只执行这个单独的定时任务，定时中断的线程就变成了这个新的独立线程，同时为了能捕获到异常，传进来的定时任务被包括成一个新的任务和异常，查看这个异常，就能知道定时任务是否有异常发生。由于我们新建的线程`t_new`是一个内部的线程，所以定时器随时的中断它没有副作用产生。代码如下：

```java
public static void timedRun(final Runnable r, long timeout, TimeUnit unit) throws InterruptedException {
  class RethrowableTask implements Runnable {
    private volatile Throwable t;
    public void run() {
      try {
        r.run();
      } catch(Throwable t) {
        this.t = t;
      }

    }
    void rethrow() {
      if(null != t) {
        throw launderThrowable(t);
      }
    }
  }
  RethrowableTask task =  new RethrowableTask();
  final Thread taskThread = new Thread(task, "t_new");
  taskThread.start();
	cancelExec.schedule(new Runnable() {
    public void run() {
      // 这里的taskThread是新建的一个内部线程，中断它不会有副作用，
      // 而之前是中断的是当前线程，对外暴露的，所以不能随意中止
      taskThread.interrupt();
    }
  }, timeout, unit);
  
  taskThread.join(unit.toMillils(timeout));
  task.rethrow();
}
```

这个定时任务已经很完美了，但还有个不足，由于它依赖join的阻塞超时特性，因此存在着join的不足：无法知道执行控制是因为线程正常退出而返回还是因为join超时而返回，因为Join本身不会返回某个状态来摆明它是否成功，这是Thread API本身的缺陷。

## 通过Future来实现取消

我们已经使用了一种抽象机制`**Future**`来管理任务的生命周期，处理异常，以及实现取消。通常，使用现有库中的类比自己编写的更好，因此我们将继续使用Future和任务执行框架构建超时返回的`timeRun`方法。

```java
private static final ExecutorService taskExec = Executors.newCachedThreadPool();

public static void timedRun(Runnable r, long timeout, TimeUnit unit)
																				throws InterruptedException {
  Future<?> task = taskExec.submit(r);
  try {
    task.get(timeout, unit);
  } catch (TimeoutException e) {
    // task will be cancelled below
  } catch (ExecutionException e) {
    // exception thrown in task; rethrow
    throw launderThrowable(e.getCause());
  } finally {
    // Harmless if task already completed
    task.cancel(true); // interrupt if running
  }
}
```

ExecutorService.submit将返回一个Future来描述任务。

## 处理不可中断的阻塞

并非所有的可阻塞方法或者阻塞机制都能响应中断。如果一个线程由于执行同步的Socket I/O或者等待内置锁而阻塞，那么中断请求只能设置线程的中断状态，除此之外没有其他任何作用。

1. Java.io包中的同步Socket I/O：虽然InputStream和OutputStream中的read和write等方法都不会响应中断，但通过关闭底层的套接字，可以使由于执行read或write等方法而被阻塞的线程抛出一个SocketException。
2. Java.io包中的同步I/O：
3. Selector的异步I/O：
4. 获取某个锁：如果一个线程由于等待某个内置锁而阻塞，那么将无法响应中断，因为线程认为它肯定会获得锁，所以将不会理会中断请求。但是，Lock类提供了LockInterruptibly方法，允许在等待第一个锁的同时仍能响应中断。



下面的ReaderThread给出了如何封装非标准的取消操作。ReaderThread管理了一个socket连接，它采用同步的方式从socket中读取数据，并将接收到的数据传递给processBuffer。为了结束某个用户的socket或者关闭服务器，ReaderThread改写了interrupt方法，使其能处理标准的中断，也能关闭底层的socket。

```java
public class ReaderThread extends Thread {
  private final Socket socket;
  private final InputStream in;

  public ReaderThread(Socket socket) throw IOException{
    this.socket = socket;
    this.in = socket.getInputStream();
  }

  public void interrupt() {
    try {
      // 关闭socket
      socket.close();
    } catch(IOException ignored) {

    } finally {
      // 处理标准的中断
      super.interrupt();
    }
  }

  public void run() {
    try {
      byte[] byf = new byte[BUFSZ];
      while(true) {
        int count = in.read(buf);
        if(count < 0) {
          break;
        } else if(count > 0) {
          processBuffer(buf, count);
        }
      }
    }catch(IOException e) {
      // 允许线程退出
    }
  }
}
```

## 采用newTaskFor来封装非标准的取消

我们可以通过newTaskFor方法进一步优化ReaderThread中封装非标准取消的技术，这是ThreadPoolExecutor的功能。当把一个Callable提交给ExecutorService时，submit方法会返回一个Future，我们可以通过这个Future来取消任务。newTaskFor是一个工厂方法，它将创建Future来表示任务。newTaskFor还能返回一个RunnableFuture接口，该接口扩展了Future和Runnable，并有FutureTask实现。

通过定制表示任务的Future可以改变Future.cancel的行为。例如，定制的取消代码可以实现日志记录或者收集取消操作的统计信息，以及取消一些不响应中断的操作。通过改写interrupt方法，ReaderThread可以取消基于socket的线程。同样，通过改写任务的Future.cancel也可以实现类似的功能。



```java
import java.io.IOException;
import java.net.Socket;
import java.util.concurrent.*;

import net.jcip.annotations.*;

/**
 * SocketUsingTask
 * <p/>
 * Encapsulating nonstandard cancellation in a task with newTaskFor
 *
 * @author Brian Goetz and Tim Peierls
 */


interface CancellableTask <T> extends Callable<T> {
  void cancel();
  RunnableFuture<T> newTask();
}

public abstract class SocketUsingTask <T> implements CancellableTask<T> {
  @GuardedBy("this") 
  private Socket socket;

  protected synchronized void setSocket(Socket s) {
    socket = s;
  }

  // 
  public synchronized void cancel() {
    try {
      if (socket != null) {
        socket.close();
      }
    } catch (IOException ignored) {
    }
  }

  public RunnableFuture<T> newTask() {
    return new FutureTask<T>(this) {
      public boolean cancel(boolean mayInterruptIfRunning) {
        try {
          SocketUsingTask.this.cancel();
        } finally {
          return super.cancel(mayInterruptIfRunning);
        }
      }
    };
  }
}




@ThreadSafe
class CancellingExecutor extends ThreadPoolExecutor {
  public CancellingExecutor(int corePoolSize, int maximumPoolSize, long keepAliveTime, TimeUnit unit, BlockingQueue<Runnable> workQueue) {
    super(corePoolSize, maximumPoolSize, keepAliveTime, unit, workQueue);
  }

  public CancellingExecutor(int corePoolSize, int maximumPoolSize, long keepAliveTime, TimeUnit unit, BlockingQueue<Runnable> workQueue, ThreadFactory threadFactory) {
    super(corePoolSize, maximumPoolSize, keepAliveTime, unit, workQueue, threadFactory);
  }

  public CancellingExecutor(int corePoolSize, int maximumPoolSize, long keepAliveTime, TimeUnit unit, BlockingQueue<Runnable> workQueue, RejectedExecutionHandler handler) {
    super(corePoolSize, maximumPoolSize, keepAliveTime, unit, workQueue, handler);
  }

  public CancellingExecutor(int corePoolSize, int maximumPoolSize, long keepAliveTime, TimeUnit unit, BlockingQueue<Runnable> workQueue, ThreadFactory threadFactory, RejectedExecutionHandler handler) {
    super(corePoolSize, maximumPoolSize, keepAliveTime, unit, workQueue, threadFactory, handler);
  }

  protected <T> RunnableFuture<T> newTaskFor(Callable<T> callable) {
    if (callable instanceof CancellableTask)
      return ((CancellableTask<T>) callable).newTask();
    else
      return super.newTaskFor(callable);
  }
}
```



# 停止基于线程的服务

应用程序通常会创建拥有多个线程的服务，例如线程池，如果创建线程的服务准备退出，那线程也需要自行结束。

正确的封装原则是：只有拥有某个线程的线程才能对其拥有的线程进行操控，比如中断或修改优先级。线程由Thread类表示，并且像其他对象一样可以被自由共享，然后线程有一个相应的所有者，即创建该线程的类，因此线程池是其工作者线程的所有者，如果要中断这些线程，那么应该使用线程池。

与其他封装对象一样，线程的所有权是不可以传递的。服务应该提供生命周期方法来关闭它自己以及它拥有的线程，这样，当应用程序关闭服务时，服务就可以关闭所有的线程了。

在Executorservice中，提供了shutdown和shutdownNow等方法。

## 示例：日志服务

在大多数服务器应用程序上都会用到日志，在代码中插入println语句就是一种简单的日志。PrintWrite这样的字符流类是线程安全的，但它会给一些高容量的程序带来一定的性能开销。另一种日志策略是通过log方法将日志消息放到某个队列中，然后由其他线程来处理。

LogWrite类给出了一个简单的日志服务示例，其中日志操作在单独的线程中执行。产生日志消息的线程将日志写入消息放入LogWrite类的BlockingQueue，然后日志线程再从阻塞队列中读取后进行写入。这种模式是多生产者单消费者，如果消费者的处理速度低于生产者的生成速度，那么BlockingQueue将阻塞生产者。

```java
public class LogWriter {
  private final BlockingQueue queue;
  private final LoggerThread logger;
  private static final int CAPACITY = 1000;

  public LogWriter(Writer writer) {
    this.queue = new LinkedBlockingQueue<>(CAPACITY);
    this.logger = new LoggerThread(writer);
  }

  public void start() {
    logger.start();
  }
  public void log(String msg) throw InterruptedException {
    queue.put(msg);
  }

  public class LoggerThread extends Thread {
    private final PrintWriter writer;
    public LoggerThread(Writer writer) {
      this.writer = new PrintWriter(writer, true); // autoflush
    }

    public void run() {
      try {
        while(true) {
          writer.println(queue.take());
        }
      } catch (InterruptedException e) {

      } finally {
        writer.close();
      }
    }
  }
}
```

为了使像LogWriter这样的服务在软件产品中能发挥实际的作用，还需要实现一种终止日志线程的方法，从而避免使JVM无法关闭。要停止日志线程还是很容易的，因为它会反复调用take，而take能响应中断。如果将日志线程修改为当捕获到InterruptedException时退出，那么只需要中断日志线程就能停止服务。

然而，这样直接中断会丢失那些正在等待被写入日志的信息，不仅如此，其他线程将在调用log时被阻塞，因为日志消息队列是满的，因为这些线程将无法接触阻塞状态。当取消一个生产者-消费者操作时，需要同时取消生产者和消费者。所以这里直接中断消费者是容易的，但中断生产者就十分困难。

另一种关闭LogWriter的方法是：设置某个「已请求关闭」标志，以避免进一步提交日志消息。如下代码：

```java
public void log(String msg) throw InterruptedException {
	if(!shutdownRequested) {
        queue.put(msg);
    } else {
        throw new IllegalStateException("logger is shut down");
    }
}
```

在收到关闭请求后，消费者会把队列中所有的消息写入日志，并解除所有在调用log时阻塞的生产者。然而可能有这样一种情况，消费者立刻消费完了队列中所有的消息（当生产者还没来得及put进来的时候），然后宣告清空了队列，随后关闭了writer，但是生产者还有大量的put导致消费者宣告清空队列后还有大量的消息塞满了队列，然后其他的put就被阻塞住了，此时writer.close已经无法关闭了，同时一些生产者的阻塞状态也无法接触了。可以通过一些技巧来降低这种情况的发生，比如宣布队列被清空后，让消费者等待数秒钟再次清空阻塞队列，然后关闭writer，但它没有解决问题的本质。问题的本质是log的实现是一种「先判断再运行」的代码，因此要解决这个竞态条件，保证日志消息的提交是一个原子操作。但put本身就可以阻塞，所以采用的方法是：通过原子方法来检查关闭请求，并且有条件地递增一个计数器来保持提交消息的权利。

```java
import java.io.PrintWriter;
import java.io.Writer;
import java.util.concurrent.*;

import net.jcip.annotations.*;

/**
 * LogService
 * <p/>
 * Adding reliable cancellation to LogWriter
 *
 * @author Brian Goetz and Tim Peierls
 */
public class LogService {
  private final BlockingQueue<String> queue;
  private final LoggerThread loggerThread;
  private final PrintWriter writer;
  @GuardedBy("this") 
  private boolean isShutdown;
  @GuardedBy("this") 
  private int reservations;

  public LogService(Writer writer) {
    this.queue = new LinkedBlockingQueue<String>();
    this.loggerThread = new LoggerThread();
    this.writer = new PrintWriter(writer);
  }

  public void start() {
    loggerThread.start();
  }

  public void stop() {
    synchronized (this) {
      isShutdown = true;
    }
    loggerThread.interrupt();
  }

  
  public void log(String msg) throws InterruptedException {
    // ++不是原子性的，必须要加锁，个人觉得这里if的判断其实是没有必要加锁的，毕竟只是读，但isShutdown并不是volatile的，所以还是需要加锁保持可见性。
    synchronized (this) {
      if (isShutdown)
        throw new IllegalStateException(/*...*/);
      ++reservations;
    }
    queue.put(msg);
  }

  // 消费者
  private class LoggerThread extends Thread {
    public void run() {
      try {
        while (true) {
          try {
            // 原子方法检测关闭请求，类锁
            synchronized (LogService.this) {
              if (isShutdown && reservations == 0)
                break;
            }
            String msg = queue.take();
            // 可能有多个消费者，--并不是原子性的，如果不对--加类锁，就可能导致提前break，但并没有处理完消息
            synchronized (LogService.this) {
              --reservations;
            }
            writer.println(msg);
          } catch (InterruptedException e) { 
            /* retry */
          }
        }
      } finally {
        writer.close();
      }
    }
  }
}
```

保证了关闭请求发生后，消费者能消费掉剩余的所有生产者消息。

## 关闭ExecutorService

shutdown：正常关闭，速度慢，但更安全，等待队列中的所有任务都执行完毕后才关闭。

shutdownNow：强行关闭，速度更快，风险高。任务可能执行到一半被关闭。



通常会将ExecutorService封装在某个更高级的服务中，并且该服务能提供自己的生命周期方法，例如下面的LogService，它将管理线程的工作委托给一个ExecutorService，通过封装ExecutorService，可以将所有权链从应用程序扩展到服务以及线程，所有权链上的各个成员都将管理它拥有的服务或线程的生命周期。

```java
public class LogService {
  private final ExecutorService executor = Executors.newSingleThreadExecutor();
  private final PrintWriter writer = new PrintWriter(System.out);

  public void start() {}

  public void stop() throws InterruptedException {
    try {
      executor.shutdown();
      // 
      executor.awaitTermination(Integer.MAX_VALUE, TimeUnit.SECONDS);
    } finally {
      writer.close();
    }
  }

  public void log(String msg) {
    try {
      executor.execute(() -> writer.println(msg));
    } catch (RejectedExecutionException e) {tognguo jiang 
    }
  }
}
```

## 毒丸计划

另一种关闭生产者-消费者服务的方式是使用「毒丸」对象，毒丸是指一个放在队列上的元素，当得到这个对象时，立即停止。FIFO的队列中，毒丸对象将确保消费者在关闭之前完成队列中的所有工作，在提交毒丸对象之前提交的所有工作都将被处理，而生产者在提交了毒丸后，将不会再提交任何工作。

## 示例：只执行一次的服务

如果某个方法需要处理一批任务，并且当所有任务都处理完成后才返回，那么可以**通过一个私有的Executor来简化服务的生命周期管理**，其中该Executor的生命周期是由这个方法来控制的，在方法开始时启动，方法结束后关闭。

## shutdownNow的局限性

当通过shutdownNow来强行关闭ExecutorService时，它会尝试取消正在执行的任务（Interrupt），并返回所有已经提交但尚未开始的任务。

```java
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.concurrent.AbstractExecutorService;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.TimeUnit;

public class TrackingExecutor extends AbstractExecutorService {
  private final ExecutorService exec;
  // 被取消的任务清单
  private final Set<Runnable> taskCancelledAtShutdown = Collections.synchronizedSet(new HashSet<>());

  public TrackingExecutor(ExecutorService exec) {
    this.exec = exec;
  }
  @Override
  public void shutdown() {
    exec.shutdown();
  }

  // 终止所有任务，返回队列中还没有执行的任务
  @Override
  public List<Runnable> shutdownNow() {
    return exec.shutdownNow();
  }

  @Override
  public boolean isShutdown() {
    return exec.isShutdown();
  }

  @Override
  public boolean isTerminated() {
    return exec.isTerminated();
  }

  @Override
  public boolean awaitTermination(long timeout, TimeUnit unit) throws InterruptedException {
    return exec.awaitTermination(timeout, unit);
  }

  public List<Runnable> getCancelledTasks() {
    return new ArrayList<>(taskCancelledAtShutdown);
  }
  @Override
  public void execute(Runnable command) {
    exec.execute(new Runnable() {
      @Override
      public void run() {
        try {
          command.run();
        } finally {
          if (isShutdown() && Thread.currentThread().isInterrupted()) {
            taskCancelledAtShutdown.add(command);
          }
        }
      }
    });

  }
}
```



```java
import java.net.URL;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;

public abstract class WebCrawler {
  private volatile TrackingExecutor exec;

  private final Set<URL> urlsToCrawl = new HashSet<>();

  private final ConcurrentMap<URL, Boolean> seen = new ConcurrentHashMap<>();
  private static final long TIMEOUT = 500;
  private static final TimeUnit UNIT = TimeUnit.MILLISECONDS;

  public WebCrawler(URL startUrl) {
    urlsToCrawl.add(startUrl);
  }

  public synchronized void start() {
    exec = new TrackingExecutor(Executors.newCachedThreadPool());
    for (URL url : urlsToCrawl) {
      submitCrawlTask(url);
    }
    urlsToCrawl.clear();
  }

  public synchronized void stop() throws InterruptedException {
    try {
      // 队列中还未执行的任务
      saveUncrawled(exec.shutdownNow());

      if (exec.awaitTermination(TIMEOUT, UNIT)) {
        // 执行中被取消的任务
        saveUncrawled(exec.getCancelledTasks());
      }
    } finally {
      exec = null;
    }
  }

  protected abstract List<URL> processPage(URL url);

  private void saveUncrawled(List<Runnable> uncrawled) {
    for (Runnable task : uncrawled) {
      urlsToCrawl.add(((CrawlTask) task).getPage());
    }
  }

  private void submitCrawlTask(URL u) {
    exec.execute(new CrawlTask(u));
  }

  // 爬虫任务
  private class CrawlTask implements Runnable {
    private final URL url;

    CrawlTask(URL url) {
      this.url = url;
    }

    private int count = 1;

    boolean alreadyCrawled() {
      return seen.putIfAbsent(url, true) != null;
    }

    void markUncrawled() {
      seen.remove(url);
      System.out.printf("marking %s uncrawled%n", url);
    }

    public void run() {
      for (URL link : processPage(url)) {
        if (Thread.currentThread().isInterrupted()) {
          return;
        }
        submitCrawlTask(link);
      }
    }

    public URL getPage() {
      return url;
    }
  }
}
```

TrackingExecutor中存在一个不可避免的竞态条件，一些被认为已经取消的任务实际上已经完成。这个问题的原因是：在任务执行最后一条指令以及线程池将任务记录为「结束」的两个时刻之间，线程池可能被关闭。如果任务不是幂等的，那应用程序就需要考虑到这一风险。

# 处理非正常的线程终止

多线程中的异常很容易被忽略。



导致线程提前死亡的最主要原因就是RuntimeException，由于这种异常表示出现了某种编程错误或者其他不可修复的错误，因此它们通常不会被捕获。它们不会在调用栈中逐层传递，而是默认地在控制台中输出栈追踪信息，并终止线程。

线程非正常退出可能造成线程遗漏（Thread Leakage），如果该线程是时间分发线程，那么该线程表示的服务将不能再使用。

下面给出了如何在线程池内部构建一个工作者线程。如果任务抛出了一个未检查异常，那么它将终结线程，但会首先通知框架该线程已经终结。然后，框架可能会用新的线程来代替这个工作。

```java
public void run() {
    Throwable thrown = null;
    try {
        while(!isInterrupted()) {
            runTask(getTaskFromWorkQueue());
        }
    } catch (Throwable e) {
        thrown = e;
    } finally {
        threadExited(this, thrown);
    }
}
```



## 未捕获异常的处理

除了主动解决未检查异常，Thread API中同样提供了UncaughtExceptionHandler，它能检测出某个线程由于未捕获的异常而终结的情况。

当一个线程由于未捕获异常而退出时，JVM会把这个事件报告给应用程序提供的UncaughtExceptionHandler异常处理器。

标准线程池允许当发生未捕获异常时结束线程，但由于使用了一个try-finally代码块来接受通知，因此当线程结束时，将会有新的线程替它。如果没有提供捕获异常处理器或者其他的故障通知机制，那么任务会悄悄失败。如果你希望在任务由于发生异常而失败时获得通知，并且执行一些特定于任务的恢复操作，那么可以将任务封装在能捕获异常的Runnable或Callable中，或者改写ThreadPoolExecutor的afterExecute方法。

令人困惑的是，只有通过execute提交的任务，才能将它抛出的异常交给未捕获异常处理器，而通过submit提交的任务，无论是抛出的未检查异常还是已检查异常，都会被认为是任务返回状态的一部份。如果一个由submit提交的任务由于抛出了异常而结束，那么这个异常将被Future.get封装在ExecutionException中重新抛出。

# JVM关闭

JVM即可以正常关闭(orderly)，也可以强行(abrupt)关闭。正常关闭(orderly shutdown)的触发方式：

1. 当最后一个非守护线程结束后
2. 调用System.exit
3. 通过其他特定于平台的方法被关闭时，如发送SIGINT 或者Crtl-C

强行关闭(abrupt shutdown)的方式有：

1. 调用Runtime.halt
2. 通过操作系统关闭JVM进程，如SIGKILL

## 

## 关闭钩子

在正常关闭中，JVM首先调用所有已注册的关闭钩子（shutdown hook），关闭钩子是指通过Runtime.addShutdownHook注册的但尚未开始的线程，关闭钩子可以用于实现服务或者应用程序的清理工作，例如删除临时文件，或者清除无法由操作系统自动清除的资源。

强行关闭时，不会运行关闭钩子，而是直接关闭JVM。

关闭钩子是并发执行的，关闭钩子程序应该保证线程安全。

```java
public void start() {
  Runtime.getRuntime().addShutdownHook(new Thread() {
    public void run() {
      try {
        LogService.this.stop();
      } catch (InterruptedException ignored) {

      }
    }
  });
}
```

## 守护线程

有时候，你希望创建一个线程来执行辅助工作，但又不希望这个线程阻碍JVM的关闭（因为JVM会在所有非守护线程关闭后进行关闭），这种情况就需要使用守护线程（Daemon Thread）。

线程可以分为两种：普通线程和守护线程。在JVM启动时创建的所有线程中，除了主线程以外，其他线程都是守护线程（例如垃圾回收器以及其他执行辅助工作的线程）。当创建一个新的线程时，新线程将继承创建它的线程的守护状态，所有主线程创建的新线程都是普通线程，守护线程创建的新线程则是守护线程。

普通线程和守护线程之间的差异仅在于当线程退出时发生的操作。当JVM退出时，所有仍然存在的守护线程都将被直接抛弃，既不会执行finally代码块，stacks are also not unwound,。

我们应尽可能少地使用守护线程，守护线程最好用于执行「内部」任务，例如周期性地从内存的缓存中移除逾期的数据。



## 终结器

Finalizers

当不再需要内存资源时，可以通过垃圾回收器来回收它们，但对于一些资源，比如文件句柄或套接字句柄，当不需要它们时，必须显式的交换给操作系统。

文件句柄： 在文件I/O中，要从一个文件读取数据，应用程序首先要调用操作系统函数并传送文件名，并选一个到该文件的路径来打开文件。该函数取回一个顺序号，即文件句柄（file handle），该文件句柄对于打开的文件是唯一的识别依据。要从文件中读取一块数据，应用程序需要调用函数readFile，并将文件句柄在内存中的地址和要拷贝的字节数传送给操作系统。当完成任务后，再通过调用系统函数来关闭该文件。

为了实现这个功能，垃圾回收器对那些定义了finalize方法的对象会进行特殊处理：在回收器释放它们后，调用它们的finalize方法，从而保证一些持久化的资源会被释放掉。

我们要尽量避免编写或使用包含终结器的类。

# 小结

本章主要是讲任务、线程、服务、应用程序的生命周期结束问题，这些问题可能会增加它们在设计和实现时的复杂性。Java提供的是协商式的中断机制来实现取消操作，但这**依赖如何构建取消操作的协议**，以及能够始终遵循这些协议。通过使用FutureTask和Executor框架，可以帮助我们构建可取消的任务和服务。

关键字：协商、**「已请求取消」**标志、可中断阻塞方法、中断策略、中断状态、响应中断、异常捕获、恢复中断状态、计时任务中断、特定API进行终止、**停止基于线程的服务、ExecutorService.shutdown和shutdownNow的应用、UncaughtExceptionHandler、JVM正常关闭。**



任务可能由于各种各样的原因需要进行取消，任务由线程持有执行，Java中没有抢占式的方法来停止线程，只有协商式的通知策略，所以一个可取消的任务就必须拥有取消策略。

协商取消任务的最简单方式使用一个**「已请求取消」**标志，任务中循环遍历这一标记来判断是否有取消事件发生，但它的缺点是如果任务计算时间很长或者任务阻塞住了，那么可能永远不会检查这个取消标志，所以对于一些**可中断的阻塞方法**来说，都会有一个**中断策略**来停止阻塞。事实上，每个线程都有一个**中断状态**，阻塞方法如sleep、wait、Blocking.put/等也都支持被中断，中断某个线程的任务有interrupt方法。

**响应中断**的策略，一个是继续抛出中断异常，一个是**捕获中断异常并恢复中断状态**。可中断的阻塞方法在被中断后会抛出中断异常，同时清除中断状态，捕获中断异常需要看时机，一般可中断的阻塞方法都是在入口处检查中断状态然后抛出异常，所以在异常捕获处恢复中断可能导致**无限循环**。如果是无限循环的场景，可以通过使用一个局部变量使得能在退出方法前一刻恢复中断。

**计时运行的任务如何准确的中断**：计时任务的难点有两个，一个是独立线程的异常无法被其他线程捕获，另一个是任务超时后如何正确的中止。**解决异常捕获的方法是由调用线程自己执行任务并捕获异常**，同时启动一个外部线程定时对调用线程进行超时中断，但这种解决方法的缺陷是非常明显的，它可能因为任务提前完成破坏调用线程运行的其他任务，所以一般是再创建一个新的内部线程包裹任务和异常，通过外部线程对这个新建的线程进行超时中断，新线程的限时join方法来进行超时等待。另外就是使用Future.get来实现任务取消和计时运行更加成熟（底层原理可能是差不多的，后续研究）。



还有一些阻塞方法没有上面的中断机制，比如等待内置锁、socket的执行，但它有自己的**特定的API进行终止**，比如socket.close()。还可以通过newTaskFor返回一个Future接口来改变Future.cancel的行为。例如，定制的取消代码可以实现日志记录或者收集取消操作的统计信息，以及取消一些不响应中断的操作。通过改写interrupt方法，ReaderThread可以取消基于socket的线程。同样，通过改写任务的Future.cancel也可以实现类似的功能。

**停止基于线程的服务**：线程池也有安全结束的需求，服务拥有工作者线程，服务关闭线程

比如日志服务是由基于生产者-消费者模式的，消费线程由日志服务拥有，但生产线程则是外部线程，停止日志服务意味着要安全关闭消费线程和取消外部的生产线程。关闭消费者线程的原则是等待所有进入队列中的任务都执行完毕，同时还要阻塞生产者写入新的日志，这需要很精妙的设计，否则就可能发生生产者的线程的阻塞，一个比较好的设计是通过标记和计数器同步访问和修改来保证整个过程的线程安全。

ExecutorService线程池服务也提供了**shutdown和shutdownNow的API来关闭服务**。shutdown是安全的关闭线程池，它会等待任务执行完毕后关闭，shutdownNow则是强行关闭，它会中断正在执行的任务，关闭速度更快，shutdownNow不要轻易使用。毒丸计划是一种正常关闭策略之一，通过在队列中插入关闭的对象来实现工作线程的停止，但它对数量很大的生产者和消费者很难用。ExecutorService.shutdown可以用来简化某个服务的生命周期管理；使用shutdownNow强行关闭ExecutorService时，它会尝试取消正在执行的任务，并返回所有已提交但尚未开始的任务，这个返回可以用来进行任务的重启。这都是**shutdown和shutdownNow的一些应用**，但这里没有讲shutdown关闭的底层原理。



**非正常的线程终止如未捕获的异常会导致线程的死亡**，线程池可以采用一定的策略来使一个线程安全的终结，安全就是说某个线程挂掉需要告知线程池，线程池再采用其他的应对策略。这是主动的检查方法，Thread API提供了**UncaughtExceptionHandler**，它能检测出某个线程由于未捕获异常而终结的情况，这是JVM保证的，异常处理器一般捕获到异常后都是打印日志，或者通过Future.get抛出去。

JVM的正常关闭前会调用注册的关闭钩子，先执行关闭钩子关闭某些服务或者线程，没有正常线程时，JVM就会进行关闭，守护线程的使用需要谨慎，因为守护线程都将会被直接抛弃。终结器是说那些不能被垃圾回收器回收的资源，一般是持有了操作系统的资源，比如socket和文件句柄之类的，定义了finalize方法的对象就可能持有终结起资源，这部分需要专门调用finalize方法进行资源的释放。
