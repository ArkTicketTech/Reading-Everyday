#### call指令


CPU 前进的方向永远是 CS：IP 这两个寄存器。

call、ret、jmp 都是此类指令，它们在原理上是修改寄存器 CS 和 IP 的值，将 CPU 导向新的位置。

- ret（return）指令的功能是在**栈顶（寄存器 ss：sp 所指向的地址）弹出 2 字节**的内容来替换 IP 寄存器

- retf（return far）是从栈顶取得 4 字节，栈顶处的 2 字节用来替换 IP 寄存器，另外的 2 字节用来替换 CS 寄存器。
- 小结一下，ret 和 retf 的区别便是 ret 用于近返回，retf 用于远返回

#### ret指令

call 指令调用函数有四种方式。

1. 16 位实模式相对近调用

   - 何谓近？call 指令所调用目标函数和当前代码段是同一个段，即在同一个 64KB 的空间内，所以只给 出段内偏移地址就好了，不用给出段基址。

   - 何谓相对？由于是在同一个代码段中，所以只要给出目标函数的相对地址即可。
