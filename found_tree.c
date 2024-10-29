#include <stdlib.h>
#include <string.h>
#include "found_tree.h"

tree_node found_tree;
extern const int partition[];

tree_node* allocate_tree_node()
{
	tree_node* node = calloc(1, sizeof(tree_node));
	ALLOC_VALIDATE(node)

	return node;
}

void initialize_found_tree()
{
	found_tree.is_leaf = false;

	// Initialize children to 0 (null).
	for (int i = 0; i < FOUND_TREE_CHILDREN_LEN; i++) found_tree.children[i] = 0;
}

int digit_to_index(int digit)
{
	if (digit == SUBPERM_SEPARATOR) return FOUND_TREE_CHILDREN_LEN-1;
	return digit-1;
}

int index_to_digit(int index)
{
	if (index == FOUND_TREE_CHILDREN_LEN-1) return SUBPERM_SEPARATOR;
	return index+1;
}

void set_digit(tree_node* tree, tree_node* child, int digit)
{
	tree->children[digit_to_index(digit)] = child;
}

tree_node* get_digit(tree_node* tree, int digit)
{
	return tree->children[digit_to_index(digit)];
}

// <level> is -1 if it shouldn't be a leaf.
// Returns a pointer to the new node.
tree_node* add_permutation_digit(tree_node* tree, int digit, int level)
{
	tree_node* node = allocate_tree_node();

	if (level != -1)
	{
		node->is_leaf = true;
		node->level = level;
	}
	else
	{
		node->is_leaf = false;
	}

	set_digit(tree, node, digit);

	return node;
}

// Finds the subperm whose first digit is the smallest. Returns the index in partition.
int get_smallest_subperm(const int* perm)
{
	int smallestIdxK = 0;
	int smallest = perm[0];
	int idxPerm = partition[0];
	for (int i = 1; i < PARTITION_SIZE; i++)
	{
		if (perm[idxPerm] < smallest)
		{
			smallestIdxK = i;
			smallest = perm[idxPerm];
		}

		idxPerm += partition[i];
	}

	return smallestIdxK;
}

int partition_idx_to_offset(int partition_idx)
{
	int offset = 0;
	for (int i = 0; i < partition_idx; i++)
		offset += partition[i];
	return offset;
}

// Sorts the permutation according to first digits of its sub-permutations.
// Populates new_partition with the partition corresponding to the sorted perm.
void sort_permutation(int *perm,  int* new_partition)
{
	int *sorted_perm = calloc(PERM_MEMORY_SIZE, sizeof(int));
	ALLOC_VALIDATE(sorted_perm)

	// Every time find the smallest sub-perm and put it in sorted_perm.
	int* ptr = sorted_perm;
	for (int i = 0; i < PARTITION_SIZE; i++)
	{
		int smallest_subperm_partition_idx = get_smallest_subperm(perm);
		int subperm_length = partition[smallest_subperm_partition_idx];
		int offset = partition_idx_to_offset(smallest_subperm_partition_idx);

		new_partition[i] = subperm_length;

		// Copy the subperm to the sorted_perm.
		memcpy(ptr, perm + offset, partition[smallest_subperm_partition_idx] * sizeof(int));
		ptr += subperm_length;

		perm[offset] = PERM_MEMORY_SIZE+1; // So it won't be selected again.
	}

	memcpy(perm, sorted_perm, PERM_MEMORY_SIZE * sizeof(int));
	free(sorted_perm);
}

bool add_permutation(int* perm, int level)
{
//	int new_partition[PARTITION_SIZE];
//	sort_permutation(perm, new_partition);

	tree_node* node = &found_tree; // Start from the root.
	for (int i = 0; i < PERM_MEMORY_SIZE; i++)
	{
		tree_node* t = get_digit(node, perm[i]);
		if (t)
		{
			// The digit already exists, just go to next digit
			node = t;
			continue;
		}

		node = add_permutation_digit(node, perm[i], (i+1 == PERM_MEMORY_SIZE) ? level : -1);
		if (i + 1 == PERM_MEMORY_SIZE) return true; // We just added the last digit. It was a new one!
	}

	return false;
}

// Helper function to print the tree recursively
void print_found_tree_recursive(tree_node* tree, int depth, int my_index) {
	if (tree == NULL) {
		return;
	}

	// Print leading spaces to indicate depth
	for (int i = 0; i < depth; i++) {
		printf("    ");
	}

	if (tree->is_leaf) {
		// Print leaf tree level
		printf("Leaf (Level %d)\n", tree->level);
	} else {
		// Print internal tree and its children
		printf("%d\n", my_index+1);
		for (int i = 0; i < FOUND_TREE_CHILDREN_LEN; i++) {
			print_found_tree_recursive(tree->children[i], depth + 1, i);
		}
	}
}

void print_found_tree()
{
	print_found_tree_recursive(&found_tree, 0, -1);
}

void iterate_permutations_recursive(tree_node* tree, iterator_callback callback, void* extra_data, int* perm_so_far, int depth)
{
	if (tree->is_leaf && depth == PERM_MEMORY_SIZE)
 		callback(perm_so_far, tree->level, extra_data);
	else
	{
		for (int i = 0; i < FOUND_TREE_CHILDREN_LEN; i++)
		{
			if (!tree->children[i]) continue;

			perm_so_far[depth] = index_to_digit(i);
			iterate_permutations_recursive(tree->children[i], callback, extra_data, perm_so_far, depth+1);
		}
	}
}

void iterate_permutations(iterator_callback callback, void* extra_data)
{
	int perm[PERM_MEMORY_SIZE];
	iterate_permutations_recursive(&found_tree, callback, extra_data, perm, 0);
}

// Helper function to free the tree recursively
void free_found_tree_recursive(tree_node* node) {
	if (node == NULL) {
		return;
	}

	// If the node is not a leaf, free its children
	if (!node->is_leaf) {
		for (int i = 0; i < FOUND_TREE_CHILDREN_LEN; i++) {
			if (node->children[i] != NULL) {
				free_found_tree_recursive(node->children[i]);
			}
		}
	}

	// Free the node itself
	if (node != &found_tree) free(node);
}

void free_found_tree()
{
	free_found_tree_recursive(&found_tree);
}

