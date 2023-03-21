### The promise object
由于 promise_type 对象中包含了 coroutine state ，因此可以在 promise_type 中加入一个 `value_` 成员用于在协程函数和主函数中传递值。那如何从主函数中获取到 promise_type 呢？可以将在 main 函数中的拿 `std::coroutine_handle<>`  转为拿`std::coroutine_handle<ReturnObject3::promise_type>`，然后通过 coroutine handle 的 `promise()` 方法拿到我们所需的 `promise_type&`。

how can a coroutine obtain its own promise object?
* 和例子 1 类似，`co_await` 在一个自定义的 awaiter 上能拿拿到 promise object。
* 本节例子中的 `Awaiter::await_suspend` 允许返回 bool 值，且默认返回 false；此时，这意味着协程函数并不会真正挂起（除非第一个 await_ready 返回了 false 或者是 await_suspend 的返回类型为 bool 且总返回 ture)。
* 为了返回 promise object 的地址，加入了 await_resume 方法。
* 此外，await_suspend 除了返回 void 和 bool 外，还可以返回 coroutine handle，返回后的 handle 会立马让协程函数恢复执行，但存在效率问题。

```cpp
#include <chrono>
#include <concepts>
#include <coroutine>
#include <exception>
#include <iostream>
#include <thread>

template <typename PromiseType>
struct GetPromise {
    PromiseType *p_;
    bool await_ready() { return false; }  // says yes call await_suspend
    bool await_suspend(std::coroutine_handle<PromiseType> h) {
        p_ = &h.promise();
        return false;  // says no don't suspend coroutine after all
    }
    PromiseType *await_resume() { return p_; }
};

struct ReturnObject3 {
    struct promise_type {
        unsigned value_;
        ReturnObject3 get_return_object() {
            return ReturnObject3{
                .h_ = std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void unhandled_exception() {}
    };
    std::coroutine_handle<promise_type> h_;
    operator std::coroutine_handle<promise_type>() const { return h_; }
};

ReturnObject3 counter3() {
    auto pp = co_await GetPromise<ReturnObject3::promise_type>{};
    for (unsigned i = 0;; ++i) {
        pp->value_ = i;
        co_await std::suspend_always{};
    }
}

int main() {
    std::coroutine_handle<ReturnObject3::promise_type> h = counter3();
    ReturnObject3::promise_type &promise = h.promise();
    for (int i = 0; i < 3; ++i) {
        std::cout << "counter3: " << promise.value_ << std::endl;
        h();
    }
    h.destroy();

    return 0;
}
```

上述例子除了返回值类型外，还可以返回 counter3 中的指针类型。原因如下：coroutine state object 是存储在堆内存中的，故该部分内存能在协程和主函数中访问到。