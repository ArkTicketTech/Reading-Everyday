## 第二十六章 扩展的inline说明符

	1. 在C++17标准之前，定义类的非常量静态成员变量是一件让人头痛的事情，因为变量的声明和定义必须分开进行。

    ```cpp
    class X {
    public:
      static std::string text;
    };
    std::string X::text{ "hello" };
    
    int main() {
      X::text += " world";
      std::cout << X::text << std::endl;
    }”
    ```


	为了保证代码能够顺利地编译，我们必须保证静态成员变量的定义有且只有一份，稍有不慎就会引发错误。这对于整型、浮点型等类型来说不是问题，他们是允许在声明时定义的，但是对于`std::string`这种复合类型来说，就不行。


	1. C++17标准中增强了inline说明符的能力，它允许我们内联定义静态变量。
