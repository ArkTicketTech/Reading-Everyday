# AI算法工程师手册笔记

## 数学基础

### 1.线性代数基础

Url：[线性代数基础](https://www.huaxiaozhuan.com/%E6%95%B0%E5%AD%A6%E5%9F%BA%E7%A1%80/chapters/1_algebra.html)

+ $\mathbb{R^{m\times n}}$
+ 范数计算：$F=\vert \vert \mathbf{A} \vert \vert_F =\sqrt{\sum{i,j}a^2_{i,j}}$
+ 矩阵的迹及其性质
+ 向量线性相关/无关定义
+ 维数
+ 三维向量点积
+ 三维向量叉积
+ 三维向量混合积
+ 两个向量的并矢
+ 矩阵：阿达马积/克罗内积
+ 特殊函数：
  + sigmoid
    + 公式：$\sigma=\frac{1}{1+\exp(-x)}$
  + softplus
    + 公式：$\zeta (x)=\log(1+\exp(x))$
  + 两个公式的性质
  + logit函数
  + 伽马函数：$\Gamma$定义、图像和性质
  + 贝塔函数：定义、性质
  + 斯特林公式：$n! \approx \sqrt{2\pi n} (\frac{n}{e})^n$

### 2.概率论与随机过程

Url：[概率论基础](https://www.huaxiaozhuan.com/%E6%95%B0%E5%AD%A6%E5%9F%BA%E7%A1%80/chapters/2_probability.html)

+ 条件概率与独立事件
  + 条件概率含义、条件概率分布的链式法则、相互独立的数学描述、两个随机变量关于另一个随机变量条件独立的数学描述
+ 联合概率分布
  + 定义
  + 当$X$和$Y$都是离散随机变量时，定义；概率质量函数分布；

### 3.数值计算基础

Url：[数值计算](https://www.huaxiaozhuan.com/%E6%95%B0%E5%AD%A6%E5%9F%BA%E7%A1%80/chapters/3_numerical_computaion.html)

+ 数值稳定性	
  + 上溢出、下溢出

## 统计学习

### 0 机器学习简介

