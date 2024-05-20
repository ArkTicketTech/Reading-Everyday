# gotips pp. 185-222

## 61. 使⽤内置锁的类型（sync.Mutex嵌⼊）

```go
type Lockable[T any] struct {
    sync.Mutex
    value T
}

func (l *Lockable[T]) Get() T {
    l.Lock()
    defer l.Unlock()

    return l.value
}

func (l *Lockable[T]) Set(v T) {
    l.Lock()
    defer l.Unlock()

    l.value = v
}
```

sync.Mutex 类型无法直接包含或锁定一个泛型类型 T，这可能会导致编译错误或不安全的并发访问

## 62.context.Value不是我们的朋友

略

## 63.避免使⽤time.Sleep()，它不能被context感知且⽆法被中断

略

## 64.让main()函数更清晰并且易于测试

- ⽴即使⽤log.Fatal停⽌程序，defer函数都不会执⾏
- 能够更改参数（例如os.Args）或者修改安装程序的环境，以便在测试时覆盖到不同的场景

```go
func main() {
    if err := run(os.Args[1:]); err != nil {
        log.Fatal(err)
    }
}
func run(args []string) error {
    conf, err := config.Fetch(args)
    if err != nil {
        return fmt.Errorf("failed to fetch config: %w", err)
    }
    db, err := connectDB(conf.DatabaseURL)
    if err != nil {
        return fmt.Errorf("failed to connect to database: %w", err)
    }
    defer db.Close() // This will now be called when run() exits.
    // ...
    return nil
}
```

## 65.在fmt.Errorf中简化你的错误信息

略

## 66.使⽤泛型返回指针

```go
// 定义一个结构体类型 Data
type Data struct {
    Value int
}
// 假设的 getData 函数，返回一个 Data 类型的值
func getData() Data {
    return Data{Value: 42}
}
func main() {
    // 使用内联匿名函数创建指针
    ptr := func(t Data) *Data { return &t }(getData())
    // 输出指针指向的值
    fmt.Println(ptr.Value) // 输出：42
}
```

差点儿没认出来匿名函数...

```go
func Ptr[T any](v T) *T {
    return &v
}
```

## 67.如何处理⻓函数签名

略

## 68.使⽤deadcode⼯具来找到和删除⽆⽤的函数

```go
go install http://golang.org/x/tools/cmd/deadcode@latest
```

<https://go.dev/blog/deadcode>

## 69.通过errgrup管理多个goroutine

```go
$ go get -u golang.org/x/sync

func main() {
    urls := []string{
        "http://example.com",
        "http://example.net",
    }

    var g errgroup.Group

    for _, url := range urls {
        g.Go(func() error {
            return fetch(url)
        })
    }

    if err := g.Wait(); err != nil {
        fmt.Printf("Fetch error: %v\n", err)
    }
}
```

- 使⽤ `g.Wait()`` 等待所有 goroutine 完成，它会返回第⼀个发⽣的错误，⽽不是所有错误

内部实现机制：

- sync.WaitGroup ⽤于等待所有goroutine完成
- sync.Once 确保以线程安全的⽅式捕获第⼀个错误
- ⼀个信号量 chan ⽤于控制同时运⾏的 goroutine 数量（使⽤errg.SetLimit() 设置限制）

## 70.实现⼀个感知context的sleep 函数

```go
func Sleep(ctx context.Context, duration time.Duration) error {
    timer := time.NewTimer(duration)
    defer timer.Stop()
    select {
    case <-ctx.Done():
        return ctx.Err()
    case <-timer.C:
        return nil
    }
}
```
