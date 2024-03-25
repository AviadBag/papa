#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "main.h"
#include "found_tree.h"

#define RED "\033[31m" /* Red */
#define RESET "\033[0m"

int current_level;
unsigned long long found = 0;
unsigned long long *polynom;
unsigned int max_inversion = 0;

// Returns a new, dynamically allocated, permutation. Exits program on error.
int *allocate_perm()
{
	int *perm = calloc(K, sizeof(int));
	ALLOC_VALIDATE(perm)

	return perm;
}

// Shifts the GIVEN PERM left.
void shift_left(int *perm)
{
	int first = perm[0]; // Store the first element
	for (int i = 0; i < K - 1; i++)
	{
		perm[i] = perm[i + 1]; // Shift each element one position to the left
	}
	perm[K - 1] = first; // Move the first element to the last position
}

int *get_initial_perm()
{
	int *perm = allocate_perm();
	for (int i = 0; i < K; i++)
		perm[i] = i + 1;
	return perm;
}

unsigned int get_inversion(const int *perm)
{
	unsigned int inv = 0;
	for (int i = 0; i < K - 1; i++)
	{
		for (int j = i + 1; j < K; j++)
		{
			if (perm[i] > perm[j])
				inv++;
		}
	}

	return inv;
}

void print_perm(int *perm)
{
	printf("(");
	for (int i = 0; i < K; i++)
	{
		if (i + 1 == K)
			printf("%d)", perm[i]);
		else
			printf("%d, ", perm[i]);
	}
}

// Returns the index of the given number in the permutation. -1 if not found
int index_of(const int *perm, int what)
{
	for (int i = 0; i < K; i++)
		if (perm[i] == what)
			return i;

	return -1;
}

int *duplicate_perm(const int *perm)
{
	int *new_perm = allocate_perm();
	memcpy(new_perm, perm, K * sizeof(int));
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

int *Si(const int *perm, int i)
{
	return Sij(perm, i, i + 1);
}

// Returns the index of the smallest number in the perm
int index_of_smallest(const int *perm)
{
	int index = 0;
	for (int i = 0; i < K; i++)
	{
		if (perm[i] < perm[index])
			index = i;
	}

	return index;
}

// Rearranges the given perm, so the first number is the lowest one. (Keeps the permutation intact).
void rearrange_perm(int *perm)
{
	int smallest_i = index_of_smallest(perm);
	if (smallest_i == 0)
		return; // Already arranged

	int *perm_temp = duplicate_perm(perm);
	int n = K - smallest_i;

	// Move <n> cycles
	for (int i = 0; i < K; i++)
	{
		perm[(i + n) % K] = perm_temp[i];
	}

	free(perm_temp);
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
					print_perm(sij_perm);
					printf(" | ");
				}
			}
			free(sij_perm);
		}
	}

	return found;
}

unsigned long long get_final_size()
{
	unsigned long long final_size = 1;
	for (int i = 0; i <= (K - 1); i++)
		final_size *= (N - i);
	final_size /= K;

	return final_size;
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
		print_perm(initial_perm);
		printf(" |");
	}

	free(initial_perm);
}

void get_data()
{
	unsigned long long final_size = get_final_size();

	// 2, 3, sh-ager
	while (found < final_size)
	{
		if (PRINT_PERMS)
			printf("\nLevel %d:\n| ", current_level + 1);
		iterate_permutations(&find_children_for_current_level);
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

// Sets r to be p(s)
void perms_composition(const int *p, const int *s, int *r)
{
	for (int i = 0; i < K; i++)
	{
		r[i] = p[s[i] - 1];
	}
}

// Finds the minimum inversion of p*(c^i)*s
unsigned int find_min_inversion(int *p, int *s)
{
	unsigned int min_inv = UINT_MAX;
	for (int i = 0; i < K; i++)
	{
		int comp[K];
		perms_composition(p, s, comp);
		unsigned int inv = get_inversion(comp);
		if (inv < min_inv)
			min_inv = inv;

		shift_left(s); // Next shift. (Finally will cancel out).
	}

	return min_inv;
}

void calculate_values_iterator(int *perm, int level)
{
	static int phase = 1;
	static int *phase1_perm;

	if (phase == 1)
	{
		// We only have one perm, we need to go over all perms with this perm.
		phase = 2;
		phase1_perm = perm;
		iterate_permutations(&calculate_values_iterator);
		phase = 1;
	}
	else
	{
		// We have two perms!
		unsigned int min_inv = find_min_inversion(phase1_perm, perm);
		if (min_inv > max_inversion)
			max_inversion = min_inv;

		if (PRINT_PERMS)
		{
			printf("Minimum inversion for ");
			print_perm(phase1_perm);
			printf(" and ");
			print_perm(perm);
			printf(" is: %d\n", min_inv);
		}
	}
}

void calculate_values()
{
	// Iterate over all pairs of permutations.
	iterate_permutations(&calculate_values_iterator);
	printf("Max inversion: %d\n", max_inversion);
}

int number_of_cycles(const int *perm)
{
	int cycles = 0;
	bool visited[K];
	memset(visited, 0, K * sizeof(bool));

	for (int i = 0; i < K; i++)
	{
		if (!visited[i])
		{
			cycles++;
			int j = i;
			do
			{
				visited[j] = true;
				j = perm[j] - 1;
			} while (j != i);
		}
	}

	return cycles;
}

int main()
{
	init_data();
	get_data();
	calculate_values();
	print_polynom();
	cleanup();

	return 0;
}
