# TLS 记录协议概述

## 基本结构
- TLS 通过记录协议进行数据加密
- 每个记录包含:
  - 头部
  - 加密数据
- 记录大小限制:
  - 最大 16KB
  - 超过限制时分割为多条记录

## 密钥机制
- 使用单向密钥:
  - 浏览器→服务器密钥
  - 服务器→浏览器密钥
- 密钥由 TLS 密钥交换协议生成
- 双方都知道这两个密钥

## 有状态加密
- 使用内部状态维护加密
- 包含两个 64 位计数器:
  - 浏览器→服务器流量计数器
  - 服务器→浏览器流量计数器

### 计数器特点
- 初始化为零
- 每次发送记录时递增
- 双方同步维护计数器状态

### 计数器作用
- 防止重放攻击
- 攻击者无法重放旧记录(计数器已递增)
