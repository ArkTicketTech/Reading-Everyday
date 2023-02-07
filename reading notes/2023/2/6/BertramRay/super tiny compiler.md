https://github.com/jamiebuilds/the-super-tiny-compiler/blob/master/the-super-tiny-compiler.js

- target
  - from            lisp-like function   to   c-like function 
  - e.g. :                 (add 2 2)                 add(2, 2)

- Most compilers break down into three primary stages: Parsing, Transformation, and Code Generation
- parsing 分为 lexical analysis 和 syntactic analysis 两部分
  - lexical analysis 将源码拆分为tokens
  - syntactic analysis 将tokens组织为AST（abstract syntax tree）

------------

 ### raw code

 *   (add 2 (subtract 4 2))

 ### tokens

 *   [
 *   { type: 'paren',  value: '('        },
 *   { type: 'name',   value: 'add'      },
 *   { type: 'number', value: '2'        },
 *   { type: 'paren',  value: '('        },
 *   { type: 'name',   value: 'subtract' },
 *   { type: 'number', value: '4'        },
 *   { type: 'number', value: '2'        },
 *   { type: 'paren',  value: ')'        },
 *   { type: 'paren',  value: ')'        },
 *   ]

 ### AST

  *   {
 *   type: 'Program',
 *   body: [{
 *   type: 'CallExpression',
 *   name: 'add',
 *   params: [{
 *   type: 'NumberLiteral',
 *   value: '2',
 *   }, {
 *   type: 'CallExpression',
 *   name: 'subtract',
 *   params: [{
 *   type: 'NumberLiteral',
 *   value: '4',
 *   }, {
 *   type: 'NumberLiteral',
 *   value: '2',
 *   }]
 *   }]
 *   }]
 *   }

 - the traversal process goes to each node in dfs
 - 需要有一个visitor函数，根据node的类型进行相应的node转换操作
 - code generations时有时会重用tokens，但是大部分场景还是使用刚刚创建的转换过的AST

------------------

- 实际代码实现时，tokenizer部分的代码通过一个curser对raw code进行扫描，并转化成相关token，似乎并没有在这个阶段做充分的语法检查（比如括号是否闭合，或者混合串等），如果遇到无法识别的字符就抛出异常

- 在实现parser时，使用了walk函数，和tokenizer不同，parser是递归的
- 实现traverser时，traverser接收AST和visitor作为参数