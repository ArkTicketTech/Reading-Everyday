DelayQueue是一个延时队列。

它实现了一个无界的BlockingQueue，用于放置实现了Delayed接口的对象，其中的对象只能在其到期时才能从队列中取走，对象放置的队列容器是优先队列。



# 应用场景

DelayQueue的应用场景是某个动作触发后过一段时间就触发另一个动作，比如：

1. 订单下单15分钟后未支付就取消订单
2. 下单成功1分钟后发送短信通知
3. 缓存过期
4. 任务超时处理



# 原理分析

前置知识：

1. 阻塞队列
2. 优先队列：根据delay时间排序
3. leader-follower 模式：优化阻塞通知

1. https://stackoverflow.com/questions/48493830/what-exactly-is-the-leader-used-for-in-delayqueue

1. 条件队列Condition：实现阻塞和通知



## Leader-follower模型


在Leader-follower线程模型一开始会创建一个线程池，并且会选取一个线程作为leader线程，leader线程负责监听网络请求，其它线程为follower处于waiting状态，

1. 当leader线程接受到一个请求后，会释放自己作为leader的权利，
2. 然后从follower线程中选择一个线程进行激活，
3. 然后激活的线程被选择为新的leader线程作为服务监听，
4. 然后老的leader则负责处理自己接受到的请求（现在老的leader线程状态变为了processing），处理完成后，状态从processing转换为follower





```java
public class DelayQueue<E extends Delayed> extends AbstractQueue<E>
implements BlockingQueue<E> {

    private final transient ReentrantLock lock = new ReentrantLock();
    // 状态变量
    private final PriorityQueue<E> q = new PriorityQueue<E>();

    private Thread leader;

    // 状态变量由锁保护，那么锁对象和条件队列的对象必须是一个对象
    // Condition signalled when a newer element becomes available at the head of the queue or a new thread may need to become leader.
    private final Condition available = lock.newCondition();


}
```



## offer

```java
public boolean offer(E e) {
    final ReentrantLock lock = this.lock;
	// q实际上就是状态变量，
	// 状态变量由锁保护，那么锁对象和条件队列的对象必须是一个对象
    lock.lock();
    try {
        q.offer(e);
        if (q.peek() == e) {
            // 成功插入数据，释放自己作为leader的权利
            leader = null;
            // 通知有新元素添加
            available.signal();
        }
        return true;
    } finally {
        lock.unlock();
    }
}
```



## take()

```java
/**
     * Retrieves and removes the head of this queue, waiting if necessary
     * until an element with an expired delay is available on this queue.
     *
     * @return the head of this queue
     * @throws InterruptedException {@inheritDoc}
     */
public E take() throws InterruptedException {
final ReentrantLock lock = this.lock;
  lock.lockInterruptibly();
  try {
    for (;;) {
      E first = q.peek();
      if (first == null)
        // 没有新元素则挂起线程，新元素添加后会进行通知
        available.await();
      else {
        
        long delay = first.getDelay(NANOSECONDS);
        // 有新元素且到达过期时间就直接返回
        if (delay <= 0L)
          return q.poll();
        // 说明此时还没有达到过期时间
        first = null; // don't retain ref while waiting
        // 如果leader不为null，说明已经有线程先于当前线程在进行等待delay，所以需要等待那个线程先执行完
        // 在下面的available.awaitNanos(delay)阻塞
        if (leader != null)
          available.await();
        else {
          // 没有leader线程，那么当前线程转化为leader线程
          Thread thisThread = Thread.currentThread();
          leader = thisThread;
          try {
            // 等待delay秒后获取数据，如果这期间有新的元素被添加，该线程同样会被唤醒并开启下一轮循环
            available.awaitNanos(delay);
          } finally {
            // await期间会释放掉锁，如果这期间没有新的元素被添加，那么该线程不再作为leader，而是继续处理返回数据
           
            if (leader == thisThread)
              leader = null;
          }
        }
      }
    }
  } finally {
    // 这里是当前线程执行拉去队列头部元素后通知其他挂起的线程继续执行
    // 只有返回队列元素才能走到这里，走到这里如果leader为null说明当前没有线程在等待获取元素，如果队列还有元素，就会通知其他挂起的线程继续执行
    if (leader == null && q.peek() != null)
      available.signal();
    lock.unlock();
  }
}
```

整个DelayQueue的核心就是这两个函数了，可以从下面的角度思考DelayQueue如何工作：

1. 队列为空，线程A进行take时，线程B加入新的对象元素
2. 队列不为空只有一个对象，线程take
3. 队列不为空且有两个对象，线程并发的take
4. 队列不为空，线程A进行take时在等待过期时间到临时，线程B添加新的元素
