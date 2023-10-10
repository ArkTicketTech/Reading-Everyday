安全性是指永远不会发生糟糕的事情，活跃性是指：某件正确的事情最终会发生。

当某个操作无法执行下去的时候，就会发生活跃性问题。在串行程序中，活跃性问题的形式之一就是无意中造成的无限循环。活跃性问题包括死锁、饥饿、活锁。



在安全性与活跃性之间通常存在着某种制衡。我们使用加锁机制来确保线程安全，

1. 但如果过度使用加锁，则可能导致锁顺序死锁(Lock-Ordering DeadLock）；
2. 同样，如果使用线程池和信号量来限制对资源的使用，那么这些被线程的行为可能导致资源死锁（Resource DeadLock）。



Java程序无法从死锁中恢复过来，因此在设计时就一定要排除那些可能导致死锁出现的跳啊键，本章将介绍一些导致活跃性故障的原因，以及如何避免它们。



# 死锁

经典的哲学家就餐问题很好的描述了死锁情况。5个哲学家去吃中餐，坐在一张圆桌上。它们有5根筷子，并且每一根筷子放在每两个人中间。哲学家们时而思考，时而进餐。每个人都需要一双筷子才能吃到东西，并在吃完后将筷子放回原处继续思考。有些筷子管理算法能使每个人都能相对及时地吃到东西，但有些算法却可能导致一些或者所有哲学家都「饿死」，比如每个人都立即抓住自己左边的筷子，然后等待自己右边的筷子空出来，但同时又不放下已经拿到的筷子，这种情况将产生所谓的死锁：每个人都拥有其他人需要的资源，同时又等待其他人已经拥有的资源，并且每个人在获得所有需要的资源之前都不会放弃已经拥有的资源。

当一个线程永远地豉油一个锁，并且其他线程都尝试获得这个锁时，那么它们将永远被阻塞。在线程A持有锁L并想获得锁M的同时，线程B持有锁M并尝试获得锁L，那么这两个线程将永远地等待下去，这种情况就是最简单的死锁形式，或者称为抱死（Deadly Embrace），其中多个线程由于存在环路的锁依赖关系而永远地等待下去。

在数据库系统的设计中考虑了检测死锁以及从死锁中恢复，在执行一个事务时可能需要获取多个锁，并一直持有这些锁直到事务提交，因此在两个事务之间很可能发生死锁，但数据库服务器不会让死锁一直持续下去，当它检测到一组事务发生了死锁时，将选择一个牺牲者并放弃这个事务。

JVM并没有解决死锁的问题，当一组Java线程发生死锁时，它们将一直等待下去不会被打断。根据线程完成工作的不同，死锁可能造成应用程序完全停止，或者某个子系统停止，或者是性能降低。死锁往往发生在高负载的情况下。

## 锁顺序死锁



```java
public class LeftRightDeadLock {
  private final Object left = new Object();
  private final Object right = new Object();

  public void leftRight() {
    synchronized(left) {
      synchronized(right) {
        doSomething();
      }
    }
  }

  public void rightLeft() {
    synchronized(right) {
      synchronized(left) {
        doSomething();
      }
    }
  }
}
```

leftRight和rightLeft尝试以不同的顺序获取相同的锁，很可能就产生了死锁。如果按照相同的顺序请求锁，就不会出现循环的加锁依赖性，也就不会产生死锁。

如果所有线程以固定的顺序来获得锁，那么在程序中就不会出现锁顺序死锁的问题。

要想验证锁顺序的一致性，需要对程序中的加锁行为进行全局分析。





## 动态的锁顺序死锁



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

有时候并不能很清楚的知道是否在锁顺序上有足够的控制权来避免死锁的发生，比如加锁顺序是函数参数的顺序，而这些参数是依赖于外部输入的，如果一个线程从X向Y转账，另一个线程从Y向X转账，那就可能发生死锁。

```java
A: transferMoney(X, Y, 100);
B: transferMoney(Y, X, 200);
```

要解决这个问题，必须定义锁的顺序，并在整个应用程序中都按照这个顺序来获得锁。锁的顺序可以通过System.identityHashCode方法计算哈希值，通过哈希值的大小来确定顺序，极小的可能下，两个对象拥有相同的散列值，这种情况可以引入一个「加时赛」锁tieLock，保证每次只有一个线程获取这个锁。如果Account中包含一个唯一的、不可变的，并且具备可比性的键值，比如账号，就可以通过键值对对象进行排序，也不需要「加时赛」锁了。

```java
public class InduceLockOrder {
  private static final Object tieLock = new Object();

  public void transferMoney(final Account fromAcct,
                            final Account toAcct,
                            final DollarAmount amount)
  throws InsufficientFundsException {
    class Helper {
      public void transfer() throws InsufficientFundsException {
        if (fromAcct.getBalance().compareTo(amount) < 0)
          throw new InsufficientFundsException();
        else {
          fromAcct.debit(amount);
          toAcct.credit(amount);
        }
      }
    }
    int fromHash = System.identityHashCode(fromAcct);
    int toHash = System.identityHashCode(toAcct);

    if (fromHash < toHash) {
      synchronized (fromAcct) {
        synchronized (toAcct) {
          new Helper().transfer();
        }
      }
    } else if (fromHash > toHash) {
      synchronized (toAcct) {
        synchronized (fromAcct) {
          new Helper().transfer();
        }
      }
    } else {
      synchronized (tieLock) {
        synchronized (fromAcct) {
          synchronized (toAcct) {
            new Helper().transfer();
          }
        }
      }
    }
  }

  interface DollarAmount extends Comparable<DollarAmount> {
  }
  interface Account {
    void debit(DollarAmount d);
    void credit(DollarAmount d);
    DollarAmount getBalance();
    int getAcctNo();
  }
  class InsufficientFundsException extends Exception {
  }
}
```

死锁一旦发生，后果往往都是很严重的，所以要严肃对待。



## 在协作对象之间发生的死锁

出租车系统调度，Taxi代表一个出租车对象，包含位置和目的地两个属性，Dispatcher代表一个出租车车队。

```java
public class Taxi {
  private Point location, destination;
  private final Dispatcher dispatcher;
  
  public Taxi(Dispatcher dispatcher) {
  	this.dispatcher = dispatcher;
  }

  public synchronized Point getLocation() {
    return location;
  }

  public synchronized void setLocation(Point location) {
    this.location = location;
    if(location.equals(destination)) {
      // 到达目的地，该车变成空闲状态
      dispatcher.notifyAvailable(this);
    }
  }
}

class Dispatcher {
  // 所有的出租车
  private final Set<Taxi> taxis;
  // 空闲的出租车
  private final Set<Taxi> availableTaxis;
  public Dispatcher() {
    taxis = new HashSet<>();
    availableTaxis = new HashSet<>();
  }

  public synchronized void notifyAvailable(Taxi taxi) {
    availableTaxis.add(taxi);
  }
 
  public synchronized Image getImage() {
    Image image = new Image();
    for(Taxi t : taxis) {
      image.drawMarker(t.getLocation());
    }
    return image;
  }
  
}
```

看起来没有任何方法会显式地获取两个锁，但setLocation和getImage都会获得两个锁。如果一个线程在出租车到达目的地后收到GPS信号调用setLocation方法，它会首先获取taxi的锁，然后获取Dispatcher的锁，同样，调用getImage方法的线程会首先获得Dispatcher的锁，然后获取taxi的锁，这种情况就跟之前LeftRightDeadLock中的情况相同了，两个线程按照不同的顺序获得两个锁，就可能产生死锁。但这种情况要查找死锁比较困难，所以需要警惕在持有锁的情况下调用某个外部方法。

如果持有锁时调用某个外部方法，在这个外部方法中困难会获取其他锁，或者阻塞时间过长，都会导致其他线程无法及时获得当前被持有的锁。



## 开放调用

如果在调用某个方法时不需要持有锁，那么这种调用被称为开放调用（Open Call），其实就是不要在持有锁时去调用外部方法。

这种通过开放调用来避免死锁的方法，类似于采用封装机制来提供线程安全的方法。开放调用避免了复杂的获取锁的顺序，使得更容易确保一致的顺序获取锁。

我们把上面的例子改为开放调用：

```java
class CooperatingNoDeadlock {
  @ThreadSafe
  class Taxi {
    @GuardedBy("this") private Point location, destination;
    private final Dispatcher dispatcher;

    public Taxi(Dispatcher dispatcher) {
      this.dispatcher = dispatcher;
    }

    public synchronized Point getLocation() {
      return location;
    }

    public synchronized void setLocation(Point location) {
      boolean reachedDestination;
      synchronized (this) {
        this.location = location;
        reachedDestination = location.equals(destination);
      }
      if (reachedDestination)
        dispatcher.notifyAvailable(this);
    }

    public synchronized Point getDestination() {
      return destination;
    }

    public synchronized void setDestination(Point destination) {
      this.destination = destination;
    }
  }

  @ThreadSafe
  class Dispatcher {
    @GuardedBy("this") private final Set<Taxi> taxis;
    @GuardedBy("this") private final Set<Taxi> availableTaxis;

    public Dispatcher() {
      taxis = new HashSet<Taxi>();
      availableTaxis = new HashSet<Taxi>();
    }

    public synchronized void notifyAvailable(Taxi taxi) {
      availableTaxis.add(taxi);
    }

    public Image getImage() {
      Set<Taxi> copy;
      synchronized (this) {
        copy = new HashSet<Taxi>(taxis);
      }
      Image image = new Image();
      for (Taxi t : copy)
      image.drawMarker(t.getLocation());
      return image;
    }
  }

  class Image {
    public void drawMarker(Point p) {
    }
  }

}
```

修改为开放调用后，消除了发生死锁的风险。但是却将setLocation的原子操作变成了非原子操作：更新出租车的位置并通知这个出租车已经处于空闲状态。但这里这两个操作的原子性看起来其实没有必要。同时getImage的语义也发生了变化，之前是获取某个时刻下整个车队位置的快照，现在是获得每辆出租车不同时刻的位置。因为之前setLocation和getLocation都是有锁的，所以getImage可以获取快照，但现在都没了synchronized锁，所以getImage时可能发生不断的setLocation。

在某些情况下，如果要保持原子性，需要通过其他技术来实现，通过一些协议来实现。

## 资源死锁

当线程间相互等待并持有对方需要的锁时，就会发生死锁，当线程持有和等待的目标变成资源的时候，也会发生与之类似的死锁。假如你有两个放入池中的资源，比如分别是到两个数据库的连接池。资源池通常通过信号量实现，当池为空的时候发生阻塞，如果一个任务需要连接到两个数据库，并且两个资源并不是按照相同顺序调用的，线程A可能持有数据库D1的连接，并等待连接到数据库D2，而线程B持有D2的连接，并等待D1的连接，如果两个线程都在等待两个资源池释放资源，如果每个线程池都有N套连接，死锁的发生需要N套循环等待的线程，并且产生了死锁的时序，当线程池越大时，这种情况越不可能发生，但当线程池非常小时，就可能发生这样一个情况。

另一种基于资源的死锁是之前提及的线程饥饿死锁，一个任务提交到单线程的Executor，但这个任务需要等待线程池提交新的任务。需要等待其他任务的结果的任务是生成线程饥饿死锁的来源，有界池和相互依赖的任务不能放在一起使用。

# 死锁的避免与诊断

对于锁顺序死锁，在设计时必须考虑锁的顺序，尽量减少潜在的加锁交互数量，将获取锁时需要遵循的协议写入正式文档并始终遵循这些协议。

在使用细粒度锁的程序中，可以通过一种两阶段策略（two-party Strategy)来检查代码中的死锁：首先找出什么地方将获取多个锁，然后对所有这些实例进行全局分析，从而确保它们在整个程序中获取锁的顺序都保持一致。尽可能使用开放调用，这能极大地简化分析过程。



## 支持定时的锁

还有一些技术可以通过检测死锁和从死锁中恢复过来，即显式使用Lock类中的定时tryLock功能来代替内置锁机制。当使用内置锁时，只要没有获取锁，就会永远等待下去，而显式锁则可以指定一个超时时限，在等待超过该时间后tryLock会返回一个失败信息，如果超时时限比获取锁的时间要长很多，那么就可以在发生某个意外情况下重新获得控制权。



## 通过Thread Dump来分析死锁

Thread Dump是一个诊断Java应用问题的工具，它包括各个运行中的线程的栈追踪信息，加锁信息（每个线程持有了哪些锁，在哪些栈桢中获得这些锁，以及被阻塞的线程正在等待获取哪一个锁），Thread Dump时，JVM将找出死锁信息，如死锁中涉及哪些锁和线程，以及这个锁的获取操作位于程序的哪些位置。

要触发Thread Dump操作，有几种方法：

1. jstack -l <pid> > threadDump.txt

1. 1. pid可以通过jps找到

1. visualVM

一般当服务器挂起，崩溃或者性能低下时，就需要抓取服务器的线程堆栈（Thread Dump）用于后续的分析。在实际运行中，往往一次 dump的信息，还不足以确认问题。为了反映线程状态的动态变化，需要接连多次做thread dump，每次间隔10-20s，建议至少产生三次 dump信息，如果每次 dump都指向同一个问题，我们才确定问题的典型性。





# 其他活跃性危险

尽管死锁是最常见的活跃性危险，但在并发程序中还存在一些其他的活跃性危险，包括饥饿、丢失信号和活锁等。

## 饥饿

当**线程由于无法访问它所需要的资源而不能继续执行时**，就发生了饥饿，引发饥饿的最常见的资源就是CPU时钟周期。如果在Java程序中对线程的优先级使用不当，或者在持有锁时执行一些无法结束的结构，例如无限循环，或者无限制的等待某个资源，那么也可能导致饥饿，因为其他需要这个锁的线程将无法得到它。

尽量不要修改线程的优先级，这很可能造成线程饥饿

## 糟糕的响应性

除饥饿以外的另一个问题是糟糕的响应性，后台线程与事件线程竞争影响面向用户的线程的响应；不良的锁管理导致长任务也会导致糟糕的响应。

## 活锁

活锁是另一种形式的活跃性问题，该问题尽管不会阻塞线程，但也不能继续执行任务，因为线程将不断重复执行相同的操作，而且总会失败。

死锁是阻塞时就死等，但活锁是发现死锁后立刻放开已经获得锁进行重试。

当多个相互协作的线程都对彼此进行响应从而修改各自的状态，并使得任何一个线程都无法继续执行下去的时候，就发生了活锁。比如线程A持有锁D1等待D2，线程B持有D2等待D1，由于它们发现了问题所以都开始释放各自持有的锁，然后重新开始执行，结果因为又是同时开始的，所以会反复的重试失败下去，陷入了一种失败的循环。

```latex
start
A locks D1, B locks D2
A locks D2 fail, B locks D1 fail
A release D1, B release D2
try again, goto start
```

要解决这种活锁问题，需要在重试机制中引入随机性，让线程A和线程B都等待一个随机时间后进行重试，再加上回退（以太协议定义了在重复发生冲突时采用指数回退机制）来避免活锁的发生。
