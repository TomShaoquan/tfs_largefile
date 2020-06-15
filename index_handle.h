#ifndef _INDEX_HANDLE_H_
#define _INDEX_HANDLE_H_
#include"common.h"
#include"mmap_file_op.h"
#include"common.h"

namespace qiniu
{
	namespace largefile
	{
		struct IndexHeader
		{
			IndexHeader()
			{
				memset(this, 0, sizeof(IndexHeader));
			}
			BlockInfo block_info_;	//meta block info
			int32_t bucket_size_;	//hash bucket size
			int32_t data_file_offset_;	//offset to write next data in block
			int32_t index_file_size_;	//offset after index_header +all buckets
			int32_t free_head_offset_;	//free meta mode list for reuse
		};

		class IndexHandle
		{
		public:
			IndexHandle(const std::string& base_path, const uint32_t main_block_id);
			~IndexHandle();
			int create(const uint32_t logic_block_id, const int32_t bucket_size,
				const MMapOption map_option);
			//加载
			int load(const uint32_t logic_block_id, const int32_t bucket_size,
				const MMapOption map_option);
			BlockInfo* block_info()
			{
				return reinterpret_cast<BlockInfo*>(file_op_->get_map_data());
			}
			int32_t get_bucket_size() const
			{
				return reinterpret_cast<IndexHeader*>(file_op_->get_map_data())->bucket_size_;
			}
			IndexHeader* index_header()
			{
				//C++ 强制类型转换
				return reinterpret_cast<IndexHeader*>(file_op_->get_map_data());
			}
		private:
			MMapFileOperation* file_op_;
			bool is_load_;
		};
	}
}

#endif // !_INDEX_HANDLE_H_

