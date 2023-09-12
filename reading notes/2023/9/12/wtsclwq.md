### 值类别

1. **表达式**被分为泛左值（glvalue）、右值（rvalue），泛左值又分左值（lvalue）和将亡值（xvalue），右值又分为将亡值和纯右值。

![image](https://res.craft.do/user/full/b3a9fb9e-44d6-5c1f-8cd3-0a79669fb06c/doc/95A643BD-21B2-4C4C-BD07-70AF7304121E/DCD75817-B5FC-46E7-B58B-DD90601B7C41_2/pvC1mVhPX30xCUvmtDeZmekRgtDtAq7oOZtPxEwUVqwz/Image.png)


	- 左值：**能够**在等号左边，即可以被赋值，赋值代表什么？修改目标区域内存的地址，所以左值是一个**有名字的，能够被取地址**的表达式。
	- 纯右值：**只能**在等号右边，即不能被赋值，所以右值是一个没名字的，不能被取地址的表达式。
	- 将亡值：处在生命周期末尾的，所占用的资源（内存）可以被重用的表达式。
2. 使用`static_cast<T&&>`强制类型转换可以将右值引用绑定到左值，来触发移动语义，`std::move`就是标准库提供的这样一个转换。
