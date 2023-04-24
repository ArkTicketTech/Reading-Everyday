### 1. Formats for Encoding Data
程序使用的数据编码格式：
1. 内存数据及数据结构，针对 CPU 的访问操作进行优化。
2. 磁盘 IO 和 网络 IO，数据必须编码成子包含的字节序列。

#### Language-Specific Formats
* Java 的 Java.io.Serializable
* Ruby 的 Marshal
* Python 的 pickle
这些编码存在一些问题，如下：
1. 和特定编程语言绑定在一起；
2. 解码过程中需要实例化任意类，可能存在安全问题；
3. 其目标是快速简单编码数据，通常忽略前后兼容性问题；
4. 效率问题。

#### JSON, XML and Binary Variants
JSON、XML 和 CSV 都是文本格式，存在以下问题：
* 数字编码模糊。XML 和 CSV 中难以区分数字和字符串；JSON 中无法区分整型和浮点型，且不能指定浮点精度。
* JSON 和 XML 支持 Unicode 字符串，但是不支持二进制字符串（可通过编码为 BASE64 格式来绕过，但是会增加数据尺寸）。
* XML 和 JSON 支持 schema。
* CSV 不支持 schema，应用程序需要手动定义每行每列的含义。

##### Binary encoding
JSON 的二进制变体（如 MessagePack、BSON、 BJSON 、 UBJSON 、 BISON和Smile）和 XML 的二进制变体，但是使用不广泛。