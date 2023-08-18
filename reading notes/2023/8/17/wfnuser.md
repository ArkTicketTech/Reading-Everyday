Required Property 是发到 group 的； 什么时候计算出来呢？ 在 CJobGroupExpressionOptimization 时结合当前算子特性，对每个不同的 `ulRequest` 进行计算，方法为 `ComputeChildReqdProps -> compute -> popPhysical->Ped(); popPhysical->PosRequired() ...` ; 算出来的是当前 group expression 某个孩子 group 的 required properties ；保存在 `m_pdrgprp`
那同一个算子的 不同的 `ulOptReq` 算出来的 required properties 是会互相替换吗？
在后面使用的时候会拷贝，所以替换没有关系
`m_pdrgprp` 具体是怎么被用到的？
初始化后 再之后会传递到下层
``` cpp
		  	// schedule optimization job for current child group
		  	COptimizationContext *pocChild = GPOS_NEW(psc->GetGlobalMemoryPool())
		  		COptimizationContext(psc->GetGlobalMemoryPool(), pgroupChild,
		  							 m_pexprhdlPlan->Prpp(m_ulChildIndex), prprel,
		  							 stats_ctxt, psc->Peng()->UlCurrSearchStage());
		  
```