#include <stdio.h>

#ifndef _MAIN_H_
#define _MAIN_H_

typedef int bool;
#define true 1
#define false 0

#define N 7

#define NUMBER_OF_kS 1
#define kS_DEF {N}

#define PRINT_PERMS false

#define ALLOC_VALIDATE(p) if (!p) { printf("Not enough memory!"); exit(1); }

#endif //_MAIN_H_
