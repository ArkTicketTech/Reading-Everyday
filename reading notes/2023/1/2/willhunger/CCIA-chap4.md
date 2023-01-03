#### 4.2.3 使用 std::promise
`std::promise/std::future` 对提供一种机制：future 可以阻塞等待线程，提供数据的线程可以使用 promise 对相关值进行设置，并将 future 的状态置为"就绪"。
当 primise 调用 set_value() 成员函数时，future 的状态变为就绪；另外，设置值之前销毁 std::promise，会存储一个异常。

#### 4.2.4 将异常存于 future 中
当 `std::async` 所调用函数内部抛出一个异常时，这个异常会存储到 future 中，并且 future 的状态会置为就绪，后续调用 get() 会判处已存储的异常（标准为规定重新抛出的异常是原始的异常对象，还是拷贝后的，编译器决定）。同样，函数打包到 `std::packaged_task` 中也一样。

此外，显示的函数调用也能设置异常：

```cpp
extern std::promise<double> some_promise;
try
{
	some_promise.set_value(calculate_value());
}
catch(...)
{
	some_promise.set_exception(std::current_exception());
}
```

或者使用 `std::copy_exception` 直接存储新的异常而不抛出，比 try/catch 更加清晰，当异常类型已知，就应该优先使用。不是因为代码简单，而是给编译器提供了极大的优化空间。
```cpp
some_promise.set_exception(std::copy_exception(std::logic_error("foo ")));
```

此外，当 future 的状态还不是就绪态是，对 `std::promise` 或者 `std::packaged_task` 进行析构，将会存储一个与 `std::future_errx::broken_promise` 错误状态相关的 `std::future_error` 异常。

综上：可以手动为 promise 构造异常，或者销毁 promise ，来提供异常。这种情况下：编译器没有在 future 中存储任何东西，线程可能会永远等待下去。

`std::future` 的局限性：很多线程在等待时，只有一个线程能获取结果。多个线程等待相同事件的结果时，需要使用 `std::shared_futrue`。