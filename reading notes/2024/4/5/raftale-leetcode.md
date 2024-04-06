# [1115. 交替打印 FooBar](https://leetcode.cn/problems/print-foobar-alternately/)

替打印的思想就是：

1. 打印foo前对foo进行阻塞，打印bar后对foo进行释放；
2. 打印foo后对bar进行阻塞，打印bar前对bar进行释放。

用容量为1的阻塞队列也能实现交替打印。

```java
class FooBar {
  private int n;
  private BlockingQueue<Integer> foo;
  private BlockingQueue<Integer> bar;

  public FooBar(int n) {
    this.n = n;
    foo = new ArrayBlockingQueue<>(1);
    bar = new ArrayBlockingQueue<>(1);
  }

  public void foo(Runnable printFoo) throws InterruptedException {

    for (int i = 0; i < n; i++) {
      foo.put(1);
      // printFoo.run() outputs "foo". Do not change or remove this line.
      printFoo.run();
      bar.put(1);
    }
  }

  public void bar(Runnable printBar) throws InterruptedException {

    for (int i = 0; i < n; i++) {
      bar.take();
      // printBar.run() outputs "bar". Do not change or remove this line.
      printBar.run();
      foo.take();
    }
  }
}
```
