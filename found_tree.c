#include <stdlib.h>
#include "found_tree.h"

tree_node* allocate_tree_node()
{
	tree_node* node = calloc(1, sizeof(tree_node));
	ALLOC_VALIDATE(node)

	return node;
}

void initialize_found_tree()
{
	tree.digit = -1;
	tree.is_leaf = false;
}

void add_permutation(int* perm, int level)
{

}

