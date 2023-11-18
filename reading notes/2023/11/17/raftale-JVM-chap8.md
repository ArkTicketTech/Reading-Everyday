### 动态分派

动态分派与多态性的另一个体现- 重写override，有着很密切的关联。

```java
public class DynamicDispatch {
	static abstract class Human {
        protected abstract void sayHello();
    }

    static class Man extends Human {
        protected void sayHello() {
            System.out.println("man say hello");
        }
    }

    static class Woman extends Human {
        protected void sayHello() {
            System.out.println("woman say hello");
        }
    }

    public static void main(String[] args) {
        Human man = new Man();
        Human woman = new Woman();
        man.sayHello();   // man say hello
        woman.sayHello();	// woman say hello
        man = new Woman();
        man.sayHello();   // woman say hello
    }
}
```

Java虚拟机显然不是根据静态类型来判断调用，因为同样静态类型的man两次调用产生了不同的结果，很明显是通过实际类型来判断执行方法的。

在man.sayHello()和woman.sayHello()对应的字节码是相同的invokevirtual指令，但这两句指令的最终目标方法不一样。invokevirtual是如何工作的？

1. 找到操作数栈顶的第一个元素所指向的对象的实际类型，记作C
2. 如果在类型C中找到与常量中的描述符和简单名称都相符的方法，则进行访问权限校验
3. 否则，按照继承关系从下往上对C的各个父类进行第二部的搜索和验证过程



正是因为invokevirtual指令执行的第一步就是在运行期确定接受者的实际类型，所以两次调用invokevirtual指令并不是把常量池中的方法的符号引用解析到直接引用上就结束了。还会根据方法接受者的实际类型选择方法版本，这个过程就是Java语言中重写的本质。我们把这种在运行期根据实际类型确定方法执行版本的分派过程称为动态分派。

多态的根源是虚方法的调用指令invokevirtual的执行逻辑，所以只对方法有效，对字段无效。



```java
public class FieldHasNoPolymorphic {
  static class Father { 
    public int money = 1;
                       
    public Father() { 
      money = 2;                        
      showMeTheMoney(); 
    }
    
    public void showMeTheMoney() {
      System.out.println("I am Father, i have $" + money);
                       
    } 
  }
  static class Son extends Father { 
    public int money = 3;
                                   
    public Son() { 
      money = 4;                                            
    }
    public void showMeTheMoney() {
      System.out.println("I am son, i have $" + money);                 
    }                            
  }

  public static void main(String[] args) {
    Father guy = new Son();
    System.out.println("This guy has $" + guy.money);
  }
}
```

输出：

```java
I am son, i have $0
I am son, i have $4
This guy has $2
```

Son类在创建的时候，首先隐式调用了Father的构造函数，而Father中对showMeTheMoney()的调用是一次虚方法调用，实际执行版本为Son::showMeTheMoney()，因为guy的实际类型是Son，且为虚方法调用，此时Son的money还没有赋值，所以为0。



This guy has $2 是因为guy的静态类型是Father，字段没有多态，所以直接访问的是Father的money字段。
