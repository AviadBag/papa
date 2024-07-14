#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "found_tree.h"

int current_level;
unsigned long long found = 0;
unsigned long long *polynom;

// Returns a new, dynamically allocated, permutation. Exits program on error.
int *allocate_perm()
{
	int *perm = calloc(N, sizeof(int));
	ALLOC_VALIDATE(perm)

	return perm;
}

int *get_initial_perm()
{
	int *perm = allocate_perm();
	for (int i = 0; i < N; i++)
		perm[i] = i + 1;
	return perm;
}

void print_perm_l(int *perm, int len)
{
	printf("(");
	for (int i = 0; i < len; i++)
	{
		if (i + 1 == len)
			printf("%d)", perm[i]);
		else
			printf("%d, ", perm[i]);
	}
}

void print_perm_with_division(int* perm)
{
	const int kS[] = kS_DEF;
	for (int i = 0; i < NUMBER_OF_kS; i++)
	{
		int k = kS[i];
		print_perm_l(perm, k);
		perm += k;
	}
}

// Returns the index of the given number in the permutation. -1 if not found
int index_of(const int *perm, int what)
{
	for (int i = 0; i < N; i++)
		if (perm[i] == what)
			return i;

	return -1;
}

int *duplicate_perm(const int *perm)
{
	int *new_perm = allocate_perm();
	memcpy(new_perm, perm, N * sizeof(int));
	return new_perm;
}

// Performs an S operation on the given perm. Result is returned as a newly allocated perm.
int *Sij(const int *perm, int i, int j)
{
	int *new_perm = duplicate_perm(perm);

	// Perform operation (Switch between i and j)
	int a = index_of(perm, i);
	int b = index_of(perm, j);
	if (a != -1)
		new_perm[a] = j;
	if (b != -1)
		new_perm[b] = i;

	return new_perm;
}

// Returns the index of the smallest number in the perm
int index_of_smallest(const int *perm, int len)
{
	int index = 0;
	for (int i = 0; i < len; i++)
	{
		if (perm[i] < perm[index])
			index = i;
	}

	return index;
}

// rearrange_perm(perm) with a given length.
void rearrange_perm_l(int *perm, int len)
{
	int smallest_i = index_of_smallest(perm, len);
	if (smallest_i == 0)
		return; // Already arranged

	int *perm_temp = duplicate_perm(perm);
	int n = len - smallest_i;

	// Move <n> cycles
	for (int i = 0; i < len; i++)
	{
		perm[(i + n) % len] = perm_temp[i];
	}

	free(perm_temp);
}

// Rearranges the given perm, so the first number is the lowest one. (Keeps the permutation intact).
void rearrange_perm(int *perm)
{
	const int kS[] = kS_DEF;
	for (int i = 0; i < NUMBER_OF_kS; i++)
	{
		int k = kS[i];
		rearrange_perm_l(perm, k);
		perm += k;
	}
}

unsigned long long find_children_for_perm(int *perm)
{
	// For every child, perform the corresponding S operation
	for (int i = 1; i < N; i++)
	{
		for (int j = i + 1; j <= N; j++)
		{
			int new_level = current_level + 1;

			// Get next perm
			int *sij_perm = Sij(perm, i, j);
			rearrange_perm(sij_perm);

			// Add this perm!
			if (add_permutation(sij_perm, new_level))
			{
				// It was a new perm
				found++;
				if (PRINT_PERMS)
				{
					print_perm_with_division(sij_perm);
					printf(" | ");
				}
			}
			free(sij_perm);
		}
	}

	return found;
}

// Gets children for this prem only if it's level is right. This callback will be called with every perm we have.
void find_children_for_current_level(int *perm, int level)
{
	if (level == current_level)
		find_children_for_perm(perm);
}

void iterator_generate_polynom(int *perm, int level)
{
	polynom[level]++;
}

void init_data()
{
	initialize_found_tree();

	// Add initial permutation
	int *initial_perm = get_initial_perm();
	add_permutation(initial_perm, 0);
	found++;

	if (PRINT_PERMS)
	{
		printf("Level 0:\n| ");
		print_perm_with_division(initial_perm);
		printf(" |");
	}

	free(initial_perm);
}

void get_data()
{
	// 2, 3, sh-ager
	unsigned long long last_found = 0;
	while (true)
	{
		if (PRINT_PERMS)
			printf("\nLevel %d:\n| ", current_level + 1);
		iterate_permutations(&find_children_for_current_level);
		if (found == last_found) break; // We found nothing
		last_found = found;
		current_level++;
	}
	if (PRINT_PERMS)
		putchar('\n');

	printf("Finished generating perms. Total Size: %llu\n", found);
}

void print_polynom()
{
	// Allocate polynom
	polynom = calloc(current_level + 1, sizeof(unsigned long long));
	ALLOC_VALIDATE(polynom)

	iterate_permutations(iterator_generate_polynom);

	// Print polynom
	for (int i = 0; i <= current_level; i++)
	{
		printf("%lluq^%d", polynom[i], i);
		if (i != current_level)
			printf(" + ");
		else
			putchar('\n');
	}

	free(polynom);
}

void cleanup()
{
	free_found_tree();
}

int main()
{
	init_data();
	get_data();
//	calculate_values();
	print_polynom();
	cleanup();

	return 0;
}
