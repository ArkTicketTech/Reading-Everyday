# 类型系统的语言

类型系统规定了编程语言的类型规则，而这是与特定类型检查算法无关的。  
因此，将类型系统与类型检查算法分开讨论是自然的：类型系统属于语言定义的范畴，而算法则属于编译器。通过类型系统而不是特定编译器使用的算法来解释语言的类型通常更为简单。并且，不同编译器对同一个类型系统也可能有不同的实现。  
仅有一个小问题，就是，技术上可以定义出的类型系统可能并没有可行的类型检查算法。（编者：应该是说过于复杂的类型系统可能没有办法实现高效的类型检查算法，因此实际上是不可用的）。