#include"common.h"
#include"file_op.h"
#include"index_handle.h"
#include<sstream>

using namespace std;
using namespace qiniu;
using namespace largefile;

//内存映射参数
const static largefile::MMapOption mmap_option = { 1024000,4096,4096 };
//主块文件的大小
const static uint32_t main_blocksize = 1024 * 1024 * 64;
const static uint32_t bucket_size = 1000;
static int32_t block_id = 1;
static int debug = 1;
int main(int argc,char** argv)
{
	std::string main_block_path;
	std::string index_path;
	int32_t ret = TFS_SUCCESS;

	cout << "Type your blockid:" << endl;
	cin >> block_id;
	if (block_id < 1) {
		cerr << "Invalid blockid\n" << endl;
		return -1;
	}
	//1.生成主块文件
	std::stringstream tmp_stream;
	tmp_stream << "." << MAINBLOCK_DIR_PREFIX << block_id;
	tmp_stream >> main_block_path;

	largefile::FileOperation* mainblock = new largefile::FileOperation(main_block_path, O_RDWR | O_LARGEFILE | O_CREAT);
	ret = mainblock->ftruncate_file(main_blocksize);
	if (ret != TFS_SUCCESS) {
		fprintf(stderr, "crate main block:%s failed.reason:%s\n", main_block_path.c_str(), strerror(errno));
		delete mainblock;
		return -2;
	}

	//2.创建索引文件
	largefile::IndexHandle* index_handle = new largefile::IndexHandle(".", block_id);
	if (debug) printf("init index...\n");
	ret = index_handle->create(block_id, bucket_size, mmap_option);
	if (ret != TFS_SUCCESS) {
		fprintf(stderr, "create index %d failed.\n", block_id);
		delete mainblock;
		delete index_handle;
		return -3;
	}

	//其他操作

	return 0;
}