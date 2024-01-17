#ifndef _FOUND_TREE_H_
#define _FOUND_TREE_H_

#include "main.h"

typedef struct s_tree_node
{
	bool is_leaf;
	union {
		int level; // Valid if leaf.
		struct s_tree_node* children[N]; // Valid if not a leaf.
	};
} tree_node;

extern tree_node found_tree;

void initialize_found_tree();
bool add_permutation(int* perm, int level); // Returns false if the perm already existed.
void print_found_tree();
void iterate_permutations(void (* callback)(int* perm, int level)); // Calls <callback> with all perms. No need to worry about memory.
void free_found_tree();

#endif //_FOUND_TREE_H_
