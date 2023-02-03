声明指针的两个注意事项：
1. 把星号要写在靠近变量的一侧，星号是变量表达式的一部分，
```c
int* a, b, c;

int *a, b, c;

// 这个才是声明了三个指针
int *a, *b, *c;
```
2. 声明并初始化
```c
char *massage = "Hello World!";
```
初始值赋给了`massage`变量本身
