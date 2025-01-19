什么是变量遮蔽
在环境里重复加已有变量，致同文件多绑定，易混淆，但特定场景有用。懂它有助于理解环境和变量绑定，利于语言拓展。
示例
示例1
val a = 10：静态环境a为int，动态环境a值10。val b = a * 2，b值20。
val a = 5：b仍20，是a为10时算的。val c = b，当前环境a为5，b为20，c也为20。
示例2
val d = a：环境a为5，d也为5。
val a = a + 1：非赋值。计算：a为5，a + 1得6，新环境a为6，遮蔽前环境。
val f = a * 2：环境a为6，f为12。
注意事项
不能前向引用，类型检查时变量未定义会出错，如val f = f - 3。
运行代码可看不同绑定值，被遮蔽变量提示隐藏值。
重点：val a = 1, b = a, a = 2，b为1，a为2。因算b时a为1，后续a变不影响b；val a = 2是新变量遮蔽旧变量。
别在RPL多次用同文件use命令，防重复引入绑定致混淆出错。