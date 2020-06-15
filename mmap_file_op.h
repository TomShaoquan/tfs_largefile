#ifndef _MMAP_FILE_OP_H_
#define _MMAP_FILE_OP_H_
#include "file_op.h"
#include"mmap_file.h"

using namespace qiniu;
namespace qiniu
{
	namespace largefile
	{
		class MMapFileOperation :
			public FileOperation
		{
		public:
			MMapFileOperation(const std::string& file_name, const int open_flags = O_CREAT|O_RDWR | O_LARGEFILE);
			~MMapFileOperation();
			int pread_file(char* buf, const int32_t size, const int64_t offset);
			int pwrite_file(const char* buf, const int32_t size, const int64_t offset);
			
			int mmap_file(const MMapOption mmap_option);
			int munmap();
			void* get_map_data() const;
			int flush_file();

		private:
			MMapFile* map_file_;
			// «∑Ò“—æ≠”≥…‰
			bool is_mapped_;		
		};

	}
}

#endif // !_MMAP_FILE_OP_H_


