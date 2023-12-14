Java 深拷贝 、 浅拷贝

在 Java 中，深拷贝（Deep Copy）和浅拷贝（Shallow Copy）是关于对象拷贝的两个概念。它们的区别在于是否复制了对象内部的引用以及引用指向的对象。下面分别介绍深拷贝和浅拷贝：

### 1. 浅拷贝（Shallow Copy）：

浅拷贝是指创建一个新对象，然后将原对象的字段值复制到新对象。对于基本数据类型的字段，直接复制值；对于引用类型的字段，复制的是引用，即两个对象的引用指向同一个对象。

在 Java 中，可以使用 `clone()` 方法或者拷贝构造函数来实现浅拷贝。

#### 使用 `clone()` 方法实现浅拷贝：

```
javaCopy code
class MyClass implements Cloneable {
    private int intValue;
    private String stringValue;

    // 构造函数
    public MyClass(int intValue, String stringValue) {
        this.intValue = intValue;
        this.stringValue = stringValue;
    }

    @Override
    public Object clone() throws CloneNotSupportedException {
        return super.clone();
    }
}

// 测试浅拷贝
MyClass original = new MyClass(42, "Hello");
MyClass shallowCopy = (MyClass) original.clone();
```

### 2. 深拷贝（Deep Copy）：

深拷贝是指创建一个新对象，然后递归复制原对象及其所有引用的对象，而不是复制引用。这样，原对象和拷贝对象相互独立，对一个对象的修改不会影响到另一个对象。

在 Java 中，实现深拷贝通常需要通过序列化和反序列化，或者通过手动递归复制实现。

#### 使用序列化和反序列化实现深拷贝：

```
javaCopy code
import java.io.*;

class MyClass implements Serializable {
    private int intValue;
    private String stringValue;

    // 构造函数
    public MyClass(int intValue, String stringValue) {
        this.intValue = intValue;
        this.stringValue = stringValue;
    }

    public MyClass deepCopy() throws IOException, ClassNotFoundException {
        ByteArrayOutputStream bos = new ByteArrayOutputStream();
        ObjectOutputStream oos = new ObjectOutputStream(bos);
        oos.writeObject(this);

        ByteArrayInputStream bis = new ByteArrayInputStream(bos.toByteArray());
        ObjectInputStream ois = new ObjectInputStream(bis);

        return (MyClass) ois.readObject();
    }
}

// 测试深拷贝
MyClass original = new MyClass(42, "Hello");
MyClass deepCopy = original.deepCopy();
```

需要注意的是，被复制的类需要实现 `Serializable` 接口，否则无法通过序列化和反序列化来实现深拷贝。此外，对象及其引用的所有对象都需要是可序列化的。深拷贝涉及到对象图的完全复制，因此实现方式相对较复杂。