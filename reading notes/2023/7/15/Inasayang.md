原子操作相关的顺序保证

-   照顺序一致次序（sequentially consistent order）执行
-   如果一个原子（写）操作A的效果被一个原子（读）操作B观察到，则A肯定被同步到B之前执行



和终结器相关的顺序保证

-   调用`runtime.SetFinalizer(x, f)`发生在终结调用`f(x)`被执行之前



一些常见并发编程错误

当需要同步的时候没有同步

```
func main() {
	var a []int // nil
	var b bool // false
	// 一个匿名协程。
	go func () {
		a = make([]int, 3)
		b = true // 写入b
	}()
	for !b { // 读取b
		time.Sleep(time.Second)
		runtime.Gosched()
	}
	a[0], a[1], a[2] = 0, 1, 2 // 可能会发生恐慌
}
```

-   主协程中对变量b的读取和匿名协程中的对变量b的写入可能会产生数据竞争
-   条件`b == true`成立并不能确保条件`a != nil`也成立
-   编译器和CPU可能会对调整此程序中匿名协程中的某些指令的顺序已获取更快的执行速度
-   对变量b的赋值可能会发生在对变量a的赋值之前，这将造成在修改a的元素时a依然为一个nil切片

应该使用通道或者sync标准库包中的同步技术来确保内存顺序

```
func main() {
	var a []int = nil
	c := make(chan struct{})
	go func () {	
		a = make([]int, 3)	
		c <- struct{}{}
	}()
	<-c
	a[0], a[1], a[2] = 0, 1, 2 // 绝不会造成恐慌
}
```



Pp. 442-447