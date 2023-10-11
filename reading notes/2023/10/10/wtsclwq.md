	5. constexpr还能够声明用户自定义类型，但是对于非class，需要额外的constexpr构造函数。

    ```cpp
    class X {
    public:
      X() : x1(5) {}
      int get() const{
         return x1;
      }
    private:
      int x1;
    };
    constexpr X x;                    // 编译失败，X不是字面类型
    char buffer[x.get()] = { 0 };     // 编译失败，x.get()无法在编译阶段计算
    ```


    ```cpp
    class X {
    public:
      constexpr X() : x1(5) {}
      constexpr X(int i) : x1(i) {}
      constexpr int get() const{
    	return x1;
      }
    private:
      int x1;
    };
    constexpr X x;
    char buffer[x.get()] = { 0 };
    ```

	1. constexpr支持声明浮点类型的常量表达式值，而且标准还规定其精度必须至少和运行时的精度相同。
	2. 从C++17开始，lambda表达式在条件允许的情况下都会隐式声明为constexpr。
