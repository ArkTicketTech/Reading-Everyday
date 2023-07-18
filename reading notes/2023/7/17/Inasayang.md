不当地使用用做`Future/Promise`的通道

```
doSomethingWithFutureArguments(<-fa(), <-fb())
```

没有体现出这两个函数的真正价值

两个实参值（promise回应结果）的生成实际上是串行进行的

```
ca, cb := fa(), fb()
doSomethingWithFutureArguments(<-ca, <-cb)
```





过多的`time.After`函数调用消耗了大量资源

-   `time`标准库包中的`After`函数返回一个用做延迟通知的通道
-   每个调用都需要创建一个`time.Timer`值
-   新创建的`Timer`值在传递给`After`函数调用的时长（实参）内肯定不会被垃圾回收
-   如果此函数在某个时段内被多次频繁调用，则可能导致积累很多尚未过期的`Timer`值从而造成大量的内存和计算消耗

​	

```
// 如果某两个连续的消息的间隔大于一分钟，此函数将返回。
func longRunning(messages <-chan string) {
	for {	
		select {
		case <-time.After(time.Minute):
			return
		case msg := <-messages:
			fmt.Println(msg)
		}
	}
}
```

如果`longRunning`函数被调用并且在一分钟内有一百万条消息到达， 那么在某个特定的很小时间段（大概若干秒）内将存在一百万个活跃的`Timer`值，即使其中只有一个是真正有用的

为了避免太多的`Timer`值被创建，我们应该只使用（并复用）一个`Timer`值

```
func longRunning(messages <-chan string) {
	timer := time.NewTimer(time.Minute)
	defer timer.Stop()
	for {
		select {
		case <-timer.C: // 过期了
			return
		case msg := <-messages:
			fmt.Println(msg)
			// 此if代码块很重要。
			if !timer.Stop() {
				<-timer.C
			}
		}
		// 必须重置以复用。
		timer.Reset(time.Minute)
	}
}
```

if代码块用来舍弃一个可能在执行第二个分支代码块的时候发送过来的超时通知



在多个协程中使用同一个`time.Timer`值比较容易写出不当的并发代码，所以尽量不要跨协程使用一个`Timer`值



Pp. 452-456