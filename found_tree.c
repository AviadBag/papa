#include <stdlib.h>
#include <string.h>
#include "found_tree.h"

tree_node found_tree;
const int kS[] = kS_DEF;

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
	for (int i = 0; i < N; i++) found_tree.children[i] = 0;
}

void set_digit(tree_node* tree, tree_node* child, int digit)
{
	tree->children[digit-1] = child;
}

tree_node* get_digit(tree_node* tree, int digit)
{
	return tree->children[digit-1];
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

// Finds the subperm whose first digit is the smallest. Returns the index in kS.
int get_smallest_subperm(const int* perm)
{
	int smallestIdxK = 0;
	int smallest = perm[0];
	int idxPerm = kS[0];
	for (int i = 1; i < NUMBER_OF_kS; i++)
	{
		if (perm[idxPerm] < smallest)
		{
			smallestIdxK = i;
			smallest = perm[idxPerm];
		}

		idxPerm += kS[i];
	}

	return smallestIdxK;
}

int kS_idx_to_offset(int kS_idx)
{
	int offset = 0;
	for (int i = 0; i < kS_idx; i++)
		offset += kS[i];
	return offset;
}

// Sorts the permutation according to first digits of its sub-permutations.
// Populates new_kS with the kS corresponding to the sorted perm.
void sort_permutation(int *perm,  int* new_kS)
{
	int *sorted_perm = calloc(N, sizeof(int));
	ALLOC_VALIDATE(sorted_perm)

	// Every time find the smallest sub-perm and put it in sorted_perm.
	int* ptr = sorted_perm;
	for (int i = 0; i < NUMBER_OF_kS; i++)
	{
		int smallest_subperm_kS_idx = get_smallest_subperm(perm);
		int subperm_length = kS[smallest_subperm_kS_idx];
		int offset = kS_idx_to_offset(smallest_subperm_kS_idx);

		new_kS[i] = subperm_length;

		// Copy the subperm to the sorted_perm.
		memcpy(ptr, perm + offset, kS[smallest_subperm_kS_idx] * sizeof(int));
		ptr += subperm_length;

		perm[offset] = N+1; // So it won't be selected again.
	}

	memcpy(perm, sorted_perm, N * sizeof(int));
	free(sorted_perm);
}

bool add_permutation(int* perm, int level)
{
	int new_kS[NUMBER_OF_kS];
	sort_permutation(perm, new_kS);

	tree_node* node = &found_tree; // Start from the root.
	for (int i = 0; i < N; i++)
	{
		tree_node* t = get_digit(node, perm[i]);
		if (t)
		{
			// The digit already exists, just go to next digit
			node = t;
			continue;
		}

		node = add_permutation_digit(node, perm[i], (i+1 == N) ? level : -1);
		if (i + 1 == N) return true; // We just added the last digit. It was a new one!
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
		for (int i = 0; i < N; i++) {
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
	if (tree->is_leaf && depth == N)
 		callback(perm_so_far, tree->level, extra_data);
	else
	{
		for (int i = 0; i < N; i++)
		{
			if (!tree->children[i]) continue;

			perm_so_far[depth] = i+1;
			iterate_permutations_recursive(tree->children[i], callback, extra_data, perm_so_far, depth+1);
		}
	}
}

void iterate_permutations(iterator_callback callback, void* extra_data)
{
	int perm[N];
	iterate_permutations_recursive(&found_tree, callback, extra_data, perm, 0);
}

// Helper function to free the tree recursively
void free_found_tree_recursive(tree_node* node) {
	if (node == NULL) {
		return;
	}

	// If the node is not a leaf, free its children
	if (!node->is_leaf) {
		for (int i = 0; i < N; i++) {
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

