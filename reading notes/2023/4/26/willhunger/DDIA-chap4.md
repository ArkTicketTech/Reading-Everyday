Thrift 的二进制编码格式：
1. BianryProtocal：**每个字段都有一个类型注释**（用于指示它是否是字符串 、 整数、 列表等）， 并且可以在需要时指定长度（包括字符串的长度、 列表中的项数），**但没有字段名，以紧凑的字段标签来标识字段名**
2. CompactProtocol：语义上和 BianryProtocal 相同，它通过将字段类型和标签号打包到单字节中， 并使用可变长度整数来实现。 对数字1337, 不使用全部8字节， 而是使用两个字节进行编码，每字节的最高位用来指示是否还有更多的字节。
3. DenseProtocal，仅在 C++ 实现
4. 此外，其还具有两种不同的基于 JSON 的编码格式

Protocol Buffer 和 Thrift 的 CompactProtocol 相似。