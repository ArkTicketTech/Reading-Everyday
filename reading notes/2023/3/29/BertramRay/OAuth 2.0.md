- RFC 6749 

- OAuth 2.0 的运行流程

- （A）用户打开客户端以后，客户端要求用户给予授权。

  （B）用户同意给予客户端授权。

  （C）客户端使用上一步获得的授权，向认证服务器申请令牌。

  （D）认证服务器对客户端进行认证以后，确认无误，同意发放令牌。

  （E）客户端使用令牌，向资源服务器申请获取资源。

  （F）资源服务器确认令牌无误，同意向客户端开放资源。

- 关键是其中的B步骤
  - client: third-party application
  - resource owner: user
  - user agent: browser
- OAuth 2.0 定义了4种授权方式
  - 授权码模式（authorization code）
  - 简化模式（implicit）
  - 密码模式（resource owner password credentials）
  - 客户端模式（client credentials）
- 授权码模式是最为严密的一种liu'cheng