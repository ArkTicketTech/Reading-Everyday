# gotips pp. 150-185

## 51.避免使⽤init()

略

## 52.针对容器化环境(Kubernetes、Docker等)调整`GOMAXPROCS`

默认情况下，可以并发执⾏⾼达10,000个线程，但实际上并⾏运⾏的线程数量取决于⼀个关键设置`GOMAXPROCS`。
`GOMAXPROCS`决定了可以同时运⾏⽤户级Go代码的系统线程数量上限。
它的默认值与操作系统的逻辑CPU核数是⼀致的(runtime.NumCPU()函数获取)。

Go默认没法⾃⾏识别容器配置的CPU资源限制。依旧会根据宿主机上的CPU核⼼总数进⾏运算，⽽⾮容器分配到的数量。
Go程序可能会尝试使⽤超过其被分配份额的CPU。

- <https://github.com/uber-go/automaxprocs>
- 如果对deployment或pod规范了解，可以直接在环境变量中设置`GOMAXPROCS`，以匹配CPU限制

## 53.枚举从1开始⽤于分类，从0⽤于默认情况

- 从1开始枚举是⼀种策略，确保了零值不会错误地代表⼀个有意义的状态
- 当每个新创建的实例都⾃然⽽然地对应⼀个有意义的初始状态时，从0开始枚举是可取的

决定枚举值从0还是1开始，取决于具体业务需求和安全性考虑

## 54.仅在必要时为客户端定义error(var Err = errors.New)

有的时候直接格式化出⼀个错误类型更有效

## 55.使⽤空字段防⽌结构体⽆键字⾯量

⼤⼩为零的类型的常⻅选择是空结构体（struct{}）和⻓度为零的数组

```go
type Point struct {
    _ struct{}  // This is an anonymous field of an empty struct type
    X float64   // X coordinate
    Y float64   // Y coordinate
}
a := config.Point{X: 1, Y: 2}  // Creating an instance of Point with X=1 and Y=2
```

```go
type noUnkeyed = struct{}
type Point struct {
    noUnkeyed  // 空字段，匿名结构体类型
    X float64
    Y float64
}
```

## 56.简化接⼝并只要求你真正需要的东⻄

略

## 57.Go中的标记枚举

使⽤位掩码或标志枚举

```go
const (
    IsBoolean  BasicInfo = 1 << iota
    IsInteger
    IsUnsigned
    IsFloat
    IsComplex
    IsString
    IsUntyped

    IsOrdered  = IsInteger | IsFloat | IsString
    IsNumeric  = IsInteger | IsFloat | IsComplex
    IsConstType = IsBoolean | IsNumeric | IsString
)
```

利⽤位或运算符,表明同时拥有几种属性

## 58.将互斥锁放在保护的数据附近

略

## 59.如果不需要使⽤某个参数，删除它或是显式地忽略它

```go
func (sd Downloader) FetchFile(url string, _ string) error {
    err := sd.file.Download(url)
    if err != nil {
        return err
    }
    return nil
}
```

有时必须遵循特定的模式，⽐如遵循接⼝定义或者特定的函数定义

## 60.sync.Once是执⾏单次操作的最佳⽅式

⼀个sync.Once会跟踪两件事

- ⼀个原⼦计数器(或标志),有0和1两个值
  - 调⽤时，⾸先查看原⼦计数器。如果计数器为0，则表示函数尚未执⾏
  - 这个快速通道的存在是为了当函数已经执⾏过时，后续的调⽤可以快速跳过并⽆需等待
- ⼀个⽤于保护慢路径的互斥锁
  - 若计数器不为1，则会触发慢速路径。sync.Once 会进⼊慢速模式并调⽤ doSlow(f)

```go
func (o *Once) doSlow(f func()) {
    o.m.Lock()
    defer o.m.Unlock()

    if atomic.LoadUint32(&o.done) == 0 {
        defer atomic.StoreUint32(&o.done, 1)
        f()
    }
}
```
