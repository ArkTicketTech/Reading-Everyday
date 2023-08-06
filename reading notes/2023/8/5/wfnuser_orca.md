-  
  ``` cpp
  BOOL
  CXformUtils::FDeriveStatsBeforeXform(CXform *pxform)
  {
  	GPOS_ASSERT(nullptr != pxform);
  
  	return pxform->FExploration() &&
  		   CXformExploration::Pxformexp(pxform)->FNeedsStats();
  }
  ```
	- 在 Orca 的查询优化器中，转换（Xforms）分为两类：Exploration（探索）和 Implementation（实施）。  
	- Exploration 转换主要是用于探索新的查询执行计划，也就是生成新的逻辑表达式。在这个阶段，优化器尝试各种可能的查询执行策略，并且生成对应的逻辑表达式。由于逻辑表达式只描述了执行计划的逻辑结构，不涉及具体的物理执行方式，因此，推导统计信息（DeriveStats）是非常有必要的，这些统计信息可以帮助优化器评估不同执行策略的效果。  
	- Implementation 转换则是将逻辑表达式转换为物理表达式，即确定具体的执行方式。例如，一个逻辑连接（Logical Join）可以有多种实施方式，例如哈希连接（Hash Join）、嵌套循环连接（Nested Loop Join）等。Implementation 转换是根据已经推导出的统计信息选择最优的执行方式。  
	- 因此，`pxform->FExploration()` 的判断是为了确定当前的转换是在 Exploration 阶段。只有在 Exploration 阶段，我们才需要在执行转换前推导统计信息。而在 Implementation 阶段，我们已经有了统计信息，所以不需要在执行转换前再次推导。这就是为什么有这个条件的原因。  