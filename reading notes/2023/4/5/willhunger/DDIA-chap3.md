##### B-tree optimization
* 写操作时不再使用覆盖页面和维护 WAL ，使用 copy-on-write 页面来进行，该方法类似对于 concurrency control 作用较大。LMDB 采用该方法。
* 在页面中不再保存完整的键，而是保存键的缩略信息，以节省页面空间，使得一个页内能容纳更多的键，使得 B-Tree 的 branching factor 越大，层数越少。
* 对 B-Tree 页面进行布局，使得相邻子页可以按照顺序保存在磁盘上，减少磁盘 IO 带来的开销。但缺点是，随着 B-Tree 的增长，维护页面顺序会变得更困难。相反，LSM-Tree 由于 compaction 阶段的 merge 操作，会使得连续的键在磁盘上位置上更近。
* 在 B-Tree 中维护额外的指针。例如，叶子节点间添加相邻级别的左右兄弟页指针，在顺序扫描时，不需要跳转回父页。
* B-Tree 变体 fractal tree，借鉴了 log-structed 来减少磁盘寻道。