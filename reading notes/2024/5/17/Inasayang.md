# gotips pp. 80-112

## 31.使⽤跳转标签让`break`和`continue`语句更简洁

使⽤`break`或`continue`后跟标签可以实现任何外层循环的跳转

```go
OuterLoop:
for i, row := range matrix {
    for j, value := range row {
        if value == target {
            fmt.Printf("Found %d at (%d,%d)\n", target, i, j)
            break OuterLoop
        }
    }
}
```

```go
SwitchChoice:
switch userChoice {
case 1:
    fmt.Println("Option 1 selected. Exiting...")
case 2:
    fmt.Println("Option 2 selected, run the loop")
    for i := 0; i < 5; i++ {
        if i == 2 {
            break SwitchChoice
        }
    }
default:
    fmt.Println("Default option. No specific action taken.")
}
```

## 32.Scheduling functions after context cancellation with `context.AfterFunc`

`context.AfterFunc`设置回调函数，在ctx结束后(⽆论因取消还是超时)运⾏

回调函数在⼀个新的goroutine中运⾏，该goroutine在接收到⽗级上下⽂的`ctx.done`通道发送的信号后被触发

如果上下⽂已经取消,则立即运行回调函数

您提供了⼀个`stop`函数，可以阻⽌f运⾏。

## 33. 尽量不要使⽤panic()

略

## 34.以context开头，以options结尾，并且总是⽤error来关闭

略

## 35.转换字符串时优先使⽤`strconv`⽽⾮`fmt`

- `strconv`的函数是为特定的转换任务设计的，这使得它们能⽐更通⽤的`fmt`函数执⾏得更快。
- `fmt.Sprint`函数及其变体需要通过反射来识别其正在处理的类型，并确定如何将其格式化为字符串

## 36.以下划线（_）作为前缀，命名⾮导出的全局变量

略

## 37.使⽤未导出的空结构体作为上下⽂键

```go
// 定义两个空结构体类型
type empty struct{}
type anotherEmpty struct{}
func main() {
    ctx := context.WithValue(context.Background(), empty{}, "user123")
    handleRequest(ctx)
}
func handleRequest(ctx context.Context) {
    fmt.Println(ctx.Value(empty{}))
    fmt.Println(ctx.Value(anotherEmpty{}))
}
```

每个结构体相⽐于其他结构体都是唯⼀的,使⽤未导出（私有）的空结构体,可以避免由其他包引起的任何潜在冲突

也可以使⽤其他类型。例如，⼀个底层类型为int，数值为0的number和⼀个int(0)是不同的

原理归结为`Go`如何⽐较`interface{}`，只有当两个`interface{}`的类型和值都匹配时，它们才相等

为什么会选择使⽤⼀个空的`struct{}`呢？

- ⼀个空结构体不会分配内存，没有字段因⽽不包含数据，但类型仍然可以唯⼀地标识上下⽂值

## 38.使⽤`fmt.Errorf`使你的错误信息清晰明了，不要让它们过于⾚裸

通过`fmt.Errorf`函数配合`%w`符号来实现

使用`%w`包装的错误，可以通过`errors.Is`和`errors.As`进行解包和检查

`errors.As`用于检查并解包特定类型的错误

`errors.Is`用于检查错误链中是否包含特定的错误

`%v`符号只是简单地格式化错误消息，不会保留错误链，因此无法使用`errors.Is`和`errors.As`进行解包和检查

`errors.Join`用于将多个错误合并成一个错误，适用于需要同时处理和传递多个错误的情况。合并后的错误可以使用`errors.Is`进行检查

## 39.避免在循环中使⽤defer，否则可能会导致内存溢出

使⽤匿名函数

```go
for _, file := range files {
    func(filename string) error {
        f, err := os.Open(filename)
        if err != nil {
            // ...
            return err
        }
        defer f.Close()
        return nil
    }(file)
}
```

## 40.在使⽤defer时处理错误以防⽌静默失败

略
