## Semaphore

Semaphore用AQS的同步状态保存当前许可的数量

```java
abstract static class Sync extends AbstractQueuedSynchronizer {
    private static final long serialVersionUID = 1192457210091910933L;

    Sync(int permits) {
        setState(permits);
    }

    final int getPermits() {
        return getState();
    }

    /**
    降低许可数量
    */
    final int nonfairTryAcquireShared(int acquires) {
        for (;;) {
            int available = getState();
            int remaining = available - acquires;
            // compareAndSetState失败后会重新尝试
            if (remaining < 0 || compareAndSetState(available, remaining)) {
                return remaining;
            }
        }
    }

    /**
    增加许可计数
    */
    protected final boolean tryReleaseShared(int releases) {
        for (;;) {
            int current = getState();
            // 增加许可
            int next = current + releases;
            if (next < current) // overflow
                throw new Error("Maximum permit count exceeded");
            // 修改成功则返回，失败就继续尝试
            if (compareAndSetState(current, next)) {
                return true;
            }
        }
    }
}
static final class NonfairSync extends Sync {
    private static final long serialVersionUID = -2694183684443567898L;

    NonfairSync(int permits) {
        super(permits);
    }

    protected int tryAcquireShared(int acquires) {
        return nonfairTryAcquireShared(acquires);
    }
}
```

## CountDownLatch

跟前面的二元闭锁非常类似

```java
public class CountDownLatch {
  /**
     * Synchronization control For CountDownLatch.
     * Uses AQS state to represent count.
     */
  private static final class Sync extends AbstractQueuedSynchronizer {
    private static final long serialVersionUID = 4982264981922014374L;

    Sync(int count) {
      setState(count);
    }

    int getCount() {
      return getState();
    }

    protected int tryAcquireShared(int acquires) {
      return (getState() == 0) ? 1 : -1;
    }

    protected boolean tryReleaseShared(int releases) {
      // Decrement count; signal when transition to zero
      for (;;) {
        int c = getState();
        if (c == 0)
          return false;
        int nextc = c - 1;
        if (compareAndSetState(c, nextc))
          return nextc == 0;
      }
    }
  }

  private final Sync sync;

  public void countDown() {
    sync.releaseShared(1);
  }

  public boolean await(long timeout, TimeUnit unit) throws InterruptedException {
    return sync.tryAcquireSharedNanos(1, unit.toNanos(timeout));
  }
}
public abstract class AbstractQueuedSynchronizer extends AbstractOwnableSynchronizer {
  public final boolean releaseShared(int arg) {
    if (tryReleaseShared(arg)) {
      doReleaseShared();
      return true;
    }
    return false;
  }
  public final boolean tryAcquireSharedNanos(int arg, long nanosTimeout) throws InterruptedException {
    if (Thread.interrupted())
      throw new InterruptedException();
    return tryAcquireShared(arg) >= 0 ||
    doAcquireSharedNanos(arg, nanosTimeout);
  }
}
```



## FutureTask

Java11的FutureTask没有使用AQS了。

## ReentrantReadWriteLock

ReentrantReadWriteLock中，单个AQS子类将同时管理读取加锁和写入加锁。ReentrantReadWriteLock使用了一个16位的状态来表示写入锁的计数，并使用另一个16位的状态来表示读取锁的计数。在读取锁上的操作将使用共享的获取方法和释放方法，在写入锁的操作上将使用独占的获取方法和释放方法。

AQS在内部维护一个等待线程队列，其中记录了某个线程请求的是独占访问还是共享访问。在ReentrantReadWriteLock中，当锁可用时，如果位于队列头部的线程执行写入操作，那么线程会得到这个锁，如果位于队列头部的线程执行读取访问，那么队列中在遇到第一个写入线程之前的所有线程都将获取这个锁。
