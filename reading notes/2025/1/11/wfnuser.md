RLP 而不用 json protocol buffers 的原因

1. 确保相同的输入产生相同的输出
2. 只处理字符串和列表 - 简单

字符串用 0x80 开头 - 列表用 0xc0 开头
