### 先有编译器还是先有语言？

先设计好语言的规则，然后编写能够识别这套规则的编译器

### 编译型语言和解释型语言的区别？
> 在CPU眼里只看得到脚本解释器，而这些脚本中的代码，CPU从来就不知道有它们的存在。这些脚本代码看似在按照开发人员的逻辑执行，本质上是脚本解释器在时时分析这个脚本

> 编译型语言编译出来的程序，运行时本身就是一个进程。它是由操作系统直接调用的

### 大端序和小端序
  - 小端字节序是数值的低字节放在内存的低地址处，数值的高字节放在内存的高地址。
  - 大端字节序是数值的低字节放在内存的高地址处，数值的高字节放在内存的低地址。

对比：
 - 小端：因为低位在低字节，强制转换数据型时不需要再调整字节了。
 - 大端：有符号数，其字节最高位不仅表示数值本身，还起到了符号的作用。符号位固定为第一字节，也就是最高位占据最低地址，符号直接可以取出来，容易判断正负
