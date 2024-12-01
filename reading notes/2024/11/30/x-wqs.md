Sequential chain is another type of chains. The idea is to combine multiple chains where the output of the one chain is the input of the next chain.

顺序链是另一种类型的链。将多个链组合在一起，其中一个链的输出就是下个链的输入。

There is two type of sequential chains:

1. SimpleSequentialChain: Single input/output

2. SequentialChain: multiple inputs/outputs

有两种类型的顺序链：
1.SimpleSequentialChain：单个输入/输出
2.SequentialChain：多个输入/输出

 简单顺序链，每条链都有一个输入、一个输出，一条接一条。

当使用大语言模型进行聊天对话时，大语言模型自身其实是无状态的。语言模型并不会记住和你对话时产生的历史消息。每次的请求交互、调用api都是独立的。聊天机器人看起来有记忆，实际上是因为借助了代码，提供历史消息作为和LLM对话的上下文。所以记忆存储可以明确地存储到目前为止的对话消息。在LLM生成输出时，可以基于之前所说过的会话内容，再生成新的回话，让你感觉它好像“记得”你说过的话。

Large Language Models are "stateless".

Each transaction is independent.

Chatbots appear to have memory by providing the full conversation as "context".

随着对话变得越来越长，记忆存储量也会变得非常非常大，向LLM发送大量的Token的成本也会逐渐增加，因为通常是根据需要处理的token数量收费。LangChain提供了集中便捷的记忆存储模式，来存储对话消息、累积对话内容。

LangChain provides several kinds of "memory" to store and accumulate the conversation.
