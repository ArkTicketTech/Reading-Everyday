### 第十四章 强枚举类型

	1. 由于枚举类型确实存在一些类型安全的问题，因此C++标准委员会在C++11标准中对其做出了重大升级，增加了强枚举类型。
	2. 定义强枚举类型的方法非常简单，只需要在枚举定义的enum关键字之后加上class关键字
	3. C++20标准扩展了using功能，它可以打开强枚举类型的命名空间

## 第十六章 override和final

	1. 重写（override）、重载（overload）和隐藏（overwrite）在C++中是3个完全不同的概念
	2. override的意思更接近覆盖，在C++中是指派生类覆盖了基类的虚函数
	3. overload通常是指在同一个类中有两个或两个以上的函数，他们的函数名相同，但是函数签名不同，也就是说有不同的形参。
	4. overwrite是指基类成员函数，无论它是否为虚函数，当派生类出现同名函数时，如果派生类函数签名，不同于基类函数，那么基类函数会被隐藏，如果派生类函数签名与基类函数名相同，那么需要确定基类函数是否为虚函数，如果是，那么就是override否则就是overwrite。
	5. 所以C++11标准提供了一个非常实用的override说明符，这个说明符必须放到虚函数的尾部，它明确告诉编译器这个虚函数需要覆盖基类的虚函数，一旦编译器发现该虚函数不符合重写规则，就会给出错误提示。
	6. C++11标准引入final说明符解决了上述问题，它告诉编译器该虚函数不能被派生类重写。final说明符用法和override说明符相同，需要声明在虚函数的尾部。
	7. 有时候，override和final会同时出现。这种情况通常是由中间派生类继承基类后，希望后续其他派生类不能修改本类虚函数的行为而产生的
