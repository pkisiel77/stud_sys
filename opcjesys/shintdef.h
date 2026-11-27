#ifndef SHINT_DEF
#define SHINT_DEF
#include <stdlib.h>
#define MULTSCR
/* For NCURSES, use int for shint to match term_ncurses.c expectations */
typedef int shint;
typedef unsigned int Uint;
typedef unsigned int Ushint;
#define Free free
#define Malloc malloc
#ifdef _SYS_32B_
typedef short int shint;
typedef unsigned short int Uint;
typedef unsigned short int Ushint;
#define Free free
#define Malloc malloc
#else
#define Free free
#define Malloc malloc
#endif
#endif
