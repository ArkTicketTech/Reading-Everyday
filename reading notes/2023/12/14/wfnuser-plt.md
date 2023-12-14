## 断言

本章我们会介绍如何形式化地描述类型系统。让我们从断言(judgments)的形式化描述开始。

一个典型的断言形式如下：
$$ \Gamma \vdash J $$

> $J$ 是断言，其中的自由变量在 $\Gamma$ 中定义。

我们称为 $\Gamma$ 蕴含 $J$。 这里的 $\Gamma$ 是一个静态类型环境(static typing environment)；比如，它可以是一系列不同的变量和对应的类型组成，形式化的描述为： $\phi,x_1:A_1,...,x_n:A_n$ 。 空的环境用 $\phi$ 表示， $\Gamma$ 中的变量中用 $dom(\Gamma)$ 表示，意为 $\Gamma$ 的域(domain)。