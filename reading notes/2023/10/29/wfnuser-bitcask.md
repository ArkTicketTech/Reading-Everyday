今天面试写了一个 sample code 似乎并不需要在文件里存储key的信息也可以工作 有什么优劣需要回头考究一下

```cpp
#include<iostream>
#include<cstring>
#include<map>

struct FileOffset {
	int file_id;
	int offset;
	int size;
};

class BitCask {

private:
	unordered_map<string, FileOffset> data;
	string data_dir;

	int cur_file;
	int cur_offset;

	int limit_size;

public:
	BitCask(string data_dir): data_dir(data_dir) {
	}

	bool Put(string key, string val) {
		int length = key.size();

		if (cur_offset + length > limit_size) {
			cur_offset = 0;
			file_id++;
		}

		create_file(data_dir, file_id);

		FileOffset fo = FileOffset(file_id, cur_offset, length);
		append_wal(key, fo.file_id, fo.cur_offset, fo.length);
		write_file(cur_offset, val);
		commit_wal();
		data[key] = fo;

		cur_offset += length;
	}

	bool Get(string key, string& val) {
		if (data.find(key) == data.end()) {
			return false;
		}

		FileOffset fo = data[key];
		string targetFile = build_file_path(fo, data_dir);

		if (exist(targetFile)) {
			read_file(targetFile, fo.offset, fo.size, &val);
		} else {
			throw error;
		}

		return true;
	}
}
```