通道关闭原则

-   不要在数据接收方或者在有多个发送者的情况下关闭通道(让一个通道唯一的发送者关闭此通道)
-   不要关闭已关闭的通道



粗鲁地关闭通道的方法

```
func SafeClose(ch chan T) (justClosed bool) {
	defer func() {
		if recover() != nil {
			// 一个函数的返回结果可以在defer调用中修改。
			justClosed = false
		}
	}()
	// 假设ch != nil。
	close(ch) // 如果ch已关闭，则产生一个恐慌。
	return true // <=> justClosed = true; return
}
```

此方法违反了通道关闭原则

向一个关闭状态未知的通道发送数据，同上，使用`recover`恢复



礼貌地关闭通道的方法

-   使用`sync.Once`来关闭通道

    ```
    type MyChannel struct {
    	C chan T
    	once sync.Once
    }
    func NewMyChannel() *MyChannel {
    	return &MyChannel{C: make(chan T)}
    }
    func (mc *MyChannel) SafeClose() {
    	mc.once.Do(func() {
    		close(mc.C)
    	})
    }
    ```

    优雅！

-   也可以使用`sync.Mutex`来防止多次关闭一个通道（不够优雅！）



Pp. 395-399