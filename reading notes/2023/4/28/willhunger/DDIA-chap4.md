Thrift 和 Protocol Buffers 都是基于 code generation 工具将 schema 定义转化为特定编程语言的代码。代码能对 schema 进行解码和编码。

PB 字段标识的 required 和 optional 在生成的 binary data 上没有区别，其唯一区别是 required 字段会加上判断该字段是否为空的运行时检查。

##### Filed tags and schema evolution
模式演化的关键：
1. filed tags：filed tags 来标识数据是否有效，如果随意删除或改变了一个 filed tags，那么所有的 binary data 都无法解析。
2. 向前兼容性：
	* 如果向 schema 中新增了一个字段。旧代码读取新写入数据时，可以忽略该字段。具体实现：数据类型的注释能告知解析器跳过特定字节数。
3. 向后兼容性：
		* 新加入的字段不能设置为 required，才能保证新代码可以读到就数据。因此，为了保证向后兼容性，在 schema 部署后所添加的每个字段都必须是 optional 或具有默认值。
4. 只能删除 optional 字段，required 字段永远不能被删除，而且不能再次使用相同的 filed tags（因为可能仍然有写入的数据包含旧的标签号码）

##### Datatypes and schema evolution
如果更换数据的字段类型，可能存在丢失精度或者被截断的风险。例如，int32 到 int64 是没问题的，但想过来是会被截断。

PB 没有数据类型，但是有 repeated 类型。


#### 4. The Metrics of Schemas
1. 支持验证规则，使用简单，支持的编程语言多
2. 编码紧凑
3. 利于输出文档
4. 前后兼容
5. 对于静态语言来说，支持静态语言的编译时类型检查