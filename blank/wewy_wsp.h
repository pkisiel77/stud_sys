#ifndef _WEWY_WSP_H_
#define _WEWY_WSP_H_
/* Wspolny poczatek plikow silnika formularzy (blank/wewy*.c).
 *
 * Silnik byl jednym plikiem wewy.c (wczesniej sklejonym z wewy.c i
 * wewy1.c ... wewy5.c). Ten naglowek zawiera prolog, ktory obowiazywal
 * w calym pliku. Globalne zmienne silnika definiuje tylko wewy.c
 * (#define WEWY_MAIN przed dolaczeniem); pozostale pliki dostaja je
 * z wewy1.h i opcjesys/scr_size.h jako extern. */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <time.h>
#if defined(__APPLE__)
#include <malloc/malloc.h>
#elif defined(__linux__)
#include <malloc.h>
#endif
#include "../opcjesys/opcje.h"
#include "../opcjesys/shintdef.h" /* opcje.h dolacza go tylko przy WEWY_MAIN */

#define Get_char GET_char
#define Get_Char GET_char

#include "moje.h"
#include "../loc.h"

#ifndef CLK_TCK
#define CLK_TCK CLOCKS_PER_SEC
#endif

/* Zmienne warstwy terminala (term_ncurses.c) */
extern int x_akt0, y_akt0;
extern int x_min, y_min;

void backspace(char* c);
int GETchar(void);
void ustaw_okno_graficzne();
void ramka_graficzna(int yp, int xp, int ym, int xm, unsigned int attr);
void przepisz(char* do_ob, char* z_ob, int ile);
void open_konsole_mouse(int fd_in);

#include "wewybl.h"
#include "wewy1.h"
#undef buttons
#endif
