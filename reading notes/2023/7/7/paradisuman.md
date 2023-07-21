# ix_compare

# IxNodeHandle
/* 管理B+树中的每个节点 */
friend：
- IxIndexHandle
- IxScan
```cpp
const IxFileHdr *file_hdr;      // 节点所在文件的头部信息

    Page *page;                     // 存储节点的页面

    IxPageHdr *page_hdr;            // page->data的第一部分，指针指向首地址，长度为sizeof(IxPageHdr)

    char *keys;                     // page->data的第二部分，指针指向首地址，长度为file_hdr->keys_size，每个key的长度为file_hdr->col_len

    Rid *rids;                      // page->data的第三部分，指针指向首地址
```

# IxManager
- get_index_name
- exists
- create_index

# IxScan




