本章将介绍一些组合模式，这些模式能够使一个类更容易称为线程安全的。

# 设计线程安全的类

设计线程安全的类，封装性非常重要，封装的好，可以使得在不对整个程序进行分析的情况下就可以判断一个类是否是线程安全的。

在设计线程安全的类的过程中，需要包含以下三个基本要素：

1. 找出构成对象状态的所有变量；
2. 找出约束状态变量的不变性条件；
3. 建立对象状态的并发访问管理策略：同步策略

## 收集同步需求

要确保类的线程安全性，就需要确保它的不变性条件不会在并发访问的情况下被破坏，这就需要对其状态进行推断。

同样，在操作中还会包含一些后验条件来判断状态的转换是否是有效的，例如计数器的下一个状态依赖上一个状态，并非所有的操作都存在所有的状态依赖之前的状态，如果不依赖，当然会变得更简单。

所以，是存在 由于不变性条件以及后验条件在**状态以及状态转换**上施加了各种约束 的情况的，因此这种情况就需要额外的同步与封装。如果某些状态是无效的，那么必须对底层的状态变量进行封装，否则客户代码可能会使对象处于无效状态；如果在某个操作中存在无效的状态转换（这里指的就是由于竞态条件导致的失效），那么该操作就必须是原子的。另外，如果在类中没有施加这种约束，那么就可以放宽封装性或序列化等需求，以便获得更高的性能。

在类中也可以包含同时约束多个状态变量的不变性条件。



如果不了解对象的不变性条件与后验条件，那么就不能确保线程安全性。要满足在状态变量的有效值或者状态转换上的各种约束条件，就需要借助于原子性与封装性。

## 依赖状态的操作

如果某个操作中包含基于状态的先验条件，那么这个操作就称为依赖状态的操作。比如不能从空队列中移除一个元素。

在单线程程序中，如果某个操作无法满足先验条件，那么就只能失败。但是在并发程序中，先验条件可能会由于其他线程执行的操作而变为真。在并发程序中要一直等到先验条件为真，然后再执行该操作。

在Java中，等待某个条件为真的各种内置机制（包括等待和通知机制）都与内置加锁机制紧密关联，要想正确的使用它们并不容易。

要想实现某个等待先验条件为真时才执行的操作，一种更简单的方法是通过现有库中的类，例如阻塞队列或者信号量来实现依赖状态的行为。

第14章会介绍如何利用底层机制来创建依赖状态的类。

## 状态的所有权

所有权：Ownership

所有权与封装性：对象封装它拥有的状态，对它封装的状态拥有所有权。

状态变量的所有者将决定采用何种加锁协议来维持变量状态的完整性，所有权意味着控制权，然而，如果发布了某个可变对象的引用，那么就不再拥有独占的控制权，最多是「共享控制权」。

容器类通常表现出一种「所有权分离」的形式，其中容器类拥有其自身的状态，而客户代码拥有容器中各个对象的状态。



# 实例封闭

如果某对象不是线程安全的，之前讨论过线程封闭，线程封闭说的是确保某个对象只能在单个线程内访问。线程封闭避免了共享状态，但如果不适合使用线程封闭，那加锁来保护对象的状态就是另外一种选择。

程序中经常会有封装的策略，封装提供了一种实例封闭机制（Instance confinement），实例封闭说的是，当一个对象被封装到另一个对象中时，能够访问被封装对象的所有代码路径都是已知的，通过将封闭机制与合适的加锁策略结合起来，可以确保以线程安全的方式来使用非线程安全的对象。

将数据封装在对象内部，可以将数据的访问限制在对象的方法上，从而更容易确保线程在访问数据时总能持有正确的锁。

被封闭对象一定不能超出它们既定的作用域（对象不能逸出），对象可以封闭在：

1. 私有方法中
2. 局部变量中
3. 线程内，某个线程将对象从一个方法传递到另一个方法，而不是在多个线程之间共享该对象



实例封闭是构建线程安全类的一个最简单方式，他还使得在锁策略的选择上拥有了更多的灵活性。

Java类库中有很多线程封闭的示例，其中有些类的唯一用途就是**将非线程安全的类转换为线程安全的类**。

比如**Collections.synchronizedList的**工厂方法通过装饰器模式将线程不安全的ArrayList类的方法都变成同步方法，并将请求调用转发到底层的容器对象上。只要包装器对象拥有对底层容器对象的唯一引用，即把底层容器对象封闭在包装起中，那么它就是线程安全的。

## Java监视器模式

从线程封闭原则及其逻辑推论可以得出Java监视器模式，遵循Java监视器模式的对象会把对象的所有可变状态都封装起来，并由对象自己的**内置锁(Synchronized)**来保护。

synchronized作用在方法上时，锁就是this当前对象实例。

在许多类中都使用了Java监视器模式，例如Vector和HashTable，Java监视器模式的主要优势是它的简单性，如果对性能有要求，第11章会通过更细粒度的加锁策略来提高可伸缩性。

Java监视器模式仅仅是一种编写代码的约定，对于任何一种锁对象，只要自始自终都使用该锁对象，都可以用来保护对象的状态。

锁对象可以是对象锁（this）、类锁（XxxxClass.class）、其他的私有对象锁。

下面看看用私有锁保护对象的状态：

```java
public class PrivateLock {
    private final Object myLock = new Object();
    Widget widget;
    void doSomething() {
        synchronized(myLock) {
            // xxxx
        }
    }
}
```

私有锁的好处：

1. 如果是公有的锁，需要检测客户代码在程序中是否正确的使用了该锁，但如果是私有锁，客户代码无法得到私有锁时，不需要额外的检查；
2. 如果客户代码可以通过公共方法获取该私有锁，可能产生错误，但追踪相对来说非常方便。







## 示例：车辆追踪



我们首先使用监视器模式来构建一个用于调度车辆的「车辆追踪器」。



每台车都由一个ID来标识，并且拥有一个相应的位置坐标(x, y)，`MonitorVehicleTracker`类封装了车辆的标识和位置，它非常适合作用基于MVC模式的数据模型，并且该模型由一个视图线程和多个执行更新操作的线程共享。

视图线程和执行更新操作的线程将并发地访问数据模型，故该模型必须是线程安全的。

```java
@NotThreadSafe
public class MutablePoint {
    public int x;
    public int y;
    public MutablePoint {
        x = 0; 
        y = 0;
    }
    public MutablePoint(MutablePoint p) {
        this.x = p.x;
        this.y = p.y;
    }
}

@ThreadSafe
public class MonitorVehicleTracker {
	private final Map<String, MutablePoint> locations;

    public MonitorVehicleTracker(Map<String, MutablePoint> localtions) {
        this.locations = deepCopy(locations);
    }

    public synchronized Map<String, MutablePoint> getLocations() {
        return deepCopy(locations);
    }

    public synchronized void setLocation(String id, int x, int y) {
        MutablePoint loc = locations.get(id);
        if(loc == null) {
            throw new IllegalArgumentExecption("No such ID: " + id);
        }
        loc.x = x;
        loc.y = y;
    }
    private static Map<String, MutablePoint> deepCopy(Map<String, MutablePoint> m) {
        Map<String, MutablePoint> map = new HashMap<>();
        for(String id : m.keySet()) {
            map.put(id, new MutablePoint(m.get(id));
        }
        return Collections.unmodifiableMap(map);
    }
    
}
```



视图线程读取车辆名称和位置，并将它们显示在界面上，执行更新操作的线程根据车辆传来的坐标修改车辆位置。

```java
public class Main {
  private MonitorVehicleTracker vehicles;
  // 视图线程读取车辆名称和位置，并将它们显示在界面上
  public void render() {
    Map<String, MutablePoint> locations = vehicles.getLocations();
    for(String key : locations.keySet()) {
      renderVehicle(key, locations.get(key));
    }
  }

  // 执行更新操作的线程
  void vehicleMoved(VehicleMovedEvent event) {
    Point loc = event.getNewLocaltion();
    vehicles.setLocation(event.getVehicleId(), loc.x, loc.y);
  }
}
```

虽然MutablePoint类不是线程安全的，但是追踪器类是线程安全的。它所有包含的Map对象和可变的MutablePoint对象都未曾发布。当需要返回车辆的位置时，通过MutablePoint拷贝构造函数或者deepCopy方法复制正确的值，从而生成一个新的Map对象。

这种实现方式是通过在返回客户代码之间复制可变的数据来维持线程安全的，如果遇到车辆容器非常大的情况下将降低性能，因为`getLocations()`是加了同步锁的，`deepCopy`执行时间太长，会导致锁一直被占用，严重降低视图的体验。

此外，如果调用者需要每辆车的最新信息，有可能位置发生了改变，但显示的还是旧坐标。对实时性要求很高，就需要频繁的刷新快照。



# 线程安全性的委托

Delegating thread safety

有时候组合线程安全的类后仍然是线程安全的，有时候则不一定，因为多个线程安全的类之间还有其他的不变性条件，涉及到无效的状态转换时还需要额外的努力来保证线程安全。



我们看一个线程安全的类组合的例子，其功能是统计已处理请求的数量：

```java
@ThreadSafe
public class CountingFactorizer implements Servlet {
    private final AtomicLong count = new AtomicLong(0);

    public long getCount() {
        return count.get();
    }

    public void service(ServletRequest req, ServletResponse resp) {
        BigInteger i = extractFromRequest(req);
        BigInteger[] factors = factor(i);
        count.incrementAndGet();
        encodeIntoResponse(resp, factors);
    }
}
```

CountingFactorizer的状态就是AtomicLong的状态，而AtomicLong是线程安全的，CountingFactorizer也没有对count施加额外的约束，所以CountingFactorizer是线程安全的。我们可以说CountingFactorizer将它的线程安全性**委托**给了AtomicLong：CountingFactorizer的线程安全是因为AtomicLong的线程安全。

## 示例：基于委托的车辆追踪器

对比之前的车辆追踪，做一些改动，首先是可变的类变成不可变的Point类。

```java
@Immutable
public class Point {
    public final int x;
    public final int y;
    
    public Point(int x, int y) {
        this.x = x;
        this.y = y;
    }
}
```

因为Point是不可变对象，所以Point是线程安全的。不可变的值可以被自由的分享与发布，因此在返回location时不需要复制。

DelegatingVehicleTracker没有使用任何显式的同步，所有对状态的访问都由ConcurrentHashMap来管理，而且Map所有的key和value都是不可变的。

```java
@ThreadSafe
public class DelegatingVehicleTracker {
  private final ConcurrentMap<String, Point> locations;
  private final Map<String, Point> unmodifiableMap;

  public DelegatingVehicleTracker(Map<String, Point> points) {
    locations = new ConcurrentHashMap<>(points);
    unmodifiableMap = Collections.unmodifiableMap(locations);
  }

  public Map<String, Point> getLocations() {
    return unmodifiableMap; // 实时拷贝
//     return Collections.unmodifiableMap(new HashMap<>(locations)); // 静态拷贝
  }
  public Point getLocation(String id) {
    return locations.get(id);
  }

  public void setLocation(String id, int x, int y) {
    if(locations.replace(id, new Point(x, y)) == null) {
      throw new IllegalArgumentException("invalid vehicle name: " + id);
    }
  }

  public void addVehicle(String id, int x, int y) {
    locations.put(id, new Point(x, y));
  }

  public static void main(String[] args) throws InterruptedException {
    Map<String, Point> map = new HashMap<>();
    map.put("car-1", new Point(1, 1));
    map.put("car-2", new Point(2, 2));

    DelegatingVehicleTracker tracker = new DelegatingVehicleTracker(map);
    Thread t1 = new Thread(() -> {

      tracker.getLocations().forEach((key, value) -> System.out.println(key + " ===> " + value));

      try {
        TimeUnit.SECONDS.sleep(1);
      } catch (InterruptedException e) {
        throw new RuntimeException(e);
      }
      System.out.println("再次访问");
      tracker.getLocations().forEach((key, value) -> System.out.println(key + " ===> " + value));

    });
    t1.start();

    try {
      TimeUnit.MILLISECONDS.sleep(10);
    } catch (InterruptedException e) {
      throw new RuntimeException(e);
    }

    Thread t2 = new Thread(() -> {
      tracker.setLocation("car-1", 5, 5);
      tracker.addVehicle("car-3", 3, 3);
    });
    t2.start();

    t1.join();
    t2.join();
  }
}
```

如果使用最初的可变类MutablePoint而不是不可变的Point类，就会破坏封装性，因为`getLocation`会发布一个指向一个可变状态的引用，而这个引用不是线程安全的。



返回return unmodifiableMap;和return Collections.unmodifiableMap(new HashMap<>(locations));的区别

http://www.itu.dk/people/sestoft/itu/PCPP/E2014/lecture03.pdf

## 独立的状态变量

上面的委托示例都仅仅委托给了单个线程安全的状态变量。我们还可以将线程安全性委托给多个状态变量，只要这些变量是彼此独立的，即组合而成的类并不会在其包含的多个状态变量上增加任何不变性条件。



VisualComponent是一个图形组件，允许客户程序监听鼠标和键盘事件。因为鼠标事件监听器和键盘事件监听器之间不存在任何关联，二者是彼此独立的，所以VisualComponent可以将其线程安全性委托给这两个线程安全的监听器列表。

```java
public class VisualComponent {
  private final List<KeyListener> keyListeners = new CopyOnWriteArrayList<>();
  private final LIst<MouseListener> mouseListeners = new CopyOnWriteArrayList<>();

  public void addKeyListener(KeyListener listener) {
    keyListeners.add(listener);
  }
  public void addKeyListener(KeyListener listener) {
    keyListeners.add(listener);
  }

  public void addMouseListener(MouseListener listener) {
    mouseListeners.add(listener);
  }

  public void removeKeyListener(KeyListener listener) {
    keyListeners.remove(listener);
  }

  public void removeMouseListener(MouseListener listener) {
    mouseListeners.remove(listener);
  }

}
```

用CopyOnWriteArrayList保存监听器列表，它是一个线程安全的链表。

CopyOnWrite：思想就是「写时复制」，对一块内存进行修改时，不直接修改原有内存块的内容，而是创建一个新的副本，然后将指针指向新的副本，原有的副本会被GC。跟MVCC实现可重复读的思想非常类似。

写时是互斥的，读是共享的。

不好的地方：

1. 写操作需要拷贝数组，如果数组比较大，可能导致GC；
2. 无法支撑大量的实时读

适用于：

1. 读多写少，可控的场景

## 当委托失效时

如果状态变量之间存在某种不变性条件的约束，也就是说，整个状态空间的修改需要保持原子性，否则就会有race condition, 就会产生线程不安全。

仅靠委托是不能实现线程安全性的，解决方法

1. 加锁保证原子性；
2. 复合操作打包成一个状态。

## 发布底层的状态变量

当把线程安全性委托给某个对象的底层变量时，在什么条件下才可以发布这些变量从而使其他类可以修改它们？



如果一个状态变量是线程安全的，并且没有任何不变性条件来约束它的值，在变量的操作上也不存在任何不允许的状态转换，那么就可以安全地发布这个变量。

## 示例：发布状态的车辆追踪器

我们来构造一个车辆追踪器的另一个版本，并在这个版本中发布底层的可变状态。相比于之前的不可变Point（，SafePoint是可变的，之前更新坐标是创建一个新的Point类来替换，但我们现在可以直接来修改原有Point的[x, y]坐标。

```java
@ThreadSafe
public class SafePoint {
  @GuardedBy("this") 
  private int x;
  private int y;
  
  private SafePoint(int[] a) { 
    this(a[0], a[1]); 
  } 
  
  public SafePoint(SafePoint p) { 
    this(p.get()); 
  }
  
  public SafePoint(int x, int y) {
    this.x = x;
    this.y = y; 
  }
  
  public synchronized int[] get() { 
    return new int[]{ x, y };
  }
  
  public synchronized void set(int x, int y) {
    this.x = x;
    this.y = y; 
  }
}
```

通过SafePoint，可以构造一个发布其底层可变状态的车辆追踪器，还能确保其线程安全性不被破坏。

```java
@ThreadSafe
public class PublishingVehicleTracker {
    private final Map<String, SafePoint> locations;
    private final Map<String, SafePoint> unmodifiableMap;

    public PublishingVehicleTracker(Map<String, SafePoint> locations) {
        this.locations = new ConcurrentHashMap<>(locations);
        this.unmodifiableMap = Collections.unmodifiableMap(this.locations);
    }

    public Map<String, SafePoint> getLocation() {
        return unmodifiableMap;
    }

    public SafePoint getLocation(String id) {
        return locations.get(id);
    }

    public void setLocation(String id, int x, int y) {
        if(!locations.containsKey(id)) {
                        throw new IllegalArgumentExecption("No such ID: " + id);
        }
        locations.get(id).set(x, y);
    }
}
```

PublishingVehicleTracker将其线程安全性委托给底层的ConcurrentHashMap，只是Map中的元素是线程安全的且可变的Point，而并非不可变的。

# 在现有的线程安全类中添加功能

Java类库包含许多有用的基础模块，我们应该优先选择重用这些现有的模块而不是重新开发，重用能够降低开发量、开发风险以及维护成本。有时候，某个现有的类可以支持大部分的操作，但会有一些定制化的要求，此时就需要在不破坏线程全性的

若没有就添加，

假设需要一个线程安全的链表，它需要提供一个原子的「若没有则添加」的操作。原子意味着线程安全，我们可以通过继承Vector类来实现：

```java
@ThreadSafe
public class BetterVector extends Vector<E> {
    public synchronized boolean putIfAbsent(E x) {
        boolean absent = !contains(x);
        if(absent) {
            add(x);
        }
        return absent;
    }
}
```

它能保证线程安全是因为Vector类也是使用的this加锁。

## 客户端加锁机制

对于Collections.sychronizedList封装的ArrayList，要实现原子的`putIfAbsent`，修改原有的类、继承类都是 行不通的，因为客户代码并不知道Collections.sychronizedList方法中返回的List对象类型(这里不是)。

```java
public static <T> List<T> synchronizedList(List<T> list) {
  return (list instanceof RandomAccess ?
          new SynchronizedRandomAccessList<>(list) :
          new SynchronizedList<>(list));
}
```

ArrayList实现了RandomAccess，而LinkedList没有实现，通过RandomAccess可以快速判断List类型

另一种策略是扩展类的功能，但并不是扩展类本身，而是将扩展代码放到一个辅助类ListHelper中。

```java
@NotThreadSafe
public class ListHelper<E> {
  public List<E> list = Collections.synchronizedList(new ArrayList<>());

  public synchronized boolean putIfAbsent(E x) {
    boolean absent = !contains(x);
    if(absent) {
      add(x);
    }
    return absent;
  }
}
```

但上面加的锁是加在ListHeler的this上的，而Collections.synchronizedList的锁是内部的对象锁，如果putIfAbsent的同时有其他线程通过add方法添加元素，那还是不能保证线程安全，因为它们不是一把锁，不能达到写互斥。

要想线程安全，必须使List在实现客户端或外部加锁时使用同一把锁。客户端加锁是指，使用对象本身用于保护其状态的锁来保护客户代码。

```java
static class SynchronizedCollection<E> implements Collection<E>, Serializable {
  private static final long serialVersionUID = 3053995032091335093L;

  final Collection<E> c;  // Backing Collection
  final Object mutex;     // Object on which to synchronize

  SynchronizedCollection(Collection<E> c) {
    this.c = Objects.requireNonNull(c);
    mutex = this;
  }
}
```

Collections.synchronizedList本身的锁是this，所以正确的代码为：

```java
@ThreadSafe
public class ListHelper<E> {
  public List<E> list = Collections.synchronizedList(new ArrayList<>());

  public  boolean putIfAbsent(E x) {
    synchronized(list) {
      boolean absent = !contains(x);
      if(absent) {
        add(x);
      }
      return absent;
		}
  }
}
```

客户端加锁其实很脆弱，它破坏了同步策略的封装性。谨慎使用。

## 组合

当为现有的类添加一个原子操作时，有一种更好的方法 ：组合（Composition）。

```java
@ThreadSafe
public class ImprovedList<T> implements List<T> {
  private final List<T> list;

  public ImprovedList(List<T> list) {
    this.list = list;
  }

  public synchronized boolean putIfAbsent(T x) {
    boolean absent = !contains(x);
    if(absent) {
      add(x);
    }
    return absent; 
  }

  // other method
  public synchronized void clear() {
    list.clear();
  }
}
```

ImprovedList实现List接口，委托给一个底层的List实例，ImprovedList遵循的Java监视器模式，把对象的所有可变状态都封装起来，并由对象自己的内置锁来保护，无论List是否线程安全，由于ImprovedList都提供了一致性的锁，所以ImprovedList保证了线程安全。

相比于继承类和客户端加锁，这种方式更为健壮。

# 将同步策略文档化

维护文档很重要，线程安全的应当指明出来，从而让使用者可以放心大胆的使用，而不用猜测。但有些文档很多时候也是含糊的，所以需要使用者小心的求证。

# 总结

本章讲的如何通过一些组合模式，使得一个类更容易成为线程安全的类，并且容易维护。



如果我们想设计一个线程安全的类，并不是完全自己写一个，更有成本优势的是通过封装底层提供的类来实现一个新的线程安全的类。比如自己搞一个某种特殊要求的线程安全List，肯定是要利用已有的List类。

设计安全的类需要注意：

1. 哪些是状态变量
2. 不变性条件，其实就是状态变量之间的约束条件
3. 同步策略



**有时某个对象不是线程安全的，但是我们想线程安全的去进行使用**，那么就可以使用实例封闭的技术来保证线程安全。实例封闭的思想就是：把这个线程不安全的对象封装到另一个对象中，然后只对外暴露线程安全的方法，不暴露线程不安全的方法。通过将封闭机制和加锁策略结合起来，可以确保以线程安全的方式来使用线程不安全的对象。Java监视器模式是实例封闭最简单的应用，把对象所有的状态都封装起来，并由自己的内置锁来保护。



上面的例子中，**想要线程安全的使用，还可以将状态委托给线程安全的类**（**可以是不可变对象，或者是线程安全且可变的对象**），其实就是通过一个新类组合线程安全的类，每次调用时将调用转发到底层的线程类，不暴露可能产生线程不安全的方法。



**有时候对象都是线程安全的，但将它们组合起来则不一定是线程安全的。那么复合操作需要保证原子性。**



当需要定制化的功能时，成本最低的重用现有的线程安全类，**扩展某个新的线程安全功能**。扩展分为：

1. 修改现有线程安全类：基本不可能
2. 继承现有线程安全类，然后实现新的功能：代码非常脆弱，原有类的同步策略的封装性被破坏
3. 客户端加锁机制：使用新的辅助类，然后利用Collections.synchronizedList类将线程不安全的类转为线程安全的类，然后加锁新功能，实现线程安全。需要注意的是，客户端加锁需要与Collections.synchronizedList的同步的锁必须保持一致，防止已有的功能破坏新功能。
4. 组合：使用新的类继承接口，然后通过自身的内置锁使用一致的加锁机制，并且避免暴露对外的不安全引用，即使继承接口的其他实现不是线程安全的，也能保证自身是线程安全的。
