#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdio.h>

#define N 4
#define K 4

typedef int bool;
#define true 1
#define false 0

#define ALLOC_VALIDATE(p) if (!p) { printf("Not enough memory!"); exit(1); }

#endif //_MAIN_H_
