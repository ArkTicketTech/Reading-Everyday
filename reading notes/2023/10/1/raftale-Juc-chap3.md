要编写正确的并发程序，关键问题在于：在访问共享的可变状态时需要进行正确的管理。

通过同步可以避免多个线程在多一时刻访问相同的数据，本章将介绍如何共享和发布对象，从而使它们能够安全地由多个线程同时访问。



synchronzied不仅能实现原子性或者确定临界区（Critical Section），同步还有另一个重要的方面：内存可见性（Memory Visibility）。我们不仅希望防止某个线程正在使用对象状态而另一个线程在同时修改该状态，而且希望确保当一个线程修改了对象状态后，其他线程能够看到发生的状态变化。

# 可见性

```java
public class NoVisibility {
	private static boolean ready;
    private static int number;
    private static class ReadThread extends Thread {
        public void run() {
            while(!ready) {
                Thread.yield();
            }
            System.out.println(number);
        }
    }
    public static void main(String[] args) {
        new ReadThread().start();
        number = 42;
        ready = true;
    }
	
}
```

`Thread.yield()`通知线程调度器运行具有更高/相等优先权的任务。

让掉当前线程CPU的时间片，使正在运行中的线程重新变成就绪状态，并重新竞争CPU的调度权，它可能会被当前线程继续获取到，也可能被其他线程获取到。



1. ReaderThread可能永远看不到ready的值
2. ReaderThread可能先看到写入ready的值，但是却没有看到写入number后的值：重排序

## 失效数据

NoVisibility展示了在缺乏同步的程序中可能产生错误结果的一种情况：失效数据。

当ReaderThread查看ready变量时，可能会得到一个已经失效的值，失效值可能不会同时出现，一个线程看到了某个变量的最新值，看到另一个变量的失效值。

失效的数据可能产生很危险的结果，输出错误的值、使程序无法运行、不精确的计算、无限循环等。



下面是一个线程安全的类，注意仅对set同步是不够的，调用get的线程仍然可能看见失效值。

```java
@ThreadSafe
public class SynchronizedInteger {

    private int value;
    public synchronized int get() {
        return value;
    }
    public synchronized void set(int value) {
        this.value = value;
    }
}
```



## 非原子的64位操作

Java内存模型保证，变量的读取操作和写入操作都必须是原子操作，但是对于volatile类型的long和double变量，JVM允许将64位的读操作或写操作分解为两个32位的操作。在多线程程序中使用共享且可变的long和double等类型的变量是不安全的，除非用volatile声明或者用锁保护起来。

## 加锁与可见性

加锁不仅保证了互斥性（互斥保证了原子性），还保证了内存可见性。为了确保所有线程都能看到共享变量的最新值，所有执行读操作或者写操作的线程都必须在同一个锁上进行同步。

## Volatile变量

Java中的Volatile变量用来确保将变量的更新操作通知到其他线程，编译器与运行时都会注意到这个变量是共享的，因此不会将该变量的操作与其他内存操作一起重排序。volatile变量不会被缓存在寄存器或者对其他处理器不可见的地方，因此读取volatile类型的变量时总会返回最新写入的值。



volatile变量的一种典型用法：检查某个状态标记以判断是否退出循环。如下数绵羊的例子，`asleep`必须为volatile变量，否则，当`asleep`被其他线程修改时，执行判断的线程不能

```java
volatile boolean asleep;
... 
	while(!asleep) {
        countSomeSleep();
    }
```

当且仅当满足以下所有条件时，才应该使用volatile变量：

1. 对变量的写入操作不依赖于当前变量的当前值，或者你能确保只有单个线程更新变量的值；
2. 该变量不会与其他状态变量一起纳入不变性条件中
3. 在访问变量时不需要加锁

意思就是volatile变量不能确保原子性（线程安全），只能确保可见性。

# 发布与逸出

发布对象（Publish）：使对象能够在当前作用域之外的代码中使用，比如：

1. 将一个指向该对象的引用保存到其他代码可以访问的地方
2. 在某个非私有的方法中返回该引用；
3. 将引用传递到其他类的方法中

如果在对象构造完成之前就发布该对象，就会破坏线程安全性。

对象逸出（Escape）：当某个不应该发布的对象被发布时，比如对象还没有构造完成就发布。



https://www.cnblogs.com/hapjin/p/10505337.html

不安全发布对象的例子：

```java
@Slf4j
@NotThreadSafe
public class UnsafePublish {

    private String[] states = {"a", "b", "c"};

    public String[] getStates(){
        return states;
    }

    public static void main(String[] args) {
        UnsafePublish unsafePublish = new UnsafePublish();
        log.info("{}", Arrays.toString(unsafePublish.getStates()));

        unsafePublish.getStates()[0] = "d";
        log.info("{}", Arrays.toString(unsafePublish.getStates()));
    }
}
```

`getStates()`使得在类的任何外部线程都可以访问这些域，其实我们是无法保证其他线程会不会修改这个域的。当多个线程使用这个值时，就不可控了。

## 安全的对象构造过程

看一个this 引用逸出的例子：

```java
public class ThisEscape {
  public ThisEscape(EventSource source) {
    source.registerListener(
      new EventListener() {
        public void onEvent(Event e) {
          dosomething(e);
        }
      }

      // 此时this已经逸出，但构造还没有完成
      anotherthing();
    )
  }
  void dosomething(Event e) {
    
  }
  void anotherthing() {
    
  }
  interface EventSource {
    void registerListner(EventListener e);
  }
  interface EventListener {
    void onEvent(Event e);
  }
  interface Event {
    
  }
}
```

实例化ThisEscape对象时，会调用source的registerListener方法，new EventListener()构造完毕后EventListener实例就已经有了ThisEscape的this引用，因为它可以调用onEvent方法间接调用dosomething了。但此时ThisEscape的构造函数还没有完成，所以就产生了一个不安全的发布。

在构造函数中启动一个线程也是this引用逸出的一种方式，当对象在构造函数中创建一个线程时，无论是显式创建创建还是隐式创建，this引用都会被新创建的线程共享。在对象还未完全构造之前，新的线程就可以看见它。

在构造函数中创建线程并没有错误，但最好不要立即启动它，而是通过一个start或者Initalize方法来启动。

上面正确的发布姿势是使用工厂类：

```java
public class SafeListener {
  private final EventListener listener;

  private SafeListener() {
    listener = new EventListener() {
      public void onEvent(Event e) {
        doSomething(e);
      }
    };
  }

  public static SafeListener newInstance(EventSource source) {
    SafeListener safe = new SafeListener();
    source.registerListener(safe.listener);
    return safe;
  }

  void doSomething(Event e) {
  }


  interface EventSource {
    void registerListener(EventListener e);
  }

  interface EventListener {
    void onEvent(Event e);
  }

  interface Event {
  }
}
```

# 线程封闭

当访问共享的可变数据时，通常需要使用同步。**一种避免同步的方式就是不共享数据，如果仅在单线程内访问数据，就不需要同步。这种技术被称为线程封闭**（Thread Confinement）。

线程封闭技术一种常见的应用是JDBC的connection对象，由于大多数请求都是由单个线程采用同步的方式来处理，并且在Connection对象返回之前，连接池不会再将它分配给其他线程，因此，这种连接管理模式在处理请求时隐含地将Connection对象封闭在线程中。

Java程序及其核心库提供了一些机制来帮助维持**线程封闭性**，例如局部变量和ThreadLocal类。

## ad-hoc线程封闭

ad-hoc线程封闭指的是，维护线程封闭性的职责完全由程序实现来承担，它非常脆弱，不推荐使用。

## 栈封闭

栈封闭中，只能通过局部变量访问对象。因为局部变量会拷贝到每个线程的线程栈中，只有当前线程才能访问到。

## ThreadLocal类

维护线程封闭性的一种更规范的方法是使用ThreadLocal，这个类能使线程中的某个值与保存的对象关联起来。ThreadLocal的get和set等方法为每个使用该变量的线程都存有一份独立的副本，因此get总是返回由**当前线程**在调用set时设置的最新值。



JDBC ：

1. 加载数据库驱动；

1. 1. Class.forName(driver);

1. 获取数据库连接:

1. 1. Connection connection = DriverManager.getConnection(url, user , pass);

1. 预编译Statement对象

1. 1. Statement stmt = connection.prepareStatement(sql);

1. 执行SQL：stmt.execute()
2. 操作结果集



在单线程应用程序中可能会维护一个全局的数据库连接，并在程序启动时初始化这个连接对象，从而避免在调用每个方法时都要传递一个Connection对象。由于JDBC的规范并没有保证连接是线程安全的，因此，当多线程应用直接使用全局数据库连接变量时，就不是线程安全的。但如果通过将JDBC的连接保存到ThreadLocal对象中，每个线程都会拥有属于自己的连接，从而避免了线程不安全的问题。

```java
private static ThreadLocal<Connection> connectionHolder  = new ThreadLocal<>() {
    public Connection initialValue() {
        return DriverManager.getConnection(DB_URL);
    }
}

public static Connection getConnection() {
    return connectionHolder.get();
}
```



# 不变性

满足同步需求的另一种方法是 使用不可变对象（Immutable Object)。如果对象的状态不会改变，那么多线程访问遇到的失效数据、丢失更新操作、或者观察状态不一致等问题就不会存在了。

如果某个对象在被创建后其状态就不能被修改，那么这个对象就称为不可变对象。线程安全性是不可变对象的固有属性之一，它们的不变性条件是由构造函数创建的，不可变对象只有一种状态，并且该状态由构造函数控制。

不可变对象不一定是线程安全的。



当满足以下条件时，对象才是不可变的：

1. 对象创建之后其状态就不能修改；
2. 对象的所有字段都是final类型；
3. 对象是正确创建的，创建时没有this引用逸出



在不可变对象的内部仍然可以使用可变对象来管理它们的状态：

```java
@Immutable
public final class ThreeStooges {
    private final Set<String> stooges = new HashSet<>();
    // 对象构造完成后无法对stooges进行修改
    public ThreeStooges() {
        stooges.add("Moe");
        stooges.add("Larry");
        stooges.add("Curly");
    }
    public boolean isStooge(String name) {
        return stooges.contains(name);
    }
}
```

## Final字段

final修饰的字段是不能修改的，但如果final字段引用的对象是可变的，那这个被引用的对象是可以修改的。

同时final字段能确保初始化过程的安全性，从而可以不受限制的访问不可变对象。



final 字段如何保证初始化的安全性，编译器和处理器在遇到final时遵循两个重排序规则：

1. 在构造函数内对一个final 字段的写入，与随后把这个被构造对象的引用赋值给一个引用变量，这两个操作之间不能重排序
2. 初次读一个包含final field的对象的引用，与随后初次读这个final field，这两个操作之间不能重排序。



也就是说：final field的赋值happens before 所有线程的开始。

## 使用votatile类型来发布不可变对象

没看懂

# 安全发布

## 不正确的发布：正确的对象被破坏



```java
public class Holder {
    private int n;
    public Holder(int n) {
        this.n = n;
    }
    public void assertSanity() {
        if(n != n) {
            throw new AssertionError("This statement is false.");
        }
    }
}
```



上面的holder类可能会被不正确的发布：

```java
// 不安全的发布
public Holder holder;

public void initialize() {
    holder = new Holder(42);
}
```



假设线程A在调用`someStaticVariable = initialize()`；

线程B在调用`someStaticVariable = holder.assertSanity()`；

什么情况下有可能`n 如何会!= n`呢？



当线程A执行`someStaticVariable = initialize()`时，预期的顺序是这样：

1. Alloc Memory to pointer1
2. Write pointer1 to someStaticVariable
3. Write 42 to pointer1 at offset 0

但JVM的重排序机制可能导致线程A的执行顺序为：

1. Alloc Memory to pointer1
2. Write pointer1 to someStaticVariable
3. Write 42 to pointer1 at offset 0

What this means though is that thread 2 can now call into assertSanity before n has gotten the value 42. It is possible for the value n to be read twice during assertSanity, once before operation #3 completes and once after and hence see two different values and throw an exception.

线程B看见了上述的执行过程，那对线程B来说，很有可能的是，它进入`assertSanity()`方法后，首先拿到的第一个`n = 0`，此时线程A还没有赋值`n = 42`，等到线程A赋值完毕，线程B读取到了第二个`n`为42，所以就有`n != n`这样的怪事发生。

https://stackoverflow.com/questions/1621435/not-thread-safe-object-publishing

## 不可变对象与初始化安全性

幸运的是`final`可以阻止这样的事情发生，之前说到`final`给了非常强的初始化安全性保证，也就是说

如果n是final类型的，线程A保证了对n的完全初始化后，线程B才能拿到n的值。

final field的赋值happens before 所有线程的开始。

除了同步，发布不可变对象的引用时即使没有同步，也可以安全地访问该对象，如果Holder对象是不可变的，即使Holder没有被正确的发布，`assertSanity()`也不会抛出该异常。

所以可以推导出一个结论：

任何线程都可以在不需要额外同步的情况下安全地访问不可变对象，即使在发布这些对象时没有使用同步。



这种保证还可以延伸到被正确创建对象的所有final类型的字段，但如果final字段指向了可变对象，那么在访问这些字段所指向的对象的状态时仍然需要同步，因为final只保护了引用，不保护指向的对象。

## 安全发布的常用模式

要安全的发布一个对象，对象的引用和状态必须同时对其他线程可见，一个正确构造的对象可以通过以下方式来进行安全的发布：

1. 在静态初始化函数中初始化一个对象引用；
2. 将对象的引用保存到volatile类型的field或者AtomicReferance对象中；
3. 将对象的引用保存到某个正确构造对象的final类型field中。
4. 将对象的引用保存到一个由锁保护的field中：线程安全容器

第4点就是指的线程安全容器。

在线程安全容器内部的同步意味着：如果线程A将对象X放入一个线程安全的容器，随后线程B读取这个对象，那么可以确保B看到A设置的X状态。

线程安全容器：HashTable, synchronizedMap, ConcurrentMap, Vector, CopyOnWriteArrayList, CopyOnWriteArraySet, synchronizedList, synchronizedSet, BlockingQueue, ConcurrentBlockingQueue。

线程安全数据传递机制：Future, Exchanger

通常，要发布一个静态构造的对象，最简单和最安全的方式是使用静态的初始化器：

```
public static Holder holder = new Holder(42);
```

静态初始化由JVM在类的初始化阶段执行，由于在JVM内部存在着同步机制，通过这种方式初始化的任何对象都可以被安全的发布。

静态初始化先于构造函数执行，且只会执行一次。

## 事实不可变对象

如果对象在技术上是可变的，但是其状态在发布后程序就不会再对状态做改变，这种对象被称为「事实不可变对象」。

事实不可变对象的好处是免去了同步加锁，提高了性能。

## 可变对象

对象的发布需求取决于它的可变性：

1. 不可变对象可以通过任意机制来发布
2. 事实不可变对象必须通过安全机制来发布
3. 可变对象必须通过安全方式来发布，并且必须是线程安全的或者由某个锁保护起来

## 安全的共享对象

当获得一个对象的引用时，你需要知道在这个引用上可以执行哪些操作。在使用它之前，是否需要获得一个锁？是否可以修改它的状态，或者只能读取它？许多错误就是没有理解共享对象的「既定规则」而导致的。当发布一个对象时，必须明确的说明对象的访问方式。

# 总结





本章主要介绍的是如何共享和发布对象，从而使它们能够安全的由多个线程访问。



同步不仅保证了原子性，隐藏着也保证了内存可见性，我们不仅希望防止某个线程正在使用对象状态而另一个线程在同时修改该状态，而且希望确保当一个线程修改了对象状态后，其他线程能够看到发生的状态变化。

为什么需要内存可见性，是因为底层（JVM）为提高性能的重排序机制。当没有同步时，不注意的话，可见性会导致：失效的数据、非原子的64位操作。

尽管同步加锁可以保证可见性，但性能开销太大，保护可见性还有一种选择，就是底层基于内存屏障的volatile变量，volatile不保证原子性，只保证可见性。



发布对象（Publish）是指使对象能够在当前作用域之外的代码中使用，当某个不应该发布的对象被发布时，比如对象还没有构造完成就发布，类似的不安全的发布就是对象逸出（Escape）。

对象逸出在单线程或者多线程都可能导致问题发生，对象还没有构造完成就发布的一个解决方案是：使用工厂类等待对象构造完成后再做其他操作。



访问共享的可变数据时，最简单的方法是同步保证线程安全，因为同步开销大，所以另一个方式就是避免同步。

避免同步的方式是仅在单线程内访问数据，就不需要同步，这种技术被称为**线程封闭**。

线程封闭技术有：

1. 程序自己保证封闭；
2. 使用栈空间，栈空间变量线程独有；
3. ThreadLocal类：每个线程都有一份独立的副本



另外一种保证线程安全的机制是**不可变对象**，如果某个对象在创建后其状态就不能被修改，那么这个对象就是不可变对象，不可变对象一定是线程安全的。final关键字能实现不可变对象，final天然保证了初始化过程的安全性，所以可以任意访问。



不安全的发布会导致正确的对象被破坏，安全发布的常用模式有：静态初始化、不可变对象、volatile、线程安全容器；



故在并发程序中使用和共享对象时，比较实用的策略总结为：

1. 同步加锁：保证可见性和原子性
2. volatile关键字：保证可见性
3. 线程封闭技术：避免共享数据
4. 不可变对象：只读共享，不共享写
5. 使用线程安全容器：由底层保护线程安全，底层其实还是同步加锁或者其他一些技术
