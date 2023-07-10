峰值限制（peak/burst limiting）

-   将通道用做计数信号量和通道尝试（发送或者接收）操作结合起来可用实现峰值限制

-   峰值限制的目的是防止过大的并发请求数

    ```
    ...
    bar24x7 := make(Bar, 10) // 此酒吧只能同时招待10个顾客
    for customerId := 0; ; customerId++ {
    	time.Sleep(time.Second)
    	consumer := Consumer{customerId}
    	select {
    	case bar24x7 <- consumer: // 试图进入此酒吧
    		go bar24x7.ServeConsumer(consumer)
    	default:
    		log.Print("顾客#", customerId, "不愿等待而离去")
    	}
    }
    ...
    ```



另一种“采用最快回应”的实现方式

-   每个数据源协程只需使用一个缓冲为1的通道并向其尝试发送回应数据

    ```
    func source(c chan<- int32) {
    	ra, rb := rand.Int31(), rand.Intn(3)+1
    	// 休眠1秒/2秒/3秒
    	time.Sleep(time.Duration(rb) * time.Second)
    	select {
    	case c <- ra:
    	default:
    	}
    }
    func main() {
    	rand.Seed(time.Now().UnixNano()) // Go 1.20之前需要
    	c := make(chan int32, 1) // 此通道容量必须至少为1
    	for i := 0; i < 5; i++ {
    		go source(c)
    	}
    	rnd := <-c // 只采用第一个成功发送的回应数据
    	fmt.Println(rnd)
    }
    ```

    

超时机制（timeout）

```
func requestWithTimeout(timeout time.Duration) (int, error){
	c := make(chan int)
	go doRequest(c) // 可能需要超出预期的时长回应
	select {
	case data := <-c:
		return data, nil
	case <-time.After(timeout):
		return 0, errors.New("超时了！")
	}
}
```



脉搏器（ticker）

-   每隔一定时间发送一个信号的脉搏器

​	

```
func Tick(d time.Duration) <-chan struct{} {
	c := make(chan struct{}, 1) // 容量最好为1
	go func() {
		for {
			time.Sleep(d)
			select {
			case c <- struct{}{}:
			default:
			}
		}
	}()
	return c
}
func main() {
	t := time.Now()
	for range Tick(time.Second) {
		fmt.Println(time.Since(t))
	}
}
```

time标准库包中的Tick函数提供了同样的功能，但效率更高



速率限制（rate limiting）

-   速率限制常用来限制吞吐和确保在一段时间内的资源使用不会超标

-   https://github.com/golang/go/wiki/RateLimiting

    ```
    type Request interface{}
    func handle(r Request) {fmt.Println(r.(int))}
    const RateLimitPeriod = time.Minute
    const RateLimit = 200 // 任何一分钟内最多处理200个请求
    
    func handleRequests(requests <-chan Request) {
    	quotas := make(chan time.Time, RateLimit)
    	go func() {
    		tick := time.NewTicker(RateLimitPeriod / RateLimit)
    		defer tick.Stop()
    		for t := range tick.C {
    			select {
    			case quotas <- t:
    			default:
    			}
    		}
    	}()
    	for r := range requests {
    		<-quotas
    		go handle(r)
    	}
    }
    func main() {
    	requests := make(chan Request)
    	go handleRequests(requests)
    	// time.Sleep(time.Minute)
    	for i := 0; ; i++ {
    		requests <- i
    	}
    }
    ```

    虽然可以保证任何一分钟时段内处理的请求数不会超过200，但是如果在
    开始的一分钟内没有任何请求，则接下来的某个瞬时时间点可能会同时处理最多200
    个请求(将time.Sleep行的注释去掉)，会造成卡顿情况

-   将速率限制和峰值限制一并使用来避免卡顿



Pp. 380-385