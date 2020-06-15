#ifndef _FILE_OP_H
#define _FILE_OP_H
#include"common.h"
#include <fcntl.h>

namespace qiniu
{
	namespace largefile
	{
		class FileOperation
		{
		public:
			FileOperation(const std::string& file_name, const int open_flags = O_RDWR|O_LARGEFILE);
			~FileOperation();

			//打开文件
			int open_file();
			//关闭文件
			void close_file();
			//把文件从内存立即写入磁盘 
			int flush_file();
			//删除文件
			int unlink_file();
			//读取文件
			virtual int pread_file(char* buf, const int32_t nbytes, const int64_t offset);
			//写文件
			virtual int pwrite_file(const char* buf, const int32_t nbytes, const int64_t offset);
			//在当前的位置写
			int write_file(char* buf, const int32_t nbytes);
			//获取文件大小
			int64_t get_file_size();
			//改变文件大小
			int ftruncate_file(const int64_t length);
			//移动文件指针
			int seek_file(const int64_t offset);
			//文件句柄
			int get_fd() const {
				return fd_;
			}
		protected:
			int check_file();
		protected:
			int fd_;
			//权限
			int open_flags;
			//文件名
			char* file_name_;
		protected:
			//打开权限
			static const mode_t OPEN_MODE = 0644;
			//最大读取次数
			static const int MAX_DISK_TIMES = 5;	
		};

	}
}

#endif // !_FILE_OP_H



