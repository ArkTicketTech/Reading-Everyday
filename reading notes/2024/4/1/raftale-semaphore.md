Semaphore信号量，可以控制同时访问某个特定资源的操作数量：初始化许可，在获取许可后才能执行后面的代码，执行完相关代码释放许可。

但今天发现Semaphore并不一定要先获取许可再进行释放，即使许可为非正数时，可以先释放许可，直到许可为正时获取许可就不会阻塞。这样可以做来保证多个线程顺序执行。

# [1114. 按序打印](https://leetcode.cn/problems/print-in-order/)

```java
class Foo {
  private Semaphore spa;
  private Semaphore spb;

  public Foo() {
    spa = new Semaphore(0);
    spb = new Semaphore(0); 
  }

  public void first(Runnable printFirst) throws InterruptedException {

    // printFirst.run() outputs "first". Do not change or remove this line.
    printFirst.run();
    // permit plus 1 
    spa.release();
  }

  public void second(Runnable printSecond) throws InterruptedException {
    // 释放许可后许可数量为0
    spa.acquire();
    // printSecond.run() outputs "second". Do not change or remove this line.
    printSecond.run();
    spb.release();
  }

  public void third(Runnable printThird) throws InterruptedException {
    spb.acquire();
    // printThird.run() outputs "third". Do not change or remove this line.
    printThird.run();

  }
}
```
