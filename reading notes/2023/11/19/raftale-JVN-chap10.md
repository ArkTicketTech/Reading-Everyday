Java中编译期没有具体上下文的话，是一句很含糊的表述。

因为编译器有这几种：

1. 前端编译器：将.java文件转换为class文件，如Javac
2. 即时编译器（JIT，just in time compiler）：运行期把字节码转化为本地机器码的过程。如Hotspot的c1、c2编译器，Graal编译器
3. 提前编译器（AOT，ahead of time compiler）：直接把程序编译成二进制代码的过程。



本章只讨论第一种编译器：前端编译器。


# Javac编译器

Javac本身就是由一个Java语言编写的程序。

编译过程大致分为1个准备过程和3个处理过程。


## 解析与填充符号表



### 词法、语法分析

词法分析：将源代码的字符流转变为token集合的过程。

语法分析：根据token序列构造抽象语法树的过程。

后续的所有操作都是建立在抽象语法树之上的。

### 填充符号表

符号表就是符号地址和符号信息构成的表格，它存储的是token所对应的类型、作用域等。

这一步就是为了生成记录了类中符号的类型、属性等信息的符号表，方便后续流程的应用。

## 注解处理器

插入式注解器：可以提前在编译器对代码中的特殊注解进行处理，我们可以把插入式注解处理器看作是一组编译器的插件，当这些插件工作时，允许读取、修改、添加抽象语法树中的任何元素。如果这些插件在处理注解期间对语法树进行过修改，编译器将回到解析及填充符号表的过程重新处理，直到所有插入式注解处理器都没有再对语法树进行修改为止。

Lombok就是通过插入式注解器来实现的。

## 语义分析与字节码生成

语义分析是针对 结构上正确的源程序进行上下文相关性质的检查，比如类型检查、控制流检查、数据流检查等。

### 标注检查

标注就是token，标注检查包括的内容有：变量使用前是否已经被声明、变量与赋值之间的数据类型是否能够匹配，常量折叠。

### 数据及控制流分析

数据流分析和控制流分析是对程序上下文逻辑进行更进一步的验证，它可以检查出诸如程序局部变量在使用前是否有赋值、方法的每条路径是否都有返回值、是否所有的受检查异常都被正确处理了等问题。

### 解语法糖

语法糖是指计算机语言添加的某种语法，对编译结果和功能并没有影响，但是却更方便程序员使用该语言。

Java其实属于低糖语言，所以会显得比较啰嗦。

Java最常见的语法糖包括了泛型、变长参数、自动拆箱装箱等等。



### 字节码生成

字节码生成阶段不仅仅是把前面各个步骤所生成的信息转化为字节码指令写到磁盘中，编译器还进行了少量的代码添加和转换工作。例如实例构造器方法<init>()方法和类构造器<clinit>()方法。



# Java语法糖的味道

## 泛型

泛型的本质是参数化类型（parameterized type），或者参数化多态的应用，即可以将操作的数据类型指定为方法签名中的一种特殊参数，这种参数能够用在类、接口和方法的创建中，分别构成泛型类、泛型接口、泛型方法。

### Java的泛型

java的泛型实现方式：类型擦除式泛型。

Java中的泛型只在程序源码中存在，在编译后的字节码文件中，全部泛型都被替换为原来的**裸类型**（Raw Type），并在相应的地方插入了强制转型代码，因此对于运行期的Java来说，ArrayList<Integer>和ArrayList<String>其实是一个类型。



### 泛型的历史

在没有泛型的时代，对应的集合类也可以存入不同类型的元素。

```java
Object[] array = new String[10];
array[0] = 10;
ArrayList things = new ArrayList();
things.add(Integer.valueOf(10));
things.add("hello world");
```



为了保证这些编译出来的class文件可以在Java5.0引入泛型之后继续运行，设计者有两条路可以选择：

1. 需要泛型化的类型，以前有的保持不变，然后平行的加一套泛型化版本的新类型；
2. 直接把已有的类型泛型化，即让所有需要泛型化的已有类型都原地泛型化，不添加任何平行于已有类型的泛型版本。

### 类型擦除

Java由于历史原因选择了第二条路，即直接把已有的类型泛型化。

1. 要让所有需要泛型化的已有类型原地泛型化，比如ArrayList，原地泛型化后变成了ArrayList<T>；
2. 同时要让新的泛型版本能继续使用旧的容器代码，比如ArrayList<String>泛型版本需要自动成为ArrayList的子类型。

裸类型被视为所有类型泛型化实例的共同父类型。



```java
ArrayList<Integer> ilist = new ArrayList<>();
ArrayList<String> slist = new ArrayList<>();
ArrayList list; // 裸类型
list = ilist;
list = slist;
```

如何实现裸类型？

直接简单粗暴的在编译时把ArrayList<Integer> 还原为ArrayList，只在元素访问、修改时自动插入一些强制类型转换和检查指令。

总之Java泛型的实现不是太优雅，Oracle建立一个名为Valhalla的项目来改进各种缺陷包括泛型。



擦除法所谓的擦除，仅仅是对方法的Code属性中的字节码进行擦除，实际上元数据中还是保留了泛型信息，这也是我们在编码时能通过反射手段取得参数化类型的根本依据。

## 自动拆箱、装箱与遍历循环



## 条件编译
