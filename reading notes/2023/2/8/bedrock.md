5.1.2 移位操作符

左移几位，值左边的几位被丢弃，右边的几位补零

右移几位，右边的几位被丢弃，假如是逻辑移位，左边的几位补零，假如是算术移位，符号位为0，则左边几位补零，符号位为1，则左边几位补1。

逻辑左移，与算术左移是相同的。

C语言标准说明了无符号值执行的所有移位都是逻辑移位，但是对于有符号值，到底是采用逻辑移位，还是算术移位，是取决于编译器的。因此有符号数的右移是不可移植的。

对于这样的移位，应该小心 a << -5，因为这种操作是未定义的，所以它也是由编译器决定的。
