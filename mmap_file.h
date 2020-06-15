#ifndef QINTU_LARGEFILE_MMAPFILE_H
#define QINTU_LARGEFILE_MMAPFILE_H


#include"common.h"

namespace qiniu
{
	namespace largefile
	{
		class MMapFile
		{
		public:
			MMapFile();  ////默认构造
			~MMapFile();
			explicit MMapFile(const int fd);		
			MMapFile(const MMapOption& mmap_option, const int fd);
			//同步文件
			bool sync_file();						
			//将文件映射到内存,同时设置访问权限
			bool map_file(const bool write=false);	
			//获取映射到内存的数据的首地址
			void* get_data() const;	
			//获取映射数据的大小
			int32_t get_size() const;				
			//解除映射
			bool munmap_file();
			//重新映射
			bool remap_file();						
			
		private:
			//映射的内存进行扩容  size 内存映射大小
			bool ensure_file_size(const int32_t size);
			
		private:
			//映射的大小
			int32_t size_;		
			int fd_;			//文件句柄
			void* data_;		//数据地址
			struct MMapOption mmap_file_option_;
		};

	}
}
#endif // !QINTU_LARGEFILE_MMAPFILE_H

