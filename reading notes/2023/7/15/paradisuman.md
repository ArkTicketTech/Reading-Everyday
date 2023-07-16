# analyze.h
- Query
	```
	class Query{
	    public:
	    std::shared_ptr<ast::TreeNode> parse;
	    // TODO jointree
	    // where条件
	    std::vector<Condition> conds;
	    // 投影列
	    std::vector<TabCol> cols;
	    // 表名
	    std::vector<std::string> tables;
	    // update 的set 值
	    std::vector<SetClause> set_clauses;
	    //insert 的values值
	    std::vector<Value> values;
	    Query(){}
	};
	```
- Analyze
analyze中有sm_manager
```
private:
	SmManager *sm_manager_;
	//略
private:
	    TabCol check_column(const std::vector<ColMeta> &all_cols, TabCol target);
	    void get_all_cols(const std::vector<std::string> &tab_names, std::vector<ColMeta> &all_cols);
	    void get_clause(const std::vector<std::shared_ptr<ast::BinaryExpr>> &sv_conds, std::vector<Condition> &conds);
	    void check_clause(const std::vector<std::string> &tab_names, std::vector<Condition> &conds);
	    Value convert_sv_value(const std::shared_ptr<ast::Value> &sv_val);
	    CompOp convert_sv_comp_op(ast::SvCompOp op);
```

# analyze.cpp
### do_analyze
- std::dynamic_pointer_cast<ast::SelectStmt>(parse)
类似语句判断是什么类型的stmt

- 写入query->cols

- all_cols放入所有的查询的列的表中的列

- check_column校验列是否都有所属表

- 调用get_clause(x->conds, query->conds);

- 调用check_clause(query->tables, query->conds);

- 注意ast和别的地方都有相同名称的类

### get_all_cols
all_cols放入所有的查询的列的表中所有的列

### check_column(&all_cols, TabCol target)
调用check_column(all_cols, sel_col)，找到select每个列的表，如果有多个报错。

### get_clause
先解析左值和op，然后右值分为值或者列，然后再解析。

### check_clause
先检查左值的check_column，如果右值不为value，检查check_column，如果为value，init_raw检查值对不对；获取右值类型，然后对比左右值是否匹配
