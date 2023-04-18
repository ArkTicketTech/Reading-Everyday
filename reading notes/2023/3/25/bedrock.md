弹性数据库虽然实现了用户无感知的在线伸缩容，但其仍是传统的数据库服务，在面对实例达到硬件上限或者实例所在的宿主机无法申请到额外硬件资源的情况时，仍然需要迁移扩容。
1. 在满足扩容资源需求的宿主机上申请到容器。
2. 从原来容器拷贝存量备份文件到新容器中，基于存量数据启动 mysql 服务，与主库构建主从关系，然后追加 binlog。
3. 待新从库与主库一致，进行切换。若原来的实例是主库，则先进行主从切换再删除原来数据库实例，否则直接删除原来数据库实例。