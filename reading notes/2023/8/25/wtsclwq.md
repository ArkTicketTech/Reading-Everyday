# 现代C++语言核心特性解析
## 第一章 新基础类型

1. long long: 我们知道long通常表示一个32位整型，而long long则是用来表示一个64位的整型。不得不说，这种命名方式简单粗暴。不仅写法冗余，而且表达的含义也并不清晰。
2. char16_t和char32_t: 分别用来对应Unicode字符集的UTF-16和UTF-32两种编码方法。
   > 字符集是指系统支持的所有抽象字符的集合，通常一个字符集的字符是稳定的。而编码方法是利用数字和字符集建立对应关系的一套方法，这个方法可以有很多种，比如Unicode字符集就有UTF-8、UTF-16和UTF-32这3种编码方法
