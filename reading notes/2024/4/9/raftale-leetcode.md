# [1226. 哲学家进餐](https://leetcode.cn/problems/the-dining-philosophers/)

哲学家进餐很好的描述了死锁，如果每个哲学家都抓住自己左手边的筷子，然后等待自己右手边的筷子空出来，那么每个哲学家都在等身边的哲学家先放筷子，造成无限等待的死锁情况。

哲学家就餐是最简单的死锁形式，每个线程（哲学家）就是有向图中的一个节点，每条边表示的关系是：线程A等待线程B所占有的资源，如果图中形成了一条环路，那就存在一个死锁。

死锁发生在多个线程，一个思路是每次只让一个线程工作，即每次只让一个哲学家进餐，但这降低了进餐的效率，因为5个哲学家每次最大可支持2个哲学家同时进餐。

##  固定顺序

发生死锁的一个原因是，两个线程以不同的顺序交错的获取两把锁：

```java
// 下面将发生死锁
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

所以打破死锁的一个方法就是所有线程都以固定的顺序来获取锁，即哲学家先拿左手筷子，再拿右手筷子进餐。

```java
class DiningPhilosophers {

  private final Object leftLock = new Object();
  private final Object rightLock = new Object();

  // 每个哲学家都要吃
  public DiningPhilosophers() {

  }

  // call the run() method of any runnable to execute its code
  public void wantsToEat(int philosopher,
                         Runnable pickLeftFork,
                         Runnable pickRightFork,
                         Runnable eat,
                         Runnable putLeftFork,
                         Runnable putRightFork) throws InterruptedException {
    synchronized(leftLock) {
      pickLeftFork.run();
      synchronized(rightLock) {
        pickRightFork.run();
        eat.run();
        putRightFork.run();
      }
      putLeftFork.run();
    }
  }
}
```

## 



## 控制进餐人数

哲学家在一张桌子上保持一个环形，所以哲学家只可能拿到邻居哲学家的筷子，如果少一个哲学家，那么每个哲学家都拿着左手边的筷子（因为有一个哲学家不参与竞争）这件事情将永远不会发生，自然也就无法构成死锁。

我们用信号量控制每次进餐的人数为4人，同时也要控制每个筷子只能被一个哲学家获取，其实这里是可以用独占锁来的，因为这里只有一个进餐的函数，所以进餐顺序就固定了，但这就上面固定顺序的代码一致了，看不出来区别。所以我们用信号量来控制筷子的独占获取，即使有多个不同入参的进餐函数`wantsToEat`，也能保证其正确性。

```java
class DiningPhilosophers {

  private volatile Semaphore sp;

  private Semaphore[] forks = new Semaphore[5];


  public DiningPhilosophers() {
    // 控制进餐人数为4
    sp = new Semaphore(4);
    for(int i = 0; i < 5; i++) {
      forks[i] = new Semaphore(1);
    }
  }

  // call the run() method of any runnable to execute its code
  public void wantsToEat(int philosopher,
                         Runnable pickLeftFork,
                         Runnable pickRightFork,
                         Runnable eat,
                         Runnable putLeftFork,
                         Runnable putRightFork) throws InterruptedException {
    sp.acquire();

    int left = philosopher;
    int right = (philosopher - 1 + 5) % 5;
    forks[left].acquire();
    forks[right].acquire();

    pickLeftFork.run();
    pickRightFork.run();
    eat.run();
    putRightFork.run();
    putLeftFork.run();

    forks[left].release();
    forks[right].release();

    sp.release();
  }
}
```

可以测试，当进餐人数设置为5时，就会发生死锁。



## 失败重试避免死锁

如果不避免死锁的发生，那可以在死锁发生后进行恢复，当哲学家尝试拿筷子拿不到时，就说明别人占有这个筷子，很可能发生死锁，此时哲学家放弃手中所有筷子，稍后再次重试。

```java
class DiningPhilosophers {

  private Lock leftLock;
  private Lock rightLock;

  public DiningPhilosophers() {
    leftLock = new ReentrantLock();
    rightLock = new ReentrantLock();
  }

  // call the run() method of any runnable to execute its code
  public void wantsToEat(int philosopher,
                         Runnable pickLeftFork,
                         Runnable pickRightFork,
                         Runnable eat,
                         Runnable putLeftFork,
                         Runnable putRightFork) throws InterruptedException {
    while (true) {
      // 这里如果直接用tryLock()该题会超时
      if (leftLock.tryLock(10, TimeUnit.MILLISECONDS)) {
        try {
          pickLeftFork.run();
          if (rightLock.tryLock(10, TimeUnit.MILLISECONDS)) {

            try {
              pickRightFork.run();
              eat.run();
              putRightFork.run();
            } finally {
              rightLock.unlock();
            }
            return;
          }
        } finally {
          putLeftFork.run();
          leftLock.unlock();
        }
      }
    }
  }
}
```
