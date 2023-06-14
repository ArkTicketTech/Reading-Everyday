尽管Go 编译器产生的是本地可执行代码， 这些代码仍旧运行在GO 的runtime (这部分的代码可以在 runtime包中找到)当中。 
这个runtime 类似java和.NET语言所用到的虚拟机， 它负责管理包括内存分配、垃圾回收、栈处理、goroutine、channel、 切片（slice）、map 和反射（reflection）等等。

runtime主要由C语言编写， 并且是每个GO 包的最顶级包。  可以在目录 $GOROOT/src/runtime中找到相关内容。

垃圾回收器 GO拥有简单却高效的标记。

感觉这个runtime类似于GO的内核、虚拟机，做内存分配，垃圾回收、goroutine， 切片等操作。 有点像php的zend 引擎一部分。
