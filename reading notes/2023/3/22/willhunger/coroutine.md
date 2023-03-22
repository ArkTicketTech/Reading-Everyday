### The co_return operator
为了让协程结束，C++ 新增了 `co_return` operator，有三种使用方式：
1. The coroutine can use `co_return e;` to return a final value e.
2. The coroutine can use `co_return;`  with no value (or with a void expression) to end the coroutine without a final value.
3. The coroutine can let execution fall off the end of the function, which is similar to the previous case.

方式一中，编译器会插入 `p.return_value(e)` 的函数调用；方式二、三中，编译器会调用 `p.return_void()`。为了确认协程是否结束，可以在 coroutine handle 上调用 `h.done()` 。另外，需要区分好  `coroutine_handle::done()` 和 `coroutine_handle::operator bool()`，后者用于检查 coroutine hanler 是否为空指针。

```cpp
#include <chrono>
#include <concepts>
#include <coroutine>
#include <exception>
#include <iostream>
#include <thread>

struct ReturnObject5 {
    struct promise_type {
        unsigned value_;
        ~promise_type() { std::cout << "promise_type destroyed" << std::endl; }
        ReturnObject5 get_return_object() {
            return {
                .h_ = std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() {}
        std::suspend_always yield_value(unsigned value) {
            value_ = value;
            return {};
        }
        void return_void() {}
    };
    std::coroutine_handle<promise_type> h_;
};

ReturnObject5 counter5() {
    for (unsigned i = 0; i < 3; ++i)
        co_yield i;
    // falling off end of function or co_return; => promise.return_void();
    // (co_return value; => promise.return_value(value);)
}

int main() {
    auto h = counter5().h_;
    auto &promise = h.promise();
    while (!h.done()) {  // Do NOT use while(h) (which checks h non-NULL)
        std::cout << "counter5: " << promise.value_ << std::endl;
        h();
    }
    h.destroy();
}
```

本例用在 promise_type 中加入了 `return_void()` 方法。如果使用了 `co_return`，但是没有 `return_void` 或 `return_value` 方法，编译时会报错。若没有 `return_void()` 方法，则会导致未定义行为。

co_return `promise_type::return_void()` 和  `promise_type::return_value(v)` 同时返回 void，其不会返回任何 awaitable object。

但在协程结束时，编译器需要更新 coroutine state 和 一次性停止协程，处理过程是：
* 由 `promise_type::final_suspend()` 方法来完成。
* C++ 的协程函数体的伪代码表达如下：
	* 协程返回时，相当于隐式调用 `co_await promise.final_suspend();`；
	* 如果 `final_suspend()` 真正停止了 coroutine，那么 coroutine 会最后一次被更新，且保持合法，协程外的函数调用 `destroy()` 方法负责析构； 如果 `final_suspend()` 未能停止了 coroutine，则 coroutine state 会被自动析构。
	* 如果无需在主函数使用 coroutine state，可以将 `final_suspend()`  返回 `std::suspend_never`，这样可以自动析构，也无需保存最后一次的状态以及进行手动析构。相反的时，如果在协程结束后还需要访问 coroutine handle 或 promise object，那么可以将 `final_suspend()`  返回 `std::suspend_always` 或者是 awaitable object。
	* 如果将本节例子中 `final_suspend()` 的返回类型换回 `std::suspend_never`，则会导致未定义行为。
	
	```cpp
	{
		promise-type promise promise-constructor-arguments ;
		try {
			co_await promise.initial_suspend();
			function-body
		} catch ( ... ) {
			if (!initial-await-resume-called)
				throw ;
			promise.unhandled_exception() ;
		}
	final-suspend :
		co_await promise.final_suspend() ;
	}
	// "The coroutine state is destroyed when control flows  
	// off the end of the coroutine"
	```
