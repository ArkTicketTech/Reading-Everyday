今天看书了，但只有slides做了笔记。

## Lecture #04: Database Storage II

### Data Representation

#### Lager vaues

DBMS uses separate `overflow` storage pages.

#### External vaule storage

external file treated as a `BLOB` type.

The DBMS cannot manipulate the contents of an external file.

* No durability protections
* No transaction protections

#### System catalogs

A DBMS stores `meta-data` about databases in its `internal catalogs`.

* Tables, columns, indexes, views
* Users, permissions
* Internal statistics

## Lecture #05: Storage Models & Compression

### N-ary storage model

cons:

* Fast inserts, updates, and deletes
* Good for quries that need the entire tuple

pros:

* Not good for scanning large portions of the table and/or a subset of the attributes

### Decomposition storage model(DSM)

"column store": stores the values of a `single attribute` for `all tuples` contiguously in a page.

 cons:

* Reduce the amount wasted I/O
* Better query processing and data compression

pros:

* Slow for point queries, inserts, updates, and deletes because of tuple splitting/stitching
