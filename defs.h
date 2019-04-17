#ifndef NUFS_DEFS_H
#define NUFS_DEFS_H

#include <alloca.h>
#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

#define FUSE_USE_VERSION 26
#include <fuse.h>

typedef unsigned short index_t;

#define PAGE_SIZE 4096

#define FALSE 0
#define TRUE 1

#define SUCCESS 0

#define T_DIR 040000
#define T_FILE 0100000
#define T_LINK 120000
#define T_ANY T_DIR | T_FILE | T_LINK

// dir.c
int dir_add_entry(index_t dir_id, index_t node_id, const char* name);
int dir_rm_entry(index_t dir_id, index_t node_id);
int dir_has_entry(index_t dir_id, const char* name);
int dir_rm_entries(index_t dir_id);
int dir_read(index_t dir_id, void* buf, fuse_fill_dir_t filler);

// fs.c
index_t get_node_id(const char* path);
index_t get_parent_node_id(const char* path, char** child_ptr);
index_t get_empty_node();
int dealloc_node(index_t node_id);
index_t node_alloc(const char* path, mode_t mode);

// node.c
int node_has_mode(index_t node_id, int mask);
int node_set_mode(index_t node_id, mode_t mode);
int node_get_mode(index_t node_id);
size_t node_get_size(index_t node_id);
int node_get_parent_node_id(int node_id);
int node_truncate(int node_id, off_t size);
char* node_get_data(int node_id, off_t data_index);

// nufs.c
int nufs_access(const char* path, int mask);
int nufs_chmod(const char* path, mode_t mode);
int nufs_getattr(const char* path, struct stat* st);
int nufs_link(const char* from, const char* to);
int nufs_mkdir(const char* path, mode_t mode);
int nufs_mknod(const char* path, mode_t mode, dev_t rdev);
int nufs_open(const char* path, struct fuse_file_info* f_info);
int nufs_read(const char* path, char* buf, size_t size, off_t offset, struct fuse_file_info* f_info);
int nufs_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* f_info);
int nufs_rename(const char* from, const char* to);
int nufs_rmdir(const char* path);
int nufs_truncate(const char* path, off_t size);
int nufs_unlink(const char* path);
int nufs_utimes(const char* path, const struct timespec ts[2]);
int nufs_write(const char* path, const char* buf, size_t size, off_t offset, struct fuse_file_info* f_info);
void nufs_init_ops(struct fuse_operations* ops);

#endif