#ifndef SHINT_DEF
#define SHINT_DEF
#ifdef _DOS_
#undef MULTSCR
#ifdef _DOS_WC
#define _SYS_32B_
#include <malloc.h>
#else
#ifdef _CVC_
#include <stdlib.h>
#else
#include <alloc.h>
#endif
#endif
#elif defined(_NCURSES_)   /* for NCURSES */
#include <stdlib.h>
#define MULTSCR
/* For NCURSES, use int for shint to match term_ncurses.c expectations */
typedef int shint;
typedef unsigned int Uint;
typedef unsigned int Ushint;
#define Free free
#define Malloc malloc
#else   /* dla QNX WC */
#define _SYS_32B_
#include <stdlib.h>
#define MULTSCR
#endif
#ifdef _SYS_32B_
typedef short int shint;
typedef unsigned short int Uint;
typedef unsigned short int Ushint;
#define Free free
#define Malloc malloc
#else
#ifndef _NCURSES_
typedef int shint;
typedef unsigned int Uint;
typedef unsigned int Ushint;
#endif
#ifdef _CVC_
#define Free free
#define Malloc malloc
#else
#define Free farfree
#define Malloc farmalloc
#endif
#endif
#ifdef _DOS_
typedef shint pid_t;
typedef shint nid_t;
#endif
#endif
