# 索引组织表

在InnoDB存储引擎中，表都是根据主键顺序组织存放的，这种存储方式的表成为索引组织表（index organized table）。每个表都有主键，没有显式定义的会使用唯一索引或者自动创建主键。

# InnoDB逻辑存储结构

所有数据都被逻辑地存放在一个空间中，称之为表空间（tablespace）。表空间又由段（segment）、区（extent）、页（page）组成。


## 表空间

所有数据都存放在表空间，默认共享表空间，如果启动了参数innodb_file_per_table，数据、索引、插入缓冲bitmap页存放在独立表空间，其他如undo、插入缓冲索引页、系统事务信息 、doublewrite buffer还是存放在共享表空间。

## 段

表空间是由各个段组成的，常见的段有数据段、索引段、回滚段等。

InnoDB是索引组织的，数据即索引，索引即数据。

数据段即为B+树的叶子结点，索引段即为B+树的非索引结点。



## 区

区是连续页组成的空间，在任何情况下每个区的大小都为1MB。为了保证区中页的连续性，InnoDB一次性从磁盘申请4～5个区。默认情况下，InnoDB页的大小为16KB，即一个区中一共有64个连续的页。

## 页

页是InnoDB磁盘管理的最小单位。

默认页大小为16KB，但通过innodb_page_size可以修改页的大小。

## 行

row

InnoDB是面向行的，也就是说数据是按照行进行存放的。

# InnoDB行记录格式

compact行记录格式：MySQL5.0中引入的，其设计目的是高效地存储数据。简单来说，一个页存放的行数越多，其性能就越高。


redundant行记录格式：为了兼容MySQL5.0之前版本的页格式


行溢出数据：有时可以将大对象列数据放在数据页面之外。

Compressed和Dynamic行记录格式：


char的行结构存储：





# InnoDB数据页结构

1. File Header：记录页的一些头信息
2. Page Header：记录数据页的状态信息
3. Infimun和Supremum Record：限定记录边界
4. User Records：行记录
5. Free Space：空闲空间，当记录被删除后，该空间会被加入到空闲链表中
6. Page Directory：页目录，存放的是记录的相对位置
7. File Trailer：检测页是否已经完整地写入磁盘


# 约束

参数sql_mode需要设置为严格约束

约束有：唯一约束、外键约束、触发器约束

# 分区表

分区不是在存储引擎完成的。分区的过程是将一个表或索引分解为多个更小、更可管理的部分。

MySQL支持的分区类型是水平分区，分区是局部分区索引，一个分区中既存了数据又存放了索引。

```
show variables like '%partition'
```

分区的目的主要是减轻大文件的压力，分区需要有分区键，

1. Range分区：

```
create table t( id int ) engine = INNDB partition by range(id)( partition p0 values less than(10), partition p1 values less than(20) );
```

1. List分区

```
create table t( id int, b int ) engine = INNDB partition by List(b)( partition p0 values in (1, 3, 5, 7, 9), partition p1 values in (0, 2, 4, 6, 8) );
```

1. Hash分区

```
create table t( id int, b Datetime ) engine = INNDB partition by hash(year(b))(  // mod(year(b), 4) partitions 4 ); 
```

1. Key分区

```sql
create table t(
	id int,
	b Datetime
) engine = INNDB
partition by key(b)(  // mod(md5(b), 4)
	partitions 4
);
```

key分区与hash分区的区别是：hash分区是用用户自定义的hash函数，key分区使用mysql内置的md5函数。

1. columns分区
2. 子分区：分区的基础上再进行分区

不论创建何种类型的分区，如果表中主键或唯一索引时，分区列必须是唯一索引的一个组成部分。



## 分区和性能

适用于OLAP。

如果100w和1000w行的数据本身构成的b+树的层次都一样的，那IO查询次数没有变化，分区就没有意义；

如果1000万的层数比100万的层数多一层，那么分区可以避免一次IO，但如果是`select * from table where key = 'k001'`，有可能要查询所有的区，那么一次查询的IO比未分区前多了「分区数」倍。

还要注意范围查询在分区上的性能。
