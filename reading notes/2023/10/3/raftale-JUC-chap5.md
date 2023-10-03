本章将介绍一些最有用的并发构建模块，以及在使用这些模块来构造并发程序时的一些常用模式。

# 同步容器类

同步容器类包括Vector和HashTable，还有一些同步的封装器类是由Collections.synchronizedXXX等工厂方法创建的。这些类实现线程安全的方式是：将它们的状态封装起来，并对每个公有方法都进行同步，使得每次只有一个线程能访问容器的状态。

## 同步容器类的问题



同步容器类是线程安全的，但如果在同步容器类上有复合操作时，复合操作不能保证原子性，多个线程并发修改容器时，就会出现线程不安全问题。复合操作一般包括：

1. 迭代：反复访问元素，直到遍历完容器中所有元素；
2. 跳转：根据执行顺序找到当前元素的下一个元素；
3. 条件运算：例如「若没有则添加」、「先检查再运行」

我们看一个条件运算「先检查再执行」的例子，线程A执行getLast，先检查List长度，再获取最后一个元素，但如果此时线程B修改了最后一个元素，那线程A获得的元素就是原来的倒数第二个元素，这个就明显破坏了getLast的原子性。甚至当只剩下一个元素时，getLast可能会抛出ArrayIndexOutOfBoundsException异常。

```java
package net.jcip.examples;

import java.util.*;

/**
 * UnsafeVectorHelpers
 * <p/>
 * Compound actions on a Vector that may produce confusing results
 *
 * @author Brian Goetz and Tim Peierls
 */
public class UnsafeVectorHelpers {
  public static Object getLast(Vector list) {
    int lastIndex = list.size() - 1;
    return list.get(lastIndex);
  }

  public static void deleteLast(Vector list) {
    int lastIndex = list.size() - 1;
    list.remove(lastIndex);
  }
}
```


可以通过加锁来要保证原子性，使getLast和deleteLast是互斥的，同一时刻只能串行执行。同步容器类支持客户端加锁的，即客户端的锁必须与Vector保持一致（第四章讨论过，如果锁不保持一致，那vector对象还是可能在其他地方修改容器），那么新加的操作就与容器的其他操作一样都是原子操作，同步容器类通过自身的锁来保护它的每个方法，Vector的锁就是this对象锁，所以客户端加锁如下：

```java
package net.jcip.examples;

import java.util.*;

/**
 * SafeVectorHelpers
 * <p/>
 * Compound actions on Vector using client-side locking
 *
 * @author Brian Goetz and Tim Peierls
 */
public class SafeVectorHelpers {
  public static Object getLast(Vector list) {
    synchronized (list) {
      int lastIndex = list.size() - 1;
      return list.get(lastIndex);
    }
  }

  public static void deleteLast(Vector list) {
    synchronized (list) {
      int lastIndex = list.size() - 1;
      list.remove(lastIndex);
    }
  }
}
```

## 迭代器与ConcurrentModificationException

迭代器本身也是一种复合操作，迭代时，当前线程或者其他线程可能改变容器中的元素而使迭代器属性发生变化。

设计同步容器类的迭代器时，并没有考虑到并发修改的问题，如果容器在迭代过程中被修改，就会抛出`ConcurrentModificationException`异常。这种迭代器的「及时失败」只能作为并发问题的预警指示器，这是一种设计上的权衡，降低了并发修改操作的检测代码对程序性能的影响。

## 隐藏迭代器

虽然加锁可以防止迭代器抛出ConcurrentModificationException异常，但你必须要记住在所有对共享容器进行迭代的地方都需要加锁，但实际上在某些情况下，迭代器会隐藏起来。

标准输出容器会隐式的对容器进行迭代打印、hashCode和equals也会间接执行迭代，还有containsAll/removeAll/retainAll，以及把容器作为参数的构造函数 都会间接的进行迭代，所有间接的迭代操作都可能抛出ConcurrentModificationException

# 并发容器

同步容器将所有对容器状态的访问都串行化，以实现线程安全，但代价就是严重降低并发性，当多个线程竞争容器的锁时，吞吐量将严重降低。

并发容器被设计出来替代同步容器，并发容器专门针对多个线程并发访问而设计的。如`ConcurrentHashMap`被用来替代同步且基于散列的Map，CopyOnWriteArayList被用来替代读多写少的同步List。



## ConcurrentHashMap

同步容器类在执行每个操作期间都持有一个锁。

ConcurrentHashMap使用了一种完全不同的加锁策略来提高更高的并发性和伸缩性，它并不是将每个方法都在同一个锁上同步并使得每次只能有一个线程访问容器，而是使用一种粒度更细的**分段锁(Lock Striping）**来实现更大程度的共享。

ConcurrentHashMap的size和isEmpty的语义并不是准确的，它只是一个估计值，因为size在并发的场景下随时可能过期，所以它在并发的场景下用处非常小，这些操作需求被弱化来换取更重要操作的性能优化，如get/put/containsKey/remove等。

## 额外的原子Map操作

由于ConcurrentHashMap不能被加锁来执行独占访问，因此我们无法使用客户端加锁来创建新的原子操作。但一些常见的原子操作已经集成在ConcurrentHashMap内部了，如：

1. 若没有则添加：putIfAbsent
2. 若相等则移除：remove
3. 若相等则替换：replace



##  CopyOnWriteArrayList

CopyOnWriteArrayList用于替代同步的List。

「写入时复制（Copy-On-Write）」容器的线程安全性在于，只要正确发布一个事实不可变的对象，那么在访问该对象时就不再需要进一步的同步。

事实不可变在第三章讲过：如果对象在技术上是可变的，但是其状态在发布后程序就不会再对状态做改变，这种对象被称为「事实不可变对象」。事实不可变对象的好处是免去了同步加锁，提高了性能

CopyOnWriteArrayList在每次修改时，都会创建并重新发布一个新的容器副本，从而实现可变性。迭代器保留一个指向底层基础数组的引用，这个数组不会被修改，因此只需要保证数组内容的可见性（volatile）。

```java
public class CopyOnWriteArrayList<E>
implements List<E>, RandomAccess, Cloneable, java.io.Serializable {
  private static final long serialVersionUID = 8673264195747942595L;

  /**
     * The lock protecting all mutators.  (We have a mild preference
     * for builtin monitors over ReentrantLock when either will do.)
     */
  final transient Object lock = new Object();

  /** The array, accessed only via getArray/setArray. */
  private transient volatile Object[] array;

  final Object[] getArray() {
    return array;
  }

  final void setArray(Object[] a) {
    array = a;
  }

  public E get(int index) {
    return elementAt(getArray(), index);
  }

  static <E> E elementAt(Object[] a, int index) {
    return (E) a[index];
  }
  public boolean add(E e) {
    synchronized (lock) {
      Object[] es = getArray();
      int len = es.length;
      es = Arrays.copyOf(es, len + 1);
      es[len] = e;
      setArray(es);
      return true;
    }
  }

  public Object[] toArray() {
    return getArray().clone();
  }

}
```

显然，每次修改容器时都会复制底层数组，这在数组规模较大时开销较大。所以它的适用场景是：仅当迭代操作远远多于修改操作时。

这个场景其实是很多事件通知系统的场景：分发通知时需要迭代已注册监听器链表，并调用每一个监听器，而监听器的注册和注销操作远远小于接受事件通知。

# 阻塞队列和生产者 - 消费者模式

阻塞队列支持「生产者-消费者」设计模式。基于阻塞队列构建的「生产者-消费者」设计中：生产者生产数据，并将数据放入队列，当消费者准备处理数据时，将从队列中获取数据。

BlockingQueue简化了「生产者-消费者」设计的实现过程，它支持任意数量的生产者和消费者。一种最常见的「生产者-消费者」模式就是线程池与工作队列的组合。



阻塞队列简化了消费者程序的编码，因为take操作会一直阻塞直到有可用的数据。

阻塞队列也简化了生产者程序的编码，如果生产者生产数据的速率比消费者消费速率快的多，不加限制则会耗尽内存，如果使用有界队列，队列充满时，put方法的阻塞特性将阻塞生产者的生产工作。

阻塞队列还提供了一个offer方法，如果数据项不能被添加到队列中，将返回一个失败状态，应用程序则可以根据这个状态来创建更多灵活的策略处理负荷过载的情况，例如减轻负载，将多余的工作项序列化并写入磁盘，减少生产者线程的数量，或者通过某种方式来抑制生产者线程。

BlockingQueue是一个接口，它有许多实现：

1. LinkedBlockingQueue和ArrayBlockingQueue 是FIFO队列；
2. PriorityBlockingQueue优先队列；
3. SynchronousQueue：它不是一个真正的队列，因为它的队列长度为0，相当于生产者对消费者的直接交付。所以它的take和put会阻塞到交付为止。它的适用场景是消费者足够多，能够立刻的消费生产者的生产数据。



## 示例：桌面搜索

有一种类型的程序适合被分解为生产者和消费者，例如代理程序，它将扫描本地驱动器上的文件并建立索引以便随后进行搜索，类似于某些桌面搜索程序或者windows索引服务。

```java
package net.jcip.examples;

import java.io.File;
import java.io.FileFilter;
import java.util.concurrent.*;

/**
 * ProducerConsumer
 * <p/>
 * Producer and consumer tasks in a desktop search application
 *
 * @author Brian Goetz and Tim Peierls
 */
public class ProducerConsumer {

  static class FileCrawler implements Runnable {
    private final BlockingQueue<File> fileQueue;
    private final FileFilter fileFilter;
    private final File root;

    public FileCrawler(BlockingQueue<File> fileQueue,
                       final FileFilter fileFilter,
                       File root) {
      this.fileQueue = fileQueue;
      this.root = root;
      this.fileFilter = new FileFilter() {
        public boolean accept(File f) {
          return f.isDirectory() || fileFilter.accept(f);
        }
      };
    }

    private boolean alreadyIndexed(File f) {
      return false;
    }

    public void run() {
      try {
        crawl(root);
      } catch (InterruptedException e) {
        Thread.currentThread().interrupt();
      }
    }

    private void crawl(File root) throws InterruptedException {
      File[] entries = root.listFiles(fileFilter);
      if (entries != null) {
        for (File entry : entries)
        if (entry.isDirectory())
          crawl(entry);
        else if (!alreadyIndexed(entry))
          fileQueue.put(entry);
      }
    }
  }



  static class Indexer implements Runnable {
    private final BlockingQueue<File> queue;

    public Indexer(BlockingQueue<File> queue) {
      this.queue = queue;
    }

    public void run() {
      try {
        while (true) {
          indexFile(queue.take());
        }

      } catch (InterruptedException e) {
        Thread.currentThread().interrupt();
      }
    }

    public void indexFile(File file) {
      // Index the file...
    };
  }


  private static final int BOUND = 10;
  private static final int N_CONSUMERS = Runtime.getRuntime().availableProcessors();

  public static void startIndexing(File[] roots) {
    BlockingQueue<File> queue = new LinkedBlockingQueue<File>(BOUND);
    FileFilter filter = new FileFilter() {
      public boolean accept(File file) {
        return true;
      }
    };

    for (File root : roots) {
      new Thread(new FileCrawler(queue, filter, root)).start();
    }


    for (int i = 0; i < N_CONSUMERS; i++) {
      new Thread(new Indexer(queue)).start();
    }
  }
}
```

生产者-消费者模式

1. 将一个问题拆解后变成两个更简单的组件，增强了代码可读性和可重用性。
2. 能带来性能优势，生产者和消费者可以并行地执行。

## 串行线程封闭

在JUC中实现的各种阻塞队列都包含了足够的内部同步机制，从而安全地将对象从生产者线程发布到消费者线程。

对于可变对象，生产者-消费者这种设计与阻塞队列一起，促进了**串行线程封闭**，从而将对象所有权从生产者交付给消费者。**线程封闭对象只能由单个线程持有**，但可以通过安全地发布该对象转移所有权。在转移所有权后，也只有另一个线程能获得这个对象的访问权限，并且发布对象的线程不会再访问它。这种安全的发布确保了对于新的所有者来说是可见的，并且由于最初的所有者不会再访问它，因此**对象将被封闭在新的线程中**。

对象池利用了串行线程封闭，将对象借给一个请求线程，只要对象池包含足够的内部同步来安全地发布池中的对象，并且只要客户代码本身不会发布池中的对象，或者在将对象返回给对象池后就不再使用它，就可以安全地在线程之间传递所有权。

## 双端队列与工作密取

Deque是双端队列接口，BlockingDeque是阻塞双端队列接口；

阻塞队列适用于生产者-消费者模式，双端队列适用于工作密取模式。在生产者-消费者模式中，所有消费者有一个共享的工作队列，而在工作密取设计中，每个消费者都有各自的双端队列。如果一个消费者完成了自己双端队列中的全部工作，那么它可以从其他消费者双端队列末尾秘密地获取工作。

密取工作模式比传统的生产者-消费者模式具有更高的可伸缩性，这是因为工作者线程不会在单个共享的任务队列上发生竞争，当工作线程需要访问另一个队列时，它会从队列的尾部获取工作，进一步降低了队列上的竞争程度。

工作密取模式适用于既是消费者也是生产者问题，当消费者执行某个工作时可能导致出现更多的工作。例如，爬虫程序处理一个页面时，会发现更多的页面需要处理，那它就把这些待处理的任务放到队列的末尾中。其他线程队列为空时，也会在该队列尾部继续找新的任务，从而确保每个线程都保持忙碌状态。
