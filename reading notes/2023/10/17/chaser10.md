1. mysql架构分为两层 server层和存储引擎层
	1. server层负责**建立连接、分析和执行sql**
	2. 存储引擎负责数据的存储和提取

#### 一条语句怎么执行的
1. 连接器
	1. mysql基于tcp长连接进行数据传输
2. 查询缓存
3. 解析sql
	1. 词法分析
	2. 语法分析
4. 执行sql
	1. prepare阶段，预处理阶段  检查表、字段是否存在
	2. optimize阶段，确定执行方案
	3. execute阶段，执行器调用存储引擎提供的接口获取一条记录 判断能否返回给用户 然后继续获取记录

#### 一行记录如何存储
##### 一个表有哪些文件
- db.opt 数据库的默认字符集和字符校验规则
- .frm 表结构
- .ibd 独占表空间 表数据既可存在共享表空间文件(ibdata1) 也可以存放独占表空间

###### 表空间
1. 记录都是按行进行存放的  每行记录根据**不同的行格式，有不同的存储结构**
2. 数据是按页单位来进行读写，为什么呢 因为局部性原理，读取一行可能还要读取相邻行，这样读写都能在缓存中进行而不用多次的读取-修改-写回，数据页大小是16kb也更好的利用了内存页。页的类型有很多种，包括数据页、undo日志页、溢出页。数据表中的行记录时用数据页来管理的
3. 为了能够让索引直接进行顺序io，而不是随机io，那么就让链表中相邻的页物理位置也相邻 所以当表中数据量大时就不再按照页为单位进行分配了，而是按照区为分配，连续的64个页会被划为一个页，这样就使得链表中相邻的页的物理位置也相邻，就能使用顺序io了，所以区也是物理分区
4. 表空间由各个段组成，段由各个区组成。段是一个逻辑分区，分为索引段：存放b+树的非叶子节点的区的集合，数据段：存放b+树的叶子节点的区的集合 回滚段：存放的是回滚数据的区的集合

###### 行格式
1. redundant 老古董
2. compact
3. dynamic 默认 compressed 紧凑的行格式

###### compact行格式
1. 记录的额外信息 变长字段列表+null值列表+记录头信息 当一行数据都没有变长字段 或者没有null值就没有对应的列表
2. 记录的真实数据