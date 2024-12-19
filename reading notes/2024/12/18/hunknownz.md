Java 内存模型

内存模型主要为了解决可见性和有序性问题。

内存模型规范了 JVM 提供按需禁用缓存和编译优化的方法。

Happens-Before 规则是在约束编译器的优化行为，允许编译器优化，但是优化后的行为要遵守 Happens-Before 规则。

Happends-Before 前 3 项：

1. 在一个线程中（单线程），按照程序顺序，前面的操作 Happens-Before 于后续的任意操作。
2. 对一个 volatile 变量的写操作，Happens-Before 于后续对这个 volatile 变量的读操作。
3. 如果 A Happens-Before B，且 B Happens-Before C，那么 A Happens-Before C。

1. 程序顺序性。
2. volatile。
3. 传递性。
