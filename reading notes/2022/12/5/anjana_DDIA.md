## Chapter 10: Batch Processing批处理系统

As usual, history has a tendency of repeating itself.

> the ideas and lessons from Unix carry over to large-scale, heterogeneous distributed data systems.
>
> UNIX的思想和经验普遍见于大规模、异构分布式数据系统。

### Batch Processing with Unix Tools

#### Simple Log analysis简单日志分析

```shell
cat /var/log/nginx/access.log |
awk '{print $7}' |
sort |
uniq -c |
sort -r -n |
head -n 5
```

`in a matter of seconds` 在几秒钟内

##### Chains of commands versus custom program

a simple program to do the same thing

```ruby
counts=Hash.new(0)

File.open('/var/log/nginx/access.log') do |file|
  file.each do |line|
    url=line.split[6]
    counts[url]+=1
  end
end

top5=counts.map{|url,count| [count,url]}.sort.reverse[0...5]
top5.each{|count,url| puts "#{count} #{url}"}
```

##### Sorting versus in-memory aggregation

排序 VS 内存聚合

排序：

* UNIX流水线
* job's working set is larger than the available memory: make efficient use of disks 高效地使用磁盘
* SSTables and LSM-Tables
  * chunks of data can be sorted in memory 
  * written out to disk as segment files
  * multiple sorted segments can be merged into a larger sorted file

* Merge-sort has sequential access patterns that perform well on disks. ? 不太理解这句。

内存聚合：

* Ruby脚本使用一个URL的内存哈希表
* 适用于大多数中小型网站（取决于URL的不同个数）

发现中文翻译的确不太好。中文副标题为“排序与内存聚合”，刚开始还没反应过来这其实是对日志文件的两种不同处理方式。去翻英文版，发现标题为`Sorting versus in-memory aggregation`，使用了`versus`,马上可以体会到对日志分析的不同处理方法。还是读英文版吧，毕竟是第一手而不是又经过另一个人翻译有偏差的二手资料。阮一峰翻译好的一部分原因是他会保留一些英文表示，可以理解为该书翻译是为了更多读者可以看懂，但versus使用一个“与”代替反而失掉了准确性😅。

#### The Unix Philosophy UNIX设计哲学

##### A uniform interface 统一接口

interface: file文件（file Descriptor文件描述符) 

A file is just an ordered sequence of bytes.

* an actual file on the filesystem
* a `communication channel` to`another process`(Unix socket, stdin, stdout)
* a device driver (say /dev/audio or /dev/Ip0)
* a socket representating a TCP connection

##### Separation of logic and wiring 逻辑与布线分离

比如这个，英文为“Separation of logic and wiring”，也是很明显英文容易理解，当时读中文就在想这什么意思。

stdin、stdout

pros: 

* loose coupling松耦合/late binding后期绑定/inversion of control控制反转

cons: 

* 不利于多个输入输出的程序 Programs that need multiple inputs or outputs are possible but tricky. ？不太懂
* Can't pipe a program's output into a network connection.  ？不太懂

##### Transparentcy and experimentation 透明与测试

### MapReduct and Distributed Filesystems

> 有意思的类比：`MapReduce有点像分布在数千台机器上的UNIX工具。`

A single MapReduce job is comparable to a single Unix process.

* takes one/more inputs
* produces one/more outputs: 
  * are written once in a sequential fashion
  * not modifying any existing part of a file once it has been written

#### HDFS

shard-nothing approach: HDFS

* requires no special hardware 
* only computers connected by a `conventional datacenter network`.

shard-disk approach

* Network Attached Storage(NAS)
* Storage Area Network(SAN)
* implemented by a `centralized storage appliance`集中存储设备, often using custom hardware and special network `infrastructure` such as Fibre Channel

`NameNode`: a center server keeps track of which file blocks are stored on which machine.

#### MapReduce Job Execution

The sorting is performed in stages. 分阶段进行排序。

MapReduce discards the partial output of a `failed job`.



