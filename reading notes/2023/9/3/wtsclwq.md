移动语义和右值引用相配合：

在关闭返回值优化的情况下，运行如下代码：

```c++
class BigMemoryPool {
public:
  static const int PoolSize = 4096;
  BigMemoryPool() : pool_(new char[PoolSize]) {}
  ~BigMemoryPool()
  {
      if (pool_ != nullptr) {
            delete[] pool_;
      }
  }
  BigMemoryPool(BigMemoryPool&& other)
  {
      std::cout << "move big memory pool." << std::endl;
      pool_ = other.pool_;
      other.pool_ = nullptr;
  }

  BigMemoryPool(const BigMemoryPool& other) : pool_(new char[PoolSize])
  {
      std::cout << "copy big memory pool." << std::endl;
      memcpy(pool_, other.pool_, PoolSize);
  }
private:
  char *pool_;
};

BigMemoryPool get_pool(const BigMemoryPool& pool)
{
  return pool; // 第一次拷贝构造
}

BigMemoryPool make_pool()
{
  BigMemoryPool pool;
  return get_pool(pool); // 第二次拷贝/移动构造
}

int main()
{
  BigMemoryPool my_pool = make_pool(); //第三次拷贝/移动构造
}
```

如果上述代码中，没有移动构造函数，那么运行后会出现：

```shel	
copy big memory pool. 
copy big memory pool.
copy big memory pool.
```

而有了移动构造函数，再运行：

```shel	
copy big memory pool.
move big memory pool.
move big memory pool.
```

右值编译器会**优先选择使用移动构造函数**去构造目标对象。当移动构造函数不存在的时候才会退而求其次地使用复制构造函数。
