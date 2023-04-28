### 2. Modes of Dataflow
#### 1. Dataflow Through Databases
靠数据来保证数据流传递的可靠性。
> 定期 dump 数据可以可以包括不同时期的不同 schema。

#### 2. Dataflow Through Services: Rest and RPC
微服务设计目标：通过使服务可独立部署和演化，让程序更易于更改和维护。
##### web service
* REST：基于 HTTP 原则的设计理念，强调简单的数据格式，使用 URI 来标识资源，使用 HTTP 协议来进行缓存控制、身份验证等功能。
* SOAP：基于 XML，避免使用 HTTP 功能，附带了较多的相关标准（例如 Web 服务框架等）

##### The problems with remote procedure calls
RPC 可能存在的问题：
* 网络问题，不清楚具体是什么情况。例如，response 丢失，会导致 rpc timeout 重试多次，导致多次调用开销。
* RPC 调用的开销问题，网络 IO
* 需要考虑不同编程语言之间的具体实现，例如数据结构

##### Current directions for RPC
* 明确了远程请求和本地函数调用之间的区别
	* Finagle 和 Rest.Ii 使用 Futures (Promises)来封装可能失败的异步操作。
	* gRPC 支持 streams。
* RPC 框架支持服务发现机制。

REST 的优势：
* 利于实验和调试
* 支持主流编程语言和平台
* 生态完善（服务器、缓存、代理等工具）

相反，RPC 框架更加适合于同一组织内多项服务之间的情况，通常发生在同一数据中心内。

##### Data encoding and evolution for RPC
RPC 方案的向后和向前兼容性取决于它使用的具体编码技术。

RPC 服务需要长期保持兼容性，如果要进行一些破坏兼容性的更改，则服务提供者往往需要同时维护多个版本的 API，易造成版本碎片。