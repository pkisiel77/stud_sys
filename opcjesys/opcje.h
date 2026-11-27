// Platform-specific includes
// ncurses version
#define farmalloc malloc
#define farfree free
#define _CVC_
#define HWND void*

#ifndef _GLOBAL_OPCJESYS_
#define _GLOBAL_OPCJESYS_
//#define getch VCGetch
#define EKRAN_X_MAX 80
#define EKRAN_Y_MAX 25
struct BUFOR_EKRANU
{
	char znak[EKRAN_X_MAX][EKRAN_Y_MAX];
	unsigned int atrybut[EKRAN_X_MAX][EKRAN_Y_MAX];
};
//struct BUFOR_EKRANU be;
//HANDLE hConsoleOutput, hConsoleInput;

#ifdef WEWY_MAIN // dla wewy_main
 #include "../opcjesys/shintdef.h"
#endif // koniec wewy_main
#ifdef _MOJE_H_ // hedery dla moje.h
 #include <stddef.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <math.h>
 #include <time.h>
 #include <string.h>
 #include <ctype.h>
  #include "alloc.h"
  typedef  int pid_t;
  #include "../blank/wewybl.h"
  #include "../opcjesys/scr_size.h"
#endif // koniec hedrow dla moje.h
#ifdef _WEWYBL_H_INCLUDE // hedery dla wewybl.h
 #include "../opcjesys/shintdef.h"
 #include "../opcjesys/mystderr.h"
#endif // koniec init. hederow dla wewybl.h
#ifdef _WEWY1_H_INCLUDE
# include "../opcjesys/scr_size.h"
#endif
#endif

