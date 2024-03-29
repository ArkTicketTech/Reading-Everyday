通用寄存器

| 寄存器 | 助记名称                            | 功能描述                                                     |
| ------ | ----------------------------------- | ------------------------------------------------------------ |
| ax     | 累加器（accumulator）               | 使用频度最高，常用于算术运算、逻辑运算、保存与外设输入输出的数据 |
| bx     | 基址寄存器（base）                  | 常用来存储内存地址，用此地址作为基址，用来遍历一片内存区域   |
| cx     | 计数器（counter）                   | 顾名思义，计数器的作用就是计数，所以常用于循环指令中的循环次数 |
| dx     | 数据寄存器（data）                  | 可用于存放数据，通常情况下只用于保存外设控制器的端口号地址   |
| si     | 源变址寄存器（source index）        | 常用于字符串操作中的数据源地址，即被传送的数据在哪里。通常需要与其他指 令配合使用，如批量数据传送指令族 movs[bwd] |
| di     | 目的变址寄存器（destination index） | 和 si 一样，常用于字符串操作。但 di 是用于数据的目的地址，即数据被传送到哪里 |
| sp     | 栈指针寄存器（stack pointer）       | 其段基址是 SS，用来指向栈顶。随着栈中数据的进出，push 和 pop 这两个对栈操 作的指令会修改 sp 的值 |
| bp     | 基址指针（base pointer）            | 访问栈有两种方式，一种是用 push 和 pop 指令操作栈，sp 指针的值会自动更新，但我 们只能获取栈顶 sp 指针指向的数据。很多时候，我们需要读写在栈底和栈顶之间的数 据，处理器为了让开发人员方便控制栈中数据，还提供了把栈当成数据段来访问的方 式，即提供了寄存器 bp，所以 bp 默认的段寄存器就是 SS，可通过 SS：bp 的方式把栈 当成普通的数据段来访问，只不过 bp 不像 sp 那样随 push、pop 自动改变 |

#### 实模式下内存分段的由来

实模式的“实”体现在：**程序中用到的地址都是真实的物理地址，“段基址：段内偏移”产生的逻辑 地址就是物理地址**，也就是程序员看到的完全是真实的内存。

