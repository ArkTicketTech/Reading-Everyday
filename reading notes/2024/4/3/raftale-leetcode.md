# [1115. 交替打印 FooBar](https://leetcode.cn/problems/print-foobar-alternately/)

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

