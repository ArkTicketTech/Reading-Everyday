Go的主要卖点：节省内存，程序启动快，代码执行速度快和编译速度快

Go的特性：

-   内置并发编程
    -   使用协程（`goroutine`）作为基本的计算单元。轻松创建协程。
    -   使用通道（`channel`）实现协程间的同步和通信。
-   内置映射（`map`）和切片（`slice`）类型
-   支持多态（`polymorphism`）
-   使用接口（`interface`）来实现装盒（`value boxing`）和反射（`reflection`）
-   支持指针
-   支持函数闭包（`closure`）
-   支持方法
-   支持延迟函数调用（`defer`）
-   支持类型内嵌（`type embedding`）
-   支持类型推断（`type deduction or type inference`）
-   内存安全
-   自动垃圾回收
-   良好的代码跨平台性
-   自定义泛型
-   标准库齐全
-   可读性



标准编译器`gc`（`Go compiler`，不是`garbage collection`）。支持跨平台编译。

`gccgo`是`gcc`编译器项目的子项目，使用广泛度不如`gc`。



C/C++/Rust的缺点：

-   编译时间长
-   没有动态语言灵活
-   无内置并发支持（Rust除外）



-   `GOPATH`文件夹中的`pkg`子文件夹用来缓存被本地项目所依赖的Go模块的版本
-   `GOBIN`环境变量用来指定`go install` 子命令产生的Go应用程序二进制可执行文件的存储目录。默认为`GOPATH`文件夹中的`bin`子目录。



Pp. 1-14