
#include"index_handle.h"
#include<sstream>
int debug = 1;
qiniu::largefile::IndexHandle::IndexHandle(const std::string& base_path, const uint32_t main_block_id)
{
	//create file_op handle boject 
	std::stringstream tmp_stream;
	//  /root/pop/index/1
	tmp_stream << base_path << INDEX_DIR_PREFIX << main_block_id;

	std::string index_path;
	tmp_stream >> index_path;
	file_op_ = new MMapFileOperation(index_path, O_CREAT | O_RDWR | O_LARGEFILE);
	is_load_ = false;
}

qiniu::largefile::IndexHandle::~IndexHandle()
{
	if (file_op_) {
		delete file_op_;
		file_op_ = NULL;
	}
}

int qiniu::largefile::IndexHandle::create(const uint32_t logic_block_id, const int32_t bucket_size, const MMapOption map_option)
{
	int ret = TFS_SUCCESS;
	if (debug) printf("create index block_id:%u bucket size:%d\n\n\
max_map_size:%d \nfirst_map_size:%d \nper_map_size:%d\n\n",
		logic_block_id, bucket_size, map_option.max_mmap_size_, map_option.first_mmap_size_, map_option.per_mmap_size_);
	if (is_load_)
	{
		return EXIT_INDEX_ALREADY_LOADED_ERROR;
	}
	int64_t file_size = file_op_->get_file_size();
	if (file_size < 0) {
		return TFS_ERROR;
	}
	else if (file_size == 0)	//empty file
	{
		IndexHeader i_header;
		i_header.block_info_.block_id_ = logic_block_id;
		i_header.block_info_.sql_no_ = 1;
		i_header.bucket_size_ = bucket_size;
		i_header.index_file_size_ = sizeof(IndexHeader) + bucket_size * sizeof(int32_t);

		//index header + total buckets
		char* init_data = new char[i_header.index_file_size_];
		memcpy(init_data, &i_header, sizeof(IndexHeader));
		memset(init_data + sizeof(IndexHeader), 0, i_header.index_file_size_ - sizeof(IndexHeader));
		ret = file_op_->pwrite_file(init_data, i_header.index_file_size_, 0);
		delete[] init_data;
		init_data = NULL;
		if (ret != TFS_SUCCESS) {
			return ret;
		}
	}
	else //file_size  > 0
	{
		return EXIT_META_UNEXPECT_FOUND_ERROR;
	}
	ret = file_op_->mmap_file(map_option);
	if (ret != TFS_SUCCESS) {
		return ret;
	}
	is_load_ = true;
	if (debug) printf("create index successful.\n");
	return TFS_SUCCESS;
}

int qiniu::largefile::IndexHandle::load(const uint32_t logic_block_id, const int32_t bucket_size, const MMapOption map_option)
{
	int ret = TFS_SUCCESS;
	if (is_load_)
	{
		return EXIT_INDEX_ALREADY_LOADED_ERROR;
	}
	int64_t file_size = file_op_->get_file_size();
	if (file_size < 0) {
		return file_size;
	}
	else if (file_size == 0)	//empty file
	{
		return EXIT_INDEX_CORRUPT_ERROR;
	}
	MMapOption tmp_map_option = map_option;
	if (file_size > tmp_map_option.first_mmap_size_&& file_size <= tmp_map_option.max_mmap_size_)
	{
		tmp_map_option.first_mmap_size_ = file_size;
	}
	ret = file_op_->mmap_file(tmp_map_option);
	if (ret != TFS_SUCCESS) {
		return ret;
	}
	if (0 == get_bucket_size() || 0 == block_info()->block_id_)
	{
		fprintf(stderr, "index corrupt blockid:%u bucket size:%d\n", block_info()->block_id_, get_bucket_size());
		return EXIT_INDEX_CORRUPT_ERROR;
	}

	//check file size
	int32_t index_file_size = sizeof(IndexHeader) + get_bucket_size() * sizeof(int32_t);
	if (file_size < index_file_size) {
		fprintf(stderr, "index corrupt error blockid:%u bucket size:%d file size:%d index file size:%d\n",
			block_info()->block_id_, get_bucket_size(),file_size,index_file_size);
		return EXIT_INDEX_CORRUPT_ERROR;
	}
	
	//check block id
	if (logic_block_id != block_info()->block_id_) {
		fprintf(stderr, "block id confict blockid:%u index blockid:%u\n", logic_block_id, block_info()->block_id_);
		return EXIT_BLOCKID_CONFLICT_ERROR;
	}

	//check bucket size
	if (bucket_size != get_bucket_size())
	{
		fprintf(stderr, "index config error old bucket size:%d new bucket size:%d\n", bucket_size,get_bucket_size());
		return EXIT_BUCKET_CONFIGURE_ERROR;
	}
	is_load_ = true;
	printf("load successful!\n");
}
