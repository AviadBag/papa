#include <stdio.h>

#ifndef _MAIN_H_
#define _MAIN_H_

typedef int bool;
#define true 1
#define false 0

#define N 6

#define NUMBER_OF_kS 3
#define kS_DEF {3, 2, 1}

#define PRINT_PERMS true

#define ALLOC_VALIDATE(p) if (!p) { printf("Not enough memory!"); exit(1); }

#endif //_MAIN_H_
