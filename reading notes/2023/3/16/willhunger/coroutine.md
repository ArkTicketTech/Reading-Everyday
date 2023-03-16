### The coroutine return object

协程函数的 return type 限制如下：协程函数的 return type R 中必须存在一个 `R::promise_type`，其中，`R::promise_type` 的限制如下：
* `R::promise_type` 必须包含 `R get_return_object()` ，其返回值即为协程函数的返回值。该返回值通常被称为 `future`。

上节例子中将 `coroutine_handle*` 指针传递给协程函数 `counter()`，本节例子直接使用从 `counter()` 的返回值更好。前一种方法是我们手动传递 coroutine handle 到协程函数中，后一种方法直接调用 `get_return_object()` 函数拿到。

* `A std::coroutine_handle<T>` for any type T can be implicitly converted to a  
`std::coroutine_handle<void>`. Either type can be invoked to resume the corou-  
tine with the same effect. 
* the non-void types allow you to convert back and  forth between a coroutine handle and the promise_type sitting in the coroutine state.  Specifically, within the promise type, we can get the coroutine handle using the static method coroutine_handle::from_pomise. (`std::coroutine_handle<promise_type>::from_promise(*this)`)

```cpp
#include <chrono>
#include <concepts>
#include <coroutine>
#include <exception>
#include <iostream>
#include <thread>

struct ReturnObject2 {
    struct promise_type {
        ReturnObject2 get_return_object() {
            return {
                // Uses C++20 designated initializer syntax
                .h_ = std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void unhandled_exception() {}
    };
    std::coroutine_handle<promise_type> h_;
    operator std::coroutine_handle<promise_type>() const { return h_; }
    // A coroutine_handle<promise_type> converts to coroutine_handle<>
    operator std::coroutine_handle<>() const { return h_; }
};

ReturnObject2 counter2() {
    for (unsigned i = 0;; ++i) {
        co_await std::suspend_always{};
        std::cout << "counter2: " << i << std::endl;
    }
}

int main() {
    std::coroutine_handle<> h = counter2();
    for (int i = 0; i < 3; ++i) {
        std::cout << "In main2 function\n";
        h();
    }
    h.destroy();
}
```

本节例子特点：
1. 不再使用 awaiter 去存储 coroutine handle，使用 co_await std::suspend_always{}。
2. coroutine handle 更像是 C pointer，而不像是对象。从例子中可以看到，包含 `ReturnObject2::h_` 的对象已经析构了，但是已经拷贝给了 h，因此需要手动调用 `h.destroy()`。