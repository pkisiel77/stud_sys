#ifndef SHINT_DEF
#define SHINT_DEF
#include <stdlib.h>
#define MULTSCR
/* For NCURSES, use int for shint to match term_ncurses.c expectations */
typedef int shint;
typedef unsigned int Uint;
typedef unsigned int Ushint;
#ifdef _SYS_32B_
typedef short int shint;
typedef unsigned short int Uint;
typedef unsigned short int Ushint;
#endif
/* Alokacje silnika formularzy z licznikiem lwmall (blank/wewy.c):
   raport pamieci po sesji raportow. Trwale struktury systemu
   (uslugi, agenda) uzywaja zwyklego malloc/free. */
void* Malloc(size_t rozmiar);
void Free(void* p);
#endif
