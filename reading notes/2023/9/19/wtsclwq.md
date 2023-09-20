## 第十七章 基于范围的for循环

	1. 在C++11标准中基于范围的for循环相当于以下伪代码:

    ```cpp
    {
      auto && __range = range_expression;
      for (auto __begin = begin_expr, __end = end_expr; __begin != __end; ++__begin) {
           range_declaration = *__begin;
           loop_statement
      }
    }
    ```


	其中begin_expr和end_expr可能是__range.begin()和__range.end()，或者是begin(__range)和end(__range)，区别是成员函数和普通函数。

	所以C++17标准取消了_begin和_end必须是相同类型的限制，只要二者的比较能返回bool值即可。

	因此伪代码如下：

    ```cpp
    {
      auto && __range = range_expression;
      auto __begin = begin_expr;
      auto __end = end_expr;
      for (; __begin != __end; ++__begin) {
           range_declaration = *__begin;
           loop_statement
      }
    }
    ```

	1. 论是C++11还是C++17标准，基于范围的for循环伪代码都是由以下这句代码开始的：

    ```cpp
    auto && __range = range_expression;
    ```


	如果range_ expression是一个泛左值，会产生未定义行为。
