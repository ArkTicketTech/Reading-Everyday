# [1115. 交替打印 FooBar](https://leetcode.cn/problems/print-foobar-alternately/)
交替打印的思想就是：
1. 打印foo前对foo进行阻塞，打印bar后对foo进行释放；
2. 打印foo后对bar进行阻塞，打印bar前对bar进行释放。
   
如果使用semaphore，需要注意的是release()并不是阻塞的，即使没有许可，也能进行释放，所以先释放许可再获取许可 才是真正的阻塞。
```java
class FooBar {
  private int n;
  private Semaphore foo;
  private Semaphore bar;

  public FooBar(int n) {
    this.n = n;
    foo = new Semaphore(1);
    bar = new Semaphore(0);
  }

  public void foo(Runnable printFoo) throws InterruptedException {

    for (int i = 0; i < n; i++) {
      foo.acquire();
      // printFoo.run() outputs "foo". Do not change or remove this line.
      printFoo.run();
      bar.release();
    }
  }

  public void bar(Runnable printBar) throws InterruptedException {

    for (int i = 0; i < n; i++) {
      bar.acquire();
      // printBar.run() outputs "bar". Do not change or remove this line.
      printBar.run();
      foo.release();
    }
  }
}
```

