

一个包可以在一个源文件里被引入多次

-   每次的引入名称必须不同
-   相同的包引入引用着同一个包实例

```
import "fmt"
import "io"
import inout "io"

func main() {
	fmt.Println(&inout.EOF == &io.EOF) // true
}
```



switch流程控制代码块中的数字常量case表达式不能重复，但是布尔常量case表达式可以重复(字符串没试过)

```
switch 123 {
case 123:
case 123: // error: duplicate case
}
```

此行为依赖于编译器。事实上，标准编译器同样不允许重复的字符串case表达式，但是gccgo编译器却允许



switch流程控制代码块里的switch表达式总是被估值为类型确定值

```
switch 123 {
case int64(123): // error: 类型不匹配
case uint32(789): // error: 类型不匹配
}
```

表达式123被视为一个int值，而不是一个类型不确定的整数



switch流程控制代码块中的switch表达式的缺省默认值为类型确定值true（其类型为预声明类型bool）

```

switch { // <=> switch true {
case true: fmt.Println("true")
case false: fmt.Println("false")
}
```



嵌套的延迟函数调用可以修改外层函数的返回结果

```
func F() (r int) {
	defer func() {
		r = 789
	}()
	return 123 // <=> r = 123; return
}
func main() {
	fmt.Println(F()) // 789
}
```



Pp. 517-522