# 3.3大规模数据的学习算法

设计矩阵$Φ$的维数为$n×b$，如果训练样本数$n$或参数个数$b$是非常大的数值，会导致计算内存不足。此时，会使用随机梯度算法。

随机梯度算法是指，沿着训练平方误差$J_{ls}$的梯度下降，对参数$\theta$依次进行学习。

![image.png](image.png)

当梯度 $\nabla j_{LS}{(i)}$ 为负时，参数会朝着正的方向 (图中右方向) 加以修正；当梯度 $\nabla j_{LS}{(i)}$ 为正时，参数会朝着负的方向 (图中左方向) 加以修正。

与线性模型相对应的训练平方误差$J_{ls}$ 为凸函数。

所以通过梯度法就可以得到训练平方误差 在值域范围内的最优解，即全局最优解。因为凸函数只有一个峰值。

![image.png](image%201.png)

连接任意两点$\theta_1$、$\theta_2$的线段一定在函数的上部。

使用随机梯度算法对线性模型进行最小二乘法学习法的算法流程 :

1. 给$\theta$以适当的初值
2. 随机选择 一个训练样本（例如: 选择顺序为$i$的训练样本$(x_i,y_i)$） 
3. 对于选定的训练样本，采用使其梯度下降的方式，对参数$\theta$进行更新 
    
    $$
    \theta \leftarrow \theta - \varepsilon \nabla J_{LS}^{(i)}(\theta)
    $$
    
    $\varepsilon$ 是名为学习系数的无穷小正标量，表示梯度下降的步幅。
    
    - **学习率: 一个小的正数，用来控制每次更新参数时的步长。**
    - **步长太大可能导致错过最优解，步长太小则可能导致收敛速度过慢。**
    
    $\nabla J_{LS}^{(i)}$ 是顺序为 $i$ 的训练样本相对应的训练平方误差的梯度，表示梯度下降的方向。
    
    $$
    \nabla J_{LS}^{(i)}(\theta) = \phi(x_i) (f_\theta(x_i) - y_i)
    $$
    
4. 直到解$\theta$达到收敛精度为止,重复2，3