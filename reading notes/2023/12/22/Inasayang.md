-   校验和checksum
    -   最弱的保证形式，不能检测多个比特位的损坏
-   循环冗余校验crc
    -   可以帮助检测突发错误（多个连续比特位的损坏）
    -   确保数据没有非人为的，意外的变化，而非用于抵御攻击或认为的修改



页头保存可用于页定位，维护和优化的信息

-   页内容
-   布局的标志位
-   页中单元格的数量
-   标记空闲空间的上届与下届的偏移量
-   其他有用数据



魔数，页头中放置的一个值，一个多字节块，包含一个常数值，表示改块代表一个页，并指定页的类型或标识其版本；通常用于校验和完整性检查



同级指针和最右指针





