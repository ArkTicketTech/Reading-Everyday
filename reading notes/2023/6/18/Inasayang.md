非类型安全指针

-   相对于C指针，Go指针有很多限制
    -   Go指针不支持算术运算，并且对于任意两个指针值，很可能它们不能转换到对方的类型

-   关于`unsafe`标准库包
    -   非类型安全指针`unsafe.Pointer`被声明定义为
        -   `type Pointer *ArbitraryType` 
        -   这里的`ArbitraryType`仅仅是暗示`unsafe.Pointer`类型值可以被转换为任意类型安全指针（反之亦然）。`unsafe.Pointer`类似于C语言中的`void*`
    -   非类型安全指针是指底层类型为`unsafe.Pointer`的类型
    -   非类型安全指针的零值也使用预声明的`nil`标识符来表示
    -   Go 1.17之前，`unsafe`标准库包只提供了三个函数
        -   `func Alignof(variable ArbitraryType) uintptr`
            -   用来取得一个值在内存中的地址对齐保证（`address alignment guarantee`）
            -   注意，同一个类型的值做为结构体字段和非结构体字段时地址对齐保证可能是不同的
            -   对于目前的标准编译器，同一个类型的值做为结构体字段和非结构体字段时的地址对齐保证总是相同的。 `gccgo`编译器对这两种情形是区别对待的
        -   `func Offsetof(selector ArbitraryType) uintptr`
            -   用来取得一个结构体值的某个字段的地址相对于此结构体值的地址的偏移
            -   在一个程序中，对于同一个结构体类型的不同值的对应相同字段，此函数的返回值总是相同的
        -   `func Sizeof(variable ArbitraryType) uintptr`
            -   用来取得一个值的尺寸（即此值的类型的尺寸)
            -   在一个程序中，对于同一个类型的不同值，此函数的返回值总是相同的
        -   三个函数的返回值的类型均为内置类型`uintptr`
        -   `uintptr`类型的值可以转换为非类型安全指针（反之亦然）
        -   `uintptr`类型的值可以转换为非类型安全指针（反之亦然）
        -   传递给`Offsetof`函数的实参必须为一个字段选择器形式`value.field`。 此选择器可以表示一个内嵌字段，但此选择器的路径中不能包含指针类型的隐式字段
    -   Go 1.17引入了一个新类型和两个新函数
        -   新类型为`IntegerType`
            -   此类型不代表着一个具体类型，它只是表示任意整数类型
                -   `type IntegerType int` 
                -   `func Add(ptr Pointer, len IntegerType) Pointer`
                    -   在一个（非安全）指针表示的地址上添加一个偏移量，然后返回表示新地址的一个指针
                -   `func Slice(ptr *ArbitraryType, len IntegerType) []ArbitraryType`
                    -   从一个任意（安全）指针派生出一个指定长度的切片
    -   Go 1.20进一步引入了三个函数
        -   `func String(ptr *byte, len IntegerType) string`
            -   从一个任意（安全）byte指针派生出一个指定长度的字符串
        -   `func StringData(str string) *byte`
            -   用来获取一个字符串底层字节序列中的第一个byte的指针
        -   `func SliceData(slice []ArbitraryType) *ArbitraryType`
            -   用来获取一个切片底层元素序列中的第一个元素的指针



Pp. 264-268