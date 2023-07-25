子切片造成的暂时性内存泄露

```
var s0 []int
func g(s1 []int) {
	// 假设s1的长度远大于30。
	s0 = s1[len(s1)-30:]
}
```

承载着切片s1的元素的内存块的开头大段内存将不再可用（假设没有其它值引用着此内存块）。 同时因为s0仍在引用着此内存块，所以此内存块得不到释放

```
func g(s1 []int) {
	s0 = make([]int, 30)
	copy(s0, s1[len(s1)-30:])
	// 现在，如果再没有其它值引用着承载着s1元素的内存块，
	// 则此内存块可以被回收了。
}
```



因为未重置丢失的切片元素中的指针而造成的临时性内存泄露

```
func h() []*int {
	s := []*int{new(int), new(int), new(int), new(int)}
	// 使用此s切片 ...
	return s[1:3:3]
}
```

h函数调用之后，s的首尾两个元素将不再可用

只要h函数调用返回的切片仍在被使用中，它的各个元素就不会回收，包括首尾两个已经丢失的元素。 

因此这两个已经丢失的元素引用着的两个int值也不会被回收，即使我们再也无法使用这两个int值

```
func h() []*int {
	s := []*int{new(int), new(int), new(int), new(int)}
	// 使用此s切片 ...
	s[0], s[len(s)-1] = nil, nil // 重置首尾元素指针
	return s[1:3:3]
}
```

重置丢失的元素中的指针



因为没有停止不再使用的`time.Ticker`值而造成的永久性内存泄露

-   当一个`time.Timer`值不再被使用，一段时间后它将被自动垃圾回收掉
-   对于一个不再使用的`time.Ticker`值，必须调用它的Stop方法结束它，否则它将永远不会得到回收



因为不正确地使用终结器（`finalizer`）而造成的永久性内存泄露

-   将一个终结器设置到一个循环引用值组中的一个值上可能导致被此值组中的值所引用的内存块永远得不到回收
    （https://golang.google.cn/pkg/runtime/#SetFinalizer）

​	

```
func memoryLeaking() {
	type T struct {
		v [1<<20]int
		t *T
	}
	var finalizer = func(t *T) {
		fmt.Println("finalizer called")
	}
	var x, y T
	// 此SetFinalizer函数调用将使x逃逸到堆上。
	runtime.SetFinalizer(&x, finalizer)
	// 下面这行将形成一个包含x和y的循环引用值组。
	// 这有可能造成x和y不可回收。
	x.t, y.t = &y, &x // y也逃逸到了堆上。
}
```



Pp. 473-482