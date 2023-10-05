# 在线程中执行任务

在围绕任务执行来设计应用程序结构时，第一步就是要找出清晰的任务边界。各个任务之间的独立性有助于实现并发。

在正常的负载下，服务器应用程序应该同时表现出良好的吞吐量和快速的响应性。应用程序提供商希望程序支持尽可能的用户，从而降低每个用户的服务成本，而用户则希望获得尽快的响应。而且，当负载过荷时，应用程序的性能应该是逐渐降低，而不是直接失败。要实现上述目标，应该选择清晰的任务边界以及明确的任务执行策略。



大多数服务器应用程序都提供了一种自然的任务边界选择方式：以独立的客户请求为边界。Web服务器、邮件服务器、文件服务器以及数据库服务器等，这些服务都能通过网络接受远程客户的连接请求。将独立的请求作为任务边界，既可以实现任务的独立性，又可以实现合理的任务规模。

## 串行的执行任务

串行处理机制无法提供高吞吐量或快速响应性。

## 显式地为任务创建线程

通过为每个请求创建一个新的线程来提供服务，从而实现更高的响应性：



主线程交替执行「接受外部请求」与「分发请求」等操作：

1. 任务处理过程从主线程中分离出来，使得主循环能够更快地重新等待下一个到来的连接，提高了响应性；
2. 任务可以并行处理，从而能同时服务同个请求，提高了吞吐量；
3. 任务处理代码必须是线程安全的，因为当有多个任务时会并发的调用这段代码。

只要请求的到达速率不超过服务器的请求处理能力，那么为每个任务分配一个线程的方法就能提高性能和吞吐率。

## 无限制创建线程的不足

生产环境中，为每个任务分配一个线程不太现实：

1. 线程生命周期的开销非常高：线程的创建和销毁有开销
2. 资源消耗：活跃的线程会消耗系统资源，尤其是内存，如果可运行的线程数量多于可用处理器的数量，那么有些线程将闲置。大量空闲的线程会占用许多内存，给垃圾回收器带来压力，而且大量线程在竞争CPU资源还会产生其他的性能开销，如果你已经拥有足够多的线程使所有CPU保持在忙碌状态，那么再创建更多的线程反而会降低性能
3. 稳定性：可创建线程的数量受到机器和参数的限制，破坏限制可能造成OOM。

在一定的范围内，增加线程可用提高系统的吞吐率，但如果超过这个范围，再创建更多的线程只会降低程序的执行速度。

# Executor框架

任务是一组逻辑工作单元，而线程则是使任务异步执行的机制。

线程池简化了线程的管理工作，并且JUC包提供了一种灵活的线程池实现作为Executor框架的一部份。在Java类库中，任务执行的主要抽象不是Thread，而是Executor：

```java
public interface Executor {
    void execute(Runnable command);
}
```

Executor支持多种不同类型的任务执行策略，它提供了一种标准的方法将任务的提交过程和执行过程解耦开，并用Runnable来表示任务，Executor的实现还提供了对生命周期的支持，以及统计信息收集、应用程序管理机制和性能监视等机制。

Executor基于生产者-消费者模式，提交任务的操作相当于生产者，执行任务的线程相当于消费者。如果要在程序中实现生产者-消费者等设计，那么最简单的方法通常就是使用Executor。

## 示例：基于Executor的web服务器

基于线程池的Web服务器

```java
public class TaskExecutionWebServer {
    private static final int NThreads = 100;
    private static final Executor exec = Executors.newFixedThreadPool(NThreads);

    public static void main(String[] args) {
        ServerSocket socket = new ServerSocket(80);
        while(true) {
            final Socket connection = socket.accept();
            Runnable task = new Runnable() {
                public void run() {
                    handleRequest(connection);
                }
            }
            exec.execute(task);
        }
    }
}
```



## 执行策略

各种执行策略都是一种资源管理工具，最佳策略取决于可用的计算资源以及对服务质量的需求。

## 线程池

线程池：管理一组同构工作线程的资源池。

工作队列（Work Queue）：工作队列中保存了所有等待执行的任务。

工作者线程（Worker Thread）的任务很简单，从工作队列中获取一个任务，执行任务，然后返回线程池并等待下一个任务。

线程池的优势：

1. 重用现有线程，避免线程创建和销毁的巨大开销，提高响应；
2. 创建足够多的线程以便使处理器保持忙碌状态；
3. 防止过多线程相互竞争资源而使应用程序耗尽资源

可以通过Executor中的静态工厂方法来创建线程池：

1. newFixedThreadPool：固定长度的线程池
2. newCachedThreadPool：可缓存的线程池
3. newSingThreadExecutor：单线程线程池，如果这个线程异常结束，会创建另一个线程来替代；
4. newScheduledThreadPool：固定长度的线程池，而且以延迟或定时的方法来执行任务

## Executor的生命周期

JVM只有再所有非守护线程全部终止后才会退出，因此，如果无法正确地关闭Executor，那么JVM将无法结束。

为了解决执行服务的生命周期，ExecutorService接口继承了Executor接口，增加了一些用于生命周期管理的方法。ExecutorService的生命周期有3中状态：

1. 运行running：创建时
2. 关闭shutting down：

1. 1. shutdown()方法将执行平缓的关闭过程，不再接受新的任务、同时等待旧任务执行完成；
2. shutdownNow()：粗暴的关闭，取消所有正在运行中的任务，不再执行队列中尚未开始的任务。

1. 已终止terminated：shutdown





## 延迟任务与周期任务

Timer类负责管理延迟任务以及周期任务，然而，Timer是基于绝对时间而不是相对事件的调度机制，因为任务的执行对系统时钟的变化很敏感，不应该再考虑使用Timer。ScheduledThreadPoolExecutor是Timer的良好替代。

如果要构建自己的调度服务，那么可以使用DelayQueue，它实现了BlockingQueue，并为ScheduledThreadPoolExecutor提供调度功能。DelayQueue管理着一组Delayed对象。每个Delayed对象都有一个相应的延时事件：再DelayQueue中，只有某个元素逾期后，才能从DelayQueue中执行take操作。从DelayQueue中返回的对象将根据它们的延时时间进行排序。

# 找出可利用的并行性

Executor框架帮助指定执行策略，但如果要使用Executor，必须将任务表述为一个Runnable。

本节中我们将开发一些不同版本的组件，并且每个版本都实现了不同程度的并发性。该示例组件实现浏览器程序中的页面渲染功能，它的作用是将HTML页面绘制到图像缓存中。

## 示例：串行的页面渲染器

```java
public class SingleThreadRenderer {
    void renderPage(CharSequence source) {
        renderText(source);
        List<ImageData> imageDate = new ArrayList<ImageData>();

        for(ImageInfo imageInfo : scanForImageInfo(source)) {
            imageData.add(imageInfo.downloadImage());
        }
        for(ImageData data : imageData) {
            renderImage(data);
        }
    }
}
```

图片下载的大部分时间都是在等待I/O操作执行完成，这期间CPU几乎不做任何工作，因此这种串行执行方法并没有充分利用CPU。

## 携带结果的任务Callable与Future

Runnable不能返回一个值或者抛出一个受检查的异常，Callable是一种更好的抽象，它任务主入口点（call）将返回一个值，或可能抛出一个异常。在Executor中包含了一些辅助方法能将其他类型的任务封装为一个Callable。

Runnable和Callable描述的都是抽象的计算任务，这些任务都是有范围的。

Future表示一个任务的生命周期，并提供了相应的方法来判断是否已经完成或者取消，以及获取任务的结果和取消任务等。Future隐含等意义是，任务的生命周期只能前进，不能后退，当某个任务完成后，它就永远停留在完成状态上。

ExecutorService中的所有submit方法都将返回一个Future。

还可以显式的为某个指定的Runnable或Callable实例化一个FutureTask， FutureTask可以直接提交给Executor，也可以直接调用run方法。



## 示例：使用Future实现页面渲染器



为了使页面渲染器实现更高的并发，首先将渲染任务分解为两个任务，一个是渲染文本，一个是下载所有图像， Callable和Future有助于表示这些协同任务之间的交互。创建一个Callable来下载所有的图像，并将其提交到一个ExecutorService，这将返回一个描述任务执行情况的Future。

```java
import java.util.*;
import java.util.concurrent.*;
import static net.jcip.examples.LaunderThrowable.launderThrowable;

public abstract class FutureRenderer {
  private final ExecutorService executor = Executors.newCachedThreadPool();

  void renderPage(CharSequence source) {
    final List<ImageInfo> imageInfos = scanForImageInfo(source);
    Callable<List<ImageData>> task =
    new Callable<List<ImageData>>() {
      public List<ImageData> call() {
        List<ImageData> result = new ArrayList<ImageData>();
        for (ImageInfo imageInfo : imageInfos) {
          result.add(imageInfo.downloadImage());
        }
        return result;
      }
    };

    Future<List<ImageData>> future = executor.submit(task);
    renderText(source);

    try {
      //阻塞直到获取结果
      List<ImageData> imageData = future.get();
      for (ImageData data : imageData) {
        renderImage(data);
      }
      
    } catch (InterruptedException e) {
      // Re-assert the thread's interrupted status
      Thread.currentThread().interrupt();
      // We don't need the result, so cancel the task too
      future.cancel(true);
    } catch (ExecutionException e) {
      throw launderThrowable(e.getCause());
    }
  }

  interface ImageData {
    
  }

  interface ImageInfo {
    ImageData downloadImage();
  }

  abstract void renderText(CharSequence s);

  abstract List<ImageInfo> scanForImageInfo(CharSequence s);

  abstract void renderImage(ImageData i);
}
```



FutureRender使得渲染文本任务与下载图形数据的任务并发地执行。

## 在异构任务并行化中存在的局限

实际上拆分异构任务进行重大的性能提升是很苦难的，只有当大量相互独立且同构的任务可以并发处理时，才能体现出将程序的工作负载分配到多个任务中带来的真正性能提升。



## CompletionService: Executor 与BlockingQueue

如果向Executor提交了一组计算任务，并且希望在计算完成后获得结果，那就需要保存与每个任务关联的Future，然后轮询使用get获取参数。

CompletionService简化了这一过程，它把Executor和BlockingQueue的功能融合在一起。只需要直接使用CompletionService提交Callable任务，然后用CompletionService不断的从阻塞队列take结果。

## 示例：使用CompletionService实现页面渲染器

```java
import java.util.*;
import java.util.concurrent.*;
import static net.jcip.examples.LaunderThrowable.launderThrowable;


public abstract class Renderer {
  private final ExecutorService executor;

  Renderer(ExecutorService executor) {
    this.executor = executor;
  }

  void renderPage(CharSequence source) {
    final List<ImageInfo> info = scanForImageInfo(source);
    
    CompletionService<ImageData> completionService = 
    	new ExecutorCompletionService<ImageData>(executor);

    for (final ImageInfo imageInfo : info){
      completionService.submit(new Callable<ImageData>() {
        public ImageData call() {
          return imageInfo.downloadImage();
        }
      });
    }


    renderText(source);

    try {
      for (int t = 0, n = info.size(); t < n; t++) {
        Future<ImageData> f = completionService.take();
        ImageData imageData = f.get();
        renderImage(imageData);
      }
    } catch (InterruptedException e) {
      Thread.currentThread().interrupt();
    } catch (ExecutionException e) {
      throw launderThrowable(e.getCause());
    }
  }

  interface ImageData {
  }

  interface ImageInfo {
    ImageData downloadImage();
  }

  abstract void renderText(CharSequence s);

  abstract List<ImageInfo> scanForImageInfo(CharSequence s);

  abstract void renderImage(ImageData i);

}
```

## 为任务设置时限

有时候，如果某个任务在指定时间内无法完成，那么将不再需要它的结果，此时可以放弃这个任务。

在使用限时任务时需要注意，当这些任务超时后应当立即停止，从而避免为继续计算一个不再使用的结果而浪费计算资源。要实现这个功能，可以由任务本身来管理它的限定时间，并且在超时后中止执行或者取消任务。此时可再次使用Future，如果一个限时的get方法抛出了TimeoutException，那么可以通过Future来取消任务。如果编写的任务是可以取消的，那么就提前中止它，避免消耗过多的资源。

## 示例：旅行预订门户网站

用户输入旅行的日期和其他要求，门户网站获取并显示来自多条航线、旅店或者汽车公司的报价。

任务拆解：从一个公司获取报价的过程是独立的。



```java
public class TimeBudget {
  private static ExecutorService executor = Executors.newCachedThreadPool();

  public List<TravelQuote> getRankedTravelQuotes(TravelInfo travelInfo, Set<TravelCompany> companies, 
                  Comparator<TravelQuote> ranking, long time, TimeUnit unit) throws InterruptedException {

    // 每个公司的报价任务
    List<QuoteTask> tasks = new ArrayList<>();
    for (TravelCompany company : companies) {
      tasks.add(new QuoteTask(company, travelInfo));
    }

    // 提交一组任务，任务带超时
    List<Future<TravelQuote>> futures = executor.invokeAll(tasks, time, unit);

    
    List<TravelQuote> quotes = new ArrayList<>(tasks.size());
    Iterator<QuoteTask> taskIter = tasks.iterator();

    // 阻塞获取结果
    for (Future<TravelQuote> f : futures) {
      QuoteTask task = taskIter.next();
      try {
        quotes.add(f.get());
      } catch (ExecutionException e) {
        // 任务抛出异常
        quotes.add(task.getFailureQuote(e.getCause()));
      } catch (CancellationException e) {
        // 任务被取消
        quotes.add(task.getTimeoutQuote(e));
      }
    }

    Collections.sort(quotes, ranking);
    return quotes;
  }

}

class QuoteTask implements Callable<TravelQuote> {
  private final TravelCompany company;
  private final TravelInfo travelInfo;

  public QuoteTask(TravelCompany company, TravelInfo travelInfo) {
    this.company = company;
    this.travelInfo = travelInfo;
  }

  TravelQuote getFailureQuote(Throwable t) {
    return null;
  }

  TravelQuote getTimeoutQuote(CancellationException e) {
    return null;
  }

  public TravelQuote call() throws Exception {
    return company.solicitQuote(travelInfo);
  }
}

interface TravelCompany {
  TravelQuote solicitQuote(TravelInfo travelInfo) throws Exception;
}

interface TravelQuote {
  
}

interface TravelInfo {
  
}
```
