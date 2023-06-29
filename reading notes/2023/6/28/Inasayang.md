一些`recover`调用相当于空操作

-   内置`recover`函数必须在合适的位置调用才能发挥作用；否则，它的调用相当于空操作

​	

```
func main() {
	defer func() {
		defer func() {
			recover() // 空操作
		}()
	}()
	defer func() {
		func() {
			recover() // 空操作
		}()
	}()
	func() {
		defer func() {
			recover() // 空操作
		}()
	}()
	func() {
		defer recover() // 空操作
	}()
	func() {
		recover() // 空操作
	}()
	recover() // 空操作
	defer recover() // 空操作
	panic("bye")
}
```

下面这个`recover`调用是有作用的

```
func main() {
	defer func() {
		recover() // 将恢复恐慌"byte"	
	}()
	panic("bye")
}
```

https://golang.org/ref/spec#Handling_panics

在下面的情况下，`recover`函数调用的返回值为`nil`：

-   传递给相应`panic`函数调用的实参为`nil`
-   当前协程并没有处于恐慌状态
-   `recover`函数并未直接在一个延迟函数调用中调用（recover函数并未直接被一个延迟函数调用所直接调用， 或者它直接被一个延迟函数调用所直接调用但是此延迟调用没有被和期望被恢复的恐慌相关联的函数调用所直接调用）



第一个例子中，期望被恢复的恐慌和`main`函数调用相关联；

第一个`recover`调用确实被一个延迟函数调用所直接调用；

但是此延迟函数调用并没有被`main`函数直接调用。



要命...







Go中很多常见的名称，比如`int`、`bool`、`string`、`len`、`cap`、`nil`等，并不是关键字，它们是预声明标识符。 这些预声明的标识符声明在万物代码块中，所以它们可以被声明在内层的相同标识符所遮挡。

```
const len = 3 // 遮挡了内置函数len
var true = 0 // 遮挡了内置常量true
type nil struct {} // 遮挡了内置变量nil
func int(){} // 遮挡了内置类型int
```



Pp. 326-339