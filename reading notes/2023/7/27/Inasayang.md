赋值规则

-   赋值可以看作是隐式类型转换
-   赋值语句中的目标值必须为一个可寻址的值、一个映射元素表达式或者一个空标识符
-   在一个赋值中，源值被复制给了目标值。精确地说，源值的直接部分被复制给了目标值
-   函数传参和结果返回其实都是赋值

值比较规则

-   两个值是否可以比较取决于其中一个值是否可以隐式转换为另一个值的类型

-   尽管切片/映射/函数类型为不可比较类型，但是它们的值可以和类型不确定的预声明nil标识符比较

-   两个类型不确定的布尔值可以相互比较(看了半天才看懂)

    ```
    // Untyped boolean constants
        const a = true
        const b = false
    ```

-   两个类型不确定的数字值可以相互比较
-   两个类型不确定的字符串值可以相互比较
-   两个类型不确定的nil值不能相互比较
-   动态类型均为同一个不可比较类型的两个接口值的比较将产生一个恐慌

```
func main() {
	type T struct {
        a interface{}
        b int
    }
	var x interface{} = []int{}
	var y = T{a: x}
	var z = [3]T{{a: y}}
	// 这三个比较均会产生一个恐慌。
	_ = x == x
	_ = y == y
	_ = z == z
}
```

Pp. 497-500