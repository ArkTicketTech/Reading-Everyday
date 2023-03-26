### Generic generator example
一旦 coroutine 被停止，再也无法等到原来的协程函数返回。

为了 rethrow 协程中的异常到主函数中，我们可以调用 `unhandled_exception()`，将`std::current_exception();` 存储到  promise object 中的 `std::exception_ptr` ，当 `std::exception_ptr` 非空时，协程函数可以调用 `std::rethrow_exception` 将异常传递到主函数中。

上述例子中还有一个问题：协程函数在被第一次 `co_await` 之前就被计算了第一个 0 值。但是也有原因需要将第一个返回值的计算推迟到第一次协程挂起时，原因如下：
1. 协程中的计算开销较大，这种情况下需要节省协程计算的开销（例如，另外一个协程中产生了异常，这次计算就没有必要了）。
2. 在需要手动析构 coroutine handle 的情况下，如果协程在第一次挂起前抛出异常，那么不好处理该异常。如下例子所示：
	* 如果 mkCoroutineB() 在第一次挂起前抛出了异常，那么该例子中的 mkCoroutineA() 将永远无法被销毁。
	* 为了解决该问题，可以将 `initial_suspend()` 设置为永远返回 `std::suspend_always`，这样 mkCoroutineB() 一开始就会被挂起。
```cpp
void f() {
    std::vector<std::coroutine_handle<>> coros = {mkCoroutineA(),
                                                  mkCoroutineB()};
    try {
        for (int i = 0; i < 3; ++i)
            for (auto &c : coros)
                if (!c.done()) c();
    } catch (...) {
        for (auto &c : coros) c.destroy();
        throw;
    }
    for (auto &c : coros) c.destroy();
}
```

本节例子：

```cpp
#include <chrono>
#include <concepts>
#include <coroutine>
#include <exception>
#include <iostream>
#include <thread>

template <typename T>
struct Generator {
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;
    struct promise_type {
        T value_;
        std::exception_ptr exception_;
        Generator get_return_object() {
            return Generator(handle_type::from_promise(*this));
        }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() { exception_ = std::current_exception(); }
        template <std::convertible_to<T> From>  // C++20 concept
        std::suspend_always yield_value(From &&from) {
            value_ = std::forward<From>(from);
            return {};
        }
        void return_void() {}
    };
    handle_type h_;
    Generator(handle_type h) : h_(h) {}
    ~Generator() { h_.destroy(); }
    explicit operator bool() {
        fill();
        return !h_.done();
    }
    T operator()() {
        fill();
        full_ = false;
        return std::move(h_.promise().value_);
    }

   private:
    bool full_ = false;
    void fill() {
        if (!full_) {
            h_();
            if (h_.promise().exception_)
                std::rethrow_exception(h_.promise().exception_);
            full_ = true;
        }
    }
};

Generator<unsigned> counter6() {
    for (unsigned i = 0; i < 3;) co_yield i++;
}

int main() {
    auto gen = counter6();
    while (gen) std::cout << "counter6: " << gen() << std::endl;
}
```

上述例子中，
* 重载了 `operator bool()` 来确定 Generator 是否还输出值
* 重载了 `T operator()` 来返回下一个值。
* coroutine handle 是在 Generator 内被析构的，因为 coroutine handle 在本例中不再需要，故 `final_suspend` 可以直接返回 `std::suspend_always` 类型。
