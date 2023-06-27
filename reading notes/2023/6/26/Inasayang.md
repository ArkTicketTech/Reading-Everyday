一些恐慌/恢复用例

用例1：避免恐慌导致程序崩溃

此用例广泛地使用于并发程序中

```
func main() {
	listener, err := net.Listen("tcp", ":12345")
	if err != nil {
		log.Fatalln(err)
	}
	for {
		conn, err := listener.Accept()
		if err != nil {
	 		log.Println(err)
		}
		// 在一个新协程中处理客户端连接。
		go ClientHandler(conn)
	}
}
func ClientHandler(c net.Conn) {
	defer func() {
		if v := recover(); v != nil {
			log.Println("捕获了一个恐慌：", v)
			log.Println("防止了程序崩溃")
		}
		c.Close()
	}()
	panic("未知错误") // 演示目的产生的一个恐慌
}
```





用例2：自动重启因为恐慌而退出的协程

当在一个协程将要退出时，程序侦测到此协程是因为一个恐慌而导致此次退出时，可以立即重新创建一个相同功能的协程

```
func shouldNotExit() {
	for {
		time.Sleep(time.Second) // 模拟一个工作负载
		// 模拟一个未预料到的恐慌。
		if time.Now().UnixNano() & 0x3 == 0 {
			panic("unexpected situation")
		}
	}
}
func NeverExit(name string, f func()) {
	defer func() {
		if v := recover(); v != nil { // 侦测到一个恐慌
			log.Printf("协程%s崩溃了，准备重启一个", name)
			go NeverExit(name, f) // 重启一个同功能协程
		}
	}()
	f()
}
func main() {
	log.SetFlags(0)
	go NeverExit("job#A", shouldNotExit)
	go NeverExit("job#B", shouldNotExit)
	select{} // 永久阻塞主线程
}
```



用例3：使用`panic/recover`函数调用模拟长程跳转

可以使用`panic/recover`函数调用来模拟跨函数跳转，尽管一般这种方式并不推荐使用

一旦一个恐慌在一个内嵌函数中产生，当前协程中的执行将会跳转到延迟调用处

```
func main() {
	n := func () (result int) {
		defer func() {
		if v := recover(); v != nil {
			if n, ok := v.(int); ok {
				result = n
			}
		}
	}()
	func () {
		func () {
			func () {
			// ...
				panic(123) // 用恐慌来表示成功返回
			}()
			// ...
		}()
	}()
	// ...
	return 0
	}()
	fmt.Println(n) // 123
}
```

不是很明白什么场景下会用这个



用例4：使用`panic/recover`函数调用来减少错误检查代码

略...



Pp. 315-321