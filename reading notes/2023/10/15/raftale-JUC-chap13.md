# Lock 和 ReentrantLock

ReentrantLock 并不是一种替代内置锁 synchronized 的一种方法，而是当内置锁加锁机制不适用时，作为一种可选择的高级功能。

Lock 接口定义了一组抽象的加锁操作，与内置加锁机制不同的是，Lock 提供了一种无条件、可轮询的、定时的以及可中断的锁获取操作，所有加锁和解锁都是显式的。在 Lock 的实现中必须提供与内置锁相同的内存可见性语义，但在加锁语义、调度算法、顺序保证、性能特性等方面可以有所不同。

```java
public interface Lock {
    // 加锁
    void lock();
    // 加可中断的锁
    void lockInterruptibly() throws InterruptedException;
    // 尝试获取锁，立即返回；
    boolean tryLock();
    // 指定时间内尝试获取锁，超过时间没拿到锁返回false
    boolean tryLock(long timeout, TimeUnit unit) throws InterruptedException;
    // 解锁
    void unlock();
    //
    Condition newCondition();
}
```

ReentrantLock 实现了 Lock 接口，并提供了与 synchronized 相同的互斥性和内存可见性。

创建ReentrantLock 的原因是内置锁在功能上存在一些局限性：

1. 无法中断一个正在等待获取锁的线程：lockInterruptibly()可以
2. 不可能获取一个有超时时间的锁：tryLock(long timeout, TimeUnit unit) 可以



其实就是 Lock 实现更灵活的加锁方式。

Lock 使用时的标准形式：

```java
public void lockMethod() {
  Lock l = ...;  
  l.lock();  
  try {    
    // access the resource protected by this lock 
  } 
  finally {    
    l.unlock();  
  }
}
public void tryLockMethod() {

  Lock lock = new ReentrantLock();  
  if (lock.tryLock()) {    
    try {      // manipulate protected state    
    } finally {    
      lock.unlock();    
    }  
  } else {    
    // perform alternative actions  
  }
}
```

finally 块中释放锁是必须的，如果没有使用 finally 来释放锁，发生错误时也是很难追踪发生错误的位置的。所以 ReentrantLock 的使用需要非常谨慎。

## 轮询锁与定时锁

可定时的与可轮询的锁获取模式是由 tryLock 方法实现的，与无条件的锁获取模式相比，它具有更完善的错误恢复机制。在内置锁中，死锁是一个很严重的问题，恢复程序的唯一方法是重新启动程序，而防止死锁的唯一方法就是在构造程序时避免出现不一致的锁顺序。可定时的与可轮询的锁提供了另外一种选择：避免死锁的发生。

我们用 tryLock 来解决 10.1.2 章节中关于动态顺序死锁的问题：

```java
public void transferMoney(Account fromAccount, Account toAccount, DollarAmout amount) throws InsufficientFundsException {
  synchronized (fromAccount) {
    synchronized(toAccount) {
      if(fromAccount.getBalance().compareTo(amount) < 0) {
        throw new InsufficientFundsException();
      } else {
        fromAccount.debit(amount);
        toAccount.credit(amount);
      }
    }
  }
}
```





使用 tryLock 来获取两个锁，如果不能同时获得，那就回退并重新尝试。在休眠时间内包含随机，从而降低活锁发生的可能性。如果在指定时间内不能获取所有需要的锁，那么 transferMoney 将返回一个失败状态，从而使该操作平缓地失败。

```java
import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.locks.*;
import static java.util.concurrent.TimeUnit.NANOSECONDS;

/**
 * DeadlockAvoidance
 * <p/>
 * Avoiding lock-ordering deadlock using tryLock
 *
 * @author Brian Goetz and Tim Peierls
 */
public class DeadlockAvoidance {
  class Account {
    public Lock lock;

    void debit(DollarAmount d) {
    }

    void credit(DollarAmount d) {
    }

    DollarAmount getBalance() {
      return null;
    }
  }

  private static Random rnd = new Random();

  public boolean transferMoney(Account fromAcct,
                               Account toAcct,
                               DollarAmount amount,
                               long timeout,
                               TimeUnit unit)
  throws InsufficientFundsException, InterruptedException {
    long fixedDelay = getFixedDelayComponentNanos(timeout, unit);
    long randMod = getRandomDelayModulusNanos(timeout, unit);
    long timeoutNanos = unit.toNanos(timeout);
    long startTime = System.nanoTime();

    while (true) {
      // 需要同时获取这两把锁
      if (fromAcct.lock.tryLock()) {
        try {
          if (toAcct.lock.tryLock()) {
            try {
              if (fromAcct.getBalance().compareTo(amount) < 0) {
                throw new InsufficientFundsException();
              } else {
                fromAcct.debit(amount);
                toAcct.credit(amount);
                return true;
              }
            } finally {
              toAcct.lock.unlock();
            }
          }
        } finally {
          fromAcct.lock.unlock();
        }
      }

      if (System.nanoTime() - startTime >= timeoutNanos){
        // 超时返回
        return false;
      }
      // 休眠       
      NANOSECONDS.sleep(fixedDelay + rnd.nextLong() % randMod);
    }
  }

  private static final int DELAY_FIXED = 1;
  private static final int DELAY_RANDOM = 2;

  static long getFixedDelayComponentNanos(long timeout, TimeUnit unit) {
    return DELAY_FIXED;
  }

  static long getRandomDelayModulusNanos(long timeout, TimeUnit unit) {
    return DELAY_RANDOM;
  }

  static class DollarAmount implements Comparable<DollarAmount> {
    public int compareTo(DollarAmount other) {
      return 0;
    }

    DollarAmount(int dollars) {
    }
  }



  class InsufficientFundsException extends Exception {
  }
}
```

定时锁：future.get(timeout, unit)可能就是用的定时锁，在有时间限制的操作中，如果操作不能在指定时间完成，那么就会使程序提前结束。

定时的 tryLock 能够在带有时间限制的操作中实现独占加锁行为。

```java
import java.util.concurrent.*;
import java.util.concurrent.locks.*;
import static java.util.concurrent.TimeUnit.NANOSECONDS;

/**
 * TimedLocking
 * <p/>
 * Locking with a time budget
 *
 * @author Brian Goetz and Tim Peierls
 */
public class TimedLocking {
  private Lock lock = new ReentrantLock();

  public boolean trySendOnSharedLine(String message,
                                     long timeout, TimeUnit unit)
  throws InterruptedException {
    long nanosToLock = unit.toNanos(timeout) - estimatedNanosToSend(message);
    // 超过时间都没有获取到锁时，直接返回false
    if (!lock.tryLock(nanosToLock, NANOSECONDS)) {
      return false;
    }
    try {
      return sendOnSharedLine(message);
    } finally {
      lock.unlock();
    }
  }

  private boolean sendOnSharedLine(String message) {
    /* send something */
    return true;
  }

  long estimatedNanosToSend(String message) {
    return message.length();
  }
}
```



## 可中断的锁获取操作

正如定时的锁获取操作能够在带有时间限制的操作中使用独占锁，可中断的锁获取操作同样能在可取消的操作中使用加锁。lockInterruptibly 方法能够在获得锁的同时保持对中断的响应。

```java
import java.util.concurrent.locks.*;

/**
 * InterruptibleLocking
 *
 * @author Brian Goetz and Tim Peierls
 */
public class InterruptibleLocking {
  private Lock lock = new ReentrantLock();

  public boolean sendOnSharedLine(String message)
  throws InterruptedException {
    lock.lockInterruptibly();
    try {
      return cancellableSendOnSharedLine(message);
    } finally {
      lock.unlock();
    }
  }

  private boolean cancellableSendOnSharedLine(String message) throws InterruptedException {
    /* send something */
    return true;
  }

}
```

定时的 tryLock 同样能响应中断，因此当需要实现一个定时的和可中断的锁获取操作时，可以使用 tryLock 方法。



## 非块结构的加锁

内置锁的获取和释放等操作都是基于代码块的，基于锁分段的思想，可以 给每个链表节点加一个独立的锁，使不同线程能独立的对链表不同部分进行操作。

连锁式加锁。

# 性能考虑因素

# 公平性

ReentrantLock 的构造函数中提供了两种公平性选择：创建一个非公平的锁（默认）或者一个公平的锁。

```java
public class ReentrantLock implements Lock, java.io.Serializable {
  public ReentrantLock(boolean fair) {
    sync = fair ? new FairSync() : new NonfairSync();
  }  
}
```

在公平锁上，线程将按照它们发出请求的顺序来获得锁。

但当一个线程请求非公平锁时，则允许插队，如果在发生请求的同时该锁的状态变为可用，那么这个线程将跳过队列中所有的等待线程并获取这个锁，如果锁被持有，那请求线程将会被放入到队列中。

1. 非公平锁无法防止某个线程在合适的时候进行插队；
2. 获取公平锁时，所有请求的线程都放入队列中遵循先来后到的原则；



在激烈竞争的情况下，非公平锁的性能高于公平锁的性能，原因在于：恢复一个被挂起的线程与该线程开始真正运行之间存在着严重的延迟，也就是上下文切换的开销。假设线程 A 持有一个锁，线程 B 被挂起，当 A 释放锁时，B 将被唤醒，与此同时，C 也在请求这个锁，C 很可能在 B 被完全唤醒之前获得这个锁。这是一种双赢的局面，B 获得锁的上下文开销没有减少，但 C 更早的获取了锁，吞吐量获得了提高。

当持有锁的时间相对较长，或者请求锁的平均时间间隔较长，那么应该使用公平锁。在这些情况下，插队不会带来吞吐量的提升。

与默认的 ReentrantLock 一样，内置加锁并不会提供确定的公平性保证，但在大多数情况下，在锁实现上实现统计上的公平性保证就足够了。

# 在 Synchronized 和 ReentrantLock 之间进行选择



在一些内置锁无法满足需求的情况下，ReentrantLock 可以作为一种高级工具。当需要一些高级功能时才应该使用 ReentrantLock，这些功能包括：可定时的、可轮询的、可中断的锁获取操作，公平队列，以及非块结构的锁。否则，还是应该优先使用 synchronzied。



# 读写锁

ReentrantLock 实现了一种标准的互斥锁：每次最多只有一个线程持有ReentrantLock，但对于维护数据的完整性来说，互斥通常是一种过于强硬的加锁规则，因此也就不必要的限制了并发性。互斥是一种保守的加锁策略，可以避免：

1. 写写冲突
2. 写读冲突
3. 读读冲突

许多情况下 ，数据操作都是读操作，如果能够放宽加锁需求，允许多个执行读操作的线程同时访问数据结构，只要每个线程都能确保读取到最新的数据，并且在读取数据时不会有其他的线程修改数据，那么就可以避免读读冲突，从而提高性能。这种情况可以使用读写锁：

1. 一个资源可以被多个读操作同时访问
2. 或者被一个写操作访问
3. 1、2 点不能同时进行

Juc 包中有 ReadWriteLock 接口，ReadWriteLock 保护的数据进行读取时，必须先获取到读锁，需要进行修改时，必须先获得写锁。尽管两个锁看上去是彼此独立的，但读取锁和写入锁只是「读-写锁」的不同视图。

```java
public interface ReadWriteLock {
  // 用于读操作
  Lock readLock();

  // 用于写操作
  Lock writeLock();
}
```



「读-写锁」的加锁策略中，允许多个读操作同时进行，但每次只允许一个写操作（写读相互阻塞，但读不阻塞读），与 Lock 一样，ReadWriteLock 可以采用不同的实现方式，这些方式在性能、调度保证、获取优先性、公平性以及加锁语义等方面可能有所不同。

读-写锁是一直性能优化措施，对于在多处理器系统上被频繁读取的数据结构，读-写锁能提高性能。而在其他情况下，读写锁的性能比独占锁略差一些，因为读写锁的复杂度更高。

在读取锁和写入锁之间的交互可以采用多种实现方式，ReadWriteLock 中的一些可选实现包括

1. 写锁释放后的优先选择哪个线程：当一个写入操作释放写入锁时，队列中同时存在读线程和写线程，那么应该优先选择读线程还是写线程，还是最先发出请求的线程？
2. 读线程插队：如果锁由读线程持有，但有写线程正在等待，那么新到达的读线程能否立即获得访问权，还是应该在写线程后面等待？如果允许读线程插队，那么将提高并发性，但写线程却可能发生饥饿问题。
3. 重入性：读锁和写锁是否是可重入的。
4. 降级：如果一个线程持有写入锁，那么它能够在不被释放的情况下获得读取锁？这样可能会使得写入锁被降级为读取锁，同时不允许其他写线程修改被保护的资源。（这是为了提升性能？）
5. 升级：读取锁是否有优先其他正在等待的读线程和写线程而升级为一个写入锁？在大多数的读写锁实现中并不支持升级，因为如果没有显式的升级操作，那么很容易造成死锁，如果两个线程都试图同时升级为写入锁，那么两者都不会释放读取锁。



ReentrantReadWriteLock：

1. 写读相互阻塞，但读不阻塞读；
2. 读锁和写锁都提供了可重入的加锁语义；
3. 构造函数提供了非公平锁和公平锁的选项；

1. 1. 公平锁：等待时间最长的线程将获取锁。
2. 非公平锁：线程获取访问许可的顺序是不确定的，写线程降级为读线程是可以的，但读线程升级为写线程是不可以的。



当锁的持有时间较长并且大部分操作都不会修改守护资源时，读写锁能够显著提高并发性，甚至可能比 ConcurrentHashMap 性能更高。下面我们用ReentrantReadWriteLock来包装 Map：

```java
public class ReadWriteMap<K, V> {
    private final Map<K, V> map;
    private final ReadWriteLock lock = new ReentrantReadWriteLock();
    private final Lock r = lock.readLock();
    private final lock w = lock.writeLock();

    public ReadWriteMap(Map<K, V> map) {
        this.map = map;
    }

    public V put(K key, V value) {
        w.lock();
        try {
            return map.put(key, value);
        } finally {
            w.unlock();
        }
    }

    public V get(Object key) {
        r.lock();
        try {
            return map.get(key);
        } finally {
            r.unlock();
        }
    }
}
```

