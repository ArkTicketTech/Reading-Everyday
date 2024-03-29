# 运行时数据区域


## 程序计数器



程序计数器在硬件层面是一种寄存器，用来存储指令地址提供给处理器执行，JVM层面是一块较小的内存空间，用来存储字节码的指令地址，提供给执行引擎执行。每个线程都有一个独立的程序计数器。

字节码吗解释器工作时就是通过改变这个计数器的值来选取下一条需要执行的字节码指令。



## Java虚拟机栈

线程私有，生命周期与线程相同。虚拟机栈描述的是Java方法执行的线程内存模型：每个方法被执行的时候，JVm都会同步创建一个栈帧（Stack Frame）用于存储

1. 局部变量表：存储方法的参数、定义在方法内的局部变量、返回值地址

1. 1. 存储方法的参数、定义在方法内的局部变量，包括8大基本数据类型、对象的引用地址、返回值地址
2. 存储的基本单元为变量槽（slot），32位以内的数据占用一个slot，64位的占用两个slot
3. 局部变量表是一个数字数组；
4. 局部变量表的大小是在编译期间决定下来的；

1. 操作数栈：用来存储操作数的栈，操作数大部分就是方法内的变量。
2. 动态连接：每个栈帧都包含一个指向运行时常量池中该栈帧所属方法的引用，持有这个引用是为了支持方法调用过程中的动态连接。

1. 1. 其实就是说将编译期间的符号引用转为直接引用，符号引用是因为类似于多态，具体执行那个方法只有在运行时才能确定，所以运行时才替换为真正的地址引用。

1. 返回地址
2. 其他附加信息

每一个方法被调用直到执行完毕的过程，就对应着一个栈帧在虚拟机栈中从入栈到出栈的过程。

栈帧的生成时机：运行时。

## 本地方法栈

虚拟机栈是为Java方法服务的，本地方法栈是为了native方法服务的。

## Java堆

Java堆是被所有线程共享的一块内存区域，在虚拟机启动时创建。堆的唯一目的就是存放对象实例，

如果在堆中没有完成内存实例分配，并且堆也无法再扩展时，JVM会抛出OutOfMemoryError异常。

## 方法区

方法区是各个线程共享的内存区域，它用于存储已被虚拟机加载的类型信息、常量、静态变量、代码缓存等数据。

方法区是一个抽象，永久代和元空间是其实现。JDK8以前，HotSpot中方法区是用的永久代，8以后用的是本地内存实现的元空间。

如果方法区无法满足新的内存分配需求时，将抛出OutOfMemoryError异常。

## 运行时常量池

Runtime Constant Pool是方法区的一部份。Class文件中除了有类的版本、字段、方法、接口等描述信息外，还有一项信息是常量池表，用于存放编译期间生成的各种字面量与符号引用，这部分内容将在类加载后存放到方法区的运行时常量池中。

运行期间也可以将新的常量池放入运行时常量池中。

## 直接内存

直接内存不是JVM的内存区域，但也被频繁调用，而且也有可能导致OOM。

# HotSpot 虚拟机对象探秘

## 对象的创建

创建过程：

1. 当JVM遇到字节码new指令时，首先去检查这个指令的参数是否能在常量池中定位到一个类的符号引用，并且检查这个符号引用代表的类是否已经被加载、解析和初始化过；如果没有，那就执行相应的类加载过程。
2. 根据对象所需内存大小在堆中划分可用空间，内存大小是在类加载后就能确定的；

1. 1. 划分可用空间可能有并发问题，解决方法有：

1. 1. 1. CAS保证操作的原子性
2. 内存分配的动作按照线程在不同的空间，即每个线程在堆中预先分配一小块内存，成为本地线程分配缓冲（TLAB）。这样保证了只有TLAB用完了，分配新的TLAB时才需要考虑线程安全；

1. 内存分配完成后，JVm必须将分配到的内存空间（不包括对象头）都初始化为零值。
2. 接下来，JVM还需要设置对象头的信息，包括属于哪个类的实例、如何才能找到类的元数据信息、对象的GC分代年龄。JVM的视角来看，此时一个新的对象就产生了。
3. 紧接着执行对象的构造函数<init>()，一个真正可用的对象才算完全构造出来

## 对象的内存布局

对象在堆内存中的存储布局可以划分为：

1. 对象头Header：

1. 1. 存储对象自身的运行时数据，如哈希码、GC分代年龄、锁状态标志、线程持有的锁、偏向线程ID、偏向时间戳；数据长度在64位的虚拟机下为64位，这部分数据也被称为mark word。
2. 存储类型指针：通过这个指针来确定对象数据哪个类的实例。但并不是所有的JVM都会在对象头中保存这个。

1. 实例数据Instance Data：程序代码中定义的各种类型的字段内容
2. 对齐填充Padding：因为HotSpot规定对象起始地址必须是8字节的整数倍，所以对齐填充来保证这一点，没有其他意义。

## 对象的访问定位

创建对象是为了后续使用该对象，Java程序会通过栈上的reference数据来操作堆上的具体应用。主流的访问方式有两种：

1. 句柄访问：reference存储堆中的句柄地址，句柄地址再指向对象实例数据地址
2. 直接访问：reference直接指向对象实例数据地址

HotSpot使用的是直接访问。

# OOM

1. Java堆溢出
2. 虚拟机栈和本地方法栈溢出
3. 方法区和运行时常量池溢出
4. 本地直接内存溢出
