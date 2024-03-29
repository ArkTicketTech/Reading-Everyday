## 第三十二章 新增预处理器和宏

	1. C++17标准为预处理器增加了一个新特性`__has_include`，用于判断某个头文件是否能够被包含进来。
	2. 属性测试宏（`__has_cpp_attribute`）可以指示编译环境是否支持某种属性，该属性可以是标准属性，也可以是编译环境厂商特有的属性。
	3. `fallthrough`是C++17标准中引入的属性，该属性可以在switch语句的上下文中提示编译器直落行为是有意的，并不需要给出警告。
	4. `nodiscard`是在C++17标准中引入的属性，该属性声明函数的返回值不应该被舍弃，否则鼓励编译器给出警告提示。
	5. `maybe_unused`是在C++17标准中引入的属性，该属性声明实体可能不会被应用以消除编译器警告。
	6. `likely`和`unlikely`是C++20标准引入的属性，两个属性都是声明在标签或者语句上的。其中`likely`属性允许编译器对该属性所在的执行路径相对于其他执行路径进行优化；而`unlikely`属性恰恰相反。
	7. `no_unique_address`是C++20标准引入的属性，该属性指示编译器该数据成员不需要唯一的地址，也就是说它不需要与其他非静态数据成员使用不同的地址。
