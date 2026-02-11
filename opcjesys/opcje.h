// Platform-specific includes
#ifndef _NCURSES_
// Windows version
#include <windows.h>
#define farmalloc malloc
#define farfree free
#define _CVC_
#else
// ncurses version
#define farmalloc malloc
#define farfree free
#define _CVC_
#define HWND void*
#endif

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
#ifndef _NCURSES_
extern HWND handle_window_device;
#endif

#ifdef _MAIN_MODUL_ // modu�u g��wnego czyli main
 #include "../blank/konsola.c"
#endif // koniec dla modu�u g��wnego
#ifdef WEWY_MAIN // dla wewy_main
 #include "../opcjesys/SHINTDEF.H"
#endif // koniec wewy_main
#ifdef _MOJE_H_ // hedery dla moje.h
 #include <stddef.h>
 #include <stdio.h>
 #include <stdlib.h>
 #ifndef _NCURSES_
 #include <conio.h>
 #endif
 #include <math.h>
 #include <time.h>
 #include <string.h>
 #include <ctype.h>
 #if defined(_DOS_) || defined(_NCURSES_) // dla DOS lub ncurses
  #ifndef _NCURSES_
  #include <dos.h>
  #endif
  #include "alloc.h"
  #ifndef _NCURSES_
  #include <process.h>
  typedef  int shint;
  typedef  int pid_t;
  #else
  typedef  int pid_t;
  #endif
  #include "../blank/wewybl.h"
  #include "../opcjesys/SCR_SIZE.H"
 #endif // koniec DOS/ncurses
#endif // koniec hedr�w dla moje.h
#ifdef _WEWYBL_H_INCLUDE // hedery dla wewybl.h
 #include "../opcjesys/SHINTDEF.H"
 #include "../opcjesys/MYSTDERR.H"
#endif // koniec init. heder�w dla wewybl.h
#ifdef _WEWY1_H_INCLUDE
# include "../opcjesys/SCR_SIZE.H"
#endif
#ifdef GRAF
//extern HDC hdc;
//extern HWND hwnd;
//#define getch VCGetch
//#define MAX_LICZBA_KOLOROW 25 // do kolor�w systemowch w WEWYBL.H
#endif // koniec dla win_graf
#endif

