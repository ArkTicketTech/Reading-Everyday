# Synchronizer 剖析

ReentrantLock和semaphore这两个接口之间存在许多共同点，这两个类都可以用作阀门，即每次只允许一定数量的线程通过，并当线程到达阀门时，可以选择：

1. 通过：获取锁lock或者获取许可acquire
2. 等待：获取锁lock时阻塞或者获取许可时阻塞
3. 取消：tryLock和tryAcquire超时或者中断



它们如此相似的原因是它们在实现时都使用了一个共同的基类，即AbstractQueuedSynchronizer(AQS)，这个类也是其他许多同步类的基类。AQS是一个用于构建锁和同步器的框架，许多同步器都可以通过AQS很容易并且很高效地构造出来。比如CountDownLatch, ReentrantReadWriteLock, SynchronousQueue和FutureTask。

AQS既然作为一个框架，它解决了在实现同步器时涉及的大量细节问题，例如等待线程采用FIFO队列操作顺序。

基于AQS来构建同步器能带来许多好处，它不仅能极大地减少实现工作，而且也不必处理在多个位置上发生的竞争问题。

# AbstractQueuedSynchronizer

在基于AQS构建的同步器类中，最基本的操作包含各种形式的获取操作和释放操作。获取操作是一种依赖状态的操作，并且通常会阻塞。不同的同步器中获取操作的含义：

1. Lock：获取锁
2. Semaphore：获取信号量
3. CountDownlatch：等待并直到闭锁到达结束状态；
4. FutureTask：等待并直到任务已经完成

释放：所有请求时被阻塞的线程都会开始执行。

如果一个类想成为状态依赖的类，那么它必须拥有一些状态。AQS负责管理同步器类中的状态，它管理了一个整数状态信息state，可以通过getstate、setstate以及compareAndSetState等protected类型方法来进行操作。

```java
public abstract class AbstractQueuedSynchronizer extends AbstractOwnableSynchronizer
implements java.io.Serializable {   /**
     * The synchronization state.
     */
    private volatile int state;
    
    protected final int getState() {
        return state;
    }

    protected final void setState(int newState) {
        state = newState;

    }
    protected final boolean compareAndSetState(int expect, int update) {
        return STATE.compareAndSet(this, expect, update);
    }
}
```

这个state可以表示任意状态。例如：

1. ReentrantLock：表示所有者线程已经重复获取该锁的次数
2. Semaphore：表示剩余的许可数量
3. FutureTask：表示任务的状态

在同步器类中还可以自行管理一些额外的状态变量，例如，ReentrantLock保存了锁的当前所有者的信息，这样就能区分获取操作是重入的还是竞争的。

下面给出了AQS的获取操作与释放操作的伪代码：

```java
public class AbstractQueuedSynchronizer {

    boolean acquire() throws InterruptedException { 
        while (state does not permit acquire) {
            if (blocking acquisition requested) {
                enqueue current thread if not already queued;
                block current thread;
            } else {
                return failure;
            }
            possibly update synchronization state;
            dequeue thread if it was queued ;
            return success;
        }

    }

    void release() {
        update synchronization state
        if (new state may permit a blocked thread to acquire) {
            unblock one or more queued threads
        }
    }
}
```

根据同步器的不同，获取操作可以是一种独占操作（ReentrantLock），也可以是一个非独占操作（Semaphore、CountDownLatch）。

1. 首先，同步器判断当前状态是否允许获取操作：如果是，则允许线程执行，否则就阻塞或则会失败。
2. 其次，更新同步器的状态：例如获取一个锁后，锁的状态从未被持有变成已被持有

独占获取需实现AQS中的方法：

1. tryAcquire
2. tryRelease
3. isHeldExclusively

共享获取需实现AQS中的方法：

1. tryAcquireShared
2. tryReleaseShared



## 一个简单的闭锁

我们用AQS实现一个二元闭锁，实际上就是获取和释放操作。AQS中的state来表示闭锁状态，继承AQS的类Sync实现了tryAcquireShared和tryReleaseShared来对状态的变化进行自定义，signal的语义是进行释放操作，在signal方法中调用releaseShared进行释放操作，releaseShared将执行Sync中的自定义tryReleaseShared释放操作，await是获取操作，也是一样的道理。这个例子中，内部类Sync继承AQS，将状态交给AQS管理，Sync做的事情就是对状态进行自定义变换（override tryXxx方法），然后AQS的acqurie、acquireShared、release、releaseShared会调用子类中的状态变化方法（带try的方法）进行状态的扭转。

这貌似是某个设计模式，忘了

```java
public class OneSlotLatch {


  private final Sync sync = new Sync();

  public void signal() {
    sync.releaseShared(0);
  }

  public void await() throws InterruptedException {
    sync.acquireSharedInterruptibly(0);
  }

  private class Sync extends AbstractQueuedSynchronizer {

    /**
     * return -1: 获取操作失败
     * return 1: 获取成功
     * */
    @Override
    protected int tryAcquireShared(int arg) {
      // 状态为1，表明闭锁打开，获取可成功；反之失败。
      return (getState() == 1) ? 1 : -1;
    }

    @Override
    protected boolean tryReleaseShared(int arg) {
      // 打开闭锁
      setState(1);
      // 现在其他线程可以获取该闭锁
      return true;
    }
  }
}


public class AbstractQueuedSynchronizer {

  public final boolean releaseShared(int arg) {
    // 执行子类Sync的方法，打开闭锁
    if (tryReleaseShared(arg)) {
      doReleaseShared();
      return true;
    }
    return false;
  }
  public final void acquireSharedInterruptibly(int arg) throws InterruptedException {
    // 判断当前线程是否被中断，并清除中断标记，如果被中断，抛出异常
    if (Thread.interrupted()) {  
      throw new InterruptedException();
    }
    // 判断获取闭锁是否成功
    if (tryAcquireShared(arg) < 0) {
      // 失败则进入阻塞
      doAcquireSharedInterruptibly(arg);
    }
  }

  private void doAcquireSharedInterruptibly(int arg) throws InterruptedException {
    final Node node = addWaiter(Node.SHARED);
    try {
      // 循环保持阻塞
      for (;;) {
        final Node p = node.predecessor();
        if (p == head) {
          int r = tryAcquireShared(arg);
          // 解除阻塞并返回
          if (r >= 0) {
            setHeadAndPropagate(node, r);
            p.next = null; // help GC
            return;
          }
        }
        if (shouldParkAfterFailedAcquire(p, node) &&
            parkAndCheckInterrupt())
          throw new InterruptedException();
      }
    } catch (Throwable t) {
      cancelAcquire(node);
      throw t;
    }
  }
}
```

# AQS

我们快速了解一下如何利用AQS实现一些可阻塞的类

## ReentrantLock

ReentrantLock只支持独占方式的获取操作，因此它实现了tryAcquire、tryRelease、isHeldExclusively，ReentrantLock将同步状态用于保存锁获取的次数，

```java
public class ReentrantLock implements Lock, java.io.Serializable {

  private final Sync sync;

  public void lock() {
    sync.acquire(1);
  }

  public void unlock() {
    sync.release(1);
  }

  abstract static class Sync extends AbstractQueuedSynchronizer {
    // 每次lock都加1
    @ReservedStackAccess
    final boolean nonfairTryAcquire(int acquires) {
      final Thread current = Thread.currentThread();
      // 获取锁的次数
      int c = getState();
      // 之前没有被获取
      if (c == 0) {
        // 原子地更新state
        if (compareAndSetState(0, acquires)) {
          // 独占线程
          setExclusiveOwnerThread(current);
          return true;
        }
      } else if (current == getExclusiveOwnerThread()) {
        // 之前有被获取，但是线程是独占线程：支持可重入
        // 记录重入次数
        int nextc = c + acquires;
        if (nextc < 0) // overflow
          throw new Error("Maximum lock count exceeded");
        // 更新state
        setState(nextc);
        return true;
      }
      return false;
    }

    // 每次unlock都减1
    @ReservedStackAccess
    protected final boolean tryRelease(int releases) {
      // 减1将状态变为非正数
      int c = getState() - releases;
      // 不是独占线程，不能进行锁的释放
      if (Thread.currentThread() != getExclusiveOwnerThread())
        throw new IllegalMonitorStateException();
      // 
      boolean free = false;
      if (c == 0) {
        // 成功释放
        free = true;
        setExclusiveOwnerThread(null);
      }
      setState(c);
      return free;
    }


    static final class NonfairSync extends Sync {
      private static final long serialVersionUID = 7316153563782823691L;
      protected final boolean tryAcquire(int acquires) {
        return nonfairTryAcquire(acquires);
      }
    }
  }
}
public class AbstractQueuedSynchronizer {

  public final void acquire(int arg) {
    if (!tryAcquire(arg) &&
        acquireQueued(addWaiter(Node.EXCLUSIVE), arg))
      selfInterrupt();
  }

  public final boolean release(int arg) {
    if (tryRelease(arg)) {
      Node h = head;
      if (h != null && h.waitStatus != 0)
        unparkSuccessor(h);
      return true;
    }
    return false;
  }
}
```

