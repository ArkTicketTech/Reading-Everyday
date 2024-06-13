- Redis为什么需要持久化？ 
  - 从数据库恢复数据性能差 
- 持久化方案：
  - RDB（Redis DataBase）：数据快照化到磁盘
    - 缺点：实时性不够
  - AOF(Append Only File): 记录命令到磁盘
    - 缺点：文件增长快，需要重写
    - AOF重写：解决AOF文件不断膨胀的问题
      - fork子进程时会阻塞，重写时不会阻塞主线程
  - RDB和AOF混合：AOF记录两次RDB快照之间的操作


