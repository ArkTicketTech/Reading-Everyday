一个映射中的条目的遍历次序在两次遍历中可能并不相同。我们可以认为映射中的条目的遍历次序是随机的

-   对映射进行JSON格式化输出中的映射条目是按照它们的键值排序的
-   从Go 1.12开始，使用fmt标准库包中的打印函数打印映射时，输出的映射条目也是按照它们的键值排序的； 而在Go 1.12之前，这些打印输出时乱序的



如果两个接口值具有相同的动态类型并且此动态类型不支持比较，则比较这两个接口值将导致一个恐慌

```
var x interface{} = []int{}
_ = x == x // panic
```



类型断言可以用于将一个接口值转换为另一个接口类型，即使此接口值的类型并未实现另一个接口类型

```
type Foo interface {
	foo()
}
type T int
func (T) foo() {}
func main() {
	var x interface{} = T(123)
	// 下面这两行将编译失败。
	/*
	var _ Foo = x // error: interface{}类型没有实现Foo类型
	var _ = Foo(x) // error: interface{}类型没有实现Foo类型
	*/
	// 但是下面这行可以编译通过。
	var _ = x.(Foo) // okay
}
```



以相同实参调用两次errors.New函数返回的两个error值是不相等的

-   原因是errors.New函数会复制输入的字符串实参至一个局部变量并取此局部变量的指针作为返回error值的动态值。 两次调用会产生两个不同的指针

```
notfound := "not found"
a, b := errors.New(notfound), errors.New(notfound)
fmt.Println(a == b) // false
```



对于标准编译器，结构体中的某些零尺寸字段的尺寸有可能会被视为一个字节（6）（待定）





Pp. 535-540