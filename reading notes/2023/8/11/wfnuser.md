- # Abstract  
	- Introduce a novel infrastructure to enable ORCA's support for any user-defined access method.  
- # Motivation  
	- Just as Postgres supports user-defined access methods through the creation of extensions, it is imperative for ORCA to follow suit.  
- # Background  
	- In Postgres, we possess a well-structured abstraction of an access method that enables us to accommodate any user-defined access approach. For introducing novel index types like the 'bloom index', developers simply need to implement the API structure 'IndexAmRoutine' and establish the new index access method as an extension. Subsequently, Postgres can automatically produce associated index scan paths within the optimizer and invoke the method for the fresh index type implemented by the extension provider if the path is selected in the executor.  
	- However, this narrative takes a different turn when it comes to ORCA.  
	- Firstly, all the index types supported in ORCA are predefined within its implementation. Any time we are translating `relcache` to `dxl` or evaluating the applicability of an index, these processes are hardcoded into the related logic.  
	-  
	  ``` cpp
	  	      /* Definition */
	  	      enum EmdindexType
	  	      {
	  	          EmdindBtree,   // btree
	  	          EmdindBitmap,  // bitmap
	  	          EmdindGist,    // gist using btree or bitmap
	  	          EmdindGin,     // gin using btree or bitmap
	  	          EmdindBrin,    // brin
	  	          EmdindHash,    // hash
	  	          EmdindSentinel
	  	      };
	  	  
	  	      /* Example 1 */
	  	  	switch (index_rel->rd_rel->relam)
	  	  	{
	  	  		case BTREE_AM_OID:
	  	  			index_type = IMDIndex::EmdindBtree;
	  	  			break;
	  	  		case BITMAP_AM_OID:
	  	  			index_type = IMDIndex::EmdindBitmap;
	  	  			break;
	  	  		case BRIN_AM_OID:
	  	  			index_type = IMDIndex::EmdindBrin;
	  	  			break;
	  	  		case GIN_AM_OID:
	  	  			index_type = IMDIndex::EmdindGin;
	  	  			break;
	  	  		case GIST_AM_OID:
	  	  			index_type = IMDIndex::EmdindGist;
	  	  			break;
	  	  		case HASH_AM_OID:
	  	  			index_type = IMDIndex::EmdindHash;
	  	  			break;
	  	  		default:
	  	  			GPOS_RAISE(gpdxl::ExmaMD, gpdxl::ExmiMDObjUnsupported,
	  	  					   GPOS_WSZ_LIT("Index access method"));
	  	  	}
	  	  
	  	      /* Example 2 */
	  	  	// index expressions and index constraints not supported
	  	  	return gpdb::HeapAttIsNull(tup, Anum_pg_index_indexprs) &&
	  	  		   gpdb::HeapAttIsNull(tup, Anum_pg_index_indpred) &&
	  	  		   index_rel->rd_index->indisvalid &&
	  	  		   (BTREE_AM_OID == index_rel->rd_rel->relam ||
	  	  			BITMAP_AM_OID == index_rel->rd_rel->relam ||
	  	  			GIST_AM_OID == index_rel->rd_rel->relam ||
	  	  			GIN_AM_OID == index_rel->rd_rel->relam ||
	  	  			BRIN_AM_OID == index_rel->rd_rel->relam ||
	  	  			HASH_AM_OID == index_rel->rd_rel->relam);
	  	  
	  	      /* Example 3 */
	  	      BOOL gin_or_gist_or_brin = (pmdindex->IndexType() == IMDIndex::EmdindGist ||
	  	                                  pmdindex->IndexType() == IMDIndex::EmdindGin ||
	  	                                  pmdindex->IndexType() == IMDIndex::EmdindBrin);
	  	  
	  	      if (cmptype == IMDType::EcmptNEq || cmptype == IMDType::EcmptIDF ||
	  	          (cmptype == IMDType::EcmptOther &&
	  	          !gin_or_gist_or_brin) ||  // only GIN/GiST/BRIN indexes with a comparison type other are ok
	  	          (gin_or_gist_or_brin &&
	  	          pexprScalar->Arity() <
	  	              2))  // we do not support unary index expressions for GIN/GiST/BRIN indexes
	  	      {
	  	          return nullptr;
	  	      }
	  	  
	  ```
	- Secondly, ORCA operates on a distinct cost model, which implies that the costs computed in the Postgres Optimizer cannot be directly compared with those calculated in ORCA. Therefore, devising a method to compute costs in ORCA for user-defined indexes stands as another challenge that needs to be resolved to facilitate user-defined indexes within ORCA.  
- # Implementation  
	- We need to solve it in several different parts:  
	- ## PG_AM  
		- to provide a user-defined cost model for ORCA and other third-party optimizer, we add a new cost estimate function to `IndexAmRoutine` . It should be designed as optional. If the extension provider want to make it work in ORCA, he/she should be responsible to provide the cost estimate implementation. If he/she don't provide it, nothing bad should be happened to Postgres Optimizer.  
		-  
		  ``` c
		  		  typedef struct IndexAmRoutine
		  		  {
		  		  	NodeTag		type;
		  		  	...
		  		  
		  		  	/* third-party am cost estimate functions */
		  		  	AmOrcaCostEstimateFunc amorcacostestimate; /* can be NULL */
		  		  } IndexAmRoutine;
		  ```
		- There may be some modifications need to be made to IndexAmRoutine to provide some information that ORCA needed, which has not been discovered yet.  
	- ## DXL translation parts (Not Finished Yet)  
		- create a new index type `UserDefinedIndex` for all other non-predefined index  
			-  
			  ``` cpp
			  			      /* Definition */
			  			      enum EmdindexType
			  			      {
			  			          EmdindBtree,   // btree
			  			          EmdindBitmap,  // bitmap
			  			          EmdindGist,    // gist using btree or bitmap
			  			          EmdindGin,     // gin using btree or bitmap
			  			          EmdindBrin,    // brin
			  			          EmdindHash,    // hash
			  			          EmdindUserDefined, // user defined
			  			          EmdindSentinel
			  			      };
			  ```
		- remove hardcoded index type check like the `IsIndexSupported` , all indexes are supported if they provide the implementation of `amorcacostestimate`.  
	- ## Metadata (Not Decided Yet)  
		- ### FIndexApplicable  
			- during evaluating of `FIndexApplicable` , we need to decide if the new index can support Btree or Bitmap index  
			- and to decide what type of compare type are supported for different index (maybe `canorder` is used for that)  
			- May need to add some fields to indicate all concerns mentioned above  
			- If we haven't decide yet, we should keep the safest plan  
		- In ORCA side, we need to find a place to attach user defined cost estimate function. Since it should related to index relation, we naturally decide to attach it to `CMDIndexGPDB`:  
			-  
			  ``` cpp
			  			  	CMDIndexGPDB *index = GPOS_NEW(mp) CMDIndexGPDB(
			  			  		mp, mdid_index, mdname, index_clustered, index_partitioned, index_type,
			  			  		mdid_item_type, index_key_cols_array, included_cols, op_families_mdids,
			  			  		nullptr,  // mdpart_constraint
			  			  		child_index_oids,
			  			  		index_rel->rd_indam->amorcacostestimate);
			  ```
			- So that we can fetch the customized cost estimate function by RetrieveIndex in any place if we have `MDId`.  
	- ## Cost Model (Not Designed Yet)  
		- In ORCA, cost model should be bind to operators. The ultimate goal might look like following:  
			-  
			  ``` cpp
			  			  CCost
			  			  CCostModelGPDB::Cost(
			  			  	CExpressionHandle &exprhdl,	 // handle gives access to expression properties
			  			  	const SCostingInfo *pci) const
			  			  {
			  			  	GPOS_ASSERT(nullptr != pci);
			  			  	COperator::EOperatorId op_id = exprhdl.Pop()->Eopid();
			  			  	// All infomation the customized cost estimate function need to know
			  			  	CostInfo ci = new CostInfo();
			  			  	return exprhdl.Pop().Cost(ci);
			  			    
			  			  	/* CURRENT IMPLEMENTATION */
			  			  	// switch (op_id)
			  			  	// {
			  			  	// 	default:
			  			  	// 	{
			  			  	// 		// FIXME: macro this?
			  			  	// 		__builtin_unreachable();
			  			  	// 	}
			  			  	// 	case COperator::EopPhysicalTableScan:
			  			  	// 	case COperator::EopPhysicalDynamicTableScan:
			  			  	// 	case COperator::EopPhysicalExternalScan:
			  			  	// 	{
			  			  	// 		return CostScan(m_mp, exprhdl, this, pci);
			  			  	// 	}
			  			  
			  			  	// 	case COperator::EopPhysicalFilter:
			  			  	// 	{
			  			  	// 		return CostFilter(m_mp, exprhdl, this, pci);
			  			  	// 	}
			  			  
			  			  	// 	case COperator::EopPhysicalIndexOnlyScan:
			  			  	// 	{
			  			  	// 		return CostIndexOnlyScan(m_mp, exprhdl, this, pci);
			  			  	// 	}
			  			  	// } 
			  			  }
			  			  
			  			  
			  ```
		- However, it's a huge change which cannot be implement in one commit. We will first support two of the operators (`PhysicalIndexScan` and `PhysicalTableScan`) in polymorphism style not in switch case style.  
		- Our first implementation will looks like:  
			-  
			  ``` cpp
			  			  CCost
			  			  CCostModelGPDB::Cost(
			  			  	CExpressionHandle &exprhdl,	 // handle gives access to expression properties
			  			  	const SCostingInfo *pci) const
			  			  {
			  			  	GPOS_ASSERT(nullptr != pci);
			  			  
			  			  	COperator::EOperatorId op_id = exprhdl.Pop()->Eopid();
			  			  
			  			  	if (op_id == COperator::EopPhysicalIndexScan)
			  			  	{
			  			  		CPhysicalIndexScan *pop = (CPhysicalIndexScan*) exprhdl.Pop();
			  			  		CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
			  			  		const IMDIndex *pmdindex = md_accessor->RetrieveIndex(pop->Pindexdesc()->MDId());
			  			  		if (pmdindex->OrcaCostEsitmate() != NULL)
			  			  		{
			  			  			return CostUserDefinedIndex(m_mp, exprhdl, this, pci, pmdindex->OrcaCostEsitmate());
			  			  		}
			  			  	}
			  			  }
			  			  
			  			  // main job for this function is to create CostInfo for this particular operator
			  			  // the info for different operator seems to be different
			  			  CCost
			  			  CCostModelGPDB::CostUserDefinedIndex(CMemoryPool *,  // mp
			  			  							  CExpressionHandle &exprhdl,
			  			  							  const CCostModelGPDB *pcmgpdb,
			  			  							  const SCostingInfo *pci,
			  			  							  AmOrcaCostEstimateFunc cef)
			  			  {
			  			  	COperator *pop = exprhdl.Pop();
			  			  	CostInfo ci;
			  			  
			  			  	/* ORCA_AM_TODO: the way to get table width is currently related to pop. */
			  			  	ci.dTableWidth =
			  			  		CPhysicalScan::PopConvert(pop)->PstatsBaseTable()->Width().Get();
			  			  
			  			  	ci.dIndexFilterCostUnit =
			  			  		pcmgpdb->GetCostModelParams()
			  			  			->PcpLookup(CCostModelParamsGPDB::EcpIndexFilterCostUnit)
			  			  			->Get().Get();
			  			  	ci.dIndexScanTupCostUnit =
			  			  		pcmgpdb->GetCostModelParams()
			  			  			->PcpLookup(CCostModelParamsGPDB::EcpIndexScanTupCostUnit)
			  			  			->Get().Get();
			  			  	ci.dIndexScanTupRandomFactor =
			  			  		pcmgpdb->GetCostModelParams()
			  			  			->PcpLookup(CCostModelParamsGPDB::EcpIndexScanTupRandomFactor)
			  			  			->Get().Get();
			  			  
			  			  	GPOS_ASSERT(0 < ci.dIndexFilterCostUnit);
			  			  	GPOS_ASSERT(0 < ci.dIndexScanTupCostUnit);
			  			  	GPOS_ASSERT(0 < ci.dIndexScanTupRandomFactor);
			  			  
			  			  	ci.dRowsIndex = pci->Rows();
			  			  	ci.dNumRebinds = pci->NumRebinds();
			  			  
			  			  	ci.ulIndexKeys = CPhysicalIndexScan::PopConvert(pop)->Pindexdesc()->Keys();
			  			  
			  			  	return CCost(cef(&ci));
			  			  }
			  ```
		- The `CostInfo` should be well-designed, so that all different operators can share the same structure. And it has been decided written in c and maintained by `postgres`. ORCA and all other third-party optimizer can depend on it. A demo structure looks like following:  
			-  
			  ``` c
			  			  typedef struct CostInfo
			  			  {
			  			      double dTableWidth;
			  			      double dIndexFilterCostUnit;
			  			      double dIndexScanTupCostUnit;
			  			      double dIndexScanTupRandomFactor;
			  			      double dRowsIndex;
			  			      double dNumRebinds;
			  			      uint32_t ulIndexKeys;
			  			  } CostInfo;
			  ```
	- ## Cost Weight (No Clue Yet)  
		- For the specific operator, we need to decide it's cost weight. No clue yet. It should be decided in the similar way orca used to do.  
		-  
		  ``` c
		  		  double
		  		  hashorcacostestimate(CostInfo* ci)
		  		  {
		  		  	// We don't need random IO cost. But we have some other costs. How to decide it.
		  		  	double dCostPerIndexRow = ci->dTableWidth * ci->dIndexScanTupCostUnit;
		  		  	return ci->dNumRebinds *
		  		  				 (ci->dRowsIndex * dCostPerIndexRow);
		  		  }
		  ```
	- ## Property Enforcement  
		- Currently, for all indexes only `btscan` has order property. For other kinds of index scans we can just assume they don't have any property different than table scan  
	- ## TESTs (Not Finished Yet)  
		- related tests should be added  
- # Alternatives  
	- Using user-defined function in SQL instead of binding function to IndexAmRouting  
	- Retrieve Index in the first time then binding `ORCACostEstimateFunc` to `CExpression` and `CGroupExpression` and trigger it from expression during cost computing instead of retrieve index again  
	- Create new operators for `UserDefinedIndexScan` and `UserDefinedTableScan` instead of using the same operator as before  
	- Create extension by letting developer know the info of ORCA and depend on it. So that we can directly inject serialized cost model written in CPP to Postgres. If it cannot work, we can at least refer some header files in ORCA directly.  
	-  
	-  
	-  
	- ##  
-  