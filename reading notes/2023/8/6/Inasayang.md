Go运行时是否维护映射条目的遍历顺序？

-   对于标准编译器，映射元素的遍历顺序是随机的
-   需要自己来维护这个顺序(https://blog.golang.com/go-maps-in-action#TOC_7)
-   标准库包中的各个打印函数的结果中，映射条目总是排了序的



Go编译器是否会进行字节填充以确保结构体字段的地址对齐？

-   对于标准的Go编译器和gccgo，会
-   Go编译器将不会重新排列结构体的字段来最小化结构体值的尺寸。 因为这样做会导致意想不到的结果
-   可以手工重新排序字段来实现填充最小化



为什么一个结构体类型的最后一个字段类型的尺寸为零时会影响此结构体的尺寸？（Mark）

-   一个可寻址的结构值的所有字段都可以被取地址
-   如果非零尺寸的结构体值的最后一个字段的尺寸是零，那么取此最后一个字段的地址将会返回一个越出了此结构体值分配的内存块的地址（可能指向另一个被分配的内存块）
-   标准的Go编译器会确保取一个非零尺寸的结构体值的最后一个字段的地址时，绝对不会返回越出分配给此结构体值的内存块的地址
-   Go标准编译器通过在需要时在结构体最后的零尺寸字段之后填充一些字节来实现这一点
-   如果一个结构体的全部字段的类型都是零尺寸的(因此整个结构体也是零尺寸的)，那么就不需要再填充字节，因为标准编译器会专门处理零尺寸的内存块。

```
type T1 struct {
	a struct{}
	x int64
}
fmt.Println(unsafe.Sizeof(T1{})) // 8
```

```
type T2 struct {
	x int64
	a struct{}
}
fmt.Println(unsafe.Sizeof(T2{})) // 16
```



运行时错误信息`all goroutines are asleep - deadlock`意味着什么?

-   实际上的意思是处于阻塞状态
-   一个处于阻塞状态的协程只能被另一个协程解除阻塞，如果程序中所有的协程都进入了阻塞状态，则它们将永远都处于阻塞状态（程序死锁了）



64位整数值的地址是否能保证总是64位对齐的，以便可以被安全地原子访问？

-   传递给`sync/atomic`标准库包中的64位函数的地址必须是64位对齐的，否则在运行时导致恐慌
-   在64位架构下，64位整数的地址将保证总是64位对齐
-   在32位架构下，64位整数的地址仅保证是32位对齐（原子访问某些64位整数可能会导致恐慌）



是否每一个零值在内存中占据的字节都是零？

-   对于标准编译器，对于某些字符串类型的零值

```
var s1 string
fmt.Println(s1 == "") // true
fmt.Println(*(*uintptr)(unsafe.Pointer(&s1))) // 0
var s2 = "abc"[0:0]
fmt.Println(s2 == "") // true
fmt.Println(*(*uintptr)(unsafe.Pointer(&s2))) // 4869856
fmt.Println(s1 == s2) // true
```

-   反过来，对于标准编译器已经支持的所有架构，如果一个值的所有字节都是零，那么这个值肯定是它的类型的零值



终结器（finalizer）可以用做对象的析构函数吗？(Mark)

-   可以通过调用`runtime.SetFinalizer`函数来给一个对象设置一个终结器函数

-   一般说来，此终结器函数将在此对象被垃圾回收之前调用
-   通过`runtime.SetFinalizer`函数设置的终结器函数并不保证总会被运行
-   有一些终结器函数永远不会被调用，并且有时候不当的设置终结器函数将会阻止对象被垃圾回收
-   https://golang.google.cn/pkg/runtime/#SetFinalizer



如何使用尽可能短的代码行数来获取任意月份的天数？(有趣)

-   传递给time.Date函数的实参可以超出它们的正常范围，此函数将这些实参进行规范化。 例如，1月32日会被转换成2月1日。

```
// 2017-02-01 00:00:00 +0000 UTC
fmt.Println(time.Date(2017, 1, 32, 0, 0, 0, 0, time.UTC))
// 2017-01-31 23:59:59.999999999 +0000 UTC
fmt.Println(time.Date(2017, 1, 32, 0, 0, 0, -1, time.UTC))
// 2017-01-31 00:00:00 +0000 UTC
fmt.Println(time.Date(2017, 2, 0, 0, 0, 0, 0, time.UTC))
// 2016-12-31 00:00:00 +0000 UTC
fmt.Println(time.Date(2016, 13, 0, 0, 0, 0, 0, time.UTC))
// 2017-02-01 00:00:00 +0000 UTC
fmt.Println(time.Date(2016, 13, 32, 0, 0, 0, 0, time.UTC))
```



函数调用`time.Sleep(d)`和通道接收`<- time.After(d)`操作之间有何区别？

-   两者都会将当前的goroutine执行暂停一段时间

-   `time.Sleep(d)`函数调用将使当前的协程进入睡眠子状态，但是当前协程的（主）状态依然为运行状态
-   通道接收`<-time.After(d)`操作将使当前协程进入阻塞状态



调用`strings`和`bytes`标准库包里`TrimLeft`和`TrimRight`函数经常会返回不符预期的结果，这些函数的实现存在bugs吗？

-   标准包`strings`和`bytes`里有多个修剪（`trim`）函数。 这些函数可以被分类为两组
    -   `Trim`、`TrimLeft`、`TrimRight`、`TrimSpace`、`TrimFunc`、`TrimLeftFunc`和`TrimRightFunc`。 这些函数将修剪首尾所有满足指定（或隐含）条件的utf-8编码的Unicode码点(即rune)。（`TrimSpace`隐含了修剪各种空格符。） 这些函数将检查每个开头或结尾的rune值，直到遇到一个不满足条件的rune值为止
    -   `TrimPrefix`和`TrimSuffix`。 这两个函数会把指定前缀或后缀的子字符串（或子切片）作为一个整体进行修剪



标准库包`math/rand`和`crypto/rand`生成的随机数之间有什么区别？

-   `math/rand`标准库包生成的伪随机数序列对于给定的种子是确定的（不适用于安全敏感的环境中）
-   处于加密安全目的，应该使用`crypto/rand`标准库包生成的伪随机数序列



为什么类型[]T1和[]T2没有共享相同底层类型，即使不同的类型T1和T2共享相同的底层类型？（问题看不懂，要看解释）

-   仅当两个切片类型共享相同的底层类型时，其中一个切片类型才可以转换成另一个切片的类型而不需要使用`unsafe`机制

```
type MyInt int
var a = []int{7, 8, 9}
var b = *(*[]MyInt)(unsafe.Pointer(&a))
b[0]= 123
fmt.Println(a) // [123 8 9]
fmt.Println(b) // [123 8 9]
fmt.Printf("%T \n", a) // []int
fmt.Printf("%T \n", b) // []main.MyInt
```



Pp. 549-563