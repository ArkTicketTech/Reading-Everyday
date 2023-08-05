NaN != NaN，Inf == Inf。

```
var a = math.Sqrt(-1.0)
fmt.Println(a) // NaN
fmt.Println(a == a) // false
var x = 0.0
var y = 1.0 / x
var z = 2.0 * y
fmt.Println(y, z, y == z) // +Inf +Inf true
```



在结构体值的比较中，名为空标识符的字段将被忽略

```
type T struct {
	_ int
	_ bool
}
func main() {
	var t1 = T{123, true}
	var t2 = T{789, false}
	fmt.Println(t1 == t2) // true
}
```



栈溢出不可被挽救，它将使程序崩溃

```
func f() {
	f()
}
func main() {
	defer func() {
		recover() // 无法防止程序崩溃
	}()
	f()
}
```



`reflect.DeepEqual(x, y)`和`x == y`的结果可能会不同

-   表达式x和y的类型不相同
    -   `DeepEqual(x, y)`的结果总为false
    -   `x == y`的估值结果有可能为true
-   x和y为（同类型的）两个引用着不同其它值的指针值
    -   `x == y`的估值结果总为false
    -   `DeepEqual(x, y)`的结果可能为true
        -   `reflect.DeepEqual`将比较x和y所引用的两个值
-   当x和y均处于某个循环引用链中时，为了防止死循环，DeepEqual调用的结果可能为true
-   DeepEqual(x, y)调用无论如何不应该产生一个恐慌，但是如果x和y是两个动态类型相同的接口值并且它们的动态类型是不可比较类型的时候，x == y将产生一个恐慌



Pp. 540