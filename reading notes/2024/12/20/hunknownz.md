final 关键字

final 的含义：该变量生而不变，可以优化重排。

Java 内存模型对 final 类型变量的重排进行约束，主要提供正确构造函数没有“逸出”，就不会出现问题。

「逸出」的例子：

```java
class FinalFieldExample {
    final int x;
    int y;

    public FinalFieldExample() {
        x = 3;               // 初始化 x
        y = 4;               // 初始化 y
        global.obj = this;   // this 逸出
    }
}
```

`global.obj`被重排，在 x 之前执行，将 this发布出去。

`this`逸出：在构造函数未完成之前发布`this`，可能导致对象的部分字段未被正确初始化。
