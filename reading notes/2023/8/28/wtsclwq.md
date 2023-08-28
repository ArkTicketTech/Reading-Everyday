3. 在使用`auto`声明变量时，**既没有使用引 用，也没有使用指针**，那么编译器在推导的时候会**忽略`const`和 `volatile`限定符**。当然auto本身也支持添加cv限定符

   ```c++
   const int i = 5; 
   auto j = i;  // auto推导类型为int，而非const int
   auto &m = i; // auto推导类型为const int，m推导类型为const int&
   auto *k = i; // auto推导类型为const int，k推导类型为const int*
   const auto n = j; // auto推导类型为int，n的类型为const int  
   ```

4. 使用`auto`声明变量初始化时，目标对象如果是引用，则引用属性会被忽略

5. 使用`auto`和万能引用声明变量时，对于左值会将 `auto`推导为引用类型

   ```c++
   int i = 5;
   auto&& m = i;// auto推导类型为int&, m是int&
   auto&& j = 5; // auto推导类型为int, j是int &&
   ```

6. 使用`auto`声明变量，如果目标对象是一个**数组或者函数**，则 `auto`会被推导**为对应的指针类型**

7. 当auto关键字与列表初始化组合时（C++17标准）：
   1. 直接使用列表初始化，列表中必须为单元素，否则无法编 译，`auto`类型被推导为单元素的类型。
   2. 用等号加列表初始化，列表中可以包含单个或者多个元素， `auto`类型被推导为`std::initializer_list<T>`，其中`T`是元素类型，**元素的类型必须相同**。

