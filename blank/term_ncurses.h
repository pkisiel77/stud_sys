/*--------------------------------------
  Plik: term_ncurses.h
  Opis: Header file for ncurses terminal implementation
        Replaces Windows console with cross-platform ncurses
  
  Wersja: 1.0.0
  Data:  10.02.2026

  Kontakt: ncurses port
--------------------------------------*/

#ifndef _TERM_NCURSES_H_
#define _TERM_NCURSES_H_

#include <ncurses.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

/* Type definitions compatible with existing code */
//typedef short int shint;
//typedef unsigned short int Ushint;

/* Cursor definitions */
extern unsigned int cursor, nocursor;

/* Global position tracking */
extern int xg_akt, yg_akt;
extern char alarmclock;

/* Screen size */
extern int x_akt, y_akt;
extern int x_akt0, y_akt0;
extern int x_maxOkna, y_maxOkna;
extern int x_min, y_min;
extern int x_max, y_max;

/* Function prototypes - ncurses implementations */
void InitConsole(void);
void CloseConsole(void);
int Endgraph(void);
void setcursor(unsigned int shape);
int otworz_graf_blank(int Xw_min, int Yw_min, int Xw_max, int Yw_max, int Yz_max, unsigned int Attr);
void term_clear(int co);
void term_color(int attr);
void term_fill(int attr);
int term_type(int y, int x, char* text, int lenth, unsigned int attr);
int term_printf(int y, int x, unsigned int attr, char* format, ...);
int m_wherex(void);
int m_wherey(void);
int GET_char(void);
void m_gotoxy(int x, int y);
void clear_cur(char z);
int term_save_image(int y0, int x0, char* bufor, int dlug);
void term_restore_image(int y0, int x0, char* bufor, int dlug);
void backspace(char* c);
void term_flush(void);
void ramka_graficzna(int yp, int xp, int ym, int xm, unsigned int attr);
void ustaw_okno_graficzne(void);
void bar(int x0, int y0, int x1, int y1);
void term_cur(int y, int x);
void sound(unsigned frequency);
void nosound(void);
void delay(unsigned milliseconds);
void Cls(void);

/* Color mapping from Windows style to ncurses color pairs */
int map_attr_to_color_pair(unsigned int attr);
void init_ncurses_colors(void);

/* MessageBox replacement */
int MessageBox(void* hwnd, const char* message, const char* title, unsigned int type);

/* MessageBox types */
#define MB_OK 0
#define MB_ICONERROR 0x10
#define MB_ICONWARNING 0x30

#endif /* _TERM_NCURSES_H_ */
