### 4.2 使用 future
 `std::future`  和 `std::shared_future<>` :
* 前者只能与指定事件相关联；后者能关联关联多个事件，后者的使用时，所有的实例会同时变为就绪态，并且可以访问与事情相关的数据。
* future 不提供同步访问，当多个线程同时需要一个独立 futrue 对象适，必须使用互斥量或其它同步机制进行保护；例外：当多个线程对一个 `std::shared_future<>`  副本进行访问，即使同一个异步结果，也不需要同步 future。

#### 4.2.1 后台任务的返回值
`std::async` 会返回一个 `std::future` 对象，这个对象持有最终计算出来的结果，但需要这个值时，只需要调用这个对象的 get() 成员函数，就会阻塞线程直到 future 就绪为止，并返回计算结果。

`std::async` 的等待取决于是否启动一个线程，或是否有任务在进行同步，可以传递一个 std::launch 类型的参数：
* `std::launch::deferd`：表明函数调用延迟到 wait() 或者 get() 函数调用时才执行；
* `std::launch::async`：表明函数必须在其所在的独立线程上执行；
* `std::launch::deferd | std::launch::async`：表明可以选择这两种方式的一种，为默认选项。


```cpp
#include <string>
#include <future>

struct X
{
    void foo(int,std::string const&);
    std::string bar(std::string const&);
};


X x;
auto f1=std::async(&X::foo,&x,42,"hello");
auto f2=std::async(&X::bar,x,"goodbye");

struct Y
{
    double operator()(double);
};
Y y;
auto f3=std::async(Y(),3.141);
auto f4=std::async(std::ref(y),2.718);
X baz(X&);
auto f6=std::async(baz,std::ref(x));
class move_only
{
public:
    move_only();
    move_only(move_only&&);
    move_only(move_only const&) = delete;
    move_only& operator=(move_only&&);
    move_only& operator=(move_only const&) = delete;
    void operator()();
};
auto f5=std::async(move_only());

```