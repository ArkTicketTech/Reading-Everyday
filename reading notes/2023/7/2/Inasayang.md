什么是并发同步？

并发同步是指如何控制若干并发计算（在Go中，即协程）

-   避免在它们之间产生数据竞争的现象
-   避免在它们无所事事的时候消耗CPU资源



通道用例大全

-   使用通道进行异步和并发编程是简单和惬意的
-   通道同步技术比被很多其它语言采用的其它同步方案（比如角色模型（https://en.wikipedia.org/wiki/Actor_model）和`async/await`模式（https://en.wikipedia.org/wiki/Async/await））有着更多的应用场景和更多的使用变种



将通道用做`future/promise`

很多其它流行语言支持f`uture/promise`来实现异步（并发）编程。 `Future/promise`常常用在请求/回应场合



返回单向接收通道做为函数返回结果

```
func longTimeRequest() <-chan int32 {
	r := make(chan int32)
	go func() {
		time.Sleep(time.Second * 3) // 模拟一个工作负载
		r <- rand.Int31n(100)
	}()
	return r
}
func sumSquares(a, b int32) int32 {
	return a*a + b*b
}
func main() {
	rand.Seed(time.Now().UnixNano()) // Go 1.20之前需要
	a, b := longTimeRequest(), longTimeRequest()
	fmt.Println(sumSquares(<-a, <-b))
}
```



将单向发送通道类型用做函数实参

```
func longTimeRequest(r chan<- int32) {
	time.Sleep(time.Second * 3) // 模拟一个工作负载
	r <- rand.Int31n(100)
}
func sumSquares(a, b int32) int32 {
	return a*a + b*b
}
func main() {
	rand.Seed(time.Now().UnixNano()) // Go 1.20之前需要
	ra, rb := make(chan int32), make(chan int32)
	go longTimeRequest(ra)
	go longTimeRequest(rb)
	fmt.Println(sumSquares(<-ra, <-rb))
}
```

可以只使用一个通道来接收回应结果

```
    ...
    results := make(chan int32, 2) // 缓冲与否不重要
    go longTimeRequest(results)
    go longTimeRequest(results)
    fmt.Println(sumSquares(<-results, <-results))
}
```



采用最快回应

有时候，一份数据可能同时从多个数据源获取。这些数据源将返回相同的数据。 因为各种因素，这些数据源的回应速度参差不一，甚至某个特定数据源的多次回应速度之间也可能相差很大。 同时从多个数据源获取一份相同的数据可以有效保障低延迟。我们只需采用最快的回应并舍弃其它较慢回应。

```
func source(c chan<- int32) {
	ra, rb := rand.Int31(), rand.Intn(3) + 1
	// 睡眠1秒/2秒/3秒
	time.Sleep(time.Duration(rb) * time.Second)
	c <- ra
}
func main() {
	rand.Seed(time.Now().UnixNano()) // Go 1.20之前需要
	startTime := time.Now()
	c := make(chan int32, 5) // 必须用一个缓冲通道
	for i := 0; i < cap(c); i++ {
		go source(c)
	}
	rnd := <- c // 只有第一个回应被使用了
	fmt.Println(time.Since(startTime))
	fmt.Println(rnd)
}
```

好奇这个的使用场景



更多“请求/回应”用例变种

-   做为函数参数和返回结果使用的通道可以是缓冲的，从而使得请求协程不需阻塞到它所发送的数据被接收为止
-   有时，一个请求可能并不保证返回一份有效的数据。对于这种情形，我们可以使用一个形如struct{v T; err error}的结构体类型或者一个空接口类型做为通道的元素类型以用来区分回应的值是否有效（rust...）



Pp. 360-