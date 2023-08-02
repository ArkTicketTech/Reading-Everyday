从一个nil切片中派生子切片是允许的，只要子切片表达式中使用的所有索引都为零，则不会有恐慌产生，结果子切片同样是一个nil切片

```
func main() {
	var x []int // nil
	a := x[:]
	b := x[0:0]
	c := x[:0:0]
	// 下一行将打印出三个true。
	fmt.Println(a == nil, b == nil, c == nil)
}
```

用range遍历nil映射或者nil切片是没问题的，这属于空操作

```
func main() {
	var s []int // nil
	for range s {
	}
	var m map[string]int // nil
	for range m {
	}
}
```

用range遍历nil数组指针时，如果忽略或省略第二个迭代变量，则此遍历是没问题的。遍历中的循环步数为相应数组类型的长度

```
var a *[5]int // nil
for i, _ := range a {
	fmt.Print(i)
}
```

切片的长度和容量可以被单独修改

-   可以通过反射途径单独修改一个切片的长度或者容量

切片和数组组合字面量中的索引必须是非负常量

```
var k = 1
var x = [2]int{k: 1} // error: 索引必须为一个常量
var y = []int{k: 1} // error: 索引必须为一个常量
```

-   映射组合字面量中的键值不必为常量

切片/数组/映射组合字面量的常量索引和键值不能重复

不可寻址的数组的元素依旧是不可寻址的，但是不可寻址的切片的元素总是可寻址的

```
// 取不可寻址的切片的元素的地址是没问题的
_ = &[]int{1}[0]
// error: 不能取不可寻址的数组的元素的地址
_ = &[5]int{}[0]
// 修改不可寻址的切片的元素是没问题的
[]int{1,2,3}[1] = 9
// error: 不能修改不可寻址的数组的元素
[3]int{1,2,3}[1] = 9
```

可以从不可寻址的切片派生子切片，但是不能从不可寻址的数组派生子切片

```
// 下面几行编译没问题。
_ = []int{6, 7, 8, 9}[1:3]
var ms = map[string][]int{"abc": {0, 1, 2, 3}}
_ = ms["abc"][1:3]
// 下面几行将编译失败，因为不可从不可寻址的数组派生切片。
/*
_ = [...]int{6, 7, 8, 9}[1:3] // error
var ma = map[string][4]int{"abc": {0, 1, 2, 3}}
_ = ma["abc"][1:3] // error
*/
```

把以NaN做为键值的条目放如映射就宛如把条目放入黑洞一样

-   原因是NaN != NaN
-   可以在for-range循环中被找到

字符串转换为byte切片或rune切片后的结果切片的容量可能会大于长度

-   不应该假设结果切片的长度和容量总是相等的

```
s := "a"
x := []byte(s) // len(s) == 1
fmt.Println(cap([]byte(s))) // 32
fmt.Println(cap(x)) // 8
fmt.Println(x)
```

-   如果最后一个fmt.Println行被删除，在其前面的两行会打印相同的值32；否则，一个打印32，一个打印8
-   如果我们假设结果切片的长度和容量总是相等，就可能写出一些有bug的代码
    （https://github.com/go101/go101/wiki/The-capacity-of-the-result-byte-%28or-rune%29-slice-of-a-conversion-from-a-string-is-undefined）（Mark）



对于切片s，循环`for i = range s {...}`并不等价于循环`for i = 0; i < len(s); i++{...}`(Mark)

```
var i int
func fa(s []int, n int) int {
	i = n
	for i = 0; i < len(s); i++ {}
	return i
}
func fb(s []int, n int) int {
	i = n
	for i = range s {}
	return i
}
func main() {
	s := []int{2, 3, 5, 7, 11, 13}
	fmt.Println(fa(s, -1), fb(s, -1)) // 6 5
	s = nil
	fmt.Println(fa(s, -1), fb(s, -1)) // 0 -1
}
```





Pp. 530-535