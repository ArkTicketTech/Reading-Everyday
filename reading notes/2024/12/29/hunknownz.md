性能方面的度量指标有很多，我觉得有三个指标非常重要，就是：吞吐量、延迟和并发量。

吞吐量：指的是单位时间内能处理的请求数量。吞吐量越高，说明性能越好。
延迟：指的是从发出请求到收到响应的时间。延迟越小，说明性能越好。
并发量：指的是能同时处理的请求数量，一般来说随着并发量的增加、延迟也会增加。所以延迟这个指标，一般都会是基于并发量来说的。例如并发量是 1000 的时候，延迟是 50 毫秒。

并发量是指同时处理活跃请求，而吞吐量在一的是完成度，一段时间完成了多少请求。