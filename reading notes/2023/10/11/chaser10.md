![[Pasted image 20230806155155.png]]
![[Pasted image 20230806155213.png]]
![[Pasted image 20230806155411.png]]
![[Pasted image 20230806155622.png]]

GORM 提供的是链式 API。如果遇到任何错误，GORM 会设置 `*gorm.DB` 的 `Error` 字段
![[Pasted image 20230806204026.png]]
#### 约定
Gorm使用名为id的字段作为主键
没有定义tablename方法的话就会使用结构体的蛇形负数作为表名
使用字段名的蛇形作为列名
使用CreateAt、UpdateAt作为创建、更新时间

#### 查询
First查不到数据会返回ErrRecordNotFound
一般用find查询不到返回空数组不会返回错误
使用结构体作为条件时 结构体中的零值会被忽略 不用来构建sql语句 有零值需求用map来构建查询条件


**只有动词update delete 这些才是真正执行sql语句 执行之后就不能再去拼接条件   Where  select这些都是用来拼sql语句**  想要获取结果需要从执行的返回 

#### Gorm事务
开启事务要执行db.Begin()获取新的db对象   这样会固化一个连接不再使用连接池里的连接  接着操作要用这个新的db对象    每次操作要判断是不是出错  如果出错就要回滚  都不出错就提交事务

gorm提供了transaction方法用于自动提交事务，避免用户漏写Commit、Rollback

#### Hook
钩子方法的函数签名应该是 `func(*gorm.DB) error`
// 开始事务  
BeforeSave  
BeforeCreate  
// 关联前的 save  
// 插入记录至 db  
// 关联后的 save  
AfterCreate  
AfterSave  
// 提交或回滚事务

gorm.Model组合了这个类型那么gorm能进行软删除