# gotips pp. 3-10

## 1.函数执行时间

函数体A中第一行加上defer函数B，B函数传入当前时间，B函数体中使用`time.Since()`函数返回`duration`

## 2.多阶段defer

函数体A中第一行加上defer函数B，函数B返回函数C

## 3.预分配切⽚以提⾼性能

略

## 4.将切片解析为数组

v1.20

```go
a := []int{0, 1, 2, 3, 4, 5}
b := [3]int(a[0:3])
```

v1.17

```go
a := []int{0, 1, 2, 3, 4, 5}
b := *(*[3]int)(a[0:3])
```

## 5.方法链

略

## 6.下划线导入

略

## 8.包裹错误

使⽤`errors.Join()`

```go
if err != nil {
    return errors.Join(err, errors.New("error from Func2"))
}
```

## 9.编译时检查接口

略

## 10.避免裸露参数

略

## 11. 数字分隔符

略

## 12.使⽤crypto/rand⽣成密钥，避免使⽤math/rand

```go
buf := make([]byte, 16)
_, err := rand.Read(buf)
if err != nil {
    panic(err)
}
return fmt.Sprintf("%x", buf)
```

## 13.使⽤空切⽚还是更好的NIL切⽚

`var t []int`等价于`nil`,没有指向任何的底层数组,没有分配任何的内存

`t := []int{}`不是`nil`, 指向⼀个底层的数组，但这数组没有包含任何的元素,分配了很⼩的内存去指向⼀个空数组

更倾向于使⽤nil切⽚的⽅式

特殊情况,在使⽤JSON的时候, nil切⽚编码成JSON后的值是`null`, ⽽空切⽚编码成JSON后的值是⼀个空的JSON数组`[]`。

## 14.错误信息不要⼤写或者以标点结尾

略

## 15.什么时候使⽤空⽩导⼊和点导⼊？

空白导入

- 通常在 main 包中使⽤。
- 在那些需要引⼊副作⽤才能正确运⾏的测试中使⽤

点导入

- 处理循环依赖

有点儿怪!!!
