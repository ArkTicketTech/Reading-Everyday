## 4. Design Choices & Optimization

### 4.3 Scaleable Timestamp Ordering

* Timestamp Allocation  
  *  atomic addition with batching
  *  CPU clocks: logical clock + thread_id
  *  hardware counters
* Distributed Validation: 使用单独元组以细粒度的方式验证 OCC 读操作的验证阶段
* Local Partitions
