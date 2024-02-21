#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdio.h>

typedef int bool;
#define true 1
#define false 0

#define N 10
#define K 10
#define PRINT_PERMS true

#define ALLOC_VALIDATE(p) if (!p) { printf("Not enough memory!"); exit(1); }

#endif //_MAIN_H_
