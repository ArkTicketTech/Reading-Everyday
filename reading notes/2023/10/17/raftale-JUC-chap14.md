# 使用条件队列

## 条件谓词

条件谓词是使某个操作成为状态依赖操作的前提条件。有界缓存中，只有当缓存不为空时，take方法才能执行，否则必须等待。对take方法来说，「缓存不为空」就是它的条件谓词（即它的前提条件）。条件谓词是由类中各个状态变量构成的表达式， BaseBoundedBuffer中缓存不为空就是用count与0进行比较。

在条件等待中存在 一种重要的三元关系，包括加锁、wait方法和一个条件谓词。条件谓词包含多个状态变量，状态变量由一个锁来保护，因此在测试条件谓词之前必须持有这个锁。锁对象与条件队列对象（即调用wait和notify等方法所在的对象）必须是同一个对象，试想take方法中加锁的如果不是一个对象，当synchronized获得锁，wait释放锁并阻塞当前线程，不是一把锁的情况下synchronized的锁一直得不到释放，那put方法就无法获取锁进行，就会造成无限等待。

```java
  // BLOCKS-UNTIL: not-empty
  public synchronized V take() throws InterruptedException {
    while (isEmpty()) {
      wait();
    }
    V v = doTake();
    notifyAll();
    return v;
  }
```

所以可阻塞的状态依赖操作形式是这样的：

```java
acquire lock on object state
while(precondition does not hold) {
	release lock
	wait unitl precondition might hold;
	optionally fail if interrupted or timeout expires;
	reacquire lock;
}
preform action
	release lock
```

## 过早唤醒

wait方法的返回并不一定意味着线程正在等待的条件谓词已经变为真了：

1. 某个线程使用notifyAll唤醒线程
2. wait阻塞的线程自身也可能提前返回查看
3. 唤醒到重新获取锁这段时间，有可能其他线程抢先获取这个锁，并修改了状态；
4. 意想不到的notify



所以条件等待的标准形式为：

1. 循环中调用wait
2. 条件谓词在while的判断中
3. 测试条件谓词前需要加一把锁来保护各个状态变量
4. 调用wait、notify时，一定要持有与条件队列相关的锁

```java
void stateDependentMethod() throws InterruptedException {
	synchronized(lock) {
        while(!conditionPredicate) {
            lock.wait();
            
        }
    // 此时对象处于合适的状态
    }
}
```

## 丢失的信号

之前第10章讲到活跃性故障时，说到有丢失的信号。

丢失的信号是指：线程必须等待一个已经为真的条件，但在开始等待之前没有检查条件谓词。也就是说，线程A先发生了notify，然后线程B没有检查条件就进行wait，那这个wait的醒来需要下一个notify。这就导致了信号丢失。如果按照stateDependentMethod中的设计，就不会发生信号丢失的问题。

## 通知

每当在等待一个条件时，一定要确保条件谓词变为真时通过某种方式发出通知。

条件队列API有两个发出通知的方法，即notify和notifyAll，无论调用哪一个，都必须持有与条件队列对象相关联的锁。

1. notify: JVM会从这个条件队列上等待的多个线程随机选择一个线程来唤醒；
2. notifyAll：JVM会唤醒所有等待在这个条件队列上的线程。

```java
public class notifyTest {

  final static Object objLock = new Object();

  static boolean predicateCondition = false;

  public static void main(String[] args) throws InterruptedException {

    Vector<Thread> list = new Vector<>();
    int count = 3;
    CountDownLatch latch = new CountDownLatch(count);
    for (int i = 0; i < count; i++) {
      int finalI = i;
      new Thread(() -> {
        list.add(Thread.currentThread());
        Thread.currentThread().setName("t" + finalI);
        synchronized (objLock) {
          while (!predicateCondition) {
            log.info("线程{}: 条件谓词为假，阻塞等待", Thread.currentThread().getName());
            try {
              latch.countDown();
              objLock.wait();
            } catch (InterruptedException e) {
              throw new RuntimeException(e);
            } catch (Exception e) {
              throw new RuntimeException(e);
            }
          }
          log.info("线程{}：条件谓词为真，执行任务", Thread.currentThread().getName());
        }
      }).start();
    }
    latch.await();
    Thread.sleep(1000);

    // 主线程唤醒其他线程
    synchronized (objLock) {
      predicateCondition = true;
      log.info("notify() 将会随机唤醒一个线程，notifyAll将会唤醒所有线程");
      //      objLock.notify();
      objLock.notifyAll();
    }

    for (Thread thread : list) {
      thread.join();
    }

    log.info("main end");
  }
```

由于多个线程可以基于不同的条件谓词在同一个条件队列上等待，所以使用notify而是不notifyAll是一种危险的操作，因为单一的通知很容易导致类似于信号丢失的问题。

BoundedBuffer中很好地说明了为什么在大多数情况下应该优先选择notifAlly，而不是单个notify。这里的条件队列用于两个不同的条件谓词：「非空」和「非满」。因为notify唤醒是随机的，假如线程A在条件队列上等待条件谓词PA，同时线程B在条件队列上等待条件谓词PB，现在如果PB变为真，线程C执行一个notify：JVM将随机选一个线程并唤醒，如果唤醒的是线程A，线程A被唤醒后看到PA条件仍然是假，因此将继续等待，同时线程B本来可以开始执行，但却没有被唤醒。这虽然不是严格意义的丢失信号，但是更像一种被劫持的信号，但导致的问题是相同的：线程正在等待一个已经发生过的信号。

只有满足

1. 所有等待线程的类型都相同：只有一个条件谓词与条件队列有关，并且每个线程在从wait返回后将执行相同的操作
2. 单进单出：在条件变量上的每次通知，最多只能唤醒一个线程来执行。

BoundedBuffer不满足所有等待线程的类型都相同，等待的线程有的在等待非满，有的在等待非空。

有时开发人员并不赞同应该优先选择notifAlly，而不是单个notify。有时有很多线程执行时，notifAll唤醒所有线程，然后它们在锁上发生竞争，然后大部分又回到休眠状态，这可能出现大量的上下文切换和锁竞争开销。

我们还可以对BoundedBuffer的put和take进行性能优化，仅当操作影响到状态变化时，才进行通知。这也被称为条件通知。虽然条件通知提升了性能，但在大部分场景中，却很难正确实现，因为你要仔细的估算条件谓词，在什么场景下以及考虑到条件谓词之间的依赖关系，很容易漏掉某个边界条件，因此需要谨慎使用。

```java
public synchronized void put(V v) {
    while(isFull()) {
        wait();
    }
    boolean wasEmpty = isEmpty();
    doPut(v);
    if(wasEmpty()) {
        notifyAll();
    }
}
```
