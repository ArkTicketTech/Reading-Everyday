## 《SRE：Google运维解密》

SRE就是运行和管理这百万台服务器和众多分布式系统的关键。

SRE（Site Reliability Engineering）

##  前言

相比最后的软件结果、架构设计而言，真实的设计过程、作者本身的思考经历更有价值。实现细节永远只是短暂存在的，但是文档化的设计过程却是无价之宝。

## 序言

这样一种职业必须具备非常广泛的技能，但是和其他职业的专注点都不同。Google 将这个职位称为站点可靠性工程师（SRE,Site Reliability Engineering）。

 SRE 使用计算机科学和软件工程手段来设计和研发大型、分布式计算机软件系统。有的时候，SRE 和产品研发团队共同工作，其他时候我们需要开发这些系统的额外组件：例如备份系统和负载均衡系统等。理想情况下，同时推进这些组件在多个项目中复用。还有的时候，我们的任务是想出各种各样的办法用现有组件解决新的问题。

SRE 的主要工作是运维在分布式集群管理系统上运行的具体业务服务（service）。不论是遍布全球的存储服务，还是亿万用户赖以工作的 E-mail 服务，还是 Google 最初的 Web 搜索服务。SRE 中的“S”最开始指代的就是google.com的运维服务，因为SRE的第一个工作就是维持网站的正常运转。随着时间的推移，SRE 逐渐接管了 Google 内部绝大部分产品系统，包括 Google Cloud Platform 这类开发者平台，也包括内部的一些非网站类的基础设施系统，例如 Bigtable。

按照这个标准来招聘和管理SRE团队，我们很快发现SRE团队成员具有如下特点： 

（a）对重复性、手工性的操作有天然的排斥感。

（b）有足够的技术能力快速开发出软件系统以替代手工操作。

从本质上来说，SRE 就是在用软件工程的思维和方法论完成以前由系统管理员团队手动完成的任务。这些SRE倾向于通过设计、构建自动化工具来取代人工操作。

Google的经验法则是，SRE团队必须将50%的精力花在真实的开发工作上。

我们可以认为DevOps是SRE核心理念的普适版，可以用于更广范围内的组织结构、管理结构和人员安排。同时，SRE是DevOps模型在Google的具体实践，带有一些特别的扩展。

## SRE方法论

将生产环境中发现的Bug和产生的工单转给研发管理人员去分配，或者将开发团队成员加入到轮值on-call体系中共同承担轮值压力等。

SRE 处理运维工作的一项准则是：在每8～12小时的on-call 轮值期间最多只处理两个紧急事件。

Google 的一项准则是“对事不对人”，事后总结的目标是尽早发现和堵住漏洞，而不是通过流程去绕过和掩盖它们。

## 硬件

对Google来说，并不存在一个具体的物理服务器运行我们的邮件系统，而是采用一套集群管理系统进行资源分配，它的名称为Borg。

Google的数据中心是由一套全球覆盖的骨干网B4连接起来的。B4 是基于SDN网络技术（使用OpenFlow标准协议）构建的，可以给中型规模的骨干网络提供海量带宽，同时可以利用动态带宽管理优化网络连接，最大化平均带宽。

## 琐事繁多是不是一定不好

没有人可以通过不停地做脏活累活满足自己的职业发展。
