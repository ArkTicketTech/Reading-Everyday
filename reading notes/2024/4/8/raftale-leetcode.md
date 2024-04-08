# [1195. 交替打印字符串](https://leetcode.cn/problems/fizz-buzz-multithreaded/)

细细看题目，题目的要求可以理解每一轮四个线程为同一个数字进行一次计算判断，符合要求的线程进行输出，然后四个线程又继续对下一个数字进行计算判断。所以要求线程在一轮结束前就进行阻塞，在一轮结束后一起开始计算，这就能联想到闭锁（Latch）和栅栏（Barrier），但Latch是一次性的，所以Barrier非常适合这道题。

## 栅栏

`CyclicBarrier`可以使一定数量的参与方反复地在栅栏位置汇集，当线程到达栅栏位置时将调用`await()`方法，该方法将阻塞直到所有线程都到达栅栏位置。

```java
class FizzBuzz {
  private int n;

  private CyclicBarrier barrier;
  public FizzBuzz(int n) {
    this.n = n;
    barrier = new CyclicBarrier(4);
  }

  // printFizz.run() outputs "fizz".
  public void fizz(Runnable printFizz) throws InterruptedException {
    // 
    for(int i = 1; i <= n; i++) {
      if(i % 3 == 0 && i % 5 != 0) {
        printFizz.run();
      }
      try {
        barrier.await();
      } catch (Exception e) {

      }
    }
  }

  // printBuzz.run() outputs "buzz".
  public void buzz(Runnable printBuzz) throws InterruptedException {
    for(int i = 1; i <= n; i++) {
      if(i % 5 == 0 && i % 3 != 0) {
        printBuzz.run();
      }
      try {
        barrier.await();
      } catch (Exception e) {

      }
    }
  }

  // printFizzBuzz.run() outputs "fizzbuzz".
  public void fizzbuzz(Runnable printFizzBuzz) throws InterruptedException {
    for(int i = 1; i <= n; i++) {
      if(i % 3 == 0 && i % 5 == 0) {
        printFizzBuzz.run();
      }
      try {
        barrier.await();
      } catch (Exception e) {

      }
    }
  }

  // printNumber.accept(x) outputs "x", where x is an integer.
  public void number(IntConsumer printNumber) throws InterruptedException {
    for(int i = 1; i <= n; i++) {
      if(i % 3 != 0 && i % 5 != 0) {
        printNumber.accept(i);
      }
      try {
        barrier.await();
      } catch (Exception e) {

      }
    }
  }
}
```

## 条件队列

如果不用现有的栅栏API，使用条件队列也是可以实现的。

模拟栅栏的原理：

1. 用一个变量count作为栅栏开启的等待线程，题目为4个等待的线程，初始值为0，当count=4时开启栅栏。
2. 用一个版本号变量generation来保证开启栅栏后所有的等待线程都能够通过栅栏。
3. 因为条件都是一样的，内置条件队列和外置的差不多。

```java
class FizzBuzz {
  private int n;

  private volatile int count;
  private volatile int generation;

  public FizzBuzz(int n) {
    this.n = n;
    this.count = 0;
    this.generation = 0;
  }

  private void await() throws InterruptedException {
    int arrivalGeneration = generation;
    count++;
    while (count != 4 && arrivalGeneration == generation) {
      wait();
    }
  }

  private void signalAll() {
    if (count == 4) {
      notifyAll();
      count = 0;
      generation++;
    }
  }

  // printFizz.run() outputs "fizz".
  public void fizz(Runnable printFizz) throws InterruptedException {
    for (int i = 1; i <= n; i++) {
      synchronized (this) {
        int arrivalGeneration = generation;
        if (i % 3 == 0 && i % 5 != 0) {
          printFizz.run();
        }

        await();
        signalAll();
      }
    }
  }

  // printBuzz.run() outputs "buzz".
  public void buzz(Runnable printBuzz) throws InterruptedException {
    for (int i = 1; i <= n; i++) {
      synchronized (this) {
        int arrivalGeneration = generation;
        if (i % 5 == 0 && i % 3 != 0) {
          printBuzz.run();
        }

        await();
        signalAll();
      }
    }
  }

  // printFizzBuzz.run() outputs "fizzbuzz".
  public void fizzbuzz(Runnable printFizzBuzz) throws InterruptedException {

    for (int i = 1; i <= n; i++) {
      synchronized (this) {
        int arrivalGeneration = generation;
        if (i % 3 == 0 && i % 5 == 0) {
          printFizzBuzz.run();
        }

        await();
        signalAll();
      }
    }
  }

  // printNumber.accept(x) outputs "x", where x is an integer.
  public void number(IntConsumer printNumber) throws InterruptedException {

    for (int i = 1; i <= n; i++) {
      synchronized (this) {
        if (i % 3 != 0 && i % 5 != 0) {
          printNumber.accept(i);
        }

        await();
        signalAll();
      }
    }
  }


}
```
