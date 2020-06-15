#include "mmap_file.h"

static int debug = 1;

qiniu::largefile::MMapFile::MMapFile()
	:size_(0),fd_(1),data_(NULL)
{
}


qiniu::largefile::MMapFile::MMapFile(const int fd)
	:size_(0), fd_(fd), data_(NULL)
{
}

qiniu::largefile::MMapFile::MMapFile(const MMapOption& mmap_option, const int fd)
	:size_(0), fd_(fd), data_(NULL)
{
	mmap_file_option_.max_mmap_size_ = mmap_option.max_mmap_size_;
	mmap_file_option_.first_mmap_size_ = mmap_option.first_mmap_size_;
	mmap_file_option_.per_mmap_size_ = mmap_option.per_mmap_size_;
}


qiniu::largefile::MMapFile::~MMapFile()
{
	if (data_) {
		if (debug) printf("mmap file destruct,fd:%d,size:%d data:%p\n", fd_, size_, data_);
		msync(data_, size_, MS_SYNC);
		munmap(data_, size_);
		size_ = 0;
		data_ = NULL;
		fd_ = -1;

		mmap_file_option_.max_mmap_size_ = 0;
		mmap_file_option_.first_mmap_size_ = 0;
		mmap_file_option_.per_mmap_size_ = 0;
	}
}

bool qiniu::largefile::MMapFile::sync_file()
{
	if (NULL != data_ && size_ > 0) {
		return msync(data_, size_, MS_ASYNC)==0;	//异步同步
	}
	return true;
}

bool qiniu::largefile::MMapFile::map_file(const bool write)
{
	int flags = PROT_READ;
	if (write) {
		flags |= PROT_WRITE;
	}
	if (fd_ < 0) {
		return false;
	}
	if (0 == mmap_file_option_.max_mmap_size_) {
		return false;
	}
	if (size_ < mmap_file_option_.max_mmap_size_) {
		size_ = mmap_file_option_.first_mmap_size_;
	}
	else
	{
		size_ = mmap_file_option_.max_mmap_size_;
	}

	if (!ensure_file_size(size_)) {
		fprintf(stderr, "ensure file size failed in map_file,size:%d\n",size_);
		return false;
	}
	data_ = mmap(0, size_, flags,MAP_SHARED, fd_,0);
	if (MAP_FAILED == data_) {
		fprintf(stderr,"map file faild:%s\n",strerror(errno));
		size_ = 0;
		data_ = NULL;
		fd_ = -1;
		return false;
	}
	if(debug) printf("mmap file destruct fd:%d size:%d data:%p\n", fd_, size_, data_);
	return true;
}

void* qiniu::largefile::MMapFile::get_data() const
{
	return data_;
}

int32_t qiniu::largefile::MMapFile::get_size() const
{
	return size_;
}

bool qiniu::largefile::MMapFile::munmap_file()
{
	if (munmap(data_, size_) == 0) {
		return true;
	}
	else {
		return false;
	}
}

bool qiniu::largefile::MMapFile::remap_file()
{
	//1.防御性编程
	if (fd_ < 0 || data_ == 0) {
		fprintf(stderr, "remap error faild:%s\n", strerror(errno));
		return false;
	}
	if (size_ == mmap_file_option_.max_mmap_size_) {
		fprintf(stderr, "already mapped max size error faild:%s\n", strerror(errno));
		return false;
	}
	int32_t new_size = size_ + mmap_file_option_.per_mmap_size_;
	if (new_size > mmap_file_option_.max_mmap_size_) {
		new_size = mmap_file_option_.max_mmap_size_;
	}
	if (!ensure_file_size(new_size)) {
		fprintf(stderr, "meremap stat.fd:%d new_size:%d old size:%d data:%p\n",
			fd_,new_size, size_,data_);
		return false;
	}

	if (debug) printf("mmap file destruct,fd:%d size:%d data:%p\n", fd_, size_, data_);

	//重新映射  扩大或缩小现有的内存映射
	void* new_map_data = mremap(data_,size_, new_size,MREMAP_MAYMOVE);
	if (MAP_FAILED == new_map_data) {
		fprintf(stderr, "meremap faild fd:%d new_size:%d error desc:%s\n", 
			fd_,new_size,strerror(errno));
		return false;
	}
	else {
		if (debug) printf("mermap file success fd:%d new_size:%d error:%s\n", 
			fd_, new_size, strerror(errno));
	}
	data_ = new_map_data;
	size_ = new_size;
	return true;
}

bool qiniu::largefile::MMapFile::ensure_file_size(const int32_t size)
{
	struct stat s;
	if (fstat(fd_,&s) < 0) {
		fprintf(stderr, "fstat error faild:%s\n", strerror(errno));
		return false;
	}
	if (s.st_size < size) {
		if (ftruncate(fd_, size) < 0) {
			fprintf(stderr, "ftrucate error faild:%s\n", strerror(errno));
			return false;
		}
	}
	return true;
}
