```SML
fun max_constant e =
	case e of 
		  constant i => i
		| Negate e2 => max_constant e2
		| Add(e1,e2) => if max_constant e1 > max_constant e2
						then max_constant e1
						else max_constant e2
		| Multiply(e1,e2) => if max_constant e1 > max_constant e2
							 then max_constant e1
							 else max_constant e2
```
							 
fun max_constant e =
定义了一个名为 max_constant 的函数，接受一个参数 e
模式匹配
case e of 
使用模式匹配来根据 e 的结构选择不同的处理方式
模式匹配的分支
常数模式
constant i => i
如果 e 是一个常数表达式 constant i，其中 i 是一个整数，直接返回 i
负数模式
Negate e2 => max_constant e2
如果 e 是一个负数表达式 Negate e2，其中 e2 是一个子表达式，递归调用 max_constant 处理 e2，并返回其结果
加法模式
Add(e1,e2) => if max_constant e1 > max_constant e2
              then max_constant e1
              else max_constant e2
如果 e 是一个加法表达式 Add(e1,e2)，其中 e1 和 e2 是两个子表达式：
分别递归调用 max_constant 处理 e1 和 e2
比较两个结果，返回较大的那个
乘法模式
Multiply(e1,e2) => if max_constant e1 > max_constant e2
                   then max_constant e1
                   else max_constant e2
如果 e 是一个乘法表达式 Multiply(e1,e2)，其中 e1 和 e2 是两个子表达式：
分别递归调用 max_constant 处理 e1 和 e2。
比较两个结果，返回较大的那个。
max_constant 函数通过递归和模式匹配的方式，找出其中最大的常数值。它处理了四种类型的表达式：常数、负数、加法和乘法
