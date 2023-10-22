JUC包的许多类中，例如Semaphore和ConcurrentLinkedQueue，都提供了比synchronized机制更高的性能和可伸缩性。本章将介绍这种性能提升的主要来源：原子变量和非阻塞的同步机制。

非阻塞算法用底层的原子机器指令（CAS）代替锁来确保数据在并发访问中的一致性。非阻塞算法被广泛地用于在操作系统和JVM中实现线程/进程调度机制、垃圾回收机制以及锁和其他并发数据结构。

非阻塞方法在可伸缩性和活跃性有着巨大的优点：

1. 能够在粒度更细的层次上进行协调，能极大的减少调度开销；
2. 不存在死锁和活跃性问题；

# 锁的劣势

独占锁在并发场景始终面临着一系列劣势：

1. 独占锁，竞争失败的线程会被挂起并等待恢复（现代智能JVM不一定会挂起，可能会选择自旋等待），竞争激烈的情况下这种上下文切换开销非常大。
2. volatile具有可见性，但不支持原子性操作；
3. 自增操作不是原子性的，加锁可以保证原子操作；
4. 线程在等待锁的过程中不能做其他事情；
5. 优先级反转：当一个线程在等待锁的时候，它不能做任何事情。**如果一个线程在持有锁的情况下被延迟执行，那么所有需要这个锁的线程都无法执行下去。如果被阻塞线程的优先级较高，而持有锁的线程优先级较低**，这个严重的问题就被称为优先级反转。

1. 1. 即使高优先级的线程可以抢占执行，但仍然需要等待锁被释放，从而导致它的优先级降至低优先级的级别，如果持有锁的线程被永久地阻塞，那么所有等待这个锁的线程就无法执行下去。

锁始终是开销较大的操作，幸好的是现代处理器提供了一种粒度更细的技术，类似于volatile变量的机制，同时还要支持原子的更新操作。

# 硬件对并发的支持

独占锁是一种悲观的技术，对于细粒度的操作，还有一种更高效的方法：乐观锁。借助冲突检测机制来判断在更新过程中是否存在来自其他线程的干扰，如果存在，这个操作将失败，并且可以重试，当然也可以不重试。

在针对多处理器操作而设计的处理器提供了一些特殊指令，用于管理对共享数据的并发访问，几乎所有的现代处理器都包含了某种形式的原子读-改-写指令，例如compare-and-swap或者load-linked/storeConditional，操作系统和JVM就是使用这些指令来实现锁和并发的数据结构。

## 比较与交换

在大多数处理器架构中采用的方法是实现一个比较并交换（CAS）指令。CAS包含了三个操作数，需要读写的内存位置V、进行比较的值A，和预期写入的新值B，当且仅当V的值等于A时，CAS才会通过原子方式用新值来更新V的值，否则不会执行任何操作。该语义的模拟代码如下：

```java
public class SimulatedCAS {
    private int value;
    public synchronized int get() {
        return value;
    }
    public synchronized int compareAndSwap(int expectedValue, int newValue) {
        int oldValue = value;
        if(oldValue = expectedValue) {
            value = newValue;
        }
    }
    public synchronized boolean compareAndSet(int expectedValue, int newValue) {
        return expectedValue == compareAndSet(expectedValue, newValue);
    }
}
```

当多个线程尝试使用CAS同时更新同一个变量时，只有其中一个线程能更新变量的值，其他的线程将失败。然而，失败的线程并不会被挂起， 而只是失败返回，可以再次尝试、或者执行一些恢复操作、或者什么也不执行（因为它已经知道值被修改了）。

## 非阻塞的计数值

我们使用SimulatedCAS实现一个线程安全的计数器。

```java
public class CasCounter {
    private simulateCAS value;

    public int getValue() {
        return value.get();
    }
    public int increment() {
        int v;
        do {
            v = value.get();
        } while(v != value.compareAndSwap(v, v + 1));
        return v + 1;
    }
    
}
```

CasCounter不会阻塞，但如果其他线程同时更新计数器，那么会多次执行重试操作。通常，反复的重试是一种合理的策略，但在一些竞争很激烈的情况下，更好的方式是在重试之前首先等待一段时间或者回退，从而避免造成活锁问题。



加锁会导致操作系统级的锁定、线程挂起以及上下文切换等事件，但CAS则不会导致这些，所以CAS的性能要比加锁好一点。CAS的主要缺点是，它将使调用者处理竞争问题（通过重试、回退、放弃），而在锁中能自动处理竞争问题（线程在获得锁之前将一直阻塞）。

## JVM对CAS的支持

Java在int、long和对象的引用等类型上都公开了CAS操作，并且JVM把它们编译为底层硬件提供的最有效方法。在支持CAS的平台上，运行时把它们编译为相应的机器指令，在最坏的情况下，如果不支持CAS指令，那么JVM将使用自旋锁。在原子变量类中使用了这些底层的JVM支持，为数字类型和引用类型提供了一种高效的CAS操作，而JUC的大多数类在实现时或直接或间接的使用了这些原子变量类。

# 原子变量类

原子变量比锁的粒度更细，量级更轻，并且对于在多处理器系统上实现高性能的并发代码来说是关键的。原子变量将竞争的范围缩小到单个变量上，这是可以获得的粒度最细的情况。

1. 非竞争时，原子变量的更新不会比加锁慢，并且通常会更快；
2. 竞争时，线程执行更不易出现延迟，通常更容易成功。



共有12个原子变量类：

1. 标量类：

1. 1. AtomicInteger,
2. AtomicLong,
3. AtomicBoolean,
4. AtomicReference

1. 更新器类：

1. 1. AtomicIntegerFieldUpdater: 原子更新整型的字段的更新器。
2. AtomicLongFieldUpdater: 原子更新长整型字段的更新器。
3. AtomicReferenceFieldUpdater: 上面已经说过此处不在赘述。

1. 数组类

1. 1. AtomicIntegerArray: 原子更新整型数组里的元素。
2. AtomicLongArray: 原子更新长整型数组里的元素。
3. AtomicReferenceArray: 原子更新引用类型数组里的元素

1. 复合变量类：

1. 1.



## 原子变量是一种「更好的volatile」

原子变量自身是线程安全的，但组合起来原子变量可能之间有某种不变性条件，复合操作如果不是原子性的，那么「先检查后执行」操作就会有线程安全问题。在第3章中，对这种更新多个相关变量出现的竞态条件，通过将这些变量全部保存到一个volatile的不可变对象来实现线程安全。第4章中的NumberRange也面临着多个条件下的线程安全问题。

我们可以将这类问题通过一个不可变的原子引用来解决，AtomicReference就提供了这样的原子引用，AtomicReference的compareAndSet就通过CAS技术来实现原子操作。

```java
import java.util.concurrent.atomic.AtomicReference;

public class CasNumberRange {
  private static class IntPair {

    final int lower;
    final int upper;

    public IntPair(int lower, int upper) {
      this.lower = lower;
      this.upper = upper;
    }

  }
  // 不可变对象
  private final AtomicReference<IntPair> values = 
  										new AtomicReference<>(new IntPair(0, 0));

  public int getLower() {
    return values.get().lower;
  }

  public int getUpper() {
    return values.get().upper;
  }

  public void setUpper(int i) {
    while (true) {
      IntPair oldv = values.get();
      if (i < oldv.lower) {
        throw new IllegalArgumentException("Can't set upper to " + i + " < lower");
      }
      IntPair newv = new IntPair(oldv.lower, i);
      if (values.compareAndSet(oldv, newv)) {
        return;
      }
    }
  }

  public void setValues(int i) {
    while (true) {
      IntPair oldV = values.get();
      if (i > oldV.upper) {
        throw new IllegalArgumentException("can't set lower to " + i + " > upper");
      }
      IntPair newV = new IntPair(i, oldV.upper);
      if (values.compareAndSet(oldV, newV)) {
        return;
      }
    }
  }



}
```

## 性能比较：锁与原子变量

锁和原子变量之间的可伸缩性差异：

1. 高度竞争的情况下，锁的性能略微超过原子变量的性能，但真实的情况下，原子性能的变量还是超过锁的性能，因为阻塞会挂起线程。
2. 在中低程度的竞争下，原子变量能提供更高的可伸缩性；

# 非阻塞算法



非阻塞算法：某种算法中，一个线程的失败或挂起不会导致其他线程也失败或挂起；

无锁算法：某种算法中，算法的每个步骤中都存在线程能够执行下去。

如果在算法中仅将CAS用于协调线程之间的操作，并且能正确实现，那么它即是一种无阻塞算法，也是一种无锁算法。

如果有多个线程竞争同一个CAS，那么总会有一个线程在竞争中胜出并执行下去。

非阻塞算法中通常不会出现死锁和优先级反转问题，但可能会出现饥饿和活锁问题，因为在算法中会反复地进行重试。

在许多常见的数据结构中都可以使用非阻塞算法，包括栈、队列、优先队列、散列表。

## 非阻塞的栈

在实现相同功能的前提下，非阻塞算法通常比基于锁的算法更为复杂。创建非阻塞算法的关键在于，找出如何将原子修改的范围缩小到单个变量上，同时还要维护数据的一致性。

ConcurrentStack这样的非阻塞算法都能确保线程安全性，因为compareAndSet想锁定机制一样，既能提供原子性，又能提供可见性。AtomicReference的get提供了与volatile一样的可见性。

```java
import java.util.concurrent.atomic.AtomicReference;

public class ConcurrentStack<E> {

  AtomicReference<Node<E>> top = new AtomicReference<>();
  public void push(E item) {
    Node<E> newHead = new Node<>(item);
    Node<E> oldHead;
    do {
      oldHead = top.get();
      newHead.next = oldHead;
    } while (!top.compareAndSet(oldHead, newHead));
  }

  public E pop() {

    Node<E> oldHead;
    Node<E> newHead;
    do {
      oldHead = top.get();
      if (oldHead == null) {
        return null;
      }
      newHead = oldHead.next;
    } while(!top.compareAndSet(oldHead, newHead));

    return oldHead.item;
  }

  public static class Node<E> {

    public final E item;
    public Node<E> next;

    public Node(E item) {
      this.item = item;
    }
  }
}
```

## 非阻塞的链表

计数器和栈的非阻塞算法实现都很容易实现，但在一些复杂的数据结构上的实现则要复杂许多。

比如链表队列，它必须支持对头节点和尾节点的快速访问，因此它需要单独维护的head和tail指针。

有两个指针指向尾部的节点，一个是尾节点的上一个元素节点的next，另一个就是单独维护的tail指针。

当成功插入一个元素时，这两个指针的指向更新需要保持原子性。这两个指针的指向就是两个单独的CAS，考虑到两个CAS之间会有其他线程访问这个队列进行修改，因此我们需要两个技巧：



第一个技巧：即使在一个包含多个步骤到更新操作中，也要确保数据结构总是处于一致的状态。当线程B到达时，如果发现线程A正在执行更新操作，那么线程B就可以知道有一个操作已经部分完成，并且不能立即开始执行自己的更新操作。然后，B可以等待并直到A完成更新，从而使两个线程不会相互干扰。

第二个技巧：如果当B到达时发现A正在修改数据结构，那么在数据结构中应该有足够多的信息，使得B能完成A的更新操作，如果B帮助A完成了更新操作，那么B可以执行自己的操作，而不用等待A的操作完成。当A恢复后再试图完成其操作时，会发现B已经替它完成了。

下面实现一个这样的链表队列LinkedQueue，保证并发插入的线程安全性。

```java
package com.raftale.snippets.concurrency.chap15;

import java.util.concurrent.atomic.AtomicReference;

public class LinkedQueue<E> {

  public static class Node<E> {

    public final E item;
    public AtomicReference<Node<E>> next;

    public Node(E item, Node<E> next) {
      this.item = item;
      this.next = new AtomicReference<>(next);
    }
  }

  private final Node<E> dummy = new Node<>(null, null);

  private final AtomicReference<Node<E>> head = new AtomicReference<>(dummy);
  private final AtomicReference<Node<E>> tail = new AtomicReference<>(dummy);



  /**
  * 保证并发插入的线程安全性
  * 当队列处于不稳定的中间状态时，tail.next != null，当前线程帮助其他线程推进尾节点
  	当队列处于稳定状态时，tail.next = null，当前线程就尝试插入新节点
  * */
  public boolean put(E item) {
    Node<E> newNode = new Node<>(item , null);
    while(true) {
      // 尾节点
      Node<E> curTail = tail.get();
      Node<E> tailNext = curTail.next.get();
      // 第一个技巧，如果线程B发现线程A正在执行更新操作，那就等待
      if (curTail == tail.get()) {
        // 第二个技巧，当线程B发现自己可以帮助线程A完成更新操作
        if (tailNext != null) {                                             // 步骤A
          // 队列处于中间状态，推进尾节点
          tail.compareAndSet(curTail, tailNext);                            // 步骤B
        } else {
          // 队列处于稳定状态，可以尝试插入新节点
          if (curTail.next.compareAndSet(null, newNode)) {    // 步骤C
            // 新节点插入成功，此时处于中间状态，再将尾节点向后推动
            tail.compareAndSet(curTail, newNode);                           // 步骤D
            // 尾节点向后推动有两种可能发生， 一种是推动成功，然后返回true；另一种是推动失败，说明此时已经有其他线程帮助该线程完成推动
            return true;
          }
        }
      }
    }
  }

}
```

当插入一个新的元素时，要更新两个指针：

1. 当前最后一个元素的next指针指向新节点
2. tail指针指向新节点

这两个操作之间，队列处于中间状态：当前最后一个元素的next指针指向了新节点，但tail指针还没有更新，当tail指针指向新节点时，队列则处于稳定的状态。

很明显的是，中间状态和稳定状态有着很鲜明的区分特征：

1. 中间状态：tail.next != null;
2. 稳定状态：tail.next == null

所以前面说到的两个技巧，第一个技巧是curTail有没有变化，第二个技巧则是判断处于队列中间状态时（tail.next不为null时），当前线程B帮助线程A将tail指针指向新节点。

线程A在从中间状态更新到稳定状态时，有可能线程B已经帮助A完成，所以线程A可能面临更新失败的返回，但实际上已经被B更新成功了。当然如果没有线程B帮助，那也自然会更新成功。

## 原子的域更新器

上面针对链表队列的非阻塞算法其实就是真实的ConcurrentLinkedQueue中的插入算法，但在实际的实现中略有区别。在ConcurrentLinkedQueue中并没有使用原子引用来表示每个node，而是使用普通的volatile引用，并通过基于反射的AtomicReferenceFieldUpdater来进行更新。

原子的域更新器类表示现有volatile域的一种基于反射的视图，从而能够在已有的volatile域上使用CAS。

使用它的原因完全是为了提升性能，例如队列的链接节点，如果能去掉每个Node的原子引用创建过程，那么将极大地降低插入操作的开销。

## ABA问题

CAS中判断「V的值是否仍然为A」，如果是的话就执行更新。但如果V的值首先由A变成B，然后再变成A，虽然值看起来没有变，但它仍然代表了一种变化。一种简单的解决方案是：不是更新某个引用的值，而是更新两个值：一个引用 + 一个版本号，每次更新版本号都会发生变化，就避免了ABA问题。

AtomicStampedReference支持在两个变量上执行原子的条件更新，其思想还是之前就提到过的将多个变量封装到一个不可变的原子引用上。

```java
public class AtomicStampedReference<V> {

  private static class Pair<T> {
    final T reference;
    final int stamp;
    private Pair(T reference, int stamp) {
      this.reference = reference;
      this.stamp = stamp;
    }

    static <T> Pair<T> of(T reference, int stamp) {
      return new Pair<T>(reference, stamp);
    }
  }
  private volatile Pair<V> pair;

  public AtomicStampedReference(V initialRef, int initialStamp) {
    pair = Pair.of(initialRef, initialStamp);
  }

  public V getReference() {
    return pair.reference;
  }

  public int getStamp() {
    return pair.stamp;

  }
}
```
