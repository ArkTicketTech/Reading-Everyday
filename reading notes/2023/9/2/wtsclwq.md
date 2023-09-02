## 第六章 右值引用 （C++11 C++17 C++20）

1. 在C++中所谓的左值一般是指一个指向特定内存的具有名称的值 （具名对象），它有一个**相对稳定的内存地址**，并且有一段较长的生 命周期。而右值则是**不指向稳定内存地址**的匿名值（不具名对象）， 它的生命周期很短，通常是暂时性的。

   一些不太好判断的例子：

   ```c++
   int *p = &x++; // 编译失败 
   int *q = &++x; // 编译成功
   ```

   ```c++
   int x = 1; 
   int get_val() {
   	return x; 
   } 
   void set_val(int val) {
     x = val;
   } 
     int main() {
     int y = get_val();
     set_val(6); 
   }
   ```

   最后需要强调的是，通常字面量都是一个右值，除字符串字面量 以外

2. 右值引用的特点之一是可以延长右值的生命周期

   ```c++
   #include <iostream>
   class X {
   public:
       X() { std::cout << "X ctor" << std::endl; }
       X(const X &x) { std::cout << "X copy ctor" << std::endl; }
       ~X() { std::cout << "X dtor" << std::endl; }
       void show() { std::cout << "show X" << std::endl; }
   };
   X make_x() {
       X x1;
       return x1;
   }
   int main() {
       X &&x2 = make_x(); // 注意这里
       x2.show();
   }
   ```

   如果将`X &&x2 = make_x()`这句代码替换为`X x2 = make_x()`会发生几次构造。在 没有进行任何优化的情况下应该是**3次构造**，首先`make_x`函数中`x1`会 默认构造一次，然后`return x1`会使用复制（或者移动）构造产生临时对象，接着 `X x2 = make_x()`会使用复制构造将临时对象复制（或者移动）到`x2`，最后临时对象被销毁。**需要加上命令行参数fno-elide-constructors用于关闭函数返回值优化（RVO）。且在C++17以前的标准下，因为C++17已经将RVO作为默认行为**

    输出对比：

    ```shell
   右值引用下：
    X ctor
    X copy ctor
    X dtor
    show X
    X dtor
   
   非右值引用：
    X ctor
    X copy ctor
    X dtor
    X copy ctor
    X dtor
    show X
    X dtor
    ```

