# optimizer.h
### Optimizer类
- 查询优化器
```cpp
SmManager *sm_manager_;
Planner *planner_;

std::shared_ptr<Plan> plan_query(std::shared_ptr<Query> query, Context *context) {
        if (auto x = std::dynamic_pointer_cast<ast::Help>(query->parse)) {
            // help;
            return std::make_shared<OtherPlan>(T_Help, std::string());
        } else if (auto x = std::dynamic_pointer_cast<ast::ShowTables>(query->parse)) {
            // show tables;
            return std::make_shared<OtherPlan>(T_ShowTable, std::string());
        } else if (auto x = std::dynamic_pointer_cast<ast::DescTable>(query->parse)) {
            // desc table;
            return std::make_shared<OtherPlan>(T_DescTable, x->tab_name);
        } else if (auto x = std::dynamic_pointer_cast<ast::TxnBegin>(query->parse)) {
            // begin;
            return std::make_shared<OtherPlan>(T_Transaction_begin, std::string());
        } else if (auto x = std::dynamic_pointer_cast<ast::TxnAbort>(query->parse)) {
            // abort;
            return std::make_shared<OtherPlan>(T_Transaction_abort, std::string());
        } else if (auto x = std::dynamic_pointer_cast<ast::TxnCommit>(query->parse)) {
            // commit;
            return std::make_shared<OtherPlan>(T_Transaction_commit, std::string());
        } else if (auto x = std::dynamic_pointer_cast<ast::TxnRollback>(query->parse)) {
            // rollback;
            return std::make_shared<OtherPlan>(T_Transaction_rollback, std::string());
        } else {
        //cpp文件中定义，生成DDL语句和DML语句的查询执行计划
            return planner_->do_planner(query, context);
        }
    }

```
据query->parse判断不同的分支返回plan

# plan.h
### PlanTag
### class Plan
- 还有很多plan的子类


# class Planner
SmManager *sm_manager_;

```cpp
//查询计划
private:
	//逻辑优化
    std::shared_ptr<Query> logical_optimization(std::shared_ptr<Query> query, Context *context);
    //物理优化
    std::shared_ptr<Plan> physical_optimization(std::shared_ptr<Query> query, Context *context);
    //推测是连接
    std::shared_ptr<Plan> make_one_rel(std::shared_ptr<Query> query);
    //排序查询
    std::shared_ptr<Plan> generate_sort_plan(std::shared_ptr<Query> query, std::shared_ptr<Plan> plan);
    //select
    std::shared_ptr<Plan> generate_select_plan(std::shared_ptr<Query> query, Context *context);
    //返回索引字段
    bool get_index_cols(std::string tab_name, std::vector<Condition> curr_conds, std::vector<std::string>& index_col_names);
    //sv_type转化为coltype
    ColType interp_sv_type(ast::SvType sv_type) {
        std::map<ast::SvType, ColType> m = {
            {ast::SV_TYPE_INT, TYPE_INT}, {ast::SV_TYPE_FLOAT, TYPE_FLOAT}, {ast::SV_TYPE_STRING, TYPE_STRING}};
        return m.at(sv_type);
    }
```

# planner.h
```cpp
// 生成DDL语句和DML语句的查询执行计划
std::shared_ptr<Plan> Planner::do_planner(std::shared_ptr<Query> query, Context *context)
{

    std::shared_ptr<Plan> plannerRoot;
    if (auto x = std::dynamic_pointer_cast<ast::CreateTable>(query->parse)) {
        // create table;
        std::vector<ColDef> col_defs;
        for (auto &field : x->fields) {
            if (auto sv_col_def = std::dynamic_pointer_cast<ast::ColDef>(field)) {
                ColDef col_def = {.name = sv_col_def->col_name,
                                  .type = interp_sv_type(sv_col_def->type_len->type),
                                  .len = sv_col_def->type_len->len};
                col_defs.push_back(col_def);
            } else {
                throw InternalError("Unexpected field type");
            }
        }
        plannerRoot = std::make_shared<DDLPlan>(T_CreateTable, x->tab_name, std::vector<std::string>(), col_defs);
    } 
    //略
    } else if (auto x = std::dynamic_pointer_cast<ast::DeleteStmt>(query->parse)) {
        // delete;
        // 生成表扫描方式
        std::shared_ptr<Plan> table_scan_executors;
        // 只有一张表，不需要进行物理优化了
        // int index_no = get_indexNo(x->tab_name, query->conds);
        std::vector<std::string> index_col_names;

        bool index_exist = get_index_cols(x->tab_name, query->conds, index_col_names);
        if (index_exist == false) {  // 该表没有索引
            index_col_names.clear();
            table_scan_executors =
			//T_SeqScan顺序扫描
                std::make_shared<ScanPlan>(T_SeqScan, sm_manager_, x->tab_name, query->conds, index_col_names);
        } else {  // 存在索引
            table_scan_executors =
			//索引扫描
                std::make_shared<ScanPlan>(T_IndexScan, sm_manager_, x->tab_name, query->conds, index_col_names);

        }
        plannerRoot = std::make_shared<DMLPlan>(T_Delete, table_scan_executors, x->tab_name,  
                                                std::vector<Value>(), query->conds, std::vector<SetClause>());
    } else if (auto x = std::dynamic_pointer_cast<ast::UpdateStmt>(query->parse)) {
        // update;
        // 生成表扫描方式
        std::shared_ptr<Plan> table_scan_executors;
        // 只有一张表，不需要进行物理优化了
        // int index_no = get_indexNo(x->tab_name, query->conds);
        std::vector<std::string> index_col_names;
        bool index_exist = get_index_cols(x->tab_name, query->conds, index_col_names);
  
        if (index_exist == false) {  // 该表没有索引
        index_col_names.clear();
            table_scan_executors =
                std::make_shared<ScanPlan>(T_SeqScan, sm_manager_, x->tab_name, query->conds, index_col_names);
        } else {  // 存在索引
            table_scan_executors =
                std::make_shared<ScanPlan>(T_IndexScan, sm_manager_, x->tab_name, query->conds, index_col_names);
        }
        plannerRoot = std::make_shared<DMLPlan>(T_Update, table_scan_executors, x->tab_name,
                                                     std::vector<Value>(), query->conds,
                                                     query->set_clauses);
    } else if (auto x = std::dynamic_pointer_cast<ast::SelectStmt>(query->parse)) {
        std::shared_ptr<plannerInfo> root = std::make_shared<plannerInfo>(x);
        // 生成select语句的查询执行计划
        std::shared_ptr<Plan> projection = generate_select_plan(std::move(query), context);
        plannerRoot = std::make_shared<DMLPlan>(T_select, projection, std::string(), std::vector<Value>(),
                                                    std::vector<Condition>(), std::vector<SetClause>());
    } else {
        throw InternalError("Unexpected AST root");
    }
    return plannerRoot;
}
```
- selectsmt中会有generate_select_plan，其中会调用逻辑和物理优化

### physical_optimization
```cpp
std::shared_ptr<Plan> Planner::physical_optimization(std::shared_ptr<Query> query, Context *context)
{
    std::shared_ptr<Plan> plan = make_one_rel(query);
    // 其他物理优化
  

    // 处理orderby
    plan = generate_sort_plan(query, std::move(plan));
    return plan;
}
```

```cpp
//make_one_rel
std::shared_ptr<Plan> Planner::make_one_rel(std::shared_ptr<Query> query)

{

    auto x = std::dynamic_pointer_cast<ast::SelectStmt>(query->parse);

    std::vector<std::string> tables = query->tables;

    // // Scan table , 生成表算子列表tab_nodes

    std::vector<std::shared_ptr<Plan>> table_scan_executors(tables.size());

    for (size_t i = 0; i < tables.size(); i++) {

        auto curr_conds = pop_conds(query->conds, tables[i]);

        // int index_no = get_indexNo(tables[i], curr_conds);

        std::vector<std::string> index_col_names;

        bool index_exist = get_index_cols(tables[i], curr_conds, index_col_names);

        if (index_exist == false) {  // 该表没有索引

            index_col_names.clear();

            table_scan_executors[i] =

                std::make_shared<ScanPlan>(T_SeqScan, sm_manager_, tables[i], curr_conds, index_col_names);

        } else {  // 存在索引

            table_scan_executors[i] =

                std::make_shared<ScanPlan>(T_IndexScan, sm_manager_, tables[i], curr_conds, index_col_names);

        }

    }

    // 只有一个表，不需要join。

    if(tables.size() == 1)

    {

        return table_scan_executors[0];

    }

    // 获取where条件

    auto conds = std::move(query->conds);

    std::shared_ptr<Plan> table_join_executors;

    int scantbl[tables.size()];

    for(size_t i = 0; i < tables.size(); i++)

    {

        scantbl[i] = -1;

    }

    // 假设在ast中已经添加了jointree，这里需要修改的逻辑是，先处理jointree，然后再考虑剩下的部分

    if(conds.size() >= 1)

    {

        // 有连接条件

  

        // 根据连接条件，生成第一层join

        std::vector<std::string> joined_tables(tables.size());

        auto it = conds.begin();

        while (it != conds.end()) {

            std::shared_ptr<Plan> left , right;

            left = pop_scan(scantbl, it->lhs_col.tab_name, joined_tables, table_scan_executors);

            right = pop_scan(scantbl, it->rhs_col.tab_name, joined_tables, table_scan_executors);

            std::vector<Condition> join_conds{*it};

            //建立join

            table_join_executors = std::make_shared<JoinPlan>(T_NestLoop, std::move(left), std::move(right), join_conds);

            it = conds.erase(it);

            break;

        }

        // 根据连接条件，生成第2-n层join

        it = conds.begin();

        while (it != conds.end()) {

            std::shared_ptr<Plan> left_need_to_join_executors = nullptr;

            std::shared_ptr<Plan> right_need_to_join_executors = nullptr;

            bool isneedreverse = false;

            if (std::find(joined_tables.begin(), joined_tables.end(), it->lhs_col.tab_name) == joined_tables.end()) {

                left_need_to_join_executors = pop_scan(scantbl, it->lhs_col.tab_name, joined_tables, table_scan_executors);

            }

            if (std::find(joined_tables.begin(), joined_tables.end(), it->rhs_col.tab_name) == joined_tables.end()) {

                right_need_to_join_executors = pop_scan(scantbl, it->rhs_col.tab_name, joined_tables, table_scan_executors);

                isneedreverse = true;

            }

  

            if(left_need_to_join_executors != nullptr && right_need_to_join_executors != nullptr) {

                std::vector<Condition> join_conds{*it};

                std::shared_ptr<Plan> temp_join_executors = std::make_shared<JoinPlan>(T_NestLoop,

                                                                    std::move(left_need_to_join_executors),

                                                                    std::move(right_need_to_join_executors),

                                                                    join_conds);

                table_join_executors = std::make_shared<JoinPlan>(T_NestLoop, std::move(temp_join_executors),

                                                                    std::move(table_join_executors),

                                                                    std::vector<Condition>());

            } else if(left_need_to_join_executors != nullptr || right_need_to_join_executors != nullptr) {

                if(isneedreverse) {

                    std::map<CompOp, CompOp> swap_op = {

                        {OP_EQ, OP_EQ}, {OP_NE, OP_NE}, {OP_LT, OP_GT}, {OP_GT, OP_LT}, {OP_LE, OP_GE}, {OP_GE, OP_LE},

                    };

                    std::swap(it->lhs_col, it->rhs_col);

                    it->op = swap_op.at(it->op);

                    left_need_to_join_executors = std::move(right_need_to_join_executors);

                }

                std::vector<Condition> join_conds{*it};

                table_join_executors = std::make_shared<JoinPlan>(T_NestLoop, std::move(left_need_to_join_executors),

                                                                    std::move(table_join_executors), join_conds);

            } else {

                push_conds(std::move(&(*it)), table_join_executors);

            }

            it = conds.erase(it);

        }

    } else {

        table_join_executors = table_scan_executors[0];

        scantbl[0] = 1;

    }

  

    //连接剩余表

    for (size_t i = 0; i < tables.size(); i++) {

        if(scantbl[i] == -1) {

            table_join_executors = std::make_shared<JoinPlan>(T_NestLoop, std::move(table_scan_executors[i]),

                                                    std::move(table_join_executors), std::vector<Condition>());

        }

    }

  

    return table_join_executors;

  

}
```
- conds是where子句
- 先处理所有表，根据是否有索引放在table_scan_executors（没变），conds删除掉单句（左右表名一样或者a=1这种）
- 第一层先获取左右的scanplan，把每个左右分别和table_scan_executors连接，放在joined_tables中，连接左右col，conds删除掉
- 之后剩余conds先找join_table有没有，都没有就左右合并然后放进去，一个的话就合并一次，都在就合并到子句里面重新迭代
### get_index_cols
- 用来判断是否在索引中，并把单句查找到

### pop_scan
- 找到table_scan_executors中的相同表名元素，连接加入joined_tables，返回plans[i]