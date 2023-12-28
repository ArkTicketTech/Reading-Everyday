
cloud-init 被设计为只会执行一次，目前被云厂商广泛使用于注入 ssh key 等场景。

cloud-init 根据 /var/lib/cloud/instance 下的内容判断是否是首次执行。 主要是比对 instance id。

在 cloud-init 的 runcmd 部分执行命令以取消挂载 cloud-init 是不可行的。runcmd 执行的命令是在 VM 启动过程中的一部分，没有能力修改外部的 Kubernetes 或 KubeVirt 层面的 VM 定义。