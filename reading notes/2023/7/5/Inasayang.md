使当前协程永久阻塞

-   一个无分支的select流程控制代码块使当前协程永久处于阻塞状态



尝试发送和尝试接收

-   含有一个default分支和一个case分支的select代码块可以被用做一个尝试发送或者尝试接收操作，取决于case关键字后跟随的是一个发送操作还是一个接收操作
    -   如果case关键字后跟随的是一个发送操作，则此select代码块为一个尝试发送操作。 如果case分支的发送操作是阻塞的，则default分支将被执行，发送失败；否则发送成功，case分支得到执行
    -   如果case关键字后跟随的是一个接收操作，则此select代码块为一个尝试接收操作。 如果case分支的接收操作是阻塞的，则default分支将被执行，接收失败；否则接收成功，case分支得到执行



无阻塞地检查一个通道是否已经关闭

-   *保证没有任何协程会向一个通道发送数据*
-   并发安全地检查此通道是否已经关闭，此检查不会阻塞当前协程

```go
func IsClosed(c chan T) bool {
	select {
	case <-c:
		return true
	default:
	}
	return false
}
```

Pp. 375-380