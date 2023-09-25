## 第二十二章 指针字面量nullptr

1. `NULL`是一个宏，在C++11标准之前其本质就是`0`

```cpp
“#ifndef NULL
    #ifdef __cplusplus
        #define NULL 0
    #else
        #define NULL ((void *)0)
    #endif
#endif”
```


	C++将`NULL`定义为`0`，而C语言将`NULL`定义为`(void *)0`。


3. 鉴于`0`作为空指针常量的种种劣势，C++标准委员会在C++11中添加关键字`nullptr`表示空指针的字面量，它是一个`std::nullptr_t`类型的纯右值。

```cpp
“std::nullptr_t null1, null2;

char* ch = null1;
char* ch2 = null2;
assert(ch == 0);
assert(ch == nullptr); 
assert(ch == null2);
assert(null1 == null2);
assert(nullptr == null1);”
```


	首先，`nullptr`是关键字，而其他两个是声明的变量。其次，`nullptr`是一个纯右值，而其他两个是左值。
