- # Column-Oriented Storage  
	- 存储 trillion of rows and  petabytes of data 是一个复杂的问题  
		- 由于 dimension 表比 fact 表小的多，本章主要讨论 fact 表  
	- 对于分析型场景来说 即使fact表本身有100个列，我们通常在一次查询中只需要访问其中4，5个列  
		- select * 在分析型场景中是罕见的  
		-  
		  ``` sql
		  		  SELECT
		  		    dim_date.weekday, dim_product.category,
		  		    SUM(fact_sales.quantity) AS quantity_sold 
		  		  FROM fact_sales
		  		    JOIN dim_date ON fact_sales.date_key = dim_date.date_key
		  		    JOIN dim_product ON fact_sales.product_sk = dim_product.product_sk
		  		  WHERE
		  		    dim_date.year = 2013 AND
		  		    dim_product.category IN ('Fresh fruit', 'Candy')
		  		  GROUP BY
		  		    dim_date.weekday, dim_product.category;
		  ```
	- 如何提高类似查询的效率呢？  
		- 在大部分OLTP的数据库中，数据通常是面向行存储的  
			- 对于上述的查询，我们可以通过建立索引提高效率；但是 row-oriented storage engine 仍然需要装在所有相关的行（每行可能包含100个列）到内存里，这是效率非常低的  
		- 因此我们就有了面向列的存储模型：  
			- 我们不再将整行的数据存储在一起，而是将所有同一列的值存储在一起，每一列都用一个独立的文件存储；这样每次查询可能只需要读取并解析我们需要的列所对应的文件即可；节约了大量工作。  
			- ![image.png](../assets/image_1683095401858_0.png)  
	- ## Column Compression  
		- 除了减少查询时需要加载的列；通过压缩，我们可以减少查询时磁盘的吞吐量；列存储本身非常适合被压缩  
			- ![image.png](../assets/image_1683097018992_0.png)  
			- 上图是一个例子，大量的数据是重复的，这是一个适用于压缩的信号；每一列的数据不同，我们也会采用不同的压缩技术；上面采用的技术是 bitmap encoding  
				- 在很多时候，某些列的不同的值的数量可能比实际的行数少很多；我们可以把这n个不同的值列出来，然后转化成n个不同的bitmap：每一个bitmap表示某个值出现在哪些行  
				- 当 n 很大时，bitmap中可能有很多零；我们可以进行 run-length encoding ；进一步压缩列数据  
				- bitmap 在下面在这样的查询中很有用：  
					-  
					  ``` sql
					  					  WHERE product_sk IN (30, 68, 69)
					  					  WHERE product_sk = 31 AND store_sk = 3
					  					  
					  					  可以对 bitmap 进行通过或和与运算快速求出所需返回的行
					  ```
		- 还有很多不同的压缩格式，这里不展开了  
		- ### Memory bandwidth and vectorized processing  
			- 分析型场景下，开发者还需要关注从主存到CPU缓存的带宽瓶颈，避免分支预测错误和流水线中的bubbles，尽可能多的使用SIMD指令  
			- 面向列的存储在高效利用CPU周期上也更有优势，同样大小的L1 cache在列存场景下可以装得下更多的行；且前面提到的AND和OR的bitwise计算，通过向量化技术可以直接对压缩快进行计算  
	- ## Sort Order in Column Storage  
		- 在列存中，通常我们不必关心行的相对顺序（但列之间应该是需要保持同样的顺序），所以一般按照插入顺序存储就可；不过就和SSTable一样，有时候我们也希望在存储的时候加上一些顺序，用于某些索引机制  
			- 第一个好处在于我们存储的数据在查询时本身需要有序；我们可以利用先验知识，让数据按照某些列排序，比如在前面的例子中优先按照日期排序是一个合理的事情，这样，我们针对某个月份的数据进行查询时就不再需要扫描全部数据了；当第一列相同时，还可以选择第二列进一步排序，比如对 product_sk 进行排序，这样在同一天内所有同样产品的售卖数据就是排列在一起的  
			- 第二个好处在于我们可以更好的压缩，比如在前面的bitmap的压缩格式中，排序后的数据可能有更多的连续相同的1和0，可以用 run-length 的方式更好的压缩  
		- ### several different sort orders  
			- C-Store 和 Vertica 中用到了这个想法，即，既然不同的查询需要不同的顺序，为什么我们不将同样的数据按照不同的方式存储多份呢？ 我们本来就需要通过冗余来保证数据可靠性。  
			- 和采用多个 secondary index 的想法时类似的  
	- ## Writing to Column-Oriented Storage  
		- 前述的各种优化能提高分析型查询的效率，但是他们导致了写操作变得更加麻烦；这仍然是合理的，因为大部分分析型查询都是只读的  
		- 对于 update-in-place 的操作来说，想要在压缩列上修改数据，可能需要改写全部的列文件，因为标记行的位置在某个文件上变了，那么其他文件上也需要变。  
		- 不过我们可以通过类似LSM树的方式解决这个问题  
	- ## Aggregation: Data Cubes and Materialized Views  
		- 数据仓库常常用于计算复杂的聚合运算；如果我们会反复查询同样的聚合运算，那么一个很自然的想法就是我们可以建立一个缓存  
		- 一种方式就是建立物化视图；表现形式和表没有什么区别，但是底层实际上是对某些查询语句结果的持久化存储；当从物化视图首次读数据的时候，底层的查询会执行，并将查询结果存储下来，同时返回；当底层数据改变的时候，物化视图应该自动修正数据  
			- 在OLTP场景中不常见的一个原因就是，这样的自动修正会带来额外的成本；但是对于OLAP场景来说，写操作是罕见的  
		- 存在一种特殊的物化视图，我们称为 cube；他是从不同维度聚合的网格  
			- 这样的存储方式可以更高效的存储多维度聚合结果的物化视图；提高查询效率；但是并不是非常灵活的方式，比如，无法支持聚合的时候按照某些其他维度做过滤。  