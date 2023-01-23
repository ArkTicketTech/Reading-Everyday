# C++ Coroutine
## Introduction
[[Book/C++ Coroutine#^18uz6doipf6|This kind of code is painful to write because you have to break your code up into a bunch ofdifferent functions that, because they are different functions, don’t share local variables]]
* 在 Old C++ 时代，需要注册多个回调函数来完成不同阶段的操作。
* C++ 11 引入了 lambda 函数，可以通过匿名函数来完成上述目的，尽管如此，但是异步事件运行时的顺序易改变，难以把握所有情形。
* C++ 20 引入协程后，极大方便了编写处理事件驱动的代码。

## Tutorial
[[Book/C++ Coroutine#^809cywr0c0x|coroutines are functions that can invoke each other but do not share astack, so can flexibly suspend their execution at any point to enter a different coroutine]]

C++ 协程中的 future 和 promise 术语和 `std::promise` 和 `std::future` 没有任何关系。


### Compiling code using coroutines
编译：
* g++：`g++ -fcoroutines -std=c++20`
* clang++：`clang++ -std=c++20 -stdlib=libc++ -fcoroutines-ts`
* 使用 clang 时，需要引入 `<experimental/coroutine>` 头文件，其命名空间为 `<experimental/coroutine>`



### Coroutine handles
[[Book/C++ Coroutine#^f0z1lu3tioo|the new co_await operator ensures the current state of a functionis bundled up somewhere on the heap and creates a callable object whose invocation continuesexecution of the current function]]

* co_await 创建的可调用对象的类型是 `std::coroutine_handle<>`。
* corotuine handle 表现为 C 指针，其并没有内置的析构器去释放和协程状态相关联的内存。因此，为避免内存泄漏，必须显示的在析构函数中调用`coroutine_handle::destroy` 以析构。
* 一旦 coroutine handle 被析构，再次解引用原 coroutine handle 会产生未定义行为。
* 即使某些时候程序的控制流会不断切入或切出协程，但 corotuine handle 的生命周期和整个协程执行期间一样长。



