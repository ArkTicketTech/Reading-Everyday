#### Step 3 - Design deep dive

-   Reliability

    -   How to prevent data loss?

        -   通知系统最重要的要求之一就是不能丢失数据。通知通常可以延迟或重新排序，但绝对不能丢失。为满足这一要求，通知系统将通知数据持久保存在数据库中，并实施重试机制。

            ![](https://inasa.dev/image/systemdesign/10/12.png)

    -   Will recipients receive a notification exactly once?

        -   虽然在大多数情况下，通知会准确地传递一次，但分布式的特性可能会导致重复通知。为了减少重复通知的发生，我们引入了一种扣减机制，并谨慎处理每一个失败案例。
        -   当一个通知事件首次到达时，我们会通过检查事件 ID 来查看它是否在之前出现过。如果是以前发生过的事件，则将其丢弃。否则，我们将发送通知。

-   Additional component and considerations

    -   Notification template

        -   大型通知系统每天要发送数百万个通知，其中许多通知都采用类似的格式。引入通知模板就是为了避免从头开始创建每个通知。通知模板是一种预格式化的通知，可通过自定义参数、样式、跟踪链接等创建自己独特的通知。

            ```
            BODY:
            You dreamed of it. We dared it. [ITEM NAME] is back — only until [DATE].
            CTA:
            Order Now. Or, Save My [ITEM NAME]
            ```

            

    -   Notification setting

        -   一般来说，用户每天都会收到太多的通知，他们很容易感到不知所措。因此，许多网站和应用程序都允许用户对通知设置进行精细控制。这些信息存储在通知设置表中，其中包括以下字段：

            -   user_id   bigInt
            -   channel  varchar    # push notification, email or SMS
            -   opt_in   boolean    # opt-in to receive notification

            在向用户发送任何通知之前，我们首先会检查用户是否已选择接收此类通知。

    -   Rate limiting

        -   为了避免过多的通知让用户不知所措，我们可以限制用户可接收的通知数量。这一点很重要，因为如果发送次数过多，接收者可能会完全关闭通知。

    -   Retry mechanism

        -   当第三方服务发送通知失败时，通知将被添加到消息队列中重试。如果问题仍然存在，将向开发人员发出警报。

    -   Security in push notifications

        -   对于 iOS 或 Android 应用程序，appKey 和 appSecret 用于确保推送通知 API 的安全。只有经过认证或验证的客户端才能使用我们的 API 发送推送通知。

    -   Monitor queued notifications

        -   需要监控的一个关键指标是排队通知的总数。如果数量很大，说明工人处理通知事件的速度不够快。为了避免通知交付延迟，需要增加工人数量。

    -   Events tracking

        -   通知指标（如打开率、点击率和参与度）对于了解客户行为非常重要。分析服务实现事件跟踪。通知系统和分析服务之间通常需要集成。

            ![](https://inasa.dev/image/systemdesign/10/13.png)

-   Updated design

    ![](https://inasa.dev/image/systemdesign/10/14.png)

    -   通知服务器还具备两个关键功能：验证和速率限制
    -   添加了重试机制来处理通知失败。如果系统发送通知失败，它们会被放回消息队列，工人会重试预定次数。
    -   通知模板还提供了一致、高效的通知创建流程
    -   增加了监测和跟踪系统，用于系统健康检查和未来改进

#### Step 4 - Wrap up

在本章中，介绍了支持多种通知格式的可扩展通知系统的设计： 推送通知、短信和电子邮件。我们采用消息队列来解耦系统组件。

除了高层设计，我们还深入研究了更多组件和优化方案。

-   Reliability: 提出了一种稳健的重试机制，以尽量降低失败率
-   Security: AppKey/appSecret 对用于确保只有经过验证的客户端才能发送通知
-   Tracking and monitoring: 这些功能可在通知流程的任何阶段实施，以获取重要统计信息
-   Respect user settings: 用户可以选择不接收通知。系统会先检查用户设置，然后再发送通知
-   Rate limiting: 用户会喜欢对收到的通知数量设置频率上限



Pp. 160-165