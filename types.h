#ifndef NUFS_TYPES_H
#define NUFS_TYPES_H

#include "defs.h"

typedef struct node_t {
	int refs;
	mode_t mode;
	size_t size;
	index_t data_index_1;
	index_t data_index_2;
} node_t;

typedef struct dirent_t {
	const char name[MAX_FILE_NAME];
	index_t node_index;
} dirent_t;

typedef struct ext_node_ent_t {
	index_t node_id;
	index_t data_index;
} ext_node_ent_t;

#endif