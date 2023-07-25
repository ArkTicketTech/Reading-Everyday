Go中的nil



预声明的nil标识符可以表示很多种类型的零值

-   指针类型（包括类型安全和非类型安全指针）
-   映射类型
-   切片类型
-   函数类型
-   通道类型
-   接口类型



nil不是一个关键字

-   预声明标识符nil可以被更内层的同名标识符所遮挡

    ```
    func main() {
    	nil := 123
    	fmt.Println(nil) // 123
    	// 下面这行编译报错，因为此行中的nil是一个int值。
    	var _ map[string]int = nil
    }
    ```

    

不同种类的类型的nil值的尺寸很可能不相同

-   一个类型的所有值的内存布局都是一样的，此类型nil值也不例外（假设此类型的零值使用nil表示）

    ```
    func main() {
    	var p *struct{} = nil
    	fmt.Println( unsafe.Sizeof( p ) ) // 8
    	var s []int = nil
    	fmt.Println( unsafe.Sizeof( s ) ) // 24
    	var m map[int]bool = nil
    	fmt.Println( unsafe.Sizeof( m ) ) // 8
    	var c chan string = nil
    	fmt.Println( unsafe.Sizeof( c ) ) // 8
    	var f func() = nil
    	fmt.Println( unsafe.Sizeof( f ) ) // 8
    	var i interface{} = nil
    	fmt.Println( unsafe.Sizeof( i ) ) // 16
    }
    ```



两个不同类型的nil值可能不能相互比较

```
// error: 类型不匹配
var _ = (*int)(nil) == (*bool)(nil)
// error: 类型不匹配
var _ = (chan int)(nil) == (chan bool)(nil)
```

```
type IntPtr *int
	// 类型IntPtr的底层类型为*int。
	var _ = IntPtr(nil) == (*int)(nil)
	// 任何类型都实现了interface{}类型。
	var _ = (interface{})(nil) == (*int)(nil)
	// 一个双向通道可以隐式转换为和它的
	// 元素类型一样的单项通道类型。
	var _ = (chan int)(nil) == (chan<- int)(nil)
	var _ = (chan int)(nil) == (<-chan int)(nil)
```



同一个类型的两个nil值可能不能相互比较

-   映射类型、切片类型和函数类型是不支持比较类型
-   比较同一个不支持比较的类型的两个值（包括nil值）是非法的

```
var _ = ([]int)(nil) == ([]int)(nil)
var _ = (map[string]int)(nil) == (map[string]int)(nil)
var _ = (func())(nil) == (func())(nil)
```

但是，映射类型、切片类型和函数类型的任何值都可以和类型不确定的裸nil标识符比较。

```
// 这几行编译都没问题。
var _ = ([]int)(nil) == nil
var _ = (map[string]int)(nil) == nil
var _ = (func())(nil) == nil
```





两个nil值可能并不相等

-   比较的两个nil值中的一个的类型为接口类型，而另一个不是，则比较结果总是false

```
fmt.Println( (interface{})(nil) == (*int)(nil) ) // false
```

-   在进行此比较之前，非接口nil值将被转换为另一个nil值的接口类型，从而将此比较转化为两个接口值的比较
-   一个非接口值被转换为一个接口类型的过程可以看作是用一个接口值将此非接口值包裹起来的过程
-   一个nil接口值中什么也没包裹，但是一个包裹了nil非接口值的接口值并非什么都没包裹



访问nil映射值的条目不会产生恐慌

-   访问一个nil映射将得到此映射的类型的元素类型的零值

```
fmt.Println( (map[string]int)(nil)["key"] ) // 0
fmt.Println( (map[int]bool)(nil)[123] ) // false
fmt.Println( (map[int]*int64)(nil)[123] ) // <nil>
```



range关键字后可以跟随nil通道、nil映射、nil切片和nil数组指针

-   遍历nil映射和nil切片的循环步数均为零
-   遍历一个nil数组指针的循环步数为对应数组类型的长度。 （但是，如果此数组类型的长度不为零并且第二个循环变量未被舍弃或者忽略，则对应for-range循环将导致一个恐慌。）
-   遍历一个nil通道将使当前协程永久阻塞

```
for range []int(nil) {
	fmt.Println("Hello")
}
for range map[string]string(nil) {
	fmt.Println("world")
}
for i := range (*[5]int)(nil) {
	fmt.Println(i)
}
for range chan bool(nil) { // 阻塞在此
	fmt.Println("Bye")
}
```



通过nil非接口属主实参调用方法不会造成恐慌

```
type Slice []bool
func (s Slice) Length() int {
	return len(s)
}
func (s Slice) Modify(i int, x bool) {
	s[i] = x // panic if s is nil
}
func (p *Slice) DoNothing() {}
func (p *Slice) Append(x bool) {
	*p = append(*p, x) // 如果p为空指针，则产生一个恐慌。
}
func main() {
	// 下面这几行中的选择器不会造成恐慌。
	_ = ((Slice)(nil)).Length
	_ = ((Slice)(nil)).Modify
	_ = ((*Slice)(nil)).DoNothing
	_ = ((*Slice)(nil)).Append

	// 这两行也不会造成恐慌。
	_ = ((Slice)(nil)).Length()
	((*Slice)(nil)).DoNothing()

	// 下面这两行都会造成恐慌。但是恐慌不是因为nil
	// 属主实参造成的。恐慌都来自于这两个方法内部的
	// 对空指针的解引用操作。
	/*
	((Slice)(nil)).Modify(0, true)
	((*Slice)(nil)).Append(true)
	*/
}
```



Pp. 482-490