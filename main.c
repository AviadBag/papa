#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 9
#define K 9

#define ALLOC_VALIDATE(p) if (!p) { printf("Not enough memory!"); exit(1); }

struct s_node
{
	int* perm;
	int level;
	struct s_node* children[N];
};
typedef struct s_node s_node;

// This array will contain pointers to all found nodes.
struct {
	s_node** array; // The array itself.
	int final_size; // The final_size of the array, calculated on run-time.
	int next_index; // The index of the next free space.
} found_array;

// Returns a new, dynamically allocated, permutation. Exits program on error.
int* allocate_perm()
{
	int* perm = calloc(K, sizeof(int));
	ALLOC_VALIDATE(perm)

	return perm;
}

s_node* allocate_node()
{
	s_node* node = calloc(1, sizeof(s_node));
	ALLOC_VALIDATE(node)

	return node;
}

int* get_initial_perm()
{
	int* perm = allocate_perm();
	for (int i = 0; i < K; i++) perm[i] = i+1;
	return perm;
}

void print_perm(int* perm)
{
	printf("(");
	for (int i = 0; i < K; i++)
	{
		if (i + 1 == K) printf("%d)", perm[i]);
		else printf("%d, ", perm[i]);
	}
}

// Returns the index of the given number in the permutation. -1 if not found
int index_of(const int* perm, int what)
{
	for (int i = 0; i < K; i++)
		if (perm[i] == what)
			return i;

	return -1;
}

int* duplicate_perm(const int* perm)
{
	int* new_perm = allocate_perm();
	memcpy(new_perm, perm, K * sizeof(int));
	return new_perm;
}

// Performs an S operation on the given perm. Result is returned as a newly allocated perm.
int* S(const int* perm, int i)
{
	int* new_perm = duplicate_perm(perm);

	// Perform operation (Switch between i and i+1)
	int a = index_of(perm, i);
	int b = index_of(perm, i+1);
	if (a != -1) new_perm[a] = i+1;
	if (b != -1) new_perm[b] = i;

	return new_perm;
}

// Returns the index of the smallest number in the perm
int index_of_smallest(const int* perm)
{
	int index = 0;
	for (int i = 0; i < K; i++)
	{
		if (perm[i] < perm[index]) index = i;
	}

	return index;
}

// Rearranges the given perm, so the first number is the lowest one. (Keeps the permutation intact).
void rearrange_perm(int* perm)
{
	int smallest_i = index_of_smallest(perm);
	if (smallest_i == 0) return; // Already arranged

	int* perm_temp = duplicate_perm(perm);
	int n = K - smallest_i;

	// Move <n> cycles
	for (int i = 0; i < K; i++)
	{
		perm[(i + n) % K] = perm_temp[i];
	}

	free(perm_temp);
}

// Checks whether they are equal. Both perms must be arranged!
int perm_equal(const int* perm1, const int* perm2)
{
	for (int i = 0; i < K; i++)
		if (perm1[i] != perm2[i]) return 0;

	return 1;
}

// If not found returns null.
s_node* find_perm_node(const int* perm)
{
	for (int i = 0; i < found_array.next_index; i++)
	{
		s_node* found_perm = found_array.array[i];
		if (perm_equal(found_perm->perm, perm)) return found_perm;
	}

	return 0;
}

// Populates the children array of the given s_node. (All nodes are dynamically allocated)
void populate_children(s_node* parent_node)
{
	// For every child, perform the corresponding S operation
	for (int i = 0; i < N-1; i++)
	{
		int new_level = parent_node->level + 1;

		// Get next perm
		int* si_perm = S(parent_node->perm, i + 1);
		rearrange_perm(si_perm);

		// Check whether we already found it
		s_node* found_perm = find_perm_node(si_perm);
		if (found_perm)
		{
			parent_node->children[i] = 0;
			free(si_perm);
			continue;
		}

		// Initialize a new node for the child
		s_node* child_node = allocate_node();
		child_node->perm = si_perm;
		child_node->level = new_level;

		// Add new child to father!
		parent_node->children[i] = child_node;

		// Add to found array
		found_array.array[found_array.next_index++] = child_node;
	}
}

// Prints <n> tabs
void print_indent(int n)
{
	for (int i = 0; i < n; i++) printf("   ");
}

void print_tree(s_node * head)
{
	print_perm(head->perm);
	for (int i = 0; i < N; i++)
	{
		if (head->children[i])
		{
			print_indent(head->level+1);
			printf("s(%d) ---> ", i+1);
			print_tree(head->children[i]);
		}
	}
}

int found_array_biggest_level()
{
	int biggest = 0;
	for (int i = 0; i < found_array.next_index; i++)
		if (found_array.array[i]->level > biggest)
			biggest = found_array.array[i]->level;

	return biggest;
}

void initialize_found_array()
{
	// Calculate final_size. Formula is (N * (N-1) * ... * (N - K + 1)) / K
	found_array.final_size = 1;
	for (int i = 0; i <= (K-1); i++) found_array.final_size *= (N - i);
	found_array.final_size /= K;
//	printf("Size: %d\n", found_array.final_size);

	// Allocate memory
	found_array.array = calloc(found_array.final_size, sizeof(s_node*));
	ALLOC_VALIDATE(found_array.array)

	found_array.next_index = 0;
}

// Frees both nodes and perms
void free_found_array()
{
	for (int i = 0; i < found_array.next_index; i++)
	{
		s_node* node = found_array.array[i];
		free(node->perm);
		free(node);
	}
}

void print_level(int level)
{
	printf("Level %d:\n", level);
	for (int i = 0; i < found_array.next_index; i++)
	{
		s_node* node = found_array.array[i];
		if (node->level == level)
		{
			print_perm(node->perm);
			putchar(' ');
		}
	}

	putchar('\n');
}

// Populates the children for all nodes of given level at found_array
void populate_children_for_level(int level)
{
	int s = found_array.next_index; // It might change later

	unsigned long long counter = 0;
	for (int i = 0; i < s; i++)
	{
		s_node* node = found_array.array[i];
		if (node->level == level) counter++;
	}
	printf("Level %d has %llu nodes\n", level, counter);

	for (int i = 0; i < s; i++)
	{
		s_node* node = found_array.array[i];
		if (node->level == level) populate_children(node);
	}
}

int main()
{
	initialize_found_array();

	// Initialize head
	s_node* head = allocate_node();
	head->perm = get_initial_perm();
	head->level = 0;

	// Add head to found_array
	found_array.array[found_array.next_index++] = head;

	// 2, 3, sh-ager
	int level = 0;
	while (found_array.next_index < found_array.final_size)
	{
		printf("Starting level %d\n", level);
		populate_children_for_level(level++);
	}

	int biggest_level = found_array_biggest_level();
	for (int i = biggest_level; i >= 0; i--) print_level(i);

	printf("Done. Total Size: %d\n", found_array.next_index);

	// Allocate polynom
	unsigned long long* polynom = calloc(biggest_level+1, sizeof(unsigned long long));
	ALLOC_VALIDATE(polynom)

	// Generate polynom
	for (int i = 0; i < found_array.final_size; i++)
	{
		s_node* node = found_array.array[i];
		polynom[node->level]++;
	}

	// Print polynom
	for (int i = 0; i <= biggest_level; i++)
	{
		printf("%lluq^%d", polynom[i], i);
		if (i != biggest_level) printf(" + ");
		else putchar('\n');
	}

	free(polynom);
	free_found_array();

	return 0;
}