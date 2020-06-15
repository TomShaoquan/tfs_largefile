#include "file_op.h"

qiniu::largefile::FileOperation::FileOperation(const std::string& file_name, const int open_flags)\
	:fd_(-1),open_flags(open_flags)
{
	//复制字符串
	file_name_ = strdup(file_name.c_str());

}

qiniu::largefile::FileOperation::~FileOperation()
{
	if (fd_ > 0) {
		//使用库文件提供的函数close
		::close(fd_);
	}
	if (NULL != file_name_) {
		free(file_name_);
		file_name_ = NULL;
	}
}

int qiniu::largefile::FileOperation::open_file()
{
	if (fd_ > 0) {
		close(fd_);
		fd_ = -1;
	}
	fd_ = ::open(file_name_, open_flags, OPEN_MODE);
	if (fd_ < 0) {
		return -errno;
	}
	return fd_;
}

void qiniu::largefile::FileOperation::close_file()
{
	if (fd_ < 0) {
		return;
	}
	::close(fd_);
	fd_ = -1;
}

int qiniu::largefile::FileOperation::flush_file()
{
	//??
	if (open_flags & O_SYNC) {
		return 0;
	}
	int fd = check_file();
	if (fd < 0) {
		return fd;
	}
	return fsync(fd);
}

int qiniu::largefile::FileOperation::unlink_file()
{
	close_file();
	return ::unlink(file_name_);
}

int qiniu::largefile::FileOperation::pread_file(char* buf, const int32_t nbytes, const int64_t offset)
{
	int32_t left = nbytes;
	int64_t read_offset = offset;
	int32_t read_len = 0;
	char* p_tmp = buf;
	int i = 0;
	while (left>0)
	{
		++i;
		if (i >= MAX_DISK_TIMES) break;
		if (check_file() < 0) {
			return -errno;
		}
		read_len = ::pread64(fd_, p_tmp, left, read_offset);
		if (read_len < 0) {
			read_len = -errno;
			//繁忙   再试一次
			if (-read_len == EINTR || EAGAIN == -read_len) {
				continue;
			}
			else if(EBADF==-read_len){
				fd_ = -1;
				continue;;
			}
			else {
				return read_len;
			}
		}
		else if(0==read_len)
		{
			break;
		}
		left -= read_len;
		p_tmp += read_len;
		read_offset += read_len;
	}
	if (0 != left) {
		return EXIT_DISK_OPEN_INCOMPLETE;
	}
	//成功
	return TFS_SUCCESS;
}

int qiniu::largefile::FileOperation::pwrite_file(const char* buf, const int32_t nbytes, const int64_t offset)
{
	int32_t left = nbytes;
	int64_t write_offset = offset;
	int32_t write_len = 0;
	const char* p_tmp = buf;
	int i = 0;
	while (left > 0)
	{
		++i;
		if (i >= MAX_DISK_TIMES) break;
		if (check_file() < 0) {
			return -errno;
		}
		write_len = ::pwrite64(fd_, p_tmp, left, write_offset);
		if (write_len < 0) {
			write_len = -errno;
			//繁忙   再试一次
			if (-write_len == EINTR || EAGAIN == -write_len) {
				continue;
			}
			else if (EBADF == -write_len) {
				fd_ = -1;
				continue;;
			}
			else {
				return write_len;
			}
		}
		left -= write_len;
		p_tmp += write_len;
		write_offset += write_len;
	}
	if (0 != left) {
		return EXIT_DISK_OPEN_INCOMPLETE;
	}
	//成功
	return TFS_SUCCESS;
}

int qiniu::largefile::FileOperation::write_file(char* buf, const int32_t nbytes)
{
	int32_t left = nbytes;
	int32_t write_len = 0;
	char* p_tmp = buf;
	int i = 0;
	while (left > 0)
	{
		++i;
		if (i >= MAX_DISK_TIMES) break;
		if (check_file() < 0) {
			return -errno;
		}
		write_len = ::write(fd_, p_tmp,left);
		if (write_len < 0) {
			write_len = -errno;
			//繁忙   再试一次
			if (-write_len == EINTR || EAGAIN == -write_len) {
				continue;
			}
			else if (EBADF == -write_len) {
				fd_ = -1;
				continue;
			}
			else {
				return write_len;
			}
		}
		
		left -= write_len;
		p_tmp += write_len;
	}
	if (0 != left) {
		return EXIT_DISK_OPEN_INCOMPLETE;
	}
	//成功
	return TFS_SUCCESS;
}

int64_t qiniu::largefile::FileOperation::get_file_size()
{
	int fd = check_file();
	if (fd < 0) {
		return -1;
	}
	//获取文件大小
	struct stat buf;
	if (fstat(fd, &buf)!=0) {
		return -1;
	}
	return buf.st_size;
}

int qiniu::largefile::FileOperation::ftruncate_file(const int64_t length)
{
	int fd = check_file();
	if (fd < 0) {
		return fd;
	}
	return ftruncate(fd, length);
}

int qiniu::largefile::FileOperation::seek_file(const int64_t offset)
{
	int fd = check_file();
	if (fd < 0) {
		return fd;
	}
	return lseek(fd, offset, SEEK_SET);
}

int qiniu::largefile::FileOperation::check_file()
{
	if (fd_ < 0) {
		fd_ = open_file();
	}
	return fd_;
}
