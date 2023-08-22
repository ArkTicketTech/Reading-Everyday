投影 (projection)
关系代数中的一种， $$\pi_A(R)$$ 用于从关系R中选出属性包含在A中的列。
在 pg 中也用于引入额外的列（比如计算表达式）
> 快问快答  
create table t (int a, int b);  
以下哪些 sql 会触发 project 逻辑？  
select * from t;  
select a, b from t;  
select b, a from t;  
select a as b, b as a from t;  
select a from t;  
select a, b, b from t;  
select a + 1, b from t;  
select ctid, * from t;  

pg 对投影的实现
1）判断是否需要投影
被扫描的关系的描述符和计划节点的targetlist不匹配的时候
`tlist_matches_tupdesc` 用于判断是否需要投影
``` c
				  static bool
				  tlist_matches_tupdesc(PlanState *ps, List *tlist, Index varno, TupleDesc tupdesc)
				  {
				  	int			numattrs = tupdesc->natts;
				  	int			attrno;
				  	ListCell   *tlist_item = list_head(tlist);
				  
				  	/* Check the tlist attributes */
				  	for (attrno = 1; attrno <= numattrs; attrno++)
				  	{
				  		Form_pg_attribute att_tup = TupleDescAttr(tupdesc, attrno - 1);
				  		Var		   *var;
				  
				  		if (tlist_item == NULL)
				  			return false;		/* tlist too short */
				  		var = (Var *) ((TargetEntry *) lfirst(tlist_item))->expr;
				  		if (!var || !IsA(var, Var))
				  			return false;		/* tlist item not a Var */
				  		/* if these Asserts fail, planner messed up */
				  		Assert(var->varno == varno);
				  		Assert(var->varlevelsup == 0);
				  		if (var->varattno != attrno)
				  			return false;		/* out of order */
				  		if (att_tup->attisdropped)
				  			return false;		/* table contains dropped columns */
				  		if (att_tup->atthasmissing)
				  			return false;		/* table contains cols with missing values */
				  
				  		/*
				  		 * Note: usually the Var's type should match the tupdesc exactly, but
				  		 * in situations involving unions of columns that have different
				  		 * typmods, the Var may have come from above the union and hence have
				  		 * typmod -1.  This is a legitimate situation since the Var still
				  		 * describes the column, just not as exactly as the tupdesc does. We
				  		 * could change the planner to prevent it, but it'd then insert
				  		 * projection steps just to convert from specific typmod to typmod -1,
				  		 * which is pretty silly.
				  		 */
				  		if (var->vartype != att_tup->atttypid ||
				  			(var->vartypmod != att_tup->atttypmod &&
				  			 var->vartypmod != -1))
				  			return false;		/* type mismatch */
				  
				  		tlist_item = lnext(tlist, tlist_item);
				  	}
				  
				  	if (tlist_item)
				  		return false;			/* tlist too long */
				  
				  	return true;
				  }
```
2）构建投影所需要的信息
``` c
			  ProjectionInfo *
			  ExecBuildProjectionInfo(List *targetList,
			  						ExprContext *econtext,
			  						TupleTableSlot *slot,
			  						PlanState *parent,
			  						TupleDesc inputDesc)
			  {
			  	/* Insert EEOP_*_FETCHSOME steps as needed */
			  	ExecInitExprSlots(state, (Node *) targetList);
			  	/* Now compile each tlist column */
			  	foreach(lc, targetList)
			  	{
			          /* 考虑投影列为var的情况 */
			  		if (tle->expr != NULL &&
			  			IsA(tle->expr, Var) &&
			  			((Var *) tle->expr)->varattno > 0)
			  		{
			  			if (inputDesc == NULL)
			  				isSafeVar = true;	/* can't check, just assume OK */
			  			else if (attnum <= inputDesc->natts)
			  			{
			  				Form_pg_attribute attr = TupleDescAttr(inputDesc, attnum - 1);
			  
			  				/*
			  				 * If user attribute is dropped or has a type mismatch, don't
			  				 * use ASSIGN_*_VAR.  Instead let the normal expression
			  				 * machinery handle it (which'll possibly error out).
			  				 */
			  				if (!attr->attisdropped && variable->vartype == attr->atttypid)
			  				{
			  					isSafeVar = true;
			  				}
			  			}
			  			
			            	/* 对于简单的情况只需要 EEOP_ASSIGN_*_VAR 即可 */
			              if (isSafeVar)
			              {
			                  /* Fast-path: just generate an EEOP_ASSIGN_*_VAR step */
			                  switch (variable->varno)
			                  {
			                      case INNER_VAR:
			                          /* get the tuple from the inner node */
			                          scratch.opcode = EEOP_ASSIGN_INNER_VAR;
			                          break;
			  
			                      case OUTER_VAR:
			                          /* get the tuple from the outer node */
			                          scratch.opcode = EEOP_ASSIGN_OUTER_VAR;
			                          break;
			  
			                          /* INDEX_VAR is handled by default case */
			  
			                      default:
			                          /* get the tuple from the relation being scanned */
			                          scratch.opcode = EEOP_ASSIGN_SCAN_VAR;
			                          break;
			                  }
			  
			                	/* 
			                   * 这里是核心逻辑 构建了投影所需要的执行步骤 在执行过程中按照步骤依次执行即可
			                   * 这么做的本质是为了降低函数递归调用的运行成本
			                   */
			                  scratch.d.assign_var.attnum = attnum - 1;
			                  scratch.d.assign_var.resultnum = tle->resno - 1;
			                  ExprEvalPushStep(state, &scratch);
			              }
			            	else
			              {
			                	/* 具体来说，包含表达式计算，或者系统变量等情况时，要按照常规方式处理表达式 */
			                  /*
			                   * Otherwise, compile the column expression normally.
			                   *
			                   * We can't tell the expression to evaluate directly into the
			                   * result slot, as the result slot (and the exprstate for that
			                   * matter) can change between executions.  We instead evaluate
			                   * into the ExprState's resvalue/resnull and then move.
			                   */
			                  ExecInitExprRec(tle->expr, state,
			                                  &state->resvalue, &state->resnull);
			                	
			                	// 投影求值计算的时候会用到 attnum 和 resultnum
			                  scratch.d.assign_var.attnum = attnum - 1;
			                  scratch.d.assign_var.resultnum = tle->resno - 1;
			                  ExprEvalPushStep(state, &scratch); 
			              }
			          }
			        	
			      }
			  }
```
本节我们主要讨论 fast-path 相关逻辑
这段代码的核心逻辑在于通过调用 `ExprEvalPushStep` 构建了投影的执行过程；通过opcode标识每个步骤的类型；在执行阶段根据opcode调用不同的过程
相比于传统的表达式求值逻辑，这样写的好处在于减少函数递归调用
3）执行投影算子
``` c
			  #ifndef FRONTEND
			  static inline TupleTableSlot *
			  ExecProject(ProjectionInfo *projInfo)
			  {
			  	ExprState  *state = &projInfo->pi_state;
			  	TupleTableSlot *slot = state->resultslot; // 投影之后的结果；目前还是未计算的状态
			  
			    	/* Run the expression, discarding scalar result from the last column. */
			  	(void) ExecEvalExprSwitchContext(state, econtext, &isnull);
			  
			  	return slot;
			  }
```
本节主要介绍表达式求值的框架 投影求值也是利用表达式求值实现的 即调用`ExecEvalExprSwitchContext` 底层调用了 `ExecInterpExpr`
其执行过程建立在一套由宏定义实现的分发器机制之上；实现了对之前构建的表达式求值步骤顺次执行的过程；在过程执行中，我们会用ExprState来存储中间计算结果和其他执行状态；具体如下：
``` c
				  // opcode对应步骤的实现逻辑的标识 用于goto
				  #define EEO_CASE(name)		CASE_##name:
				  // 分发至步骤的执行逻辑
				  #define EEO_DISPATCH()		goto *((void *) op->opcode)
				  // 
				  #define EEO_OPCODE(opcode)	((intptr_t) dispatch_table[opcode])
				  // 当前步骤执行完毕时移动至下一个需要执行的步骤
				  #define EEO_NEXT() \
				  	do { \
				  		op++; \
				  		EEO_DISPATCH(); \
				  	} while (0)
				  
				  ExecInterpExpr(ExprState *state, ExprContext *econtext, bool *isnull)
				  {
				  	op = state->steps; // 存储所有的步骤，我们通过宏不断移动当前执行的步骤
				  	resultslot = state->resultslot; // 用于存放最后返回的结果值
				  	innerslot = econtext->ecxt_innertuple;
				  	outerslot = econtext->ecxt_outertuple;
				  	scanslot = econtext->ecxt_scantuple;
				    
				  	EEO_DISPATCH();
				  
				    	EEO_CASE()
				  		EEO_CASE(EEOP_DONE)
				  		{
				  			goto out;
				  		}
				    
				  		EEO_CASE(EEOP_SCAN_FETCHSOME)
				  		{
				  			CheckOpSlotCompatibility(op, scanslot);
				  
				  			slot_getsomeattrs(scanslot, op->d.fetch.last_var);
				  
				  			EEO_NEXT();
				  		}
				  
				    		EEO_CASE(EEOP_ASSIGN_SCAN_VAR)
				  		{
				  			int			resultnum = op->d.assign_var.resultnum;
				  			int			attnum = op->d.assign_var.attnum;
				  
				  			/*
				  			 * We do not need CheckVarSlotCompatibility here; that was taken
				  			 * care of at compilation time.  But see EEOP_INNER_VAR comments.
				  			 */
				  			resultslot->tts_values[resultnum] = scanslot->tts_values[attnum];
				  			resultslot->tts_isnull[resultnum] = scanslot->tts_isnull[attnum];
				  
				  			EEO_NEXT();
				  		}
				  out:
				    	*isnull = state->resnull
				    	return state->resvalue
				  }
```
这样，我们就实现了投影列计算的逻辑，最终的tuple存储在 `state->resultslot` 里，供上层算子使用