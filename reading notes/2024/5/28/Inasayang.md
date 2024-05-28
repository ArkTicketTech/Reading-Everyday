# grokking streaming systems real-time event processing pp. 21-52

## ch2.Hello, streaming systems

![ch2-01](https://inasa.dev/img/grokkingstreamingsystems/ch2/01.png)

`queue`,它在所有streaming systems中被广泛使用。

传统的分布式系统通常通过请求/响应模型（也称为同步模型）进行通信。在流系统中并非如此，因为请求/响应模型在处理实时数据时会引入不必要的延迟（技术上讲，近实时可能更准确，但流系统通常被认为是实时系统）。从高层次看，分布式流系统通过在系统各组件之间保持长时间连接来减少数据传输时间。这种长时间连接是为了持续传输数据，使流系统能够对事件发生时立即做出反应。

- `queue`可以帮助解耦系统中的模块，使每个部分能够按自己的节奏运行，而不必担心依赖关系和同步问题。
- `queue`可以帮助系统按顺序处理事件，因为它们是一种FIFO（先进先出）数据结构。

然而，使用`queue`来有序地持续传输数据并非全是美好的事。保证数据处理顺序时可能会有许多意想不到的陷阱。(ch5)

![ch2-02](https://inasa.dev/img/grokkingstreamingsystems/ch2/02.png)

![ch2-03](https://inasa.dev/img/grokkingstreamingsystems/ch2/03.png)

通常，流框架有两个职责：

- 提供一个应用程序接口（API）供用户连接自定义逻辑并构建作业
- 提供一个引擎来执行流作业

执行器如何在事件上应用用户逻辑?

![ch2-04](https://inasa.dev/img/grokkingstreamingsystems/ch2/04.png)

![ch2-05](https://inasa.dev/img/grokkingstreamingsystems/ch2/05.png)

在大多数流系统中有五个关键概念：事件（event）、作业（job）、源（source）、运算符（operator）和流（stream）。

从这张图中，可以看到源对象和运算符对象如何通过流连接在一起形成一个流作业。应该知道，流不过是从一个组件到另一个组件的连续数据传输。

![ch2-06](https://inasa.dev/img/grokkingstreamingsystems/ch2/06.png)

- 作业（Job），也称为管道（Pipeline）或拓扑（Topology），是流系统的实现。作业由组件（源和运算符）和连接这些组件的流组成。
- 流（Stream）指的是事件的持续传递。
- 事件（Event），在不同场景中也称为元组（Tuple）、元素（Element）或消息（Message），是流中不可分割的单个数据片段。
- 源（Source）是将数据从外部世界引入流系统的部分。换句话说，源是流系统的数据入口。
- 运算符（Operator），也称为转换（Transform），是接收和处理事件的部分。运算符是逻辑发生的地方。

所有流框架都有一个API，允许你为数据源编写只关注你关心的逻辑。所有数据源API都有某种生命周期钩子，它会被调用以从外部世界接收数据。在这个阶段，框架会执行你的代码。

软件框架中的生命周期钩子是框架在某种可重复模式中调用的方法。通常，这些方法允许开发人员自定义其应用程序在框架生命周期阶段的行为。在Streamwork框架中，有一个名为getEvents()的生命周期钩子（或方法）。该方法由框架连续调用，允许从外部世界拉取数据。生命周期钩子使开发人员可以编写他们关心的逻辑，而框架则处理所有繁重的工作。

### Reviewing streaming concepts

![ch2-07](https://inasa.dev/img/grokkingstreamingsystems/ch2/07.png)
