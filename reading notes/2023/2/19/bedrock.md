- switch 语句也可以用一个简单的语句来做初始化，用分号分隔。
- 每一个 case 就是一个分支，分支条件为Ture时才会执行，case后的条件表达式不用括号包裹
- go 语言中，对 switch 的case 从上到下逐一进行判断，一旦满足条件，就立即执行对应的分支并返回，其余分支不做判断，即 go 语言中的 switch的 case 后面自带 break
- 假如确实要执行下一个紧跟的 case，那么需要使用 fallthrough 关键字

```go
package main

import (
        "fmt"
)

func main() {
        switch j := 1; j {
        case 1:
                fallthrough
        case 2:
                fmt.Println("1")
        default:
                fmt.Println("没有匹配")
        }
}
```
