chap4: 性能监控、故障处理工具
1. jps： JVM process Status Tool 显示指定系统内所有的HotSpot虚拟机进程
a. -l : 输出主类的全名
b. -m/-q/-v
2. jstat：JVm statistics monitoring tool，监视虚拟机各种运行状态信息
3. jinfo：显示虚拟机配置信息，jdk9中集成到了jhsdb中
4. jmap：主动生成堆转储快照
5. jstack：java堆栈跟踪工具，threaddump生成当前时刻的线程快照。
6. VisiualVM：提供运行监控、故障处理、性能分析等能力
7. JMC：Java Mission Control，可持续在线的监控工具。
8. JFR：Java Flight Recorder，可持续收集数据的飞行记录仪
   a. 在生产环节对吞吐量的影响一般不会高于1%

chap7 ：虚拟机类加载机制

JVM把描述类的数据从class文件加载到内存，并对数据进行校验、转换解析和初始化，最终形成可以被JVM直接使用的Java类型，这个过程被称为虚拟机的类加载机制。



class文件不仅是某个具体的文件，它是一串二进制字节流，可以以磁盘文件、网络、数据库、内存或者动态产生。



# 类加载的时机


一个类型从被加载到虚拟机内存中，到卸载出内存为止，整个生命周期经历加载、验证、准备、解析、初始化、使用、卸载。

1. 加载、验证、准备、初始化、卸载的顺序是确定的，但解析可能在初始化阶段后开始，这是为了支持动态绑定



类加载的时机：

1. 遇到new、getstatic、putstatic、invokestatic这四条字节码指令时，如果类型没有进行过初始化，则需要先触发其初始化阶段。能够生成这四条指令的典型Java代码场景有：

1. 1. 使用new 关键字实例化对象的时候
2. 读取或设置一个类型的静态字段（被final修饰，已在编译期把结果放入常量池的静态字段除外）的时候
3. 调用一个类型的静态方法的时候

1. 使用java.lang.reflect包的方法对类型进行反射调用的时候，如果类型没有进行过初始化，则需要先触发其初始化
2. 当初始化类的时候，如果发现其父类还没有进行过初始化，则需要先触发其父类的初始化
3. 当虚拟机启动时，用户需要指定一个要执行的主类，虚拟机会先初始化这个类
4. 当使用JDK 7新加入的动态语言支持时，如果一个java.lang.invoke.MethodHandle实例最后的解 析结果为REF_getStatic、REF_p utStatic、REF_invokeStatic、REF_newInvokeSp ecial四种类型的方法句 柄，并且这个方法句柄对应的类没有进行过初始化，则需要先触发其初始化。
5. 当一个接口中定义了JDK 8新加入的默认方法(被default关键字修饰的接口方法)时，如果有 这个接口的实现类发生了初始化，那该接口要在其之前被初始化。

有且只有这六种会触发初始化，这六种场景中的行为称为对一个类型进行主动引用。除此此外的都称为被动引用，不会触发初始化。



被动引用的例子1：



```java
public class SuperClass {
  static {
    System.out.println("SuperClass init!");
  }
  public static int value = 123;

}

public class SubClass {
  static {
    System.out.println("SubClass init!");
  }
}

public class NotInitialization {
  public static void main(String[] args) { 
    System.out.println(SubClass.value);
  }
}
```

上述代码只会输出「SuperClass init!」和「123」，不会输出「"SubClass init!"」。对于静态字段，只有直接定义这个字段的类才会被初始化，因此通过其子类来引用父类中定义的静态字段，只会导致父类的初始化而不会触发字类的初始化。





被动引用的例子2：

```java
/**
* 被动使用类字段演示二:
* 通过数组定义来引用类，不会触发此类的初始化 **/
public class NotInitialization {
  public static void main(String[] args) {
    SuperClass[] sca = new SuperClass[10];
  } 
}
```

虽然引用了SuperClass类，但这里是创建一个数组，所以也不会触发SuperClass的初始化，不会输出「SuperClass init!」。



被动引用的例子3：

```java
/**
* 被动使用类字段演示三:
* 常量在编译阶段会存入调用类的常量池中，本质上没有直接引用到定义常量的类，因此不会触发定义常量的
类的初始化
**/
public class ConstClass {
  static {
    System.out.println("ConstClass init!");
  }
  // 如果没有final，ConstClass也会初始化
  public static final String HELLOWORLD = "hello world"; 
}
/**
* 非主动使用类字段演示 **/
public class NotInitialization {
  public static void main(String[] args) { 
    System.out.println(ConstClass.HELLOWORLD);
  } 
}
```

不会输出「ConstClass init!」，这是一个特例，因为编译期间通过常量传播优化，已经将为final的HELLOWORLD的值直接存储在NotInitialization类的常量池中，后续引用都转化为NotInitialization类对常量池的引用。



## 类加载的过程

## 加载

加载是类加载的一部份。

非数组类的加载阶段即可用使用虚拟机内置的启动类加载器来完成，也可以由用户自定义的类加载器去完成，开发人员通过定义自己的类加载器去控制字节流的获取方式（重写一个类加载器的findClass()或loadClass()方法），实现根据自己的想法来赋予应用程序获取运行代码的动态性。

数组类本身不通过类加载器创建，而是由JVM直接在内存中动态构造出来，但数组中的元素类型最终还是要靠类加载器来完成加载。
