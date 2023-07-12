- RmManager
	- DiskManager
	- BufferPoolManager

- BufferPoolManager
	- pool_size_
	- pages_
	- page_table_
	- free_list_
	- disk_manager_
	- replacer_
	- latch_

- RmFileHandle
	-friend：
	- RmScan
	- RmManager
	-private
	- DiskManager
	- RmFileHdr

- RmPageHandle
	- RmFileHdr
	- Page
	- RmPageHdr

- RmPageHdr
- Bitmap
- RmFileHdr
- RmFileHdr
- RmRecord
- RmScan
	- RmFileHandle
- struct Rid