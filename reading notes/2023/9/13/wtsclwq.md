1. 引用折叠的推导规则如下：

### 表格


**R&&**

**R&&**

**T&&**

R&

R&

T&&

**R&&**

**R**

**T&&**

R&

R&&

T&

T&

R&

R&

R&

R

T&

最终类型

T实际类型

类型模板


4. 万能引用最典型的用途被称为完美转发。因为形参在**函数A**内部被认为是左值，如果出现需要将形参不改变其值类别的情况下，传递到嵌套调用的**函数B**中时，就需要用到完美转发。主要是利用了引用折叠。

    ```cpp
    #include <iostream>
    #include <string>
    
    template<class T>
    void show_type(T t) {
      std::cout << typeid(t).name() << std::endl;
    }
    
    template<class T>
    void perfect_forwarding(T &&t) {
      show_type(static_cast<T&&>(t)); // t作为形参，被视为左值
    
    }
    
    std::string get_string() {
      return "hi world";
    }
    
    int main() {
      std::string s = "hello world";
      perfect_forwarding(s);
      perfect_forwarding(get_string());
    } 
    ```


	标准库为我们提供了更加便捷的模板函数`std:::forward` 
