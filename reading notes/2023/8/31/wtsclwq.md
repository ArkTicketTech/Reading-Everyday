2. C++11标准引入了`decltype`说明符，使用`decltype`说明符可以获取对象或者表达式的类型

   如果想正确地返回引 用类型，则需要用到decltype说明符

   ```c++
   template<class T> auto return_ref(T& t)->decltype(t) 
   { return t; }
   
   int x1 = 0; static_assert(std::is_reference_v<decltype(return_ref(x1))> );
   ```

3. 推导规则：`decltype(e)`（其中`e`的类型为`T`）
   1. 如果`e`是一个未加括号的标识符表达式（结构化绑定除外）或者未加括号的类成员访问，则`decltype(e)`推断出的类型是`e`的类型 `T`。
   2. 如果`e`是一个**函数调用或者仿函数调用**，那么decltype(e) 推断出的类型是其**返回值**的类型。
   3. 如果`e`是一个类型为`T`的左值，则`decltype(e)`是`T&`。
   4. 如果`e`是一个类型为`T`的将亡值，则`decltype(e)`是`T&&`。
   5. 除去以上情况，则`decltype(e)`是`T`。

4. 通常情况下，`decltype(e)`所推导的类型会同步`e`的`cv`限定符

## 第五章 函数返回类型后置（C++11）

1. 在返回类型比较复杂的时候，比如返回一个函数指针类型， 返回类型后置可能会是一个不错的选择

2. **C++11**标准中函数返回类型后置的作用之一是**推导函数模板的返回类型**，当然前提是需要用到`decltype`说明符.

   只用`decltype`关键字也能写出自动推导 返回类型的函数模板，但是**函数可读性**却差了很多

   ```c++
   template<class T1, class T2> 
   auto sum1(T1 t1, T2 t2)->decltype(t1 + t2) {
     return t1 + t2; 
   }
   int main() { auto x1 = sum1(4, 2); }
   
   template<class T1, class T2> 
   decltype(T1() + T2()) sum2(T1 t1, T2 t2) { 
   	return t1 + t2; 
   }
   // 这种写法并不通用，它存在一 个潜在问题，由于T1() + T2()表达式使用了T1和T2类型的默认构 造函数，因此编译器要求T1和T2的默认构造函数必须存在，否则会编 译失败
   ```

