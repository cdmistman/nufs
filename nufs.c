#include "defs.h"
#include "types.h"
#include "util.h"

int
nufs_access(const char* path, int mask)
{
	//todo idk what this is supposed to return?
	int node_id = get_node_id(path);
	if (node_id < 0) return node_id;
	if (node_has_mode(node_id, mask) == TRUE) return SUCCESS;
	return -EPERM;
}

int
nufs_chmod(const char* path, mode_t mode)
{
	int node_id = get_node_id(path);
	if (node_id < 0) return node_id;

	// get the new mode while keeping the file type (file, dir, or link)
	mode_t new_mode = node_get_mode(node_id) & T_ANY;
	new_mode = new_mode | mode;
	
	// return the successfulness of setting the new mode to the node
	return node_set_mode(node_id, new_mode);
}

int
nufs_getattr(const char* path, struct stat* st)
{
	int node_id = get_node_id(path);
	if (node_id < 0) return node_id;

	//fill the stats object with the stats needed
	st->st_mode = node_get_mode(node_id);
	st->st_size = node_get_size(node_id);
	st->st_uid = getuid();

	// return success since everything has been filled
	return SUCCESS;
}

int
nufs_link(const char* from, const char* to)
{
	// todo -- ignoring linking for now
}

int
nufs_mkdir(const char* path, mode_t mode)
{
	return node_alloc(path, T_DIR | mode);

	// todo move this to node_alloc

	int node_id = get_empty_node();
	if (node_id < 0) return node_id;

	// if the mode can't be set for the new node, return try again
	if (node_set_mode(node_id, T_DIR | mode) != SUCCESS) goto TRYAGAIN;

	// the pointer to the name of the child
	char* child_name;
	// the id of the node that's the parent of the new node. the 
	// function also assigns the child name ptr to the name of the new
	// child
	int parent_id = get_parent_node_id(path, &child_name);
	// if the parent doesn't exist, or if the parent isn't a dir, then
	// there's a failure
	if (parent_id == -1 || node_has_mode(parent_id, T_DIR) == FALSE) {
		free(child_name);
		goto TRYAGAIN;
	}
	// if there's already an entry in the dir with the same name,
	// then return the appropriate error
	if (dir_has_entry(parent_id, child_name) == TRUE) return -EEXIST;
	// add the new node to the dir
	int rv = dir_add_entry(parent_id, node_id, (const char*)child_name);
	// return the return value of adding the new entry to the directory
	if (rv != SUCCESS) dealloc_node(node_id);
	free(child_name);
	return rv;

TRYAGAIN:
	dealloc_node(node_id);
	return -EAGAIN;
}

int 
nufs_mknod(const char* path, mode_t mode, dev_t rdev)
{
	return node_alloc(path, T_FILE | mode);
}

int
nufs_open(const char* path, struct fuse_file_info* f_info)
{
	int node_id = get_node_id(path);
	if (node_id < 0) return node_id;
	return SUCCESS;
}

int
nufs_read(const char* path, char* buf, size_t size, off_t offset, struct fuse_file_info* f_info)
{
	int node_id = get_node_id(path);
	if (node_id < 0) return node_id;
	
	// get the available read size
	size_t avail_read_size = node_get_size(node_id) - offset;

	// get the total read size
	size_t read_size = min(avail_read_size, size);
	if (read_size == 0) return 0;

	// index of the node's page to read from
	int page_index = offset / PAGE_SIZE;

	// the data
	char* data = node_get_data(node_id, page_index);

	// index of the current byte in data
	off_t data_index = offset % PAGE_SIZE;

	// buffer index
	off_t buf_index = 0;

	while (buf_index < read_size) {
		// if we've read all the data from the current page, get the
		// next page of data
		if (data_index % PAGE_SIZE == 0 && data_index != 0) {
			data_index = 0;
			++page_index;
			data = node_get_data(node_id, page_index);
		}

		buf[buf_index] = data[data_index];
		++buf_index;
		++data_index;
	}

	// return bytes left to read in the file
	return read_size;
}

int
nufs_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* f_info)
{
	//todo -- idk what to do
}

int
nufs_rename(const char* from, const char* to)
{
	int node_id = get_node_id(from);
	if (node_id < 0) return node_id;
	int parent_id = node_get_parent_node_id(node_id);
	if (parent_id < 0) return parent_id;
	int rv = dir_rm_entry(parent_id, node_id);
	if (rv != SUCCESS) return rv;
	rv = dir_add_entry(parent_id, node_id, to);
	if (rv != SUCCESS) {
		return dir_add_entry(parent_id, node_id, from);
	}
	return SUCCESS;
}

int
nufs_rmdir(const char* path)
{
	int node_id = get_node_id(path);
	if (node_id < 0) return node_id;
	int rv = dir_rm_entries(node_id);
	if (rv < 0) return rv;
	int parent_id = get_node_id(node_id);
	if (parent_id < 0) return parent_id;
	rv = dir_rm_entry(parent_id, node_id);
	if (rv < 0) return rv;
	return dealloc_node(node_id);
}

int
nufs_truncate(const char* path, off_t size)
{
	int node_id = get_node_id(path);
	if (node_id < 0) return node_id;
	return node_truncate(node_id, size);
}

int
nufs_unlink(const char* path)
{
	//todo -- ignoring linking for now
}

int
nufs_utimens(const char* path, const struct timespec ts[2])
{
	//todo -- ignoring timing for now
}

int
nufs_write(const char* path, const char* buf, size_t size, off_t offset, struct fuse_file_info* f_info)
{
	int node_id = get_node_id(path);
	if (node_id < 0) return node_id;
	
	// get the available write size
	size_t avail_write_size = node_get_size(node_id) - offset;

	// get total write size
	size_t write_size = min(avail_write_size, size);
	if (write_size == 0) return 0;

	// index of the node's page to write to
	int page_index = offset / PAGE_SIZE;

	// the data
	char* data = node_get_data(node_id, page_index);

	// index of the current byte in data
	off_t data_index = offset % PAGE_SIZE;

	// buffer index
	off_t buf_index = 0;

	while (buf_index < write_size) {
		// if we've written all the data allowed on the current page, get
		// the next page of data
		if (data_index % PAGE_SIZE == 0 && data_index != 0) {
			data_index = 0;
			++page_index;
			data = node_get_data(node_id, page_index);
		}

		data[data_index] = buf[buf_index];
		++buf_index;
		++data_index;
	}

	return write_size;
}

void
nufs_init_ops(struct fuse_operations* ops)
{
	memset(ops, 0, sizeof(struct fuse_operations));
    ops->access   = nufs_access;
    ops->chmod    = nufs_chmod;
    ops->getattr  = nufs_getattr;
    ops->link     = nufs_link;
    ops->mkdir    = nufs_mkdir;
    ops->mknod    = nufs_mknod;
    ops->open	  = nufs_open;
    ops->read     = nufs_read;
    ops->readdir  = nufs_readdir;
    ops->rename   = nufs_rename;
    ops->rmdir    = nufs_rmdir;
    ops->truncate = nufs_truncate;
    ops->unlink   = nufs_unlink;
    ops->utimens  = nufs_utimens;
    ops->write    = nufs_write;
}

struct fuse_operations nufs_ops;

int
main(int argc, char* argv[])
{
	//todo
}