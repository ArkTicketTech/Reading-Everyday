#### Step 3 - Design deep dive

高层设计没有回答以下问题：

-   如何创建速率限制规则？规则存储在哪里？
-   如何处理速率受限的请求？

##### Rate limiting rules

Lyft中的示例

```
domain: messaging
descriptors:
  - key: message_type
    Value: marketing
    rate_limit:
      unit: day
      requests_per_unit: 5
```

 系统配置允许每天最多 5 条营销信息

```
domain: auth
descriptors:
  - key: auth_type
    Value: login
    rate_limit:
      unit: minute
      requests_per_unit: 5
```

不允许客户在 1 分钟内登录超过 5 次

##### Exceeding the rate limit

如果请求受到速率限制，API 会向客户端返回 HTTP 响应代码 429

根据不同的使用情况，可能会将速率受限的请求挂起，以便稍后处理。例如，如果一些订单因系统超载而受到速率限制，可以保留这些订单，以便稍后处理。

###### Rate limiter headers

客户如何知道自己是否被节流？客户端又如何知道在被节流之前允许的剩余请求数？答案就在 HTTP 响应头中。速率限制器会向客户端返回以下 HTTP 标头：

-   `X-Ratelimit-Remaining`: 窗口内允许的剩余请求数
-   `X-Ratelimit-Limit`: 客户端在每个时间窗口可调用次数
-   `X-Ratelimit-Retry-After`: 在不被节流的情况下再次发出请求之前需要等待的秒数

当用户发送的请求过多时，会向客户端返回 429 (请求过多错误)和 X-Ratelimit-Retry-After 头信息。

##### Detailed design

![](https://inasa.dev/image/systemdesign/04/13.png)

-   规则存储在磁盘上。工作站会经常从磁盘上提取规则并存储到缓存。
-   当客户端向服务器发送请求时，请求会首先发送到速率限制器中间件
-   速率限制器中间件从缓存中加载规则。它从 Redis 缓存中获取计数器和上次请求时间戳。根据响应，速率限制器做出决定
    -   如果请求不受速率限制，则转发给 API 服务器
    -   如果请求受到速率限制，速率限制器会向客户端返回 429 请求过多错误信息。与此同时，请求会被丢弃或转发到队列中

##### Rate limiter in a distributed environment

-   Race condition
-   Synchronization issue

###### Race condition

高层次设计中的速率限制器

-   从 Redis 读取counter
-   检查 ( counter + 1 ) 是否超过阈值
-   如果没有，则将 Redis 中的counter递增 1

高度并发环境中可能会出现Race condition

![](https://inasa.dev/image/systemdesign/04/14.png)

锁是解决竞赛条件最明显的办法。然而，锁会大大降低系统的运行速度。通常有两种策略可以解决这个问题：

-   Lua 脚本
-   Redis 中的排序集数据结构



###### Synchronization issue

当使用多个速率限制器服务器时，就需要同步。

![](https://inasa.dev/image/systemdesign/04/15.png)

一种可能的解决方案是使用粘性会话，允许客户端向同一个速率限制器发送流量。这种解决方案并不可取，因为它既不可扩展，也不灵活。更好的方法是使用 Redis 等集中式数据存储。

![](https://inasa.dev/image/systemdesign/04/16.png)

##### 监控

略

#### Step 4 - Wrap up

-   Token bucket
-   Leaking bucket
-   Fixed window
-   Sliding window log
-   Sliding window counter



硬限制与软限制

-   Hard: 请求数不能超过阈值
-   Soft: 短时间内可以超过



Pp. 64-70