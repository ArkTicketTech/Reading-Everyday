
## 第十八章 支持初始化语句的if和switch


	1. 在C++17标准中，if控制结构可以在执行条件语句之前先执行一个初始化语句。语法如下：`if (init; condition) {}`

	其中init是初始化语句，condition是条件语句，它们之间使用分号分隔。

## 第十九章 static_assert声明

	1. 通常情况下运行时断言只会在Debug模式下使用，因为断言的行为比较粗暴，它会直接显示错误信息并终止程序。
	2. 虽然运行时断言可以满足一部分需求，但是它有一个缺点就是必须让程序运行到断言代码的位置才会触发断言。
	3. static_assert声明是C++11标准引入的特性，用于在程序编译阶段评估常量表达式并对返回false的表达式断言，我们称这种断言为静态断言。它基本上满足我们对静态断言的要求。
