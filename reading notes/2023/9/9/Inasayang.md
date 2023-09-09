### CHAPTER 10: DESIGN A NOTIFICATION SYSTEM

通知不仅仅是移动推送通知。通知格式有三种：移动推送通知、短信和电子邮件。



#### Step 1 - Understand the problem and establish design scope

-   Candidate: 系统支持哪些类型的通知？
-   Interviewer: 推送通知、短信和电子邮件。
-   Candidate:  是实时系统吗？
-   Interviewer:  假设这是一个软实时系统。我们希望用户能尽快收到通知。但是，如果系统的工作量很大，稍有延迟也是可以接受的。
-   Candidate: 支持哪些设备？
-   Interviewer:   iOS devices, android devices, and laptop/desktop.
-   Candidate:  什么会触发通知？
-   Interviewer:  通知可由客户端应用程序触发。它们也可以在服务器端安排。
-   Candidate:  用户能否选择退出？
-   Interviewer:   是的，选择退出的用户将不再收到通知。
-   Candidate:  每天发出多少通知？
-   Interviewer:   1000 万条移动推送通知、100 万条短信和 500 万封电子邮件。



#### Step 2 - Propose high-level design and get buy-in

##### Different types of notifications

-   iOS push notification

    ​	![](https://inasa.dev/image/systemdesign/10/1.png)

    -   主要需要三个组件来发送 iOS 推送通知

        -   Provider

            -   构建并向 Apple 推送通知服务 (APNS) 发送通知请求

                -   Device token: 发送推送通知的唯一标识符

                -   Payload: 一个 JSON 字典，其中包含通知的有效载荷

                    ![](https://inasa.dev/image/systemdesign/10/2.png)

        -   APNS

            -   Apple 提供的一项远程服务，用于向 iOS 设备传播推送通知

        -   iOS Device

-   Android push notification

    -   安卓系统也采用了类似的通知流程。向安卓设备发送推送通知通常不使用 APN，而是使用 Firebase Cloud Messaging（FCM）

        ![](https://inasa.dev/image/systemdesign/10/3.png)

-   SMS message

    -   通过第三方短信服务

        ![](https://inasa.dev/image/systemdesign/10/4.png)

-   Email

    -   虽然公司可以建立自己的电子邮件服务器，但许多公司还是选择了商业电子邮件服务

        ![](https://inasa.dev/image/systemdesign/10/5.png)

![](https://inasa.dev/image/systemdesign/10/6.png)

##### Contact info gathering flow

要发送通知，我们需要收集移动设备令牌、电话号码或电子邮件地址。

当用户安装应用程序或首次注册时，API 服务器会收集用户联系信息并将其存储到数据库中。

![](https://inasa.dev/image/systemdesign/10/7.png)

电子邮件地址和电话号码存储在用户表中，而设备令牌存储在设备表中。一个用户可以有多个设备，这表明推送通知可以发送到所有用户设备。

![](https://inasa.dev/image/systemdesign/10/8.png)



##### Notification sending/receiving flow

-   High-level design

    ![](https://inasa.dev/image/systemdesign/10/9.png)

    -   Service 1 to N
        -   服务可以是一个微服务、一个 cron 作业或一个触发通知发送事件的分布式系统。例如，计费服务会发送电子邮件提醒客户到期付款，购物网站会通过短信告知客户包裹将于明天送达。
    -   Notification system
        -   通知系统是发送/接收通知的核心。从简单的开始，只使用一个通知服务器。它为 1 到 N 服务提供应用程序接口，并为第三方服务创建通知有效载荷。
    -   Third-party services
        -   第三方服务负责向用户发送通知。在与第三方服务集成时，我们需要格外注意可扩展性。良好的可扩展性意味着系统具有灵活性，可以方便地插入或拔出第三方服务。另一个重要的考虑因素是，第三方服务可能在新市场或未来无法使用。例如，FCM 在中国就无法使用。因此，中国使用 Jpush、PushY 等其他第三方服务。
    -   iOS, Android, SMS, Email
        -   Single point of failure (SPOF)
            -   单一通知服务器意味着 SPOF
        -   Hard to scale
            -   通知系统在一台服务器上处理与推送通知相关的所有事务。独立扩展数据库、缓存和不同的通知处理组件具有挑战性。
        -   Performance bottleneck
            -   处理和发送通知可能需要大量资源。例如，构建 HTML 页面和等待第三方服务的响应都需要时间。在一个系统中处理所有事务可能会导致系统超负荷，尤其是在高峰时段。
    -   

-   High-level design (improved)
    -   将数据库和缓存移出通知服务器
    -   添加更多通知服务器并设置自动水平扩展
    -   引入消息队列，将系统组件解耦

![](https://inasa.dev/image/systemdesign/10/10.png)





Pp. 151-160