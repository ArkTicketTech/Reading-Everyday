类库中包含了状态依赖性的类（state-dependent classes），例如BlockingQueue等，在这些类等一些操作中有有着基于状态的前提条件，例如，不能从一个空的队列中删除元素。

创建状态依赖类的最简单方法通常是在类库中现有状态依赖类的基础上进行构造，但有时类库中并没有提供需要的功能，此时可以使用类库提供的底层机制来构造自己的同步机制，包括内置的条件队列、显式的Condition对象以及AbstractQueuedSynchronizer框架。

本章将介绍实现状态依赖性的各种选择，以及在使用平台提供的状态依赖性机制时需要遵循的各项规则。

# 状态依赖性的管理

单线程程序中，从队列中删除一个元素会判断队列是否为空，这个前提条件在单线程下永远都是正确的，但在并发程序中，基于状态的条件可能会因为其他线程的操作而改变，队列一开始还不为空，但随后因为其他线程的操作而变为空，如果没有保证原子性，那程序就会在面临失败的风险，甚至可能出现严重的错误。通常来说，一种更好的选择是等待前提条件重新变为真。

依赖状态的操作可以一直阻塞直到可以继续执行，这比使它们先失败再实现起来要更为方便且不易出错。**内置的条件队列可以使线程一直阻塞，直到对象进入某个线程可以继续执行的状态，并且当被阻塞的线程可以执行时再唤醒它们**。

为了突出高效的条件等待机制的价值，我们先介绍如何通过轮询与休眠的方式来实现阻塞等待。

有界缓存是一个状态依赖性的类，我们用有界缓存来讨论条件等待机制，有界缓存提供的put和take操作中包含一个前提条件：不能从空缓存中获取元素，不能讲元素放入已满的缓存中。当前提条件不满足时，依赖状态的操作可以：

1. 抛出一个异常或者返回一个错误状态，使其成为一个调用者的问题；
2. 保持阻塞直到对象进入正确的状态。

我们先实现一个基类BaseBoundedBuffer，然后采用不同的方法来处理前提条件失败的问题。

```java
import net.jcip.annotations.*;

/**
 * BaseBoundedBuffer
 * <p/>
 * Base class for bounded buffer implementations
 *
 * @author Brian Goetz and Tim Peierls
 */
@ThreadSafe
public abstract class BaseBoundedBuffer <V> {
  @GuardedBy("this") private final V[] buf;
  @GuardedBy("this") private int tail;
  @GuardedBy("this") private int head;
  @GuardedBy("this") private int count;

  protected BaseBoundedBuffer(int capacity) {
    this.buf = (V[]) new Object[capacity];
  }

  protected synchronized final void doPut(V v) {
    buf[tail] = v;
    if (++tail == buf.length)
      tail = 0;
    ++count;
  }

  protected synchronized final V doTake() {
    V v = buf[head];
    buf[head] = null;
    if (++head == buf.length)
      head = 0;
    --count;
    return v;
  }

  public synchronized final boolean isFull() {
    return count == buf.length;
  }

  public synchronized final boolean isEmpty() {
    return count == 0;
  }
}
```



## 将前提条件的失败传递给调用者



```java
import net.jcip.annotations.*;

/**
 * GrumpyBoundedBuffer
 * <p/>
 * Bounded buffer that balks when preconditions are not met
 *
 * @author Brian Goetz and Tim Peierls
 */
@ThreadSafe
public class GrumpyBoundedBuffer <V> extends BaseBoundedBuffer<V> {
  public GrumpyBoundedBuffer() {
    this(100);
  }

  public GrumpyBoundedBuffer(int size) {
    super(size);
  }

  public synchronized void put(V v) throws BufferFullException {
    if (isFull())
      throw new BufferFullException();
    doPut(v);
  }

  public synchronized V take() throws BufferEmptyException {
    if (isEmpty())
      throw new BufferEmptyException();
    return doTake();
  }
}

class ExampleUsage {
  private GrumpyBoundedBuffer<String> buffer;
  int SLEEP_GRANULARITY = 50;

  // 阻塞等待
  void useBuffer() throws InterruptedException {
    while (true) {
      try {
        String item = buffer.take();
        // use item
        break;
      } catch (BufferEmptyException e) {
        Thread.sleep(SLEEP_GRANULARITY);
      }
    }
  }
}

class BufferFullException extends RuntimeException {
}

class BufferEmptyException extends RuntimeException {
}
```

synchronized保证了put和take方法中先检查再执行的原子性。虽然上面的实现非常简单，但使用起来却很复杂，调用方需要考虑重试和异常捕获。useBuffer通过异常捕获和休眠一定时间再重试实现了阻塞等待，如果不休眠一定时间，那么会发生忙等待（自旋等待），会消耗过多的cpu时间，如果休眠一定时间，获取缓存的延时可能变高，比如刚开始休眠状态就发生变化。忙等待还可以使用Threa.yield()来优化。





## 通过轮询和休眠来实现简单的阻塞

将轮询与休眠的机制放入有界阻塞类中，使调用者无需每次调用都重试。

```java
import net.jcip.annotations.*;

/**
 * SleepyBoundedBuffer
 * <p/>
 * Bounded buffer using crude blocking
 *
 * @author Brian Goetz and Tim Peierls
 */
@ThreadSafe
public class SleepyBoundedBuffer <V> extends BaseBoundedBuffer<V> {
  int SLEEP_GRANULARITY = 60;

  public SleepyBoundedBuffer() {
    this(100);
  }

  public SleepyBoundedBuffer(int size) {
    super(size);
  }

  // 轮询与休眠实现阻塞等待
  public void put(V v) throws InterruptedException {
    while (true) {
      synchronized (this) {
        if (!isFull()) {
          doPut(v);
          return;
        }
      }
      Thread.sleep(SLEEP_GRANULARITY);
    }
  }

  // 轮询与休眠实现阻塞等待
  public V take() throws InterruptedException {
    while (true) {
      synchronized (this) {
        if (!isEmpty())
          return doTake();
      }
      Thread.sleep(SLEEP_GRANULARITY);
    }
  }
}
```

但轮询与休眠的缺点依然存在：响应性与消耗过多CPU时间之间需要进行权衡。同时对调用者提出了一个新的需求，需要处理InterruptedException异常

## 条件队列

条件队列名字来源于：它使得一组线程能够通过某种方式来等待特定的条件为真，条件队列中的元素是一个一个正在等待相关条件的线程。

条件队列有着事件驱动的思想，`Object.wait()`表示会自动释放锁，并请求操作系统挂起当前线程；notify表示唤醒被挂起的线程，当被挂起的线程醒来时，它将在返回之前重新获取锁。可以理解为线程进入了一个条件队列，当满足条件后线程从队列中出来继续执行任务。

```java
import net.jcip.annotations.*;

/**
 * BoundedBuffer
 * <p/>
 * Bounded buffer using condition queues
 *
 * @author Brian Goetz and Tim Peierls
 */
@ThreadSafe
public class BoundedBuffer <V> extends BaseBoundedBuffer<V> {
  // CONDITION PREDICATE: not-full (!isFull())
  // CONDITION PREDICATE: not-empty (!isEmpty())
  public BoundedBuffer() {
    this(100);
  }

  public BoundedBuffer(int size) {
    super(size);
  }

  // BLOCKS-UNTIL: not-full
  public synchronized void put(V v) throws InterruptedException {
    while (isFull()) {
      // 自动释放锁，并请求操作系统挂起当前线程；当被挂起的线程醒来时，它将在返回之前重新获取锁。
      wait();
    }
    doPut(v);
    // 向在wait的线程发出通知，条件为真，可以醒来了。
    notifyAll();
  }

  // BLOCKS-UNTIL: not-empty
  public synchronized V take() throws InterruptedException {
    while (isEmpty()) {
      wait();
    }
    V v = doTake();
    notifyAll();
    return v;
  }

  // BLOCKS-UNTIL: not-full
  // Alternate form of put() using conditional notification
  public synchronized void alternatePut(V v) throws InterruptedException {
    while (isFull()) {
      wait();
    }
    boolean wasEmpty = isEmpty();
    doPut(v);
    if (wasEmpty) {
      notifyAll();
    }
  }
}
```

条件队列实现的BoundedBuffer 优势在于：

1. 响应性更高
2. 不消耗过多的CPU事件
3. 实现简单，且有明晰的状态依赖性管理
4. 还可以使用定时版本的wait来实现限定的put和take
