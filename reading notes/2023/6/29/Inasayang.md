`select-case`流程控制代码块中的表达式估值顺序

-   各个`case`关键值后跟随的所有通道操作中的通道表达式和所有通道发送操作中的发送值表达式都将被按照它们在代码中的出现次序（从上到下从左到右）估值一次
-   以通道接收操作做为源值的赋值语句中的目标值表达式只有在此通道接收操作被选中之后才会被估值

​	例子

​	表达式`*fptr("aaa")`将永不会得到估值，因为它对应的通道接收操作`<-fchan("bbb", nil)`是个不可能被选中的阻塞操作

```
func main() {
	c := make(chan int, 1)
	c <- 0
	fchan := func(info string, c chan int) chan int {
		fmt.Println(info)
		return c
	}
	fptr := func(info string) *int {
		fmt.Println(info)
		return new(int)
	}
	select {
		case *fptr("aaa") = <-fchan("bbb", nil): // blocking
		case *fptr("ccc") = <-fchan("ddd", c): // non-blocking
		case fchan("eee", nil) <- *fptr("fff"): // blocking
		case fchan("ggg", nil) <- *fptr("hhh"): // blocking
	}
}
```

```
bbb
ddd
eee
fff
ggg
hhh
ccc
```

表达式`*fptr("ccc")`是最后一个被估值的表达式，它在对应的数据接收操作`<-fchan("ddd", c)`被选中之后才会进行估值

Pp. 340-349