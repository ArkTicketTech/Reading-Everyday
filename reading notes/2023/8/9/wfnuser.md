- Expression 在 orca 里的表示是 `CExpression`  
	- arity 表示 children 的数量；arity == 0 则为叶子结点；  
	- 本质上就是 operator 的一层 wrapper  
		- 有三种不同的 operator  
			- Scalar Operators - naming convention of the form `CScalar<identifier>`  
			- Logical Operators - naming convention of the form `CLogical<identifier>`  
			- Physical Operators - naming convention of the form `CPhysical<identifier>`  
		- 因此有三种不同的 expression  
			- Scalar Expression  
				- **Scalar Properties** - refer `CDrvdPropScalar::Derive`  
				-  
				  ```
				  				  Defined Columns: [], Used Columns: ["empname" (0)], Set Returning Function Columns: [], Has Subqs: [0], Function Properties: [Immutable], Has Non-scalar Funcs: [0], ",
				  ```
			- Logical Expression  
				- **Relational Properties** - refer `CDrvdPropRelational::Derive`  
				-  
				  ```
				  				  Output Cols: ["empname" (0), "empid" (1), "ctid" (2), "xmin" (3), "cmin" (4), "xmax" (5), "cmax" (6), "tableoid" (7), "gp_segment_id" (8)], Outer Refs: [], Not Null Cols: ["ctid" (2), "xmin" (3), "cmin" (4), "xmax" (5), "cmax" (6), "tableoid" (7), "gp_segment_id" (8)], Corr. Apply Cols: [],  Keys: (["ctid" (2), "gp_segment_id" (8)]), Max Card: unbounded, Join Depth: 1, Constraint Property: [], FDs: [("ctid" (2), "gp_segment_id" (8)) --> ("empname" (0), "empid" (1), "xmin" (3), "cmin" (4), "xmax" (5), "cmax" (6), "tableoid" (7))], Function Properties: [Immutable], Part Info: [Part Consumers: , Part Keys: ], Has Partial Indexes",
				  ```
			- Physical Expression  
				- **Plan Properties** - refer `CDrvdPropPlan::Derive`. some examples are:  
					- Order Spec  
					- Distribution Spec  
				-  
				  ```
				  				  Drvd Plan Props (ORD: <empty>, DIST: RANDOM, REWIND: MARK-RESTORE NO-MOTION), Part-Index Map: [], Part Filter Map: , CTE Map: []"
				  ```
		- 如何创建一个 expression  
			-  
			  ``` cpp
			  			  1) Create an operator
			  			  CLogicalGet *popGet = GPOS_NEW(m_mp) CLogicalGet(m_mp, pname, ptabdesc);
			  			  2) Create an expresion on top of the operator
			  			  CExpression *pexpr = GPOS_NEW(m_mp) CExpression(m_mp, popGet);
			  ```
		- 所有逻辑算子包含 `PxfsCandidates` 方法 返回 list of transformation rules  
		- 在 algebrization 阶段只有 logical scalar 表达式， scalar 和 logical properties derived and don't change throughout their life cycle  
		-  