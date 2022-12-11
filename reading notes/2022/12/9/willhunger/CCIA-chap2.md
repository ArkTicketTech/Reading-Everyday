## Chap2 线程管控
### 2.2 向线程传递参数
线程具有内部存储空间，参数会按照默认方式先复制到该处，新创建的执行线程才能直接访问它们。然后，这些副本被当成临时变量，以右值形式传给新线程上的函数或可调用对象。即便函数的相关参数按设想应该是引用，上述过程依然会发生。

注意点：
1. 如果线程函数的实参是**指向自动变量的指针**，可能实参已经销毁，但是线程形参的构造并未发生，导致引发未定义行为。

2. 线程库的内部代码会把参数的副本当成move-only型别（只能移动，不可复制），并以右值的形式传递。
	因此，对于形参为非 const 引用的线程函数，不能向其传递右值，会导致编译失败。
	解决方法就显而易见：**若需按引用方式传递参数，只要用std::ref()函数加以包装即可。**
	```cpp
	void update_data_for_widget(widget_id w,widget_data& data); 
	
	void oops_again(widget_id w) { 
		widget_data data; 
		std::thread t(update_data_for_widget,w,data); 
			// std::thread t(update_data_for_widget,w,std::ref(data));
		display_status(); 
		t.join(); 
		process_widget_data(data); 
	}
	
	```

3. 若要将某个类的成员函数设定为线程函数，我们则应传入一个函数指针，指向该成员函数。此外，我们还要给出合适的对象指针，作为该函数的第一个参数：
	```cpp
	class X { 
	public: 
		void do_lengthy_work(); 
	};
	X my_x; 
	std::thread t(&X::do_lengthy_work,&my_x);
	```

4. 移动语义给线程传参
	```cpp
		void process_big_object(std::unique_ptr<big_object>);
		
		std::unique_ptr<big_object> p(new big_object); 
		p->prepare_data(42); 
		std::thread t(process_big_object,std::move(p));
	```