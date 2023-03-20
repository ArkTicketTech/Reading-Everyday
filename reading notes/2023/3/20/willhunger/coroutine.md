### The co_yield operator
`co_yield i` 等价于 `co_await promise.yield_value(i)` ，使用 `co_yield` 后，不用拿到 promise object 再进行相关操作。

```cpp
#include <chrono>
#include <concepts>
#include <coroutine>
#include <exception>
#include <iostream>
#include <thread>

struct ReturnObject4 {
    struct promise_type {
        unsigned value_;
        ReturnObject4 get_return_object() {
            return {
                .h_ = std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void unhandled_exception() {}
        std::suspend_always yield_value(unsigned value) {
            value_ = value;
            return {};
        }
    };
    std::coroutine_handle<promise_type> h_;
};

ReturnObject4 counter4() {
    for (unsigned i = 0;; ++i)
        co_yield i;  // co yield i => co_await promise.yield_value(i)
}

int main() {
    auto h = counter4().h_;
    auto &promise = h.promise();
    for (int i = 0; i < 3; ++i) {
        std::cout << "counter4: " << promise.value_ << std::endl;
        h();
    }
    h.destroy();
}
```
