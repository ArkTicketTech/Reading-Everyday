#### Step 3 - Design deep dive

![](https://inasa.dev/image/systemdesign/07/5.png)

数据中心 ID 和机器 ID 在启动时选择，一般在系统启动运行后固定

数据中心 ID 和机器 ID 的任何更改都需要仔细审查，因为这些值的意外更改可能会导致 ID 冲突

时间戳和序列号 编号在 ID 生成器运行时生成

##### Timestamp

最重要的 41 位组成了时间戳部分。由于时间戳随时间增长，因此 ID 可以按时间排序

![](https://inasa.dev/image/systemdesign/07/6.png)

可用 41 位表示的最大时间戳为

2 ^ 41 - 1 = 2199023255551 milliseconds (ms), which gives us: ~ 69 years =2199023255551 ms / 1000 seconds / 365 days / 24 hours/ 3600 seconds.

ID 生成器将工作 69 年，而如果自定义的纪元时间与今天的日期相近，则会延迟溢出时间。69 年后，我们将需要一个新的纪元时间或采用其他技术来迁移 ID



##### Sequence number

序列号为 12 位，即 2 ^ 12 = 4096 种组合

除非同一服务器在一毫秒内生成了一个以上的 ID，否则该字段的值为 0。理论上，一台机器每毫秒最多可支持 4096 个新 ID。



#### Step 4 - Wrap up

如果面试结束时有多余的时间，这里有一些额外的谈话要点：

-   Clock synchronization（时钟同步）
    -   在设计中，假定 ID 生成服务器具有相同的时钟
    -   当服务器在多个内核上运行时，这一假设可能就不成立了
    -   在多机器情况下也存在同样的挑战
    -   借助Network time protocol
-   Section length tuning（段长度调整）
    -   序列号较少，但时间戳位数较多 对低并发和长期应用有效
-   High availability
    -   由于 ID 生成器是任务关键型系统，因此必须具有高可用性



Pp. 115-119