#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "found_tree.h"

int current_level;
unsigned long long found = 0;

// Returns a new, dynamically allocated, permutation. Exits program on error.
int* allocate_perm()
{
	int* perm = calloc(K, sizeof(int));
	ALLOC_VALIDATE(perm)

	return perm;
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

unsigned long long find_children_for_perm(int* perm)
{
	// For every child, perform the corresponding S operation
	for (int i = 0; i < N-1; i++)
	{
		int new_level = current_level + 1;

		// Get next perm
		int* si_perm = S(perm, i + 1);
		rearrange_perm(si_perm);

		// Add this perm!
		if (add_permutation(si_perm, new_level)) found++;
		free(si_perm);
	}

	return found;
}

unsigned long long get_final_size()
{
	unsigned long long final_size = 1;
	for (int i = 0; i <= (K-1); i++) final_size *= (N - i);
	final_size /= K;

	return final_size;
}

// Gets children for this prem only if it's level is right. This callback will be called with every perm we have.
void find_children_for_current_level(int* perm, int level)
{
	if (level == current_level) find_children_for_perm(perm);
}

void iterator_print_perm(int* perm, int level)
{
	print_perm(perm);
	printf(" <%d>\n", level);
}

int main()
{
	unsigned long long final_size = get_final_size();
	printf("Final size: %llu\n", final_size);

	// Add initial permutation
	int* initial_perm = get_initial_perm();
	add_permutation(initial_perm, 0);
	found++;
	free(initial_perm);

	// 2, 3, sh-ager
	while (found < final_size)
	{
		iterate_permutations(&find_children_for_current_level);
		current_level++;
	}

	iterate_permutations(&iterator_print_perm);

	printf("Done. Total Size: %llu\n", found);

	// Allocate polynom
//	unsigned long long* polynom = calloc(current_level+1, sizeof(unsigned long long));
//	ALLOC_VALIDATE(polynom)
//
//	// Generate polynom
//	for (int i = 0; i < found_array.final_size; i++)
//	{
//		s_node* node = found_array.array[i];
//		polynom[node->level]++;
//	}
//
//	// Print polynom
//	for (int i = 0; i <= biggest_level; i++)
//	{
//		printf("%lluq^%d", polynom[i], i);
//		if (i != biggest_level) printf(" + ");
//		else putchar('\n');
//	}

	return 0;
}

//int main()
//{
//	initialize_found_tree();
//	int perm1[K] = {1, 2, 3, 4};
//	int perm2[K] = {3, 2, 4, 1};
//	int perm3[K] = {1, 2, 4, 3};
//	add_permutation(perm1, 1);
//	add_permutation(perm2, 2);
//	add_permutation(perm3, 4);
//	iterate_permutations(&print_perm);
//	putchar('\n');
//	print_found_tree();
//}