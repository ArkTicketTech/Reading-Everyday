类型内嵌

-   内嵌字段也称为匿名字段
-   一个内嵌字段必须被声明为形式`T`或者一个基类型为非接口类型的指针类型`*T`，其中`T`为一个类型名但是`T`不能表示一个指针类型。
    -   一个类型名`T`只有在它既不表示一个具名指针类型也不表示一个基类型为指针类型或者接口类型的指针类型的情况下才可以被用做内嵌字段
    -   一个指针类型`*T`只有在`T`为一个类型名并且`T`既不表示一个指针类型也不表示一个接口类型的时候才能被用做内嵌字段
-   一个结构体类型中不允许有两个同名字段，此规则对匿名字段同样适用
    -   一个无名指针类型不能和它的基类型同时内嵌在同一个结构体类型中（`int`和`*int`类型不能同时内嵌在同一个结构体类型中）
-   一个结构体类型不能内嵌它自己（无论间接还是直接）



类型内嵌的意义是什么？

-   为了将被内嵌类型的功能扩展到内嵌它的结构体类型中
-   继承和内嵌
    -   类型T继承了另外一个类型，则类型T获取了另外一个类型的能力。 同时，一个T类型的值也可以被当作另外一个类型的值来使用
    -   一个类型T内嵌了另外一个类型，则另外一个类型变成了类型T的一部分。 类型T获取了另外一个类型的能力，但是T类型的任何值都不能被当作另外一个类型的值来使用

Pp. 248-253