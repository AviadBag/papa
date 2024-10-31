#ifndef _FOUND_TREE_H_
#define _FOUND_TREE_H_

#include "main.h"

#define FOUND_TREE_CHILDREN_LEN (N+1)

typedef struct s_tree_node
{
	bool is_leaf;
	union {
		int level; // Valid if leaf.
		struct s_tree_node* children[FOUND_TREE_CHILDREN_LEN]; // Valid if not a leaf.
	};
} tree_node;

typedef void (*iterator_callback)(int* perm, int level, void* extra_data);

extern tree_node found_tree;

void initialize_found_tree();
bool add_permutation(const int* perm, int level); // Returns false if the perm already existed.
void print_found_tree();
void iterate_permutations(iterator_callback callback, void* extra_data); // Calls <callback> with all perms. No need to worry about memory.
void free_found_tree();

#endif //_FOUND_TREE_H_
