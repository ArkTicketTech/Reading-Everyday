虚拟机技术和docker的区别？

docker 共享了底层的资源 - 比如 docker 里的进程，仍然是宿主机的进程，只不过在 docker 内部通过一些方式屏蔽了，造成了一些错觉； 但是比如 docker 内部 set 时间，则宿主机和其他docker容器都受影响
虚拟机建立在 hypervisor 技术之上，所有的硬件都被虚拟化了
