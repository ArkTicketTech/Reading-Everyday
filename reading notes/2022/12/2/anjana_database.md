## Lecture #03: Database Storage I

### Sequential VS. Random Access

DBMS will want to maximize sequential access.

Allocating multiple pages at the same time is called an `extent`.

### memory mapping(mmap)❌

The DBMS can use memory mapping(mmap) to store the contents of a file into the address space of a program.

Unfortunately, this means that if mmap hits a `page fault`, the process will be `blocked`.

What if we allow multiple threads to access the mmap files to hide page fault stalls?

This works good enough for read-only access. It is complicated when there are multiple writers...

solutions:

* madvise: Tell the OS how you expect to read centrain pages.
* mlock: Tell the OS that memory ranges cannot be paged out.
* msync: Tell the OS to flush memory ranges out to disk.

哈哈哈，slides中有“The OS is not your friend.”，这是DB调侃OS嘛😂。

### File Storage

Three different notions of "pages" in a DBMS:

* Hardware Page(usually 4kB)
* OS Page(usually 4KB)
* Database Page(512B-16KB) 发现Database Pages的大小差异好大。

### Page Layout

##### 1. Slotted-pages

* Page maps slots to offsets.
* Header keeps track of 
  * the number of used slots
  * the offset of the `starting location` of the `last used slot`
  * a slot array: the location of the `start` of each tuple
* To add a tuple
  * the slot array will grow from the beginning to the end
  * the data of the tuples will grow from end to the beginning

##### 2. Log-structured

* Only store log records
* Fast writes, potentially slow reads(DBMS can have indexes to avoid long reads)
* periodically compact the log——> the DBMS ends up with write amplification

### Tuple Layout

##### Tuple Header: Contains meta-data about the tuple

##### Tuple Data: Actual data for attributes

##### Unique Identifier

**Record IDS**

Each tuple is assigned a unique record identifier.

* most common: page_id+offset/slot
* Can also contain file location info

An application cannot rely on these IDs to mean anything. 不太懂什么意思？