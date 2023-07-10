# 4.5
## 类型转换和格式化函数
```
select ID,coalesce(salary, 0) as salary
from instructor
```

## 解码
```
decode
```

## 缺省值
```
CREATE tabel stu
(
cred NUMERIC （3，0）default 0.
)
```

## 大对象类型
clob字符型
blob二进制型

## 用户自定义类型
- 独特类型
- 结构化数据类型
```
create type Dollars as numeric(12, 2) final;
```
之后就可以在create中用了。
- drop type & alter type删除或修改
- MySQL中的域：
	```
	create domain DDollars as numeric(12,2) not null
	```
	域不是强类型的

## 生成唯一码值
```
ID number(5) generated always as identity
```

## create table 的拓展
```
create table tme_ like instructor
# 
create table t1 as
	(select *
	from instructor
	where dept_name = 'xxx')
with data;
```
如果省略with data，表不会载入数据。

## 模式目录与环境
- 目录
- 模式
- 创建删除：
```
create schema
drop schema
```

# 4.6 sql中的索引定义
- 创建索引
```
create index <name> on <关系名> (<属性列表>);
```
# 4.7授权
## 权限的授予和收回
```
grant <授权列表>
on <关系名或视图名>
to <用户>

#更新
grant update (budget) on department to Amit,Satoshi;
```
public 也可以用作用户；

- 收权：
```
revoke <权限列表>
on <关系名或视图名>
from <用户/角色列表>

#
revoke update (budget) on department from Amit,Satoshi
```

## 角色
 ```
create role instructor;

#
grant select on takes;
to instructor;

#
create role dean;
grant instructor to dean;
grant dean to Satoshi;
```

## 视图的授权
- 更新权限，选择权限，执行权限
sql security invoker调用该函数的用户权限而不是函数定义者权限

## 模式的授权
```
grant references (dept_name) on department to Mariano;
```

## 权限的转移
```
grant select on department to Amit with grant option;

#权限的收回；级联收回
#防止级联收回
revoke select on department from Amit,Satosahi restrict;

#cascade表示级联收回，但是缺省方式

#收回授权选项而非收回选择权限
revoke grant option for select on department from Amit;

#通过角色来授权
granted by current_role

#行级授权（oracle）,使得学生只能看到ID值和ID相匹配的那些元组
ID v= sys_context('USERENV','SESSION_USER')

```
