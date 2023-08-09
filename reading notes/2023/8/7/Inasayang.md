什么是`byte`？什么是`rune`？ 如何将`[]byte`和`[]rune`类型的值转换为字符串？

-   `byte=uint8`
-   `rune=int32`

-   一个`rune`值通常被用来存储一个`Unicode`码点
-   `[]byte`和`[]rune`类型的值可以被显式地直接转换成字符串，反之亦然



如何原子地操作指针值？

```
// 加载（读取）
var _ = (*T)(atomic.LoadPointer((*unsafe.Pointer)(unsafe.Pointer(&p))))
// 存储（修改）
atomic.StorePointer((*unsafe.Pointer)(unsafe.Pointer(&p)),unsafe.Pointer(newP))
// 交换
var oldP =(*T(atomic.SwapPointer((*unsafe.Pointer(unsafe.Pointer(&p)),unsafe.Pointer(newP)))
// 比较并交换
var swapped = atomic.CompareAndSwapPointer((*unsafe.Pointer)(unsafe.Pointer(&p)),unsafe.Pointer(oldP),unsafe.Pointer(newP),)
 _ = swapped
```



如何强制一个代码包的使用者总是使用带字段名称的组合字面量来表示此代码包中的结构体类型的值？（Mark）

-   可以在一个结构体类型定义里放置一个非导出的零尺寸的字段，编译器将会禁止代码包的使用者使用含有一些字段但却不含有字段名字的组合字面量来创建此结构体类型的值

```
type Config struct {
	_ [0]int
	Name string
	Size int
}

//_ = foo.Config{[0]int{}, "bar", 123} // 编译不通过
_ = foo.Config{Name: "bar", Size: 123} // 编译没问题
```



如何使一个结构体类型不可比较？（Mark）

-   可以放置一个非导出的零尺寸的不可比较类型的字段在结构体类型中以使此结构体类型不可比较

```
type T struct {
	dummy [0]func()
	AnotherField int
}
var x map[T]int // 编译错误：非法的键值类型

var a, b T
_ = a == b // 编译错误：非法的比较
```



不要使用其中涉及到的表达式之间会相互干涉的赋值语句

```
var a = &[]int{1, 2, 3}
var i int
func f() int {
	i = 1
	a = &[]int{7, 8, 9}
	return 0
}
func main() {
	// 表达式"a"、"i"和"f()"的估值顺序未定义。
	(*a)[i] = f()
	fmt.Println(*a)
}
```

-   如果一个多值赋值语句中涉及的表达式会相互干涉，或者不太容易确定是否会相互干涉，我们应该将此多值赋值语句分拆成多个单值赋值语句
-   可能会打印[7 0 9]、[0 8 9]或者[7 8 9]



如何模拟一些其它语言中支持的`for i in 0..N`循环代码块？

-   通过遍历一个元素尺寸为零的数组或者一个空数组指针

```
const N = 5
for i := range [N]struct{}{} {
	fmt.Println(i)
}
for i := range [N][0]int{} {
	fmt.Println(i)
}
for i := range (*[N]int)(nil) {
	fmt.Println(i)
}
```



如何在不导入reflect标准库包的情况下检查一个值是否拥有某个方法（Mark）

```
type A int
type B int
func (b B) M(x int) string {
	return fmt.Sprint(b, ": ", x)
}

func check(v interface{}) bool {
    _, has := v.(interface{M(int) string})
    return has
}

func main() {
	var a A = 123
	var b B = 789
	fmt.Println(check(a)) // false
	fmt.Println(check(b)) // true
}
```



确保并表明一个自定义类型实现了指定的接口类型（Mark）

```
var _ io.Reader = NewMyReader()
var _ io.Reader = (*MyReader)(nil) /这种多
func _() {_ = io.Reader(nil).(*MyReader)}
```



Pp. 563-577