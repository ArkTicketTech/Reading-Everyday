当数据 格式（format） 或 模式（schema） 发生变化时，通常需要对应用程序代码进行相应的更改（例如，为记录添加新字段，然后修改程序开始读写该字段）。但在大型应用程序中，代码变更通常不会立即完成：

对于 服务端（server-side） 应用程序，可能需要执行 滚动升级 （rolling upgrade） （也称为 阶段发布（staged rollout） ），一次将新版本部署到少数几个节点，检查新版本是否运行正常，然后逐渐部完所有的节点。这样无需中断服务即可部署新版本，为频繁发布提供了可行性，从而带来更好的可演化性。
对于 客户端（client-side） 应用程序，升不升级就要看用户的心情了。用户可能相当长一段时间里都不会去升级软件。
这意味着，新旧版本的代码，以及新旧数据格式可能会在系统中同时共处。系统想要继续顺利运行，就需要保持 双向兼容性：

向后兼容 (backward compatibility)

新的代码可以读取由旧的代码写入的数据。

向前兼容 (forward compatibility)

旧的代码可以读取由新的代码写入的数据。

向后兼容性通常并不难实现：新代码的作者当然知道由旧代码使用的数据格式，因此可以显示地处理它（最简单的办法是，保留旧代码即可读取旧数据）。

向前兼容性可能会更棘手，因为旧版的程序需要忽略新版数据格式中新增的部分。