### s=为什么虚拟内存要分段
首先说重定位
如果不分段，那么访问内存的时候，每个程序都需要操作绝对地址，这个地址绝对不能出错，否则就会破坏系统。
如果分段了，那么每个程序只需要记住自己每个段的起始地址，然后程序中使用起始地址+偏移量这种方式，因此如果需要改变程序的地址，只需要修改段的起始地址即可

再说地址空间
> 程序分段又是为了将大内存分成可以访问的小段，通过这样变通的方法便能够访问到所有内存了

### 代码段、数据段这些分段，与虚拟内存的分段是一个概念吗？
程序并不一定必须分段才能运行，只是为了运行起来更加方便，增强隔离性。
