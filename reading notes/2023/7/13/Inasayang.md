Go中的内存顺序保证

-   编译器优化（在编译时刻）和CPU处理器优化（在运行时刻）会常常调整指令执行顺序，从而使得指令执行顺序和代码中指定的顺序不太一致
-   指令顺序也称为[内存顺序](https://en.wikipedia.org/wiki/Memory_ordering)
-   指令执行顺序的调整规则不是任意的
    -   一个不与其它协程共享数据的协程内部的指令执行顺序调整在此协程内部必须不能被觉察到
    -   如果一些协程之间共享数据，那么在其中一个协程中发生的指令执行顺序调整将有可能被剩余的其它协程觉察到，从而影响到所有这些协程的行为

​	

```
var a string
var done bool

func setup() {
	a = "hello, world"
	done = true
	if done {
		log.Println(len(a)) // 如果被打印出来，它总是12
	}
}
func main() {
	go setup()
	for !done {
		runtime.Gosched()
	}
	log.Println(a) // 期待的打印结果：hello, world
}
```

-   此程序的行为很可能正如我们所料，`hello, world`将被打印输出

-   然而，此程序的行为并非跨编译器和跨平台架构兼容的

-   如果此程序使用一个不同的（但符合Go规范的）编译器或者不同的编译器版本编译，它的运行结果可能是不同的

-   即使此程序使用同一个编译器编译，在不同的平台架构上的运行结果也可能是不同的
-   编译器和CPU可能调整`setup`函数中的前两条语句的执行顺序，使得`setup`协程中的指令的执行顺序和下面的代码指定的顺序一致

​	

```
func setup() {
	done = true
	a = "hello, world"
	if done {
		log.Println(len(a))
	}
}
```

-   最终的打印结果有可能是空，而不是`hello, world`
-   除了没有考虑指令执行顺序调整带来的影响，此程序还存在数据竞争的问题
-   可以使用Go官方工具链中的`go build -race`命令来编译并运行一个程序





Pp. 432-437