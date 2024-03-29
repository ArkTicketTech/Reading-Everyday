值尺寸（`value size`）

-   一个值的尺寸表示此值的直接部分在内存中占用多少个字节，它的间接部分对它的尺寸没有贡献
-   如果两个值的类型为同一种类的类型，并且它们的类型的种类不为字符串、接口、数组和结构体，则这两个值的尺寸总是相等的
-   一个数组类型的尺寸取决于它的元素类型的尺寸和它的长度。它的尺寸为它的元素类型的尺寸和它的长度的乘积
-   一个结构体类型的尺寸取决于它的各个字段的类型尺寸和这些字段的排列顺序
    -   为了程序执行性能，编译器保证某些类型的值在内存中存放时必须满足特定的内存地址对齐要求
    -   地址对齐可能会造成相邻的两个字段之间在内存中被插入填充一些多余的字节
    -   一个结构体类型的尺寸必定不小于（常常会大于）此结构体类型的各个字段的类型尺寸之和



一个`word`表示一个原生字。在32位系统架构中，一个`word`为4个字节；而在64位系统架构中，一个`word`为8个字节

布尔： 1 `byte`

`int8`,`uint8(byte)`：1 `byte`

`int16`,`uint16`：2 `bytes`

`int32(rune)`,`uint32`,`float32`：4 `bytes`

`int64`,`uint64`,`float64`,`complex64`：8 `bytes`

`complex128`：16 `bytes`

`int`，`uint`，`uintptr`：1`word`

字符串： 2 `words`

指针和非类型安全指针： 1`word`

切片：3 `words`

映射： 1 `word`

通道： 1 `word`

函数： 1 `word`

接口：2 `words`

结构体：所有字段尺寸之和 +所有填充的字节数

数组：元素类型的尺寸 *长度





Pp. 349-353