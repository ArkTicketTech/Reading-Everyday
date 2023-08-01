

可以使用`os.Exit`函数调用退出一个程序和使用`runtime.Goexit`函数调用退出一个协程

递增运算符++和递减运算符--的优先级低于解引用运算符*和取地址运算符&

解引用运算符和取地址运算符的优先级低于选择器.中的属性选择操作符



移位运算中的左类型不确定操作数的类型推断规则取决于右操作数是否是常量

```
const M = 2
var _ = 1.0 << M // 编译没问题。1.0将被推断为一个int值。
var N = 2
var _ = 1.0 << N // 编译失败。1.0将被推断为一个float64值。
```



两个零尺寸值的地址可能相等，也可能不相等（Mark）

-   两个零尺寸值的地址是否相等时依赖于具体编译器实现以及具体编译器版本

```
func main() {
	a := struct{}{}
	b := struct{}{}
	x := struct{}{}
	y := struct{}{}
	m := [10]struct{}{}
	n := [10]struct{}{}
	o := [10]struct{}{}
	p := [10]struct{}{}
	fmt.Println(&x, &y, &o, &p)
	// 对于标准编译器1.20版本，x、y、o和p将
	// 逃逸到堆上，但是a、b、m和n则开辟在栈上。
	fmt.Println(&a == &b) // false
	fmt.Println(&x == &y) // true
	fmt.Println(&a == &x) // false
	
	fmt.Println(&m == &n) // false
	fmt.Println(&o == &p) // true
	fmt.Println(&n == &p) // false
}
```



从nil映射中读取元素不会导致崩溃，读取结果是一个零元素值

从一个nil映射中删除一个条目不会导致崩溃，这是一个空操作

append函数调用的结果可能会与原始切片共享一些元素，也可能不共享任何元素

从一个基础切片派生出的子切片的长度可能大于基础切片的长度

```
func main() {
	s := make([]int, 3, 9)
	fmt.Println(len(s)) // 3
	s2 := s[2:7]
	fmt.Println(len(s2)) // 5
}
```



Pp. 523-529