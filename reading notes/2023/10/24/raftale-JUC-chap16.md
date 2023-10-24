# 发布

第三章介绍了如何安全地发布一个对象，它们的安全性都来自于JMM提供的保证，而造成不正确发布的原因，就是在「发布一个共享对象」和「另一个线程访问该对象」之间缺少一种Happens-Before排序。

## 不安全的发布

当缺少Happends-Before关系时，就可能出现重排序问题，这就解释了为什么在没有充分同步的情况下发布一个对象会导致另一个线程看到一个只被部分构造的对象。



```java
public class UnsafeLazyInitialization {
  private static Resource resource;
  public static Resource getInstance() {
    if(resource == null) {
      // 不安全的发布
      resource = new Resource();
    }
    return resource;
  }
}
```

假设线程A是第一个调用getInstance的线程，它将看到 resource为null，并且初始化一个新的Resource，然后将resource设置为执行这个新实例。当线程B随后调用getInstance，它可能看到resource为空，因此就直接使用已经构造好的resource。这里面的问题在于：线程A写入resource的操作与线程B读取resource的操作之间不存在Happens-Before关系，在发布对象存在数据竞争问题，因此B并不一定能看到Resource的正确状态。

除了不可变对象以外，使用被另一个线程初始化的对象通常都是不安全的，除非对象的发布操作是在使用该对象的线程开始之前执行。

## 安全的发布

常用的安全发布模式可以确保该发布对象对于其他线程是可见的，因为它们保证发布对象的操作将在使用对象的线程开始使用该对象的引用之前执行。

Happens-Before排序是在内存访问级别上操作的，它是一个并发级汇编语言，而安全发布的运行级别更接近于程序设计。

## 安全初始化模式

有时候，我们需要推迟一些高开销的对象初始化操作，并且只有当使用这些对象时才进行初始化，但我们也看到了在误用延迟初始化导致的问题。通过将getResource方法声明为synchronized，可以修复UnsafeLazyInitialization的问题。

```java
@ThreadSafe
public class SafeLazyInitialization {
  private static Resource resource;

  public synchronized static Resource getInstance() {
    if(resource == null) {
      resource = new Resource();
    }
    return resource;
  }
}
```



使用提前初始化避免在每次调用SafeLazyInitialization中的getInstance时所产生的同步开销。静态初始化器是由JVM在类的初始化阶段执行，即在类被加载后并且被线程调用前，所以它提供了安全性保证。JVM将在初始化期间获得一个锁，并且每个线程都至少获取一次这个锁以确保这个类已经加载。

```java
@ThreadSafe
public class EagerInitalization {
  private static Resource resource = new Resource();

  public static Resource getInstance() {
    return resource;
  }
}
```

通过将这类技术与JVM的延迟加载机制结合起来，可以形成一种延迟初始化技术。JVM将推迟ResourceHolder的初始化操作，直到开始使用这个类时才初始化。由于是使用一个静态初始化来初始化resource，不需要额外的同步。

```java
@ThreadSafe
public class ResourceFactory {
    private static class ResourceHolder {
        public static Resource resource = new Resource();
    }

    public static Resource getResource() {
        return ResourceHolder.resource;
    }
}
```
