#### 4.2.2 future 与任务关联
`std::packaged_task<>` 会将 future 与函数或可调用对象进行绑定，当调用`std::packaged_task<>` 对象时，就会调用相关函数或可调用对象，当 future 状态为就绪态时，会存储器返回值。

使用：
* 构造 `std::packaged_task<>` 需要传入的函数或可调用函数需要能接受指定的参数和返回（可转换为指定返回类型的）值。类型可以不完全匹配，允许发生隐式转换。
* 函数签名的返回类型可以用来标识从 get_future() 返回的 `std::future` 的类型，而函数签名的参数列表，可用来指定 packaged_list 的函数调用操作符。

```cpp
#include <string>
#include <vector>
#include <future>
template<>
class packaged_task<std::string(std::vector<char>*,int)>
{
public:
    template<typename Callable>
    explicit packaged_task(Callable&& f);
    std::future<std::string> get_future();
    void operator()(std::vector<char>*,int);
};

```

例子：线程间传递任务
```cpp
#include <deque>
#include <mutex>
#include <future>
#include <thread>
#include <utility>

std::mutex m;
std::deque<std::packaged_task<void()> > tasks;

bool gui_shutdown_message_received();
void get_and_process_gui_message();

void gui_thread()
{
    while(!gui_shutdown_message_received())
    {
        get_and_process_gui_message();
        std::packaged_task<void()> task;
        {
            std::lock_guard<std::mutex> lk(m);
            if(tasks.empty())
                continue;
            task=std::move(tasks.front());
            tasks.pop_front();
        }
        task();
    }
}

std::thread gui_bg_thread(gui_thread);

template<typename Func>
std::future<void> post_task_for_gui_thread(Func f)
{
    std::packaged_task<void()> task(f);
    std::future<void> res=task.get_future();
    std::lock_guard<std::mutex> lk(m);
    tasks.push_back(std::move(task));
    return res;
}

```