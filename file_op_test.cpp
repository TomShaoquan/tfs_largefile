#include"file_op.h"
#include"common.h"
using namespace qiniu;
using namespace largefile;

#define BUF 64
#define WRITE_SET 128
int main33()
{
	const char* filename = "file_op.txt";
	largefile::FileOperation* fileOP = new largefile::FileOperation(filename, O_CREAT | O_RDWR | O_LARGEFILE);
	int fd = fileOP->open_file();
	if (fd < 0) {
		fprintf(stderr, "open file %s faild reason:%s\n", filename, strerror(-fd));
		return -1; 
	}
	char buffer[BUF+1];
	memset(buffer, '8', BUF);
	int ret=fileOP->pwrite_file(buffer, BUF, WRITE_SET);
	if (ret < 0) {
		if (ret == EXIT_DISK_OPEN_INCOMPLETE) {
			fprintf(stderr, "read or write length is less than required.");
		}
		else {
			fprintf(stderr, "pwrite file %s faild reason:%s\n", filename, strerror(-ret));
		}
	}
	memset(buffer, 0, BUF);
	ret = fileOP->pread_file(buffer, BUF, WRITE_SET);
	if (ret < 0) {
		if (ret == EXIT_DISK_OPEN_INCOMPLETE) {
			fprintf(stderr, "read or write length is less than required.");
		}
		else {
			fprintf(stderr, "pread file %s faild reason:%s\n", filename, strerror(-ret));
		}
	}
	else {
		buffer[BUF] = '\0';
		printf("read:%s\n", buffer);
	}
	memset(buffer, '9', BUF);
	ret=fileOP->write_file(buffer, BUF);
	if (ret < 0) {
		if (ret == EXIT_DISK_OPEN_INCOMPLETE) {
			fprintf(stderr, "read or write length is less than required.");
		}
		else {
			fprintf(stderr, "write_file file %s faild reason:%s\n", filename, strerror(-ret));
		}
	}
	fileOP->close_file();
	return 0;
}