# \## 查询字符串

很多资源，比如数据库服务，都是可以通过提问题或进行查询来缩小请求资源类型范围的。

假设在一个五金商店的数据库进行查询，以判断产品是否有货。  可以用下列URL来查询web数据库网关，看看编号为12731的条目是否有货。

http://www.joes.com/inventory-check.cgi?item=12731

？右边的内容被称为查询组件。   URL的查询组件和表示网关资源的URL路径组件一起发送给网关资源。 查询字符串以一系列“名/值”对的形式出现， 名值对之间用字符“&”分隔：

http://www.joes.com/inventory-check.cgi?item=12731&color=blue

这个被本书命名为查询字符串（组件），平时都叫做查询参数。   我觉得它其实可以和资源路径算作一体了。

**出处**：[Gourley David,Totty Brian《HTTP权威指南》(2012) P](zotero://select/library/items/WK5NQJZ4)32
