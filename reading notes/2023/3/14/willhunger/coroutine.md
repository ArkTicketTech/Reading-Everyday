### Coroutine handles
the new co_await operator ensures the current state of a functionis bundled up somewhere on the heap and creates a callable object whose invocation continuesexecution of the current function

 co_await 的特点：
* co_await operator 必须保证目前函数的运行状态被保存在 heap 中，其会创建一个可调用类型，该可调用类型能恢复执行当前函数，co_await 创建的可调用对象的类型是 `std::coroutine_handle<>`。
* corotuine handle 比较像 C 指针，其并没有内置的析构器去释放和协程状态相关联的内存。因此，为避免内存泄漏，必须显示的在析构函数中调用`coroutine_handle::destroy` 以析构。
* 一旦 coroutine handle 被析构，再次解引用原 coroutine handle 会产生未定义行为。
* 即使某些时候程序的控制流会不断切入或切出协程，但 corotuine handle 的生命周期和整个协程执行期间一样长。

co_wait 的操作行为：
* 当执行 `co_await a` 时，编译器会生成一个 coroutine handle，并将其传递至 `a.await_suspend(corotuine_hanle)` 方法中。另外，类型 `a` 必须为 awaitable object 或 awaiter，即类型 `a` 必须有 `co_await` opeator。


```cpp
#include <chrono>
#include <concepts>
#include <coroutine>
#include <exception>
#include <iostream>
#include <thread>

struct ReturnObject {
    struct promise_type {
        ReturnObject get_return_object() { return {}; }

        std::suspend_never initial_suspend() { return {}; }

        std::suspend_never final_suspend() noexcept { return {}; }

        void unhandled_exception() {}
    };
};

struct Awaiter {
    std::coroutine_handle<> *hp_;

    constexpr bool await_ready() const noexcept { return false; }

    void await_suspend(std::coroutine_handle<> h) {
        std::cout << "start op\n";

        *hp_ = h;

        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    }

    constexpr void await_resume() const noexcept {}
};

ReturnObject counter(std::coroutine_handle<> *continuation_out) {
    Awaiter a{continuation_out};

    for (unsigned i = 0;; ++i) {
        co_await a;

        std::cout << "counter: " << i << std::endl;
    }
}

int main() {
    std::coroutine_handle<> h;

    counter(&h);

    for (int i = 0; i < 3; ++i) {
        std::cout << "In main1 function start\n";

        h();

        std::cout << "In main1 function end\n";

        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }

    h.destroy();
}
```



从上述例子得知：
awaiter 中还有其它方法：
* `await_ready` ，如果其返回 true，代表 co_await 不会中断该函数。
* `await_suspend`，在调用之前，编译器必须将 coroutine handle 所引用的状态存入 heap object 中，该过程的开销较大。
* `await_resume`，上述例子中的 await_resume 返回 void，如果其返回一个值，该值会作为 co_await 表达式的返回值。

此外，coroutine 提供了两类 awaiter，分别为：
* `std::suspend_always` 的 `std::suspend_always::await_ready` 总是返回 false。
* `std::suspend_never` 的 `std::suspend_always::await_ready` 总是返回 true。
* 这两个 awaiter 的其它方法（指 `std::suspend_always::await_suspend` 和 `## std::suspend_always::await_resume` ）都是空。