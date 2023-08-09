一些编译时刻断言技巧

-   在编译时刻保证常量N不小于另一个常量M（有趣）

    ```
    // 下面任一行均可保证N >= M
    func _(x []int) {_ = x[N-M]}
    func _(){_ = []int{N-M: 0}}
    func _([N-M]int){}
    var _ [N-M]int
    const _ uint = N-M
    type _ [N-M]int
    
    // 如果M和N都是正整数常量，则我们也可以使用下一行所示的方法。
    var _ uint = N/M - 1
    
    //利用了容器组合字面量中不能出现重复的常量键值
    var _ = map[bool]struct{}{false: struct{}{}, N>=M: struct{}{}}
    //也可以不必很冗长，但需要多消耗一点（完全可以忽略的）内存
    var _ = map[bool]int{false: 0, N>=M: 1}
    ```

-   断言两个整数常量相等的方法

    ```
    var _ [N-M]int; var _ [M-N]int
    type _ [N-M]int; type _ [M-N]int
    const _, _ uint = N-M, M-N
    func _([N-M]int, [M-N]int) {}
    
    var _ = map[bool]int{false: 0, M==N: 1}
    
    var _ = [1]int{M-N: 0} // 唯一被允许的元素索引下标为0
    var _ = [1]int{}[M-N] // 唯一被允许的元素索引下标为0
    
    var _ [N-M]int = [M-N]int{}
    ```

-   断言一个常量字符串是不是一个空串的方法

    ```
    type _ [len(aStringConstant)-1]int
    var _ = map[bool]int{false: 0, aStringConstant != "": 1}
    var _ = aStringConstant[:1]
    var _ = aStringConstant[0]
    const _ = 1/len(aStringConstant)
    ```

-   为了避免包级变量消耗太多的内存，我们可以把断言代码放在一个名为空标识符的函数体中

    ```
    func _() {
        var _ = map[bool]int{false: 0, N>=M: 1}
        var _ [N-M]int
    }
    ```



如何声明一个最大的int和uint常量？

```
const MaxUint = ^uint(0)
const MaxInt = int(^uint(0) >> 1)
```



尽量避免将大尺寸的值包裹在接口值中（Mark）

-   当一个非接口值被赋值给一个接口值时，此非接口值的一个副本将被包裹到此接口值中
-   副本复制的开销和非接口值的尺寸成正比。尺寸越大，复制开销越大

```
var a [1000]int

// 这两行的开销相对较大，因为数组a中的元素都将被复制。
fmt.Println(a)
fmt.Printf("Type of a: %T\n", a)

// 这两行的开销较小，数组a中的元素没有被复制。
fmt.Printf("%v\n", a[:])
fmt.Println("Type of a:", fmt.Sprintf("%T", &a)[1:])
```



编译器指示（compiler directive）

-   一个编译器指示以注释的形式`//go:DirectiveName args`出现



更多的编译模式（build mode）

-   `go help buildmode`可以列出所有支持的模式

Pp. 577-.