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

	mode_t new_mode = node_get_mode(node_id) & T_ANY;
	new_mode = new_mode | mode;
	return node_set_mode(node_id, new_mode);
}

int
nufs_getattr(const char* path, struct stat* st)
{
	int node_id = get_node_id(path);
	if (node_id < 0) return node_id;
	st->st_mode = node_get_mode(node_id);
	st->st_size = node_get_size(node_id);
	st->st_uid = getuid();
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
	int node_id = get_empty_node();
	if (node_id < 0) return node_id;
	if (node_set_mode(node_id, T_DIR | mode) != SUCCESS) goto TRYAGAIN;
	char* child_name;
	int parent_id = get_parent_node_id(path, &child_name);
	if (parent_id == -1 || node_has_mode(parent_id, T_DIR) == FALSE) 
		goto TRYAGAIN;
	if (dir_has_entry(parent_id, child_name) == TRUE) return -EEXIST;
	int rv = dir_add_entry(parent_id, node_id, (const char*)child_name);
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
	int node_id = get_empty_node();
	if (node_id < 0) return node_id;
	//todo -- too lazy
	return node_set_mode(node_id, T_FILE | mode);
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
	//todo -- too lazy
}

int
nufs_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* f_info)
{
	//todo -- too lazy
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
	//todo -- too lazy
	return SUCCESS;
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