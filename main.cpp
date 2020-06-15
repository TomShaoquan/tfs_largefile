
#include"mmap_file.h"

#include"common.h"

/*linux 远程目录
/home/pop/projects/LinunRemoteProject/bin/x64/Debug
*/
using namespace std;
using namespace qiniu;

static const mode_t OPEN_MODE = 0644;
//1024X4=4096 10000K  4K 4K
const static largefile::MMapOption mmap_option = {10240000,4096,4096};

int open_file(string file_name,int open_flags) {
	int fd = open(file_name.c_str(), open_flags, OPEN_MODE);
	if (fd < 0) {
		return -errno;
	}
	return fd;
}
int main22()
{
	//1.创建一个文件,取得文件的句柄
	const char* fileName = "./mapfile.txt";
	int fd=open_file(fileName, O_RDWR | O_CREAT | O_LARGEFILE);
	if (fd < 0) {
		fprintf(stderr, "open file faild fileName:%s error:%s\n", fileName, strerror(-errno));
		return -1;
	}

	largefile::MMapFile* map_file = new largefile::MMapFile(mmap_option,fd);
	bool is_mapped = map_file->map_file(true);
	if (is_mapped) {
		memset(map_file->get_data(), '9', map_file->get_size());
		map_file->sync_file();
		map_file->munmap_file();		//解除映射
	}
	else {
		fprintf(stderr, "map file failure");
	}
	close(fd);
	return 0;
}