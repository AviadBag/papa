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

// Finds the subperm whose first digit is the smallest. Returns the index of that digit.
int get_smallest_subperm(const int* perm)
{
	int smallest = perm[0];
	int smallestIdx = 0;
	for (int i = 1; i < PERM_MEMORY_LEN; i++)
	{
		if (perm[i] == SUBPERM_SEPARATOR) // The end of a subperm, i+1 is the beginning of a new subperm.
		{
			if (i+1 < PERM_MEMORY_LEN) // Not the end of the last subperm
			{
				if (perm[i+1] < smallest)
				{
					smallest = perm[i+1];
					smallestIdx = i+1;
				}
			}
		}
	}

	return smallestIdx;
}

int partition_idx_to_offset(int partition_idx)
{
	int offset = 0;
	for (int i = 0; i < partition_idx; i++)
		offset += partition[i];
	return offset;
}

// Copy from src to dst until <what> is reached. (Including <what>). Returns how many bites were copied.
int copy_until(int* dst, int* src, int what)
{
	int count = 0;
	while (true)
	{
		*dst = *src;
		count++;
		if (*dst == what) break;
		dst++; src++;
	}
	return count;
}

// Sorts the permutation according to first digits of its sub-permutations.
void sort_permutation(int *perm)
{
	int *sorted_perm = allocate_perm();
	memset(sorted_perm, SUBPERM_SEPARATOR, PERM_MEMORY_LEN);

	// Every time find the smallest sub-perm and put it in sorted_perm.
	int* ptr = sorted_perm;
	for (int i = 0; i < PARTITION_SIZE; i++)
	{
		int smallest_subperm_idx = get_smallest_subperm(perm);
		ptr += copy_until(ptr, perm + smallest_subperm_idx, SUBPERM_SEPARATOR);
		perm[smallest_subperm_idx] = N+1; // So it won't trigger again
	}

	memcpy(perm, sorted_perm, PERM_MEMORY_LEN * sizeof(int));
	free(sorted_perm);
}

bool add_permutation(const int* perm, int level)
{
//	printf("\nBefore sort: ");
//	print_perm(perm);
	sort_permutation(perm);
//	printf("\nAfter sort: ");
//	print_perm(perm);
//	putchar('\n');

	tree_node* node = &found_tree; // Start from the root.
	for (int i = 0; i < PERM_MEMORY_LEN; i++)
	{
		tree_node* t = get_digit(node, perm[i]);
		if (t)
		{
			// The digit already exists, just go to next digit
			node = t;
			continue;
		}

		node = add_permutation_digit(node, perm[i], (i+1 == PERM_MEMORY_LEN) ? level : -1);
		if (i + 1 == PERM_MEMORY_LEN) return true; // We just added the last digit. It was a new one!
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
	if (tree->is_leaf && depth == PERM_MEMORY_LEN)
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
	int perm[PERM_MEMORY_LEN];
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

