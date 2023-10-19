# 显式的 condition 对象

内置的条件队列，也即是Object.wait和notify，它的每个内置锁都只能有一个相关联的条件队列，所以之前的BoundedBuffer这种类中，多个线程可能在同一个条件队列上等待不同的条件谓词。

而显式的条件队列Condition，比内置的条件队列更加灵活：在每个锁上可存在多个等待、条件等待可以是中断的、基于时限的等待、公平的或非公平的队列操作。

```java
package java.util.concurrent.locks;

import java.util.Date;
import java.util.concurrent.TimeUnit;

public interface Condition {

    void awaitUninterruptibly();

    long awaitNanos(long nanosTimeout) throws InterruptedException;

    boolean await(long time, TimeUnit unit) throws InterruptedException;

    boolean awaitUntil(Date deadline) throws InterruptedException;

    void signal();
    
    void signalAll();
}
```

之前在显式锁Lock接口上看到有一个newContition，它其实就是返回一个绑定在锁上的条件队列。

```java
public interface Lock {
    
	Condition newCondition();
}
```

每个Lock可以有任意数量的Condition对象。

| Object    | Condition |
| --------- | --------- |
| wait      | await     |
| notify    | signal    |
| notifyAll | signalAll |

通过显式锁和显式Condition重写BoundedBuffer，有很多好处：

1. 使得对条件队列的使用方式跟容易理解，分析多个condition类更清晰；
2. 通过将两个条件谓词分开并放到两个等待线程集中，Condition使其更容易满足单词通知的需求，signal比signalAll更高效，极大减少之前说到的上下文切换和锁竞争。



```java
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class ConditionBoundedBuffer<T> {

  private static final int BUFFER_SIZE = 100;

  protected final Lock lock = new ReentrantLock();
  // CONDITION PREDICATE: notFull (count < items.length)
  private final Condition notFull = lock.newCondition();
  // CONDITION PREDICATE: notEmpty (count > 0)
  private final Condition notEmpty = lock.newCondition();
  @GuardedBy("lock")
  private final T[] items = (T[]) new Object[BUFFER_SIZE];
  @GuardedBy("lock")
  private int tail, head, count;

  public void put(T x) throws InterruptedException {
    lock.lock();
    try {
      while(count == items.length) {
        // 队列满了之后进行等待
        notFull.await();
      }
      items[tail] = x;
      tail++;
      if (tail == items.length) {
        tail = 0;
      }
      count++;
      // 由于加入了一个元素，队列不再为空，发出通知
      notEmpty.signal();
    } finally {
      lock.unlock();
    }
  }

  public T take() throws InterruptedException {
    lock.lock();
    try {
      while(count == 0) {
        // 队列为空时，进行等待
        notEmpty.await();
      }
      // 获取队头元素
      T x = items[head];
      // 元素pop出，不再占用队列位置
      items[head] = null;
      head++;
      if (head == items.length) {
        head = 0;
      }
      count--;
      // 取出了一个元素，发出队列不为满的通知
      notFull.signal();
      return x;
    } finally {
      lock.unlock();
    }
  }
}
```

显式条件队列一样需要三元关系：加锁、wait、条件谓词。

1. **先加锁**
2. **在测试条件谓词之前必须持有这个锁**
3. **锁对象与条件队列对象必须是同一个对象**
