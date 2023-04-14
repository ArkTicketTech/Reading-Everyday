## Why do we need to sort？

- 关系模型/SQL 是没有排序的
- 有些查询会显式的指明返回的数据需要排序（order by），但是即使一个查询没有显式声明需要排序，我么还是需要排序的
  - distinct
  - bulk loading
  - group by
  - .......

## Sorting Algorithms

- 如果数据能全部存到内存，那么我们就可以使用例如quick sort之类的标准排序算法来优化
- 如果数据不能全部存到内存，那么我们需要使用算法来减少读写硬盘页面的开销
