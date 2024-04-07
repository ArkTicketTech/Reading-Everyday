## [1188. 设计有限阻塞队列](https://leetcode.cn/problems/design-bounded-blocking-queue/)

做这道题先看「Java并发编程实战」第14章，书中循序渐进的讲到了条件队列和其使用，包含了有界阻塞队列的实现。

实现有限阻塞队列，最基础的是用内置锁的条件队列。

为了简化实现，我们直接使用了Deque来作为队列的实现。

剩下的问题就是如何实现阻塞和线程安全特性。

如果是单线程的话，也谈不上阻塞，如果基于某个状态的前提条件（这里就是队列非空、未满）没有得到满足，那么这个条件永远无法成真，因此直接失败即可。

所以这是一个关于并发依赖状态的问题，阻塞其实就是在等待前提条件变为真，那这个就可以用条件队列来实现。

条件队列：它使得一组线程能够通过某种方式来等待特定的条件变为真。

## 内置条件队列

Java中，每个对象都可以作为一个锁，也可以作为一个条件队列，`API`就是

1. `Object.wait()`：自动释放锁，并请求操作系统挂起当前线程，从而使其他线程能够获得这个锁并修改对象的状态；
2. `Object.notify()`：随机选择唤醒一个等待的线程
3. `Object.notifyAll()`：唤醒所有在这个条件队列上等待的线程。



使用条件队列需要注意：

1. 当调用`wait, notify, notifyAll`等方法时，一定要持有与条件队列相关的锁：如果不是相同的锁，`wait`释放锁后该方法将一直持有该锁，无法释放且其他线程被阻塞。
2. 每次从wait中唤醒时，都必须再次测试条件谓词，所以一般是在一个循环中调用`wait()`：避免过早唤醒

所以状态依赖的标准形式代码为：

```java
void stateDependentMethod() throws InterruptedException {
    synchronized(lock) {
        while(!conditionPredicate()) {
            lock.wait();
        }
    }
}
```



唤醒等待线程时，我们会选用`notifyAll`而不是`notify`，因为等待条件有多个，`notify`随机选择一个唤醒，可能造成信号丢失。

```java
class BoundedBlockingQueue {

  private Deque<Integer> queue;
  private int capacity;

  public BoundedBlockingQueue(int capacity) {
    this.queue = new ArrayDeque<>();
    this.capacity = capacity;
  }

  public void enqueue(int element) throws InterruptedException {
    synchronized(this) {
      while(isFull()) {
        wait();
      }
      queue.offer(element);
      notifyAll();
    }
  }

  public int dequeue() throws InterruptedException {
    synchronized(this) {
      while (isEmpty()) {
        wait();
      }
      int num = queue.poll();
      notifyAll();
      return num;
    }

  }

  public int size() {
    return queue.size();
  }

  private boolean isEmpty() {
    return queue.isEmpty();
  }

  private boolean isFull() {
    return queue.size() == capacity;
  }
}
```



## 显式条件队列

内置锁的不足是，使用`notifyAll`时，它会唤醒所有等待的线程，所以内置锁的一个缺陷是：一个内置锁都只能有一个相关联的条件队列。

所以另一个选择是使用显式的条件队列，`Lock`是Java中一种广义的内置锁，`Condition`也是一种广义的内置条件队列。状态依赖的改变还是不变，按照上面的`stateDependentMethod`来。

使用`ReentrantLock`，创建两个条件队列对应非空、未满的条件，灵活的实现等待和通知。

`lock.await()`对应了`Object.wait()`，`lock.signal()`对应了`Object.signal()`。

在内置条件队列中，我们使用的是`notifyAll`，因为有多个等待条件，`notify`容易造成信息丢失，但现在一个条件队列对应一个前提条件，所以使用`signal`。

```java
class BoundedBlockingQueue {

  private Deque<Integer> queue;
  private int capacity;
  // 使用显式条件队列
  private final Lock lock = new ReentrantLock();
  private final Condition notFull = lock.newCondition();
  private final Condition notEmpty = lock.newCondition();

  public BoundedBlockingQueue(int capacity) {
    this.queue = new ArrayDeque<>();
    this.capacity = capacity;
  }

  public void enqueue(int element) throws InterruptedException {
    lock.lock();
    try {
      while(isFull()) {
        notFull.await();
      }
      queue.offer(element);
      notEmpty.signal();
    } finally {
      lock.unlock();
    }
  }

  public int dequeue() throws InterruptedException {
    lock.lock();
    try {
      while (isEmpty()) {
        notEmpty.await();
      }
      int num = queue.poll();
      notFull.signal();
      return num;
    } finally {
      lock.unlock();
    }
  }

  public int size() {
    return queue.size();
  }

  private boolean isEmpty() {
    return queue.isEmpty();
  }

  private boolean isFull() {
    return queue.size() == capacity;
  }
}
```
