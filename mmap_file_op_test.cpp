#include"mmap_file_op.h"
#include<iostream>
using namespace std;
using namespace qiniu;
using namespace largefile;
#define BUF 128
//共享路径:\\192.168.92.128\Share\projects\LinunRemoteProject\bin\x64\Debug
const static largefile::MMapOption mmap_option = { 10240000,4096,4096 };

int main44()
{
	const char* filename = "mmap_file_op.txt";
	MMapFileOperation mmfo(filename);
	int fd = mmfo.open_file();
	if (fd < 0) {
		fprintf(stderr, "open file %s error.reason:%s", filename, strerror(errno));
		return -1;
	}

	int ret = 0;
	ret = mmfo.mmap_file(mmap_option);
	if (ret == TFS_ERROR)
	{
		fprintf(stderr, "mmap_file error.reason:%s", strerror(errno));
		mmfo.close_file();
		return -2;
	}
	char buf[BUF+1];
	memset(buf, '6', BUF);
	ret = mmfo.pwrite_file(buf, BUF, 8);
	if (ret < 0) {
		if (ret == EXIT_DISK_OPEN_INCOMPLETE) {
			fprintf(stderr, "read or write length is less than required.");
		}
		else {
			fprintf(stderr, "pwrite file %s faild reason:%s\n", filename, strerror(-ret));
		}
	}

	//
	memset(buf, 0, BUF);
	ret = mmfo.pread_file(buf, BUF, 8);
	if (ret < 0) {
		if (ret == EXIT_DISK_OPEN_INCOMPLETE) {
			fprintf(stderr, "read or write length is less than required.");
		}
		else {
			fprintf(stderr, "pread file %s faild reason:%s\n", filename, strerror(-ret));
		}
	}
	else {
		buf[BUF] = '\0';
		printf("read:%s\n", buf);
	}
	ret = mmfo.flush_file();
	if (ret == TFS_ERROR) {
		fprintf(stderr, "flush file failed. reason:%s", strerror(errno));
		return -3;
	}

	//解除文件映射
	mmfo.close_file();

	return 0;
}