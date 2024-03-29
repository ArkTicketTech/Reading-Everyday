### 单分派与多分派

Java语言是一门静态多分派、动态单分派的语言。

### 虚拟机动态分派的实现

动态分派是非常频繁的动作，而且动态分派的方法版本选择过程需要运行时在接受者类型的方法元数据中搜索合适的目标方法。实际中，JVM不可能在真正运行时去如此频繁的搜索类型元数据。一种常见的优化手段是为此类型在方法区建立一个虚方法表。

虚方法表存放着各个方法的实际入口地址，如果某个方法在子类中没有被重写，那么子类的虚方法表中的地址入口和父类相同方法的地址入口是一致的，都指向父类的实现。如果子类中重写了这个方法，子类虚方法表中的地址就会被替换为子类实现版本的入口地址。


虚方法表一般在类加载的链接阶段进行初始化，准备了类的变量初始值后，虚拟机会把该类的虚方法表也一同初始化完毕。

查虚方法表只是优化手段之一，还有类型继承关系分析、守护内联、缓存内联等手段。

# 动态类型语言支持

invokedynamic是JDK7之后发布的字节码指令。它的目标是：实现动态类型语言支持，也是JDK8中lambda实现的技术存储。


## 动态类型语言

动态类型语言最关键的特征是：它的**类型检查的主体过程是**在运行期而不是编译器进行的。

```java
obj.println("hello world");
```

在java中，如果obj的静态类型为java.io.PrintStream，那obj的实际类型必须是PrintStream的子类才是合法的。但在js中，无论obj属于何种类型，只要这种类型的方法定义中确实包含有println方法，能够找到相同签名的方法，调用就可以成功。

产生这样的差异的原因是Java在编译期间就将println方法完整的符号引用生成出来，并作为方法调用指令的参数存储到class文件中。

```latex
invokevirtual #4; //Method java/io/PrintStream.println:(Ljava/lang/String;)V
```

这个符号引用包含了该方法定义在**哪个具体类型之中**，方法的名字以及参数顺序、参数类型和方法返回值等信息，通过这个符号引用，Java虚拟机就可以翻译出该方法的直接引用。

Js中，变量obj本身并没有类型，变量obj的值才有具体类型，所以编译器最多确定方法名称、参数和返回值这些信息，而不会去确定方法所在的具体类型，即方法接受者不固定。

## Java与动态类型

目前已经有许多动态类型语言可以运行在JVM上面了，如Groovy, Clojure, Jython等，但JVM对动态类型语言的支持一直有所欠缺，主要表现在方法调用上。JDK7之前，invokevirtual\invokespecial/invokestatic/invokeinterface的第一个参数都是被调用方法的符号引用，符号引用说明是在编译期间产生的。



```javascript
var arrays = {"abc", new ObjectX(), 123, Dog, Cat, Car...}
for(item in arrays) {
  item.sayHello();
}
```

Java中数组不可能存储各种类型的对象，因为如果有对象要调用sayHello()，根据前面提到的优化看，编译期间必须要把每一个遇到的sayHello()缓存下来，但这样会对缓存产生很大压力。

这也是JDK7中invokedynamic指令以及java.lang.invoke包出现的技术背景。

## java.lang.invoke包

这个包的目的是在之前单纯依靠符号引用来确定调用的目标方法这条路之外，提供一条新的动态确定目标方法的机制，称为方法句柄。

方法句柄的原理是将确定目标调用固化在了Java代码中，模拟了invokevirtual指令的执行过程，通过Java代码来实现了分派，而这个方法本身的返回值（MethodHandle对象），可以视为对最终调用方法的一个「引用」。MethodHandle与反射很类似，但他们的区别在于：

1. Reflection和MethodHandle本质上都是在模拟方法调用，但是Reflection是在Java代码层次的方法调用，而MethodHandle是在模拟字节码层次的方法调用。
2. Reflection的信息比MethodHandle多，MethodHandle是轻量级的
3. MethodHandle被设计为可服务所有JVM之上的语言，Reflection只为Java有用。

## invokedynamic指令



invokedynamic指令的思路跟上面的MethodHandle一样，都是将如何查找目标方法的决定权从虚拟机转嫁到具体的用户代码之中，让用户有更高的自由度。



# 基于栈道字节码解释执行引擎


## 解释执行


上述编译过程全部由编译器完成的语言：C/C++

其中一部分步骤由编译器完成（如生成语法抽象树之前的步骤），后续由虚拟机完成：如Java

全部由一个黑匣子完成：大多数JavaScript执行引擎



现今实际上各种语言都有解释执行的版本，早已经不那么泾渭分明了。

## 基于栈的指令集与基于寄存器的指令集

基于栈的优点是可移植，缺点是性能慢一点。

## 基于栈道解释器执行过程
