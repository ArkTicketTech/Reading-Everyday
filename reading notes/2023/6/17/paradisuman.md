# 8 子句查询
## 集合成员资格
```
select distinct course id
from section 
where semester = 'Fall' and year= 2017 
		and course id in (select course id 
						from section where semester = 'Spring' and year= 2018);
```

```
select distinct course id 
from section 
where semester = 'Fall' and year= 2017 
	and course id not in (select course id 
						from section 
						where semester = 'Spring' and year= 2018)
```

## 空关系查询
- exists
```
SELECT course_id
FROM section AS S
WHERE semester = 'Fall' AND year = 2017 AND
    EXISTS (
        SELECT *
        FROM section AS T
        WHERE semester = 'Spring' AND year = 2018 AND
            S.course_id = T.course_id
    );
```
- not exists
```
SELECT S.ID, S.name
FROM student AS S
WHERE NOT EXISTS (
    (SELECT course_id
    FROM course
    WHERE dept_name = 'Biology')
    EXCEPT
    (SELECT T.course_id
    FROM takes AS T
    WHERE S.ID = T.ID)
);

```

## 重复元组存在性测试
- unique
```
SELECT T.course_id
FROM course AS T
WHERE (SELECT COUNT(DISTINCT R.course_id)
       FROM section AS R
       WHERE T.course_id = R.course_id AND R.year = 2017) = 1;

```

- 另一种表达
```
SELECT T.course_id
FROM course AS T
WHERE 1 >= (SELECT COUNT(R.course_id)
             FROM section AS R
             WHERE T.course_id = R.course_id AND R.year = 2017);

```
- not unique
```
SELECT T.course_id
FROM course AS T
WHERE NOT (SELECT COUNT(DISTINCT R.course_id)
            FROM section AS R
            WHERE T.course_id = R.course_id AND R.year = 2017) = 1;

```

## from 子句中的子查询
- from
```
SELECT dept_name, avg_salary
FROM (
    SELECT dept_name, AVG(salary) AS avg_salary
    FROM instructor
    GROUP BY dept_name
) AS subquery
WHERE avg_salary > 42000;

```
- 用as
```
SELECT dept_name, avg_salary
FROM (
    SELECT dept_name, AVG(salary) AS avg_salary
    FROM instructor
    GROUP BY dept_name
) AS dept_avg
WHERE avg_salary > 42000;

```
- 有些语言不允许不用别名嵌套
>select dept name, avg(salary) as avg salary

- lateral可以查前面的
```
SELECT I1.name, I1.salary, avg_salary
FROM instructor I1, LATERAL (
    SELECT AVG(salary) AS avg_salary
    FROM instructor I2
    WHERE I2.dept_name = I1.dept_name
) AS avg_salary_subquery;

```

## with子句

## 标量子查询
- 用于select

# 9 数据库修改
## 删除
```
delete from r
where p
```
## 插入
```
intsrt into course
	values('CS-437','DATABASE','sci',4);

insert into course (course id, title, dept name, credits) 
	values ('CS-437', 'Database Systems', 'Comp. Sci.', 4); 
	
insert into course (title, course id, credits, dept name) 
	values ('Database Systems', 'CS-437', 4, 'Comp. Sci.');
```



