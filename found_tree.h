#ifndef _FOUND_TREE_H_
#define _FOUND_TREE_H_

#include "main.h"

typedef struct s_tree_node
{
	int digit;
	bool is_leaf;
	union {
		int level; // Valid if leaf.
		struct s_tree_node* children[N]; // Valid if not a leaf.
	};
} tree_node;

tree_node tree;

void initialize_found_tree();
void add_permutation(int* perm, int level);

#endif //_FOUND_TREE_H_
