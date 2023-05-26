Go中25个关键字：

```
break default func interface select
case  defer   go   map       struct
chan  else    goto package   switch
const fallthrough if range   type
continue for   import return var
```

分为四组：

-   `const`，`func`，`import`，`package`，`type`和`var`用来声明各种代码元素
-   `chan`，`interface`，`map`和`struct`用作一些组合类型的字面表示
-   `break`，`case`，`continue`，`default`，`else`，`fallthrough`，`for`，`goto`，`if`，`range`，`return`，`select`和`switch`用在流程控制语句中
-   `defer`和`go`也可以看作是流程控制关键字



标识符：一个以`Unicode`字母或`_`开头的并且完全由`Unicode`字母和`Unicode`数组组成。

标识符`_`是一个特殊字符，叫*空标识符*。

大写字母开头意味着公开，非大写字母开头意味着私有。东方字符都被视为私有。



内置基本类型:

-   布尔类型：`bool`
-   整数类型：`int8`，`uint8`，`int16`，`uint16`，`int32`，`uint32`，`int64`，`uint64`，`int`，`uint`，`uintptr`
-   浮点数类型：`float32`，`float64`
-   复数类型：`complex64`，`complex128`
-   字符串类型：`string`

上述17种内置基本类型各自属于一种Go中的类型种类（`kind`）



两种内置类型别名（`type alias`）

-   `byte`是`uint8`的内置别名。
-   `rune`是`int32`的内置别名。



`uintptr`，`int`和`uint`类型的值的尺寸依赖于具体编译器实现。

通常，在64位的架构上，`int`和`uint`类型的值是64位的；在32位的架构上，是32位的。

编译器必须保证`uintptr`类型的值的尺寸能够存下任意一个内存地址。



一个`complex64`复数值的实部和虚部都是`float32`类型的值。

一个`complex128`复数值的实部和虚部都是`float64`类型的值。



在内存中，所有的浮点数都是使用`IEEE-754`格式存储。

在内存中，一个字符串存储为一个字节序列。



```
类型定义 是不同的类型
type status bool
type MyString string
```



```
类型别名 是相同的类型
type boolean = bool
type Text = string
```



每种类型都有一个零值。

-   布尔类型：false

-   数值类型：0
-   字符串类型：""



Pp. 15-25