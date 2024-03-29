Go内存模型（Memory Model）

-   确保一个协程中的一些语句一定要在另一个协程的某些语句之后（或者之前）执行

-   不同的CPU架构提供了不同的栅栏（`fence`）指令来防止各种指令执行顺序调整

-   一些编程语言提供对应的函数来在代码中的合适位置插入各种栅栏指令

-   Go内置和标准库并没有提供直接插入各种栅栏指令的途径，栅栏指令被使用在Go中提供的各种高级数据同步技术的实现中

-   一个非缓冲通道上的发送和接收操作是如何保证特定的代码执行顺序的

    ```
    func f3() {
    	var a, b int
    	var c = make(chan bool)
    	go func() {
    		a = 1
    		c <- true
    		if b != 1 {
    			panic("b != 1") // 绝不可能发生
    		}
    	}()
    	go func() {
    		b = 1
    		<-c
    		if a != 1 {
    			panic("a != 1") // 绝不可能发生
    		}
    	}()
    }
    ```

    -   赋值语句`b = 1`肯定在条件`b != 1`被估值之前执行完毕
    -   赋值语句`a = 1`肯定在条件`a != 1`被估值之前执行完毕



​		

```
func f4() {
	var a, b, x, y int	
	c := make(chan bool)
	go func() {
		a = 1
		c <- true
		x = 1
	}()
	go func() {
		b = 1
		<-c
		y = 1
	}()
	// 一个和上面的通道操作不相关的协程。
	// 这是一个不良代码的例子，它造成了很多数据竞争。
	go func() {
		if x == 1 {
			if a != 1 {
				panic("a != 1") // 有可能发生
			}
			if b != 1 {
				panic("b != 1") // 有可能发生
			}
		}
		if y == 1 {
			if a != 1 {
				panic("a != 1") // 有可能发生
			}
			if b != 1 {
				panic("b != 1") // 有可能发生
			}
		}
	}()
}
```

这个例子有点儿懵



Pp. 437-442