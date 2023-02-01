- target
  - from            lisp-like function   to   c-like function 
  - e.g. :                 (add 2 2)                 add(2, 2)

- Most compilers break down into three primary stages: Parsing, Transformation, and Code Generation
- parsing 分为 lexical analysis 和 syntactic analysis 两部分
  - lexical analysis 将源码拆分为tokens
  - syntactic analysis 将tokens组织为AST（abstract syntax tree）