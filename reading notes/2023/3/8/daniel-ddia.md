同步复制牺牲了响应延迟和部分可用性（在某些副本有问题时不能完成写入操作），换取了所有副本的一致性（但并不能严格保证）。
异步复制放松了一致性，而换来了较低的写入延迟和较高的可用性。
在实践中，会根据对一致性和可用性的要求，进行取舍。针对所有从副本来说，可以有以下选择：

全同步：所有的从副本都同步写入。如果副本数过多，可能性能较差，当然也可以做并行化、流水线化处理。
半同步：（semi-synchronous），有一些副本为同步，另一些副本为异步。
全异步：所有的从副本都异步写入。网络环境比较好的话，可以这么配置。
