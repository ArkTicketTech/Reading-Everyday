## 示例：阀门类

第5章时，讲到latch的缺点是只能执行一次，通过latch构造的阀门在重新打开后将无法关闭。

通过使用条件等待，可以很容易地开发一个可重新关闭的ThreadGate类，如下程序锁死：

```java
import net.jcip.annotations.*;

/**
 * ThreadGate
 * <p/>
 * Recloseable gate using wait and notifyAll
 *
 * @author Brian Goetz and Tim Peierls
 */
@ThreadSafe
public class ThreadGate {
  // CONDITION-PREDICATE: opened-since(n) (isOpen || generation>n)
  @GuardedBy("this") private boolean isOpen;
  @GuardedBy("this") private int generation;

  public synchronized void close() {
    isOpen = false;
  }

  public synchronized void open() {
    ++generation;
    isOpen = true;
    notifyAll();
  }

  // BLOCKS-UNTIL: opened-since(generation on entry)
  public synchronized void await() throws InterruptedException {
    int arrivalGeneration = generation;
    // notifyAll唤醒后再次判断条件isOpen，但如果阀门打开后又很快的关闭了，那还是会陷入等待，
    // 所以用arrivalGeneration来判断前提条件，防止这种场景。
    // 不过这里跟之前说的需要再次判断前提条件是否为真感觉有点矛盾。
    while (!isOpen && arrivalGeneration == generation) {
    	wait();
    }
  }
}
```

由于ThreadGate类只支持等待打开阀门，如果还想要「等待关闭」，那必须要在close中也进行通知。这也显示出：当增加一个新的状态依赖操作时，可能要对多条修改对象的代码路径进行改动，才能正确的执行通知。



## 子类的安全问题

状态依赖的类如果保证继承不对条件通知的安全性造成威胁，要么将其等待和通知等协议完全向子类公开并且写入正式文档，要么完全禁止子类化。禁止子类化可以将类声明为final类型，或者将条件队列、锁和状态变量全部隐藏起来。

## 封装条件队列

为了避免调用者不加限制的使用条件队列，应该把条件队列封装起来，但封装条件队列对象后，BoundedBuffer中缓存对象即是锁，又是条件队列。

## 入口协议与出口协议

Entry and Exit Protocols

Wellings通过入口协议与出口协议来描述wait和notify方法的正确使用。对于每个依赖状态的操作，以及每个修改其他操作依赖状态的操作，都应该定义一个入口协议和出口协议。入口协议就是该操作的条件谓词，出口协议则包括，检查被该操作修改的所有状态变量，并确认它们是否使某个其他的条件谓词变为真，如果是，则通知相关的条件队列。

AbstractQueuedSynchronizer中使用出口协议，juc中大多数状态依赖的类都是基于这个类构建的，这个类并不是同步器类执行自己的通知，而是要求同步器方法返回一个值来表示该类的操作是否已经解除了一个或多个等待线程的阻塞，这种明确的API调用需求使得更难以「忘记」在某些状态转换发生时进行通知。
