# 范里安 - 微观经济学：现代经济观点

## 15 市场需求

在前面几章，分析了如何为单个消费者的选择建立模型。在这一章，将研究如何把单个消费者的选择加总在一起，以得到总的市场需求。一旦推导出了市场需求曲线，就可以考察它的某些性质，例如需求和收益之间的关系。

### 15.1 从个人需求到市场需求

1. 个人需求的加总

   个人需求的加总假设有$n$个消费者，如果用$x_i^1(p_1,p_2,m_i)$表示第$i$个消费者对商品1的需求函数，则商品1的市场需求，又叫商品1的总需求，就是这些消费者的个人需求的总和：
   $$
   X^{1}(p_{1},p_{2},m_{1},\cdots,m_{n})=\sum_{i=1}^{n}x_{i}^{i}(p_{1},p_{2},m_{i})
   $$

2. 市场需求曲线

   1. 含义：假设市场上只有两种商品1和2，它们的总需求函数分别是$X^1(p_1,p_2,M)$和$X^2(p_1,p_2,M)$，给定商品2的价格和总收入，就可以得到商品1的总需求和它的价格之间的关系。表现在价格——需求图上，就得到了总需求曲线。假设其他商品价格不变，收入不变，效用函数不变。

   2. 图像：

      <img src="assets/image-20231203224441264.png" alt="image-20231203224441264" style="zoom:33%;" />

   3. 分析：假设总需求函数具有$X^{1}(p_1,p_2,M)$的形式，$M$是所有消费者收入的总和。根据这个假设，整个市场的总需求就如同某个人在价格$(p_1,p_2)$和收入$M$下的需求。每个人对每种商品的需求取决于价格和他的货币收入，所以一般说来，总需求将取决于价格和每个消费者的收入（一个人=所有人总和），而非价格和总收入。然而，如果总需求可以看作某个收入恰好等于所有个人收入总和的“代表性消费者”的需求，这会使某些问题的研究变得非常方便，即假设每个消费者的需求函数对于收入是可加总的。一般而言，这个结论并不成立。
      $$
      \text{例：}x_i^1=\dfrac{m_i}{2p_1},\text{则}X^1=\dfrac{\sum m_i}{2p_1}=\dfrac{M}{2p_1}\rightarrow X^1=X^1(p_1,p_2,M)
      $$
   
   4. 性质：如果商品1和商品2是替代品，这样当商品2的价格上涨时，会导致商品1的需求增加（不管商品1是什么价格）。这就意味着商品2的价格上升，会导致商品1的总需求曲线向外移动。如果商品1和商品2是互补品，商品2价格的下降就会使商品1的总需求曲线向外移动，如下图所示。
   
      如果其他商品价格变如，或者收入变动，市场需求曲线都会移动。假设商品一和商品二是替代的，则$\frac{\partial x_1}{\partial p_2}>0$，商品二价格上升，商品一的需求上升，因此曲线向右移动。
   
      假设商品一是正常品，则$\frac{\partial x_1}{\partial M}>0$，收入上升，则商品一需求上升，需求曲线向右移动。
   
      <img src="assets/image-20231204092908198.png" alt="image-20231204092908198" style="zoom:33%;" />

### 15.2 反需求函数

1. 概念：价格是数量的函数，反需求函数$P(X)$，度量的是当人们需求$X$单位的商品1时，对该商品必须支付的市场价格。一种商品的价格度量的是该商品与所有其他商品之间的边际替代率$(MRS)$，反需求函数度量的是每个购买这种商品的消费者的边际替代率或边际支付意愿。

2. 需求函数：$Q_d=Q(P)$ 反需求函数：$P=P(Q_d)=Q^{-1}(Q_d)$
   如果我们将商品二看作其他商品支出，则每个消费者的均衡等式为$p_1=|MRS_{12}^i|$，上标$i$表示消费者$i$，由于每个人面临的市场价格是一样的：$p_{1}$,所以每个人的边际替代率也是一样的，我们在第六章结尾处讲述过，这里的边际替代率又称作边际支付意愿：消费者为了多消费一单位商品一所愿意付出的货币量，因此所有人的边际支付意愿相同，这个结论的核心就是市场价格一致。

   需求曲线如何加总？ (连续型)例：$D_1(p)=\max\left\{20-p,0\right\},D_2(p)=\max\left(10-2p,0\right)$

3. 例：“线性”需求曲线的加总

   连续型
   $$
   D_1(p)=\max\{20-p,0\} \\
   D_2(p)=\max\{10-2p\text{,}0\}
   $$
   代数法：$D(p)=D_1(p)+D_2(p)=\max(30-3p,20-p)$

   图像法：

   

   假设某个人的需求曲线是$D_1(p)=20-p$，另一个人的需求曲线是$D_2(p)=10-2p$。个人需求函数的形式为
   $$
   D_1(p)=\max\{20-p,0\} \\
   D_2(p)=\max\{10-2p\text{,}0\}
   $$
   两条需求曲线加总后得到如图所示的曲线。注意，市场需求曲线在$p=5$处扭折。需要考虑分类讨论

   <img src="assets/image-20231204091110771.png" alt="image-20231204091110771" style="zoom:33%;" />

### 15.3 离散商品

分析：离散情况下，需求曲线反映的是消费者需求用保留价格购买一个单位，而在连续情况下，只有拟线性偏好拥有这种解释。在这种情况下，市场需求曲线一定“向下倾斜”，因为市场价格下降一定会使至少愿意按下降后的价格支付的消费者增加。

图像：

<img src="assets/image-20231204092414419.png" alt="image-20231204092414419" style="zoom:33%;" />

### 15.4 广延边际和集约边际

1. 广延边际调整：消费者做出是否消费这种或那种商品的决定。$D(P)=0$ 时，$\Delta p$引起的$\Delta D$的变化称作广延边际上的调整。

2. 集约边际调整：消费者做出的对这种或那种商品消费的更多一些或更少一些的决定。$D(P)>0$ 时，$\Delta p$引起的$\Delta D$的变化称作集约边际上的调整。

3. 评价：对正常商品（$\frac{\part x_1}{\part M}>0$）必是普通商品（$\frac{\part {x_1}}{\part p_1}$）来说，集约边际上的调整是在“正常”的方向上进行的，当价格上升时，需求数量就下降。广延边际上的调整也在“正常”的方向上进行。一般可以预期，总需求曲线是向下倾斜的。总需求曲线斜率受到这两种决策的影响。比如在线性需求的加总中，当价格降到20元时，消费者1进行广延边际调整，此后他只进行集约边际的调整，如图15 -3所示。

   集约边际调整和广延边际调整最终都是表现为市场需求曲线向右下倾斜。

   <img src="assets/image-20231204094023997.png" alt="image-20231204094023997" style="zoom:33%;" />

### 15.5 弹性（$\frac{\mathrm d \ln x_1}{\mathrm d \ln p_1}$）

需求曲线斜率：$\frac{{d}q}{dx}$，表示价格变动一单位，商品需求量的变动数量。有时我们更希望研究价格和需求量之间的百分比变化。需求价格弹性：$\varepsilon=\frac{\Delta q/q}{\Delta p/p}=\frac{\Delta q}{\Delta p}\frac pq=\frac{d\ln q}{d\ln p}$，表示价格变动，商品需求量变动的百分比，这种形式下，最终求得的结果是没有单位的。当$\varepsilon=-1$时，表明价格上升，需求量下降

例：线性需求函数的弹性需求曲线：
$q=a-bp$
弹性为$\varepsilon=\frac{dq}{dp}\frac pq=\frac{-bp}{a-bp}$,可知弹性与价格水平有关，反映在图像上就是不同点的弹性不同，我们可以使用三角形法快速判断弹性水平(视频讲)。

1. 目的：需求曲线的斜率依赖于需求和价格的计量单位。如果用不同的单位测度，斜率也会不同，因此选择与计量单位无关的测度敏感度方法，即弹性——对敏感度的测度。

2. 弹性：计量一个变量对另一个变量变化的敏感度的一个概念，具体来说它计量的是针对一个变量的百分之一的变化，另一个变量相应变化的百分比。可以表示为价格对数量的比率与需求曲线斜率的乘积。其一般表达式为：
   $$
   E_{AB}=\frac{\triangle A/A}{\triangle B/B}=\frac BA\cdot\frac{\triangle A}{\triangle B}
   $$

3. 需求价格弹性$\varepsilon$：**销售数量的百分比变动除以价格的百分比变动**。用百分比度量增加幅度就使得弹性与计量单位无关。
   $$
   \varepsilon=\frac{\triangle q/q}{\triangle p/p}=\frac{\mathrm{d}q}{\mathrm{d}p}\cdot\frac pq
   $$
   由于需求曲线斜率为负，所以需求价格弹性符号一般也是负的，因此我们一般使用绝对值比较。

4. 例：线性需求曲线的弹性

   设线性需求曲线$q=a-bp$，斜率是常数$-\frac{1}{b}$，代入弹性公式：
   $$
   \varepsilon=\frac{-bp}q=\frac{-bp}{a-bp}
   $$
   当$p=0$时，需求弹性等于零。当$q=0$时，需求弹性是负无穷大。在哪个价格上，需求弹性等于-1？为了求得这个价格，我们列出方程
   $$
   \frac{-bp}{a-bp}=-1
   $$
   求解$p$，我们得到：
   $$
   p=\frac a{2b}
   $$
   <img src="assets/image-20231204102618369-1656779.png" alt="image-20231204102618369" style="zoom:33%;" />

   价格位于需求曲线中点

找点弹性，连接原点，做出三角形一条边，垂直，垂线，$q/x$。

### 15.6 弹性与需求

1. 弹性需求的分类
   + 弹性需求：如果一种商品的需求价格弹性的绝对值大于1，就说这种商品具有弹性需求。
   + 商品无弹性需求：如果一种商品需求价格弹性的绝对值小于1。
   + 单位弹性需求：如果一种商品的需求价格弹性恰好等于1，就说这种商品具有单位弹性需求。
2. 分析：对于弹性需求曲线，需求数量对价格的反应是非常敏感的：如果价格上升1%，需求的下降幅度就会超过1%。因此，把弹性看作需求数量对价格的敏感度，就可以很容易地记住弹性和无弹性的含义。**一般地，商品的需求弹性在很大程度上取决于它有多少相近的替代品**。存在完全替代品，弹性很大。

### 15.7 弹性与收益

1. 收益：商品的价格与它的销售量的乘积，即$R=pq$。收益的增加或减少取决于需求对价格变动的敏感程度。

2. 价格弹性与收益变动的关系
   $$
   R=pq
   $$
   如果价格变动至$p+\Delta p$，需求数量变动至$q+\Delta q$，新的收益为：
   $$
   \begin{aligned}R^{\prime}&=(p+\Delta p)(q+\Delta q)\\&=pq+q\Delta p+p\Delta q+\Delta p\Delta q\end{aligned}
   $$
   $R'-R$，得：
   $$
   \Delta R=q\Delta p+p\Delta q+\Delta p\Delta q
   $$
   忽略$\Delta p$和$\Delta q$，可以得到以下形式的收益变动表达式：
   $$
   \Delta R=q\Delta p+p\Delta q
   $$
   收益变动大约等于初始需求数量与价格变动的乘积再加上初始价格与需求数量变动的乘积。如果我们想要得到相对于价格变动的收益变化率的表达式，只要将上式除以$\triangle p$，可得：
   $$
   \frac{\Delta R}{\Delta p}=q+p\left.\frac{\Delta q}{\Delta p}=q(1-|\varepsilon|)\right.
   $$
   当
   $$
   \frac{\Delta R}{\Delta p}=p\left.\frac{\Delta q}{\Delta p}+q(p)>0\right. \\
   \frac pq\frac{\Delta q}{\Delta p}>-1
   $$
   表达式的左边是$ε(p)$，它是一个负数。两边乘以-1改变不等号的方向，我们得到
   $$
   |ε(p)|<1
   $$
   因此，如果需求弹性的绝对值小于1，当价格上升时收益就会增加。如果需求弹性的绝对值大于1，当价格上升时收益就会下降。

   证明：

   1. 方法二
      $$
      \begin{aligned}\Delta R&=p\Delta q+q\Delta p>0\\-\frac pq\frac{\Delta q}{\Delta p}&=\mid\varepsilon(p)\mid<1\end{aligned}
      $$

   2. 方法三
      $$
      \frac{\Delta R}{\Delta p}=q+p\frac{\Delta q}{\Delta p} \\
      \begin{aligned}=&q\left[1+\frac bq\left.\frac{\Delta q}{\Delta p}\right]\right.\\=&q\left[1+\varepsilon\left(p\right)\right]\end{aligned}
      $$
      弹性需求为负，所以表达式可以记为：
      $$
      \frac{\Delta R}{\Delta p}=q\begin{bmatrix}1-|\varepsilon(p)&\end{bmatrix}
      $$
      如果弹性绝对值大于1，$\frac{\Delta R}{\Delta p}$是负值。

3. 价格变动时收益变动图像：

   <img src="assets/image-20231204112352700.png" alt="image-20231204112352700" style="zoom:33%;" />

   分析：收益恰好是方框的面积。价格乘以数量。当价格上升时，我们在方框的上方添加一个面积约为$q\Delta p$的矩形，同时在方框的右边减去一个面积近似于$p\Delta q$的矩形。对于价格和数量的微小变化，这恰好就是上面所给出的那个表达式。（略去的部分$\Delta p\Delta q$，就是方框右上角的小矩形，相对于其他数量，它是非常微小的。)

4. 结论

   弹性与收益有如下关系： 某种商品富有弹性时，涨价会引起收益下降；当缺乏弹性时，涨价会引起收益上升；当具有单位弹性时，价格变化对收益没有影响。

5. 例：罢工和利润

   莴苣市场例子。



补充：
$$
\text{见 }\frac{dR}{dp}=q{\left(1+\frac pq\frac{dq}{dp}\right)}=q{\left(1+\varepsilon(p)\right)}=q{\left(1-|\varepsilon(p)|\right)}
$$
进一步整理发现也就是说，正负取决于弹性的大小：

若$|\varepsilon|<1$，则$\frac{dR}{dp}>0$：弹性小说明对价格不敏感，提价消费者照样买。

若$|\varepsilon|>1$，则$\frac{dR}{dp}<0$：弹性大说明消费者对价格非常敏感，稍微提价就都不买了。 

### 15.8 弹性不变的需求

1. 具有不变弹性$-1$的需求函数：
   $$
   pq = \overline R \\
   q=\frac{\overline R}{p}
   $$

2. 图像：

   <img src="assets/image-20231204121647131.png" alt="image-20231204121647131" style="zoom:33%;" />

   价格与数量乘积是一个常数。

3. 具有不变弹性$\varepsilon$的需求一般公式：
   $$
   q=Ap^{\varepsilon}
   $$
   $A$为任意的正常数，弹性$ε$通常取负值。

   表示不变弹性的需求曲线的一个简便方法是对上式取对数，从而得到

   $$
   \ln q=\ln A+\varepsilon\ln p
   $$

   在这个表达式中，$q$的对数线性地取决于$p$的对数。

假设需求价格弹性$\varepsilon$为常数，则根据定义式$\frac{d\ln q}{d\ln p}=\varepsilon \to d \ln p= \varepsilon \times d \ln p$，两侧同时积分可得：$\ln q=\varepsilon\times\ln p+A\boldsymbol{\to}q={e^A}p^\varepsilon={C}p^\varepsilon$

### 15.9 弹性与边际收益

研究商品数量发生变动时收益如何变动的问题。

1. 边际收益：总收益对产量的变化率，即多销售一单位产品时总收益的增加量。边际收益是总收益曲线相应点的切线的斜率。

2. 推导&表达式

   对于价格和数量的微小变化，收益变动为
   $$
   R=p\Delta q+q\Delta p>0
   $$
   两边除以$\triangle q$，得到**边际收益**的表达式
   $$
   MR=\frac{\Delta R}{\Delta q}=p+q\frac{\Delta p}{\Delta q}\\
   \frac{\Delta R}{\Delta q}=p\left[1+\frac{q\Delta p}{p\Delta q}\right] \\
   \frac1\varepsilon=\frac1{\frac{p\Delta q}{q\Delta p}}=\frac{q\Delta p}{p\Delta q}\\
   \frac{\Delta R}{\Delta q}=p\left(q\right)\left[1+\frac{1}{\varepsilon\left(q\right)}\right] \\
   \frac{\Delta R}{\Delta q}=p\left(q\right)\left[1-\frac{1}{|\varepsilon\left(q\right)|}\right]
   $$

3. 结论

   1. 如果需求弹性等于-1，边际收益为0——产量增加不会使收益发生变化。
   2. 如果需求是无弹性的，$|ε|$小于 1，$1-1/|\varepsilon|<0$，产量增加就使得收益减少。

4. 例子

   定价假设你要为自己生产的某种产品定价，并且，你已经对这种产品的需求曲线作出了正确的估计。假设定价的目标是要实现利润——收益扣减成本——最大化。那么，你就决不会把价格定在需求弹性小于1的水平上，也就是说，你决不会把价格定在需求缺乏弹性的水平上。为什么呢？考虑一下，如果你提价将会出现什么情况。由于需求缺乏弹性，所以收益会增加，但销售数量会减少。如果销售数量下降，生产成本一定也会下降，或至少不会增加。因此，你的全部利润一定会增加，这表明，在需求曲线缺乏弹性的点上经营不可能实现利润最大化。

### 15.10 边际收益曲线

### 

假设你是市场上的唯一卖家，售卖讲义，因此你可以随意控制供应量或者制定价格，同时你还知道市场的需求曲线，那么价格和对应数量之间的关系你也清楚，此时你的销售收入为$R=pq$,由于你可以随意制定价格，因此可以通过$p$ 调整$q$, 此时可得$R=p\times q(p)$,如果你调整价格，会引起收益如何变动？

$$
dR=q\times dp+p\times dq\rightarrow\frac{dR}{dp}=q+p\frac{dq}{dp}
$$
有时，厂商更倾向于控制产量，因此会根据产量影响价格，$p=p(q):$ 

$$
dR=q\times dp+p\times dq\rightarrow\frac{dR}{dq}=q\frac{dp}{dq}+p
$$

$$
\text{进一步得到:}\quad\frac{dR}{dq}=p{\left(1+\frac qp\frac{dp}{dq}\right)}=p{\left(1+\frac1\varepsilon\right)}=p{\left(q\right)}\times{\left(1-\frac1{\left|\varepsilon\left(q\right)\right|}\right)}
$$
 若$|\varepsilon|<1$,则$\frac{dR}{dq}<0$:弹性小，提升销量收益减少，说明价格大幅下降。
 若$|\varepsilon|>1$,则$\frac{dR}{dq}>0$:弹性大，提升销量收益增大，说明价格基本没变。 
$$
\text{若}|\varepsilon|>1\text{,则}\frac{dR}{dq}>0:
$$
例：产量决策我们假设成本是固定的，不随产量变化，那么你会如何制定产量目标呢？

<img src="assets/image-20240116204954342.png" alt="image-20240116204954342" style="zoom:33%;" />

收入最大化：一阶导为零

$\frac{dR}{dq}=0$,因此$|\varepsilon|=1$,这是本题特定的解。

我们可以想象一下你把产量定在需求价格弹性小于一的地方，会发生什么？
由于需求价格弹性小于一，消费者对价格不敏感，此时，如果你提升价格，产量变动小，因此你收入增加。

我们可以画出边际收入曲线，来更加直观的了解产量与边际收入的关系：

$$
\frac{dR}{dq}=\frac{dp\left(q\right)q}{dq}=p^{\prime}(q)q+p(q)=-bq+a-bq=a-2bq
$$
 假设需求价格弹性不变，则$\frac{dR}{dq}=p(q)\left(1-\frac1{|\varepsilon|}\right)$, 两者图像如下：

## 15.11 收入弹性：$\frac{\mathrm dR}{\mathrm dq}$

需求的收入弹性衡量需求对收入变动的反应：

$\varepsilon_m>1\colon$ 奢侈品$\varepsilon_m<1\colon$ 必需品例：求需求收入弹性

 假设利用柯布道格拉斯函数求得商品一的需求为：$x_{1}=\frac{m}{2p_{1}}$

$$
\text{则}\ln x_1=\ln m-\ln(2p_1)\text{,}\frac{d\ln x_1}{d\ln m}=1
$$
预算约束恒等式问题

预算恒等式问题例：若我们将求得的最优解$x^*(p_1,p_2,m)$代入到预算方程可得：
$$
p_1x_1(p_1,p_2,m)+p_2x_2(p_1,p_2,m)\equiv m
$$

恒等式两端对收入求导可得：

$$
p_1\frac{dx_1}{dm}+p_2\frac{dx_2}{dm}=1\boldsymbol{\rightarrow}\frac{p_1x_1}{x_1}\frac{dx_1}{dm}+\frac{p_2x_2}{x_2}\frac{dx_2}{dm}=1\boldsymbol{\rightarrow}\frac{p_1x_1}m\frac m{x_1}\frac{dx_1}{dm}+\frac{p_2x_2}m\frac m{x_2}\frac{dx_2}{dm}=1
$$

最终得：$\frac{p_1x_1}m\varepsilon_1+\frac{p_2x_2}m\varepsilon_2=1$ →$s_1\varepsilon_1+s_2\varepsilon_2=1$, 代表份额支出这表明不可能两种商品均是奢侈品或均是必需品
