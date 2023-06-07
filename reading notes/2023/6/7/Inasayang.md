字符串类型的内部结构定义

```
1| type _string struct {
2|	elements *byte // 引用着底层的字节
3|	len int // 字符串中的字节数
4| }
```

-   一个字符串的内部定义看作为一个字节序列
-   把一个字符串看作是一个元素类型为`byte`的（且元素不可修改的）切片
-   字符串类型都是可比较类型。可以用==和!=比较运算符来比较。也可以用>、<、>=和<=来比较。比较两个字符串值的时候，它们的底层字节将逐一进行比较。如果一个字符串是另一个字符串的前缀，并且另一个字符
    串较长，则另一个字符串为两者中的较大者
-   字符串值的长度也是不可独立被更改的。 一个可寻址的字符串只能通过将赋值来整体修改





字符串编码和`Unicode`码点

-   `Unicode`标准中的基本单位不是字符，而是码点（`code point`）
-   大多数码点实际上就对应着一个字符，但也有少数一些字符是由多个码点组成
-   码点值在Go中用`rune`值来表示
-   码点值的编码方式有很多，比如`UTF-8`编码和`UTF-16`编码等
-   在编译时刻，非法`UTF-8`编码的字符串常量将导致编译失败
-   在运行时刻，Go运行时无法阻止一个字符串是非法`UTF-8`编码的
-   在`UTF-8`编码中，一个码点值可能由1到4个字节组成。 每个英语码点值（均对应一个英语字符）均由一个字节组成，而每个中文码点值（均对应一个中文字符）均由三个字节组成



字符串相关的类型转换

两种新的字符串相关的类型转换规则

-   一个字符串值可以被显式转换为一个字节切片（`byte slice`），反之亦然
-   一个字符串值可以被显式转换为一个码点切片（`rune slice`），反之亦然



在一个从码点切片到字符串的转换中，码点切片中的每个码点值将被`UTF-8`编码为一到四个字节至结果字符串中。 如果一个码点值是一个不合法的`Unicode`码点值，则它将被视为`Unicode`替换字符（码点）值`0xFFFD`（`Unicode replacement character`）。 替换字符值`0xFFFD`将被`UTF-8`编码为三个字节`0xef 0xbf 0xbd`



当一个字符串被转换为一个码点切片时，此字符串中存储的字节序列将被解读为一个一个码点的`UTF-8`编码序列。 非法的`UTF-8`编码字节序列将被转化为`Unicode`替换字符值`0xFFFD`。



当一个字符串被转换为一个字节切片时，结果切片中的底层字节序列是此字符串中存储的字节序列的一份深复制。

同样，当一个字节切片被转换为一个字符串时，此字节切片中的字节序列也将被深复制到结果字符串中。

在这两种转换中，必须使用深复制的原因是字节切片中的字节元素是可修改的，但是字符串中的字节是不可修改的，所以一个字节切片和一个字符串是不能共享底层字节序列的。



在字符串和字节切片之间的转换中

-   非法的UTF-8编码字节序列将被保持原样不变
-   标准编译器做了一些优化，从而使得这些转换在某些情形下将不用深复制



Go并不支持字节切片和码点切片之间的直接转换。用下面列出的方法来实现这样的转换：

-   利用字符串做为中间过渡，相对方便但效率较低，因为需要做两次深复制
-   使用`unicode/utf8`，效率较高，但使用起来不太方便
-   使用`bytes`标准库包中的`Runes`函数，将一个字节切片转换为码点切片。 但此包中没有将码点切片转换为节切片的函数

```
1| package main
2|
3| import (
4| "bytes"
5| "unicode/utf8"
6| )
7|
8| func Runes2Bytes(rs []rune) []byte {
9|	n := 0
10| for _, r := range rs {
11| n += utf8.RuneLen(r)
12| }
13| n, bs := 0, make([]byte, n)
14| for _, r := range rs {
15| n += utf8.EncodeRune(bs[n:], r)
16| }
17| return bs
18| }
19|
20| func main() {
21| s := "颜色感染是一个有趣的游戏。"
22| bs := []byte(s) // string -> []byte
23| s = string(bs) // []byte -> string
24| rs := []rune(s) // string -> []rune
25| s = string(rs) // []rune -> string
26| rs = bytes.Runes(bs) // []byte -> []rune
27| bs = Runes2Bytes(rs) // []rune -> []byte
28| }
```



字符串和字节切片之间的转换的编译器优化

-   一个`for-range`循环中跟随`range`关键字的从字符串到字节切片的转换
-   一个在映射元素读取索引语法中被用做键值的从字节切片到字符串的转换（注意：对修改写入索引语法无效）
-   一个字符串比较表达式中被用做比较值的从字节切片到字符串的转换
-   一个（至少有一个被衔接的字符串值为非空字符串常量的）字符串衔接表达式中的从字节切片到字符串的转换

（艹）

```
6| var str = "world"
7| // 这里，转换[]byte(str)将不需要一个深复制。
8| for i, b := range []byte(str) {
9| 	fmt.Println(i, ":", b)
10|}
```



```
12| key := []byte{'k', 'e', 'y'}
13| m := map[string]string{}
14| // 这个string(key)转换仍然需要深复制。
15| m[string(key)] = "value"
16| // 这里的转换string(key)将不需要一个深复制。
17| // 即使key是一个包级变量，此优化仍然有效。
18| fmt.Println(m[string(key)]) // value
```

在最后一行中，如果在估值`string(key)`的时候有数据竞争的情况，则这行的输出有可能并不是`value`。 但是，无论如何，此行都不会造成恐慌（即使有数据竞争的情况发生）



```
1| package main
2|
3| import "fmt"
4| import "testing"
5|
6| var s string
7| var x = []byte{1023: 'x'}
8| var y = []byte{1023: 'y'}
9|
10| func fc() {
11| // 下面的四个转换都不需要深复制。
12| if string(x) != string(y) {
13| s = (" " + string(x) + string(y))[1:]
14| }
15| }
16|
17| func fd() {
18| // 两个在比较表达式中的转换不需要深复制，
19| // 但两个字符串衔接中的转换仍需要深复制。
20| // 请注意此字符串衔接和fc中的衔接的差别。
21| if string(x) != string(y) {
22| s = string(x) + string(y)
23| }
24| }
25|
26| func main() {
27| fmt.Println(testing.AllocsPerRun(1, fc)) // 1
28| fmt.Println(testing.AllocsPerRun(1, fd)) // 3
29| }
```





关于字符串的比较

两个相等的字符串的比较的时间复杂度取决于它们底层引用着字符串切片的指针是否相等。 如果相等，则对它们的比较的时间复杂度为O(1)，否则时间复杂度为O(n)

```
1| package main
2|
3| import (
4| "fmt"
5| "time"
6| )
7|
8| func main() {
9| bs := make([]byte, 1<<26)
10| s0 := string(bs)
11| s1 := string(bs)
12| s2 := s1
13|
14| // s0、s1和s2是三个相等的字符串。
15| // s0的底层字节序列是bs的一个深复制。
16| // s1的底层字节序列也是bs的一个深复制。
17| // s0和s1底层字节序列为两个不同的字节序列。
18| // s2和s1共享同一个底层字节序列。
19|
20| startTime := time.Now()
21| _ = s0 == s1
22| duration := time.Now().Sub(startTime)
23| fmt.Println("duration for (s0 == s1):", duration)
24|
25| startTime = time.Now()
26| _ = s1 == s2
27| duration = time.Now().Sub(startTime)
28| fmt.Println("duration for (s1 == s2):", duration)
29| }
```



```
duration for (s0 == s1): 10.462075ms
duration for (s1 == s2): 136ns
```

尽量避免比较两个很长的不共享底层字节序列的相等的（或者几乎相等的）字符串



Pp. 182-193