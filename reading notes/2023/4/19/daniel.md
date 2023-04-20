基于领导者（leader-based） 的同步算法，是最常用解决办法。

其中一个副本称为领导者（leader），别称主副本（primary、master）。主副本作为写入的协调者，所有写入都要发给主副本。
其他副本称为跟随者（follower），也称为只读副本（read replicas）、从副本（slaves）、次副本（secondaries）、热备（hot-standby）。主副本将改动写到本地后，将其发送给各个从副本，从副本收变动到后应用到自己状态机，这个过程称为日志同步（replication log）、变更流（change steam）。
对于读取，客户端可以从主副本和从副本中读取；但写入，客户端只能将请求发到主副本。
