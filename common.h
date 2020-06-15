#ifndef _COMMON_H_
#define _COMMON_H_
#include<stdint.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/file.h>
#include<iostream>
#include<string>
#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/unistd.h>
#include<sys/mman.h>
#include <cassert>

namespace qiniu
{
	namespace largefile
	{
		const int32_t TFS_SUCCESS = 0;
		const int32_t TFS_ERROR = -1;
		//read or write length is less than required.
		const int32_t EXIT_DISK_OPEN_INCOMPLETE = -8012;
		const int32_t EXIT_INDEX_ALREADY_LOADED_ERROR = -8013;
		const int32_t EXIT_META_UNEXPECT_FOUND_ERROR = -8014;
		const int32_t EXIT_INDEX_CORRUPT_ERROR = -8015;
		const int32_t EXIT_BLOCKID_CONFLICT_ERROR = -8016;
		const int32_t EXIT_BUCKET_CONFIGURE_ERROR = -8017;

		static const std::string MAINBLOCK_DIR_PREFIX = "/mainblock/";
		static const std::string INDEX_DIR_PREFIX = "/index/";
		static const mode_t DIR_MODE = 0755;
		//映射文件大小选择
		struct MMapOption
		{
			//最大映射大小
			int32_t max_mmap_size_;
			//第一次
			int32_t first_mmap_size_;
			int32_t per_mmap_size_;
		};
		//文件块
		struct BlockInfo
		{
			uint32_t block_id_;
			int32_t version_;
			int32_t file_count_;
			int32_t size_t_;
			int32_t del_size_count_;	//
			int32_t del_size_;	//删除文件大小
			int32_t sql_no_;

			BlockInfo()
			{
				//成员初始化
				memset(this, 0, sizeof(BlockInfo));
			}
			//重载比较运算符
			inline bool operator==(const BlockInfo& tfs)
			{
				return this->block_id_ == tfs.block_id_
					&& this->del_size_ == tfs.del_size_
					&& this->del_size_count_ == tfs.del_size_count_
					&& this->file_count_ == tfs.file_count_
					&& this->size_t_ == tfs.size_t_
					&& this->sql_no_ == tfs.sql_no_
					&& this->version_ == tfs.version_;
			}

		};

		struct MetaInfo
		{
			MetaInfo()
			{
				init();
			}
			MetaInfo(const uint64_t fileid, const int32_t inner_offset,
				const int32_t size, const int32_t next_meta_offset)
			{
				fileid_ = fileid;
				location_.inner_offset_ = inner_offset;
				location_.size_ = size;
				next_meta_offset_ = next_meta_offset;
			}

			//拷贝构造函数
			MetaInfo(const MetaInfo& meta_info)
			{
				memcpy(this, &meta_info, sizeof(MetaInfo));
			}

			//赋值构造
			MetaInfo& operator=(const MetaInfo& meta_info)
			{
				if (this == &meta_info) return *this;//a=b=c
				fileid_ =meta_info.fileid_;
				location_.inner_offset_ = meta_info.location_.inner_offset_;
				location_.size_ = meta_info.location_.size_;
				next_meta_offset_ = meta_info.next_meta_offset_;
			}
			//克隆
			MetaInfo& clone(const MetaInfo& meta_info)
			{
				assert(this != &meta_info);
				fileid_ = meta_info.fileid_;
				location_.inner_offset_ = meta_info.location_.inner_offset_;
				location_.size_ = meta_info.location_.size_;
				next_meta_offset_ = meta_info.next_meta_offset_;
				return *this;

			}

			//比较
			bool operator==(MetaInfo& rhs) const
			{
				return 
				fileid_ == rhs.fileid_
				&&location_.inner_offset_ ==rhs.location_.inner_offset_
				&&location_.size_ == rhs.location_.size_
				&&next_meta_offset_ == rhs.next_meta_offset_;
			}
			uint64_t get_key() const
			{
				return fileid_;
			}
			void set_key(const uint64_t key)
			{
				fileid_ = key;
			}

			uint64_t get_file_id() const
			{
				return fileid_;
			}
			void set_file_id(const uint64_t fileid)
			{
				fileid_ = fileid;
			}

			int32_t get_offset()
			{
				return location_.inner_offset_;
			}
			void set_offset(const int32_t offset)
			{
				location_.inner_offset_ = offset;
			}

			int32_t get_size() const
			{
				return location_.size_;
			}
			void set_size(const int32_t file_size)
			{
				location_.size_ = file_size;
			}

			int32_t get_next_neta_offset() const
			{
				return next_meta_offset_;
			}
			void set_next_neta_offset(const int32_t offset)
			{
				next_meta_offset_ = offset;
;			}
			uint64_t fileid_;
			//定义内部变量
			struct location_
			{
				int32_t inner_offset_;
				int32_t size_;
			}location_;
			int32_t next_meta_offset_;

		private:
			void init()
			{
				fileid_ = 0;
				location_.inner_offset_ = 0;
				location_.size_ = 0;
				next_meta_offset_ = 0;
			}
		};
	}
}
#endif // !_COMMON_H_

