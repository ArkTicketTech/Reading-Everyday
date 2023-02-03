```c
#include <stdio.h>

int main()
{
        printf("Delete file (are you really sure??): ");

        return 0;
}
```

```bash
$ gcc 1.c -trigraphs -o 1
Delete file (are you really sure]:
```

- 变量的属性
	- 作用域
	- 链接属性
	- 存储属性
	- 这三者决定
		- “可视性” —— 可以在什么地方使用
		- “生命期” —— 值将保持多久
- 基本数据类型
	- 整型
	- 浮点型
	- 指针
	- 聚合类型
