# gotips pp. 112-149

## 41.将你结构体中的字段按从⼤到⼩的顺序排列

可以使用`https://github.com/dkorunic/betteralign`检测

不能盲目追求对齐，可能会导致伪共享(False Sharing)影响性能

## 42.单点错误处理，降低噪⾳

略

## 43.优雅关闭你的应⽤程序

- 不接收新请求：服务器停⽌接受新的请求
- 完成正在进⾏的任务：等待当前处理的任务达到逻辑上的停⽌点
- 资源清理：释放诸如数据库连接、打开⽂件、⽹络连接等资源

```go
ctx, cancel := signal.NotifyContext(
    context.Background(), os.Interrupt, syscall.SIGTERM)
defer cancel()
server := http.Server{Addr: ":8080"}
g, gCtx := errgroup.WithContext(ctx)
g.Go(func() error { return server.ListenAndServe() })
g.Go(func() error {
    <-gCtx.Done()
    // consider using a timeout here
    return server.Shutdown(context.Background())
})
if err := g.Wait(); err != nil {
    fmt.Printf("exit with: %v\n", err)
}
```

## 44.有意地使⽤Must函数来停⽌程序

略

## 45.始终管理您协程的⽣命周期

略

## 46.避免在`switch`语句的`case`中使⽤`break`,除⾮与标签⼀起使⽤

略

## 47.表驱动测试，测试集和并⾏运⾏测试

略

## 48.避免使⽤全局变量，尤其是可变变量

通过依赖注⼊

## 49.赋予调⽤者决策权

- 与其在函数⾥触发`panic`或打印错误，不如只将错误发送给调⽤者
- 望给予调⽤者权限来控制`goroutine`的⽣命周期

## 50.使结构体不可⽐较

如果⼀个结构体中的每个字段都是可⽐较的,那么该结构体本身也是可⽐较的,这意味着可以直接使⽤`==`和`!=`运算符来⽐较两个结构体

直接⽐较包含浮点数的结构体可能会有问题,理想情况下,浮点数值应该使⽤近似值进⾏⽐较。更希望使⽤⾃定义的`.Equals`⽅法进⾏⽐较

```go
func (p Point) Equals(other Point, tolerance float64) bool {
    return math.Abs(p.X-other.X) < tolerance &&
           math.Abs(p.Y-other.Y) < tolerance
}
```

```go
type Point struct {
    _ [0]func()
    X, Y float64
}
```

`[0]func()`有3个特性

- ⾮导出的
- 零宽度(或⽆成本)
- 不可⽐较: func()是⼀个函数类型,⽽函数在Go中是不可⽐较的

直接使⽤`==`或`!=`⽐较将触发编译时错误:`"invalid operation: a == b (struct containing [0]func() cannot be compared)"`

- 不要将[0]func()放在最后,虽然不占⽤空间,但可能影响结构体⼤⼩

`_ [0]func()`的对齐要求是8字节，会影响结构体末尾的对齐要求,编译器可能会在结构体末尾添加填充字节以确保对齐，导致整体大小变为 24 字节
