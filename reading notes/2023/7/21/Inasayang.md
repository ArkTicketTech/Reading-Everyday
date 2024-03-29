内存布局

Go中的类型对齐保证（`alignment guarantee`）

-   为了充分利用CPU指令来达到最佳程序性能，为一个特定类型的值开辟的内存块的起始地址必须为某个整数N的倍数

-   N被称为此类型的值地址对齐保证，或者此类型的对齐保证。也可以说此类型的值的地址保证为N字节对齐

-   每个类型有两个对齐保证

    -   被用做结构体类型的字段类型时的对齐保证称为此类型的字段对齐保证
    -   其它情形的对齐保证称为此类型的一般对齐保证

-   对于类型T，调用`unsafe.Alignof(t)`来获得它的一般对齐保证，其中t为一个T类型的非字段值， 也可以调用`unsafe.Alignof(x.t)`来获得T的字段对齐保证，其中x为一个结构体值并且t为一个类型为T的结构体字段值

-   `unsafe`标准库包中的函数的调用都是在编译时刻估值的

-   在运行时刻，对于类型为T的一个值t，可以调用`reflect.TypeOf(t).Align()`来获得类型T的一般对齐保证， 也可以调用`reflect.TypeOf(t).FieldAlign()`来获得T的字段对齐保证

-   对于官方Go标准编译器（1.20版本），一个类型的一般对齐保证和字段对齐保证总是相等的（gccgo编译器可能不相等）

-   https://golang.google.cn/ref/spec#Size_and_alignment_guarantees

    -   对于任何类型的变量x，`unsafe.Alignof(x)`的结果最小为1
    -   对于结构体类型的变量x，`unsafe.Alignof(x)`的结果为x的所有字段的对齐保证`unsafe.Alignof(x.f)`中的最大值（最小为1）
    -   对于数组类型的变量x，`unsafe.Alignof(x)`的结果和此数组的元素类型的一个变量的对齐保证相等

-   类型种类                   对齐保证（字节数）
    ------                    ------
    bool, uint8, int8         1
    uint16, int16             2
    uint32, int32             4
    float32, complex64        4
    数组                       取决于元素类型
    结构体类型                  取决于各个字段类型
    其它类型                    一个自然字的尺寸

-   一般情况下，不必关心值地址的对齐保证。 除非打算优化一下内存消耗，或者编写跨平台移植性良好的Go代码



类型的尺寸和结构体字节填充（`structure padding`）

-   https://golang.google.cn/ref/spec#Size_and_alignment_guarantees

-   为了满足地址对齐保证要求规则，Go编译器可能会在结构体的相邻字段之间填充一些字节。 这使得一个结构体类型的尺寸并非等于它的各个字段类型尺寸的简单相加之和

-   内置类型int8的对齐保证和尺寸均为1个字节

-   内置类型int16的对齐保证和尺寸均为2个字节

-   内置类型int64的尺寸为8个字节，但它的对齐保证在32位架构上为4个字节，在64位架构上为8个字节

-   举例

    ```
    type T1 struct {
    	a int8
    	//在64位架构上，为了让字段b的地址为8字节对齐，需在这里填充7个字节
    	//在32位架构上，为了让字段b的地址为4字节对齐，需在这里填充3个字节
    	b int64
    	c int16
    	//为了让类型T1的尺寸为T1的对齐保证的倍数，
    	//在64位架构上需在这里填充6个字节
    	//在32位架构上需在这里填充2个字节
    }
    //类型T1的尺寸
    //在64位架构上为24个字节（1+7+8+2+6），
    //在32位架构上为16个字节（1+3+8+2+2）。
    type T2 struct {
    	a int8
    	//为了让字段c的地址为2字节对齐，需在这里填充1个字节。
    	c int16
    	//在64位架构上，为了让字段b的地址为8字节对齐，需在这里填充4个字节。
    	//在32位架构上，不需填充字节即可保证字段b的地址为4字节对齐的。
    	b int64
    }
    // 类型T2的尺寸在64位架构上位16个字节（1+1+2+4+8），
    // 在32位架构上为12个字节（1+1+2+8）。
    ```

    -   类型T1和T2的对齐保证均为它们的各个字段的最大对齐保证。 所以它们的对齐保证和内置类型int64相同，即在32位架构上为4个字节，在64位架构上为8个字节
    -   类型T1和T2尺寸需为它们的对齐保证的倍数，即在32位架构上为4n个字节，在64位架构上为8n个字节

-   有时候一个结构体类型中零尺寸类型的字段可能会影响到此结构体类型的尺寸



Pp. 464-468