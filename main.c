#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
	#include <time.h>

#include "main.h"
#include "found_tree.h"

int current_level;
unsigned long long found = 0;
unsigned long long *polynom;
unsigned int max_inversion = 0;

const int partition[] = PARTITION;

// Returns a new, dynamically allocated, permutation. Exits program on error.
int *allocate_perm()
{
	int *perm = calloc(PERM_MEMORY_SIZE, sizeof(int));
	ALLOC_VALIDATE(perm)

	return perm;
}

int *get_initial_perm()
{
	int *perm = allocate_perm();
	int k = 0, j = 0, l = 1;
	for (int i = 0; i < PERM_MEMORY_SIZE; i++)
	{
		if (k++ == partition[j])
		{
			k = 0;
			j++;
			perm[i] = SUBPERM_SEPARATOR;
		}
		else
		{
			perm[i] = l++;
		}
	}
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
	for (int i = 0; i < PARTITION_SIZE; i++)
	{
		int k = partition[i];
		print_perm_l(perm, k);
		perm += k;
	}
}

void print_perm(int* perm)
{
	putchar('(');
	for (int i = 0; i < PERM_MEMORY_SIZE; i++)
	{
		if (perm[i] == SUBPERM_SEPARATOR)
		{
			putchar(')');
			if (i + 1 != PERM_MEMORY_SIZE) putchar('(');
		}
		else
		{
			printf("%d", perm[i]);
			if (perm[i+1] != SUBPERM_SEPARATOR) printf(", ");
		}
	}
}

// Returns the index of the given number in the permutation. -1 if not found
int index_of(const int *perm, int what)
{
	for (int i = 0; i < PERM_MEMORY_SIZE; i++)
		if (perm[i] == what)
			return i;

	return -1;
}

int *duplicate_perm(const int *perm)
{
	int *new_perm = allocate_perm();
	memcpy(new_perm, perm, PERM_MEMORY_SIZE * sizeof(int));
	return new_perm;
}

// Performs an S operation on the given perm. Result is returned as a newly allocated perm.
int *Si(const int *perm, int i)
{
	int *new_perm = duplicate_perm(perm);

	// Perform operation (Switch between i and j)
	int a = index_of(perm, i);
	int b = index_of(perm, i+1);
	if (a != -1)
		new_perm[a] = i+1;
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
	for (int i = 0; i < PARTITION_SIZE; i++)
	{
		int k = partition[i];
		rearrange_perm_l(perm, k);
		perm += k;
	}
}

unsigned long long find_children_for_perm(int *perm)
{
	// For every child, perform the corresponding S operation
	for (int i = 1; i < PERM_MEMORY_SIZE; i++)
	{
		if (perm[i] == SUBPERM_SEPARATOR) continue;

		int new_level = current_level + 1;

		// Get next perm
		int *si_perm = Si(perm, i);
		rearrange_perm(si_perm);

		// Add this perm!
		if (add_permutation(si_perm, new_level))
		{
			// It was a new perm
			found++;
			if (PRINT_PERMS)
			{
				printf("S%d -> ", i);
				print_perm_with_division(si_perm);
				printf(" | ");
			}
		}
		free(si_perm);
	}

	return found;
}

// Gets children for this prem only if it's level is right. This callback will be called with every perm we have.
void find_children_for_current_level(int *perm, int level, __attribute__((unused)) void* extra_data)
{
	if (level == current_level)
		find_children_for_perm(perm);
}

void iterator_generate_polynom(__attribute__((unused)) int *perm, int level, __attribute__((unused)) void* extra_data)
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
	// 2, 3, sh-ager
	unsigned long long last_found = 0;
	while (true)
	{
		if (PRINT_PERMS)
			printf("\nLevel %d:\n| ", current_level + 1);
		iterate_permutations(&find_children_for_current_level, 0);
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

	iterate_permutations(iterator_generate_polynom, 0);

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

//unsigned int get_inversion(const int *perm)
//{
//	unsigned int inv = 0;
//	for (int i = 0; i < N - 1; i++)
//	{
//		for (int j = i + 1; j < N; j++)
//		{
//			if (perm[i] > perm[j])
//				inv++;
//		}
//	}
//
//	return inv;
//}
//
//// Shifts the GIVEN PERM left.
//void shift_left(int *perm)
//{
//	int first = perm[0]; // Store the first element
//	for (int i = 0; i < N - 1; i++)
//	{
//		perm[i] = perm[i + 1]; // Shift each element one position to the left
//	}
//	perm[N - 1] = first; // Move the first element to the last position
//}
//
//// Inverts perm, returns it as a new permutation.
//int* convert(int* perm)
//{
//	int* new_perm = allocate_perm();
//
//	for (int i = 1; i <= N; i++)
//	{
//		int index = index_of(perm, i);
//		new_perm[i-1] = index + 1;
//	}
//
//	return new_perm;
//}
//
//// Sets r to be p(s)
//void perms_composition(const int *p, const int *s, int *r)
//{
//	for (int i = 0; i < N; i++)
//	{
//		r[i] = p[s[i] - 1];
//	}
//}
//
//// Finds the minimum inversion of p*(c^i)*s
//unsigned int find_min_inversion(int *p, int *s)
//{
//	unsigned int min_inv = UINT_MAX;
//	for (int i = 0; i < N; i++)
//	{
//		shift_left(p); // Next shift. (Finally will cancel out).
//
//		int comp[N];
//		int* converted = convert(s);
//		perms_composition(p, converted, comp);
//		free(converted);
//
//		unsigned int inv = get_inversion(comp);
//		if (inv < min_inv)
//			min_inv = inv;
//	}
//
//	return min_inv;
//}
//
//void calculate_values_2st_iterator(int *perm, __attribute__((unused)) int level, void* extra_data)
//{
//	int* first_perm = (int*) extra_data;
//
//	unsigned int min_inv = find_min_inversion(first_perm, perm);
//	if (min_inv > max_inversion)
//		max_inversion = min_inv;
//
//	if (PRINT_PERMS)
//	{
//		printf("Minimum inversion for ");
//		print_perm_l(first_perm, N);
//		printf(" and ");
//		print_perm_l(perm, N);
//		printf(" is: %d\n", min_inv);
//	}
//}
//
//void calculate_values_1st_iterator(int *perm, __attribute__((unused)) int level, __attribute__((unused)) void* extra_data)
//{
//	// Iterate over all permutations with this permutation
//	iterate_permutations(&calculate_values_2st_iterator, (void*) perm);
//}
//
//void calculate_values()
//{
//	// Iterate over all pairs of permutations.
//	iterate_permutations(&calculate_values_1st_iterator, 0);
//	printf("Max len: %d\n", max_inversion);
//}

int main()
{
	clock_t start = clock();

	init_data();
	get_data();
//	calculate_values();
	print_polynom();
	cleanup();

	clock_t stop = clock();
	double elapsed = (double) (stop - start) / CLOCKS_PER_SEC;
	printf("\nTime elapsed: %.5fs\n", elapsed);

	return 0;
}
