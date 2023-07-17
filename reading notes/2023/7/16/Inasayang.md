使用`time.Sleep`调用来做同步 略



使一些协程永久处于阻塞状态 略



复制sync标准库包中的类型的值

-   在实践中，sync标准库包中的类型（除了Locker接口类型）的值不应该被复制。我们只应该复制它们的指针值

​		

```
type Counter struct {
	sync.Mutex
	n int64
}
// 此方法实现是没问题的。
func (c *Counter) Increase(d int64) (r int64) {
	c.Lock()
	c.n += d
	r = c.n
	c.Unlock()
	return
}
// 此方法的实现是有问题的。当它被调用时，
// 一个Counter属主值将被复制。
func (c Counter) Value() (r int64) {
	c.Lock()
	r = c.n
	c.Unlock()
	return
}
```

当`Counter.Value`方法被调用时，一个`Counter`属主值将被复制，此属主值的字段`Mutex`也将被一同复制。

应该将`Value`方法的属主参数类型更改为指针类型`*Counter`来避免复制`sync.Mutex`值

`go vet`命令将提示此例中的Value方法的声明可能是一个潜在的逻辑错误



在错误的地方调用`sync.WaitGroup.Add`方法

```
func main() {
	var wg sync.WaitGroup
	var x int32 = 0
	for i := 0; i < 100; i++ {
		go func() {
			wg.Add(1)
			atomic.AddInt32(&x, 1)
			wg.Done()
		}()
	}
	fmt.Println("等待片刻...")
	wg.Wait()
	fmt.Println(atomic.LoadInt32(&x))
}
```

Add方法的调用位置是不合适的

```
...
for i := 0; i < 100; i++ {
	wg.Add(1)
	go func() {
		atomic.AddInt32(&x, 1)
		wg.Done()
	}()
}
...
```





Pp. 447-452