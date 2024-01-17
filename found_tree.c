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

bool add_permutation(int* perm, int level)
{
	tree_node* node = &found_tree; // Start from the root.
	for (int i = 0; i < K; i++)
	{
		tree_node* t = get_digit(node, perm[i]);
		if (t)
		{
			// The digit already exists, just go to next digit
			node = t;
			continue;
		}

		node = add_permutation_digit(node, perm[i], (i+1 == K) ? level : -1);
		if (i + 1 == K) return true; // We just added the last digit. It was a new one!
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

void iterate_permutations_recursive(tree_node* tree, void (* callback)(int* perm, int level), int* perm_so_far, int depth)
{
	if (tree->is_leaf)
	{
		if (depth == K) callback(perm_so_far, tree->level);
		return; // We can go no further
	}

	else
	{
		for (int i = 0; i < N; i++)
		{
			if (!tree->children[i]) continue;

			perm_so_far[depth] = i+1;
			iterate_permutations_recursive(tree->children[i], callback, perm_so_far, depth+1);
		}
	}
}

void iterate_permutations(void (* callback)(int* perm, int level))
{
	int perm[N];
	iterate_permutations_recursive(&found_tree, callback, perm, 0);
}

