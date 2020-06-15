#include "mmap_file_op.h"
#include<inttypes.h>
#include<string>
using namespace std;

static int debug = 1;
qiniu::largefile::MMapFileOperation::MMapFileOperation(const std::string& file_name, const int open_flags)
	:FileOperation(file_name,open_flags)
	,map_file_(NULL)
	,is_mapped_(false)
{

}

qiniu::largefile::MMapFileOperation::~MMapFileOperation()
{
	if (map_file_) {
		map_file_ = NULL;
	}

}

int qiniu::largefile::MMapFileOperation::pread_file(char* buf, const int32_t size, const int64_t offset)
{
	//1.内存已经映射
	if (is_mapped_ && (offset + size) > map_file_->get_size()) {
		//__PRI64_PREFIX
		if (debug) fprintf(stdout,
			"mmap_file_op pread size:%d offset:%ld map file size:%d need remap\n",
			size, offset, map_file_->get_size());
		map_file_->remap_file();
	}
	if (is_mapped_ && (offset + size) <=map_file_->get_size()) {
		memcpy(buf, (char*)map_file_->get_data() + offset,size);
		return TFS_SUCCESS;
	}
	
	//2.内存没有映射
	return FileOperation::pread_file(buf,size,offset);
}

int qiniu::largefile::MMapFileOperation::pwrite_file(const char* buf, const int32_t size, const int64_t offset)
{
	//1.内存已经映射
	if (is_mapped_ && (offset + size) > map_file_->get_size()) {
		//__PRI64_PREFIX
		if (debug) fprintf(stdout,
			"mmap_file_op pread size:%d offset:%ld map file size:%d need remap\n",
			size, offset, map_file_->get_size());
		map_file_->remap_file();
	}
	if (is_mapped_ && (offset + size) <= map_file_->get_size()) {
		memcpy((char*)map_file_->get_data() + offset,buf,size);
		return TFS_SUCCESS;
	}

	//2.内存没有映射
	return FileOperation::pwrite_file(buf,size,offset);
}

int qiniu::largefile::MMapFileOperation::mmap_file(const MMapOption mmap_option)
{
	if (mmap_option.max_mmap_size_ < mmap_option.first_mmap_size_) {
		return TFS_ERROR;
	}
	if (mmap_option.max_mmap_size_ <= 0) {
		return TFS_ERROR;
	}
	int fd = check_file();
	if (fd < 0) {
		fprintf(stderr, "MMapFileOperation::mmap_file-check_file error!");
		return TFS_ERROR;
	}
	if (!is_mapped_) {
		if (map_file_) {
			delete map_file_;
		}
		map_file_ = new MMapFile(mmap_option, fd);
		is_mapped_ = map_file_->map_file(true);
	}
	if (is_mapped_) {
		return TFS_SUCCESS;
	}
	else {
		return TFS_ERROR;
	}
}

int qiniu::largefile::MMapFileOperation::munmap()
{
	if (is_mapped_ && map_file_ != NULL) {
		delete(map_file_);
		is_mapped_ = false;
	}
	return TFS_SUCCESS;
}

void* qiniu::largefile::MMapFileOperation::get_map_data() const
{
	if (is_mapped_) {
		return map_file_->get_data();
	}
	return NULL;
}

int qiniu::largefile::MMapFileOperation::flush_file()
{
	if (is_mapped_) {
		if (map_file_->sync_file()) {
			return TFS_SUCCESS;
		}
		else
		{
			return TFS_ERROR;
		}
	}
	//没有映射
	return FileOperation::flush_file();
}
