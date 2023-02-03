`typedef`：为各种数据类型定义新名字。

定义 指针类型时要用 `typedef`，而不是 `#define`，因为后者无法正确地处理指针类型

常量

声明：
```c
int const a;
const int a;

// 指向整型的指针
int *pi;

// 指向整型常量的指针
int const *pci;

// 指向整型的常量指针
int * const *pci;

// 指向整型常量的常量指针
int const * const cpci;

```

`#define`，另一种创建名字常量的机制，只要允许使用字面值常量的地方就能使用，而const只能适用于变量的地方
```c
#define MAX_ELEMENTS 50
int const max_elements = 50;
```
