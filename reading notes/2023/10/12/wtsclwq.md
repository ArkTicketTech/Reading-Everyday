## 第二十八章 确定的表达式求职顺序

	1. 从C++17开始，函数表达式一定会在函数的参数之前求值。也就是说在`foo(a, b, c)`中，`foo`一定会在`a`、`b`和`c`之前求值。

    ```cpp
    void f2() {
      std::string s = "but I have heard it works even if you don't believe in it";
      s.replace(0, 4, "").replace(s.find("even"), 4, "only").replace(s.find (" don't"), 6, "");
      assert(s == "I have heard it works only if you believe in it"); // OK
    }
    ```


	如果没有确定的求值顺序，那么第三行的执行顺序和结果可能是这样：

    ```cpp
    replace(0, 4, "")
    tmp1 = find("even")
    replace(tmp1, 4, "only")
    tmp2 = find(" don't")
    replace(tmp2, 6, "")”
    ```


	那结果肯定是“I have heard it works only if you believe in it

	但如果执行顺序是：

    ```cpp
    tmp1 = find("even")
    tmp2 = find(" don't")
    replace(0, 4, "")
    replace(tmp1, 4, "only")
    replace(tmp2, 6, "")”
    ```


	相应的结果就不是那么正确了，我们会得到“I have heard it works evenonlyyou donieve in it


	1. 在C++17标准中，constexpr声明静态成员变量时，也被赋予了该变量的内联属性。
	2. 在C++20标准之前，虚函数是不允许声明为constexpr的。看似有道理的规则其实并不合理，因为虚函数很多时候可能是无状态的。
