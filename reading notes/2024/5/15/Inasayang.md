# gotips pp. 28-

## 16.不要通过返回`-1`或者`nil`来表示错误

略

## 17.理解“尽快返回、尽早返回”，避免代码嵌套

- 提前处理错误
- 使⽤`return`,`break`,`continue`等等语句停⽌当前操作的执⾏

## 18.在使⽤者的包中定义接⼝，⽽不是提供者的包中定义

- 在使⽤者的包中定义接⼝，⽽不是提供者的包中定义
  - 接⼝应该由使⽤者（使⽤这个接⼝的代码）⽽不是提供者（实现这些接⼝的代
码）来定义
- 在提供者的包中使⽤具体类型作为返回值
- 避免过早定义接⼝

感觉不是很有说服力

## 19.除⾮出于⽂档说明需要，否则避免使⽤命名结果

略

## 20.传递值，⽽不是指针

略

## 21.定义⽅法时，优先使⽤指针作为接收器(receiver)

```go
type SafeCounter struct {
    mu    sync.Mutex
    count int
}
func (c SafeCounter) Inc() {
    c.mu.Lock()
    c.count++
    c.mu.Unlock()
}
func (c SafeCounter) Value() int {
    c.mu.Lock()
    defer c.mu.Unlock()
    return c.count
}
```

这样做会导致每次方法调用时都会复制结构体及其锁

## 22.使⽤结构体或变⻓参数简化函数签名

略

## 23.省略 getter ⽅法的'Get'前缀

略

## 24.避免命名中的重复

略

## 25. 在`goroutines`之间进⾏信号传递时，使⽤`chan struct{}`⽽不是`chan bool`

略

## 26.使⽤空标识符（_）明确忽略值，⽽不是⽆声地忽略它们

略

## 27.原地过滤

是利⽤原始切⽚的底层数组, 在原地切⽚过滤

```go
filtered := numbers[:0]
for _, num := range numbers {
    if isOdd(num) {
        filtered = append(filtered, num)
    }
}
```

这种其实会导致numbers得不到回收的...

## 28.将多个if-else语句转换为switch

略

## 29.避免使⽤`context.Background`，使你的协程具备承诺性

承诺性（promisable）指的是协程运⾏的最终状态应该是确定的，⽽不是⽆期限地⼀直运⾏下去，这就像协程给了使⽤⽅⼀个承诺：我要么执⾏成功，要么因为超时等原因取消执⾏，但最终在有限时间内⼀定会有⼀个明确的状态。

## 30.使⽤`context.WithoutCancel()`继续上下⽂操作

当⽗上下⽂被取消时,它的所有⼦上下⽂也会被取消。

如果创建新的上下⽂，则缺少原始事件上下⽂中的值。
