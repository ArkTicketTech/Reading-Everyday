Go中的一些语法/语义例外

嵌套函数调用

容器和容器元素的可寻址性

-   如果一个容器值是可寻址的，则它的元素也是可寻址的。
-   例外
    -   一个映射值的元素总是不可寻址的，即使此映射本身是可寻址的
-   语法糖
    -   一个切片值的元素总是可寻址的，即使此切片值本身是不可寻址的



修改值

-   不可寻址的值不可修改
-   例外
    -   尽管映射元素是不可寻址的，但是它们可以被修改（但是它们必须被整个覆盖修改）



空组合字面量

-   如果一个类型T的值可以用组合字面量表示，则T{}表示此类型的零值
-   例外
    -   对于一个映射或者切片类型T，T{}不是它的零值，它的零值使用预声明的nil表示



Pp. 500-513