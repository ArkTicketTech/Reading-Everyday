1. dockerfile：自动化脚本，创建镜像
2. image: 镜像
3. container：容器

docker其实可以看作更轻量的虚拟机，它打包镜像的时候包含了程序所需要的所有运行时依赖，实现了跨平台和资源隔离，docker是使用的Linux的namespace来实现的进程隔离。

传统的虚拟机自带一个完整的操作系统，docker的容器只包含了依赖库和必要的配置。

容器运行可能有一定的顺序依赖，或者容器非常多，那就可以通过docker compose的yaml。

docker swarm解决的集群部署问题。

K8S是docker swarm的竞品。K8S能够实现自动扩缩容。

Pod是运行中的一组容器，Pod是kubernetes中应用的最小单位.

Pod是可以在K8S中创建和管理的、最小的可部署的计算单元。
