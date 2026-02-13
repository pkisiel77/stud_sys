#ifndef _WEWY1_H_INCLUDE
#define _WEWY1_H_INCLUDE
#include "../opcjesys/scr_size.h"
extern signed char _TYP_KONSOLI_;
#ifdef _TERM_QNX_
#include <sys/types.h>
#include <sys/mouse.h>
#include <sys/dev.h>
#include <sys/proxy.h>
#include <sys/kernel.h>
#define MAX_EVENTS 10
#define LCHAR_DEV 1
/*  ---------------------- zmienne globalne z pliku create.c ----------- */
#define M_BUF_SIZ (MAX_EVENTS*sizeof(struct _mouse_ctrl)) 
extern struct _mouse_ctrl *Mouse_ctrl;
extern char *kbuf;
extern struct mouse_event Mouse_ev[], *M_ev;
extern struct _mouse_param Mouse_Par, *Mouse_par;
extern const char *nazwa_myszki="Myszka";
extern pid_t Mproxy, kproxy; 
/* --------------------- koniec zmiennych z pliku create.c ------------ */



pid_t w_pid;
#endif
#define LHASEL    7
#define LHELP    26
#define LBLANK   10
#define LRAP_BL  15
struct blankiet
 {struct okno *Okbl;
  signed char Kolor;
  char czynny;
  signed char ramka;
  char Uprawn;
  char *tytul;
  shint y_tyt;
  shint x_tyt;
  shint nr_danej;
  shint nr_rekordu;
  shint liczba_danych;
  shint X_0;
  shint Y_0;
	shint X_max;
  shint Y_max;
  unsigned int anim_pid;
  signed char nr_raportu;
  char reset;
  shint rek_danych_blank;
  signed char y_scroll;
  signed char y_shift;
  shint scroll;
 };
/* ----------------------------------------------------------------------- */
struct reports
 {signed char kod_raportu;
  char typ_bazy;
  unsigned int anim_pid;
  shint ob_pocz;
  shint ob_konc;
  shint rozmiar_ob;
  char *tytul;
  signed char Kolor;
  signed char Ramka;
  char *D;
	char *bufs;
  char free_mem;
  signed char uprawnienia;
  char *zapis;
  char **d_oryg;
  shint max_l_danych_blank;
  shint (*def_blankiet)(shint nr_ob, shint ob_pocz, shint ob_konc, shint xp,
                        shint yp, shint anim_pid, char *D);
	char *(*dane_rap_bl)(shint ob_pocz, shint ob_konc, shint *rozmiar_ob);
  void (*wpis_danych_bl)(shint ob_pocz, shint ob_konc, char *D,
                         shint rozmiar_ob, char zapis[], char *Kod_op);
 };

#ifndef WEW_EXT
struct Blok_anim
 {shint xp, yp, xm, ym;} BA;
shint mouse_free=0;
char blokada_potwierdzania=0;
char ABORT=1, BLAD=1, blokada_zapisu=0, bez_potwierdzania=0;
shint SCROLL=-1;
signed char Akt_Kolor=0, Klucz=-1, arrow=27;
char haslo[LHASEL][11];
char kod_o[LHASEL][21];
unsigned int attr_o[3], attrtx[3], attr_dat[3], attr_inf;
struct Blok_anim *blok_anim=&BA; /*NULL; */
/* ---------------- Parametry wymiarowe monitora -------------------
									dla WATCOM C beda od (0,0 - 24,79)                 */
/* #ifndef _DOS_ */
shint X_0=0, Y_0=0, X_maxBlank, Y_maxBlank;
// int x_maxOkna=0, y_maxOkna=0;
// int x_akt=0, y_akt=0, x_akt0=0, y_akt0=0;
// int x_min=0, y_min=0;
int dana_poczatkowa=0;
/* ----------------------------------------------------------------- */
unsigned int attryb=TERM_WHITE|TERM_BLUE_BG|TERM_FLUSH /*, opcje*/;
shint INSERT=-1, HELP=0, ZAPIS=0, BEZ_HASLA=0, SLEDZIC=0;
struct okno *Ok=NULL;
shint nr_danej=0, liczba_danych=0;
shint lwmall=0, lwmallp=0;
char Uprawn=6, Upr_max=10, Uprawn_max=0, *Kod_operatora_max=NULL;
signed char NR_OPER=-1;
struct blankiet *BL[LBLANK];
struct reports *RAP[LRAP_BL+1];
shint nr_blank=-1;
shint x_kursor=-1;
char lhasel=0, Uprawn_pocz=0, EDIT=0;
signed char KOD_raportu=-1, wpisano_dana=0;
char Czy_Blankiet_otwarty=0;
char *(*dane_rap_bl)(shint ob_pocz, shint ob_konc, shint *rozmiar_ob);
void (*wpis_danych_bl)(shint ob_pocz, shint ob_konc, char *D,
											 shint rozmiar_ob, char zapis[], char *Kod_op);
#else
struct Blok_anim
 {shint xp, yp, xm, ym;};
WEW_EXT shint mouse_free;
WEW_EXT char haslo[LHASEL][11];
WEW_EXT char kod_o[LHASEL][21];
WEW_EXT char blokada_potwierdzania;
WEW_EXT char ABORT, BLAD, blokada_zapisu, bez_potwierdzania;
WEW_EXT signed char Akt_Kolor;
WEW_EXT int X_0, Y_0, X_maxBlank, Y_maxBlank;
WEW_EXT int x_maxOkna, y_maxOkna, x_akt, y_akt, x_akt0, y_akt0;/*, xg_akt, yg_akt; */
WEW_EXT int x_kursor, x_min, y_min;
WEW_EXT unsigned int attryb/*, opcje*/;
WEW_EXT int INSERT, HELP, ZAPIS, BEZ_HASLA, SLEDZIC;
WEW_EXT struct okno *Ok;
WEW_EXT int nr_danej, liczba_danych;
WEW_EXT int lwmall, lwmallp;
WEW_EXT char Uprawn, Upr_max, Uprawn_max, *Kod_operatora_max;
WEW_EXT signed char NR_OPER, Klucz, arrow;
WEW_EXT struct blankiet *BL[LBLANK];
WEW_EXT struct reports *RAP[LRAP_BL+1];
WEW_EXT int nr_blank;
WEW_EXT signed char KOD_raportu, wpisano_dana;
WEW_EXT char Czy_Blankiet_otwarty, EDIT;
WEW_EXT shint SCROLL;
WEW_EXT char lhasel, Uprawn_pocz;
WEW_EXT shint dana_poczatkowa;
WEW_EXT unsigned int attr_o[], attrtx[], attr_dat[], attr_inf;
WEW_EXT struct Blok_anim *blok_anim;
WEW_EXT unsigned int cursor, nocursor;
#endif
void aktualiz_blank(shint dana, shint liczba, struct okno *okd,
			unsigned int attr_d[], unsigned int attr_o[],
			unsigned int attrtx[], unsigned int attr_inf);
void przepisz(char *do_ob, char *z_ob, shint ile);
char *strukt_danych_raportu(struct reports *R, shint *rozmiar_ob, shint i);
char wpisz_haslo(char **kod_operat, signed char potrzebne_uprawn,
                 char nr_oper, struct reports *Rp);
struct dane_wpisu
 {shint x0,  xmax, x_konc, dltn, poz_wart_abs, dlh_ogr, iobl;
  char *format, *format_bazy, *format_tx, *form, *format_wyn;
  char *help_ogr, *format_ogr, *text, *zakres, *zakr_abs, *bufor, *taknie;
  float *wartn, baza, w_min, w_max, abs_min, abs_max;
 };

shint wpisanie_danych_fl(shint x0, shint xp, shint y, shint xmax, shint x_konc,
          char iffree, shint dltn, shint poz_wart_abs, shint dlh_ogr,
          char **format, char *format_bazy, char *format_tx, char *form,
          char *format_wyn, char help_ogr[], char *format_ogr, char text[],
          char zakres[], char *zakr_abs, char *bufor, char *taknie,
          unsigned int attr, unsigned int attrtx, shint iobl, shint L_kol,
          shint size, shint dim, shint ind_max, shint index, char *tabl,
          float *wart, float wartn[], float baza, float w_min, float w_max,
          float abs_min, float abs_max);
void *free_all_rep_mem(struct reports *R);
shint zapis_raportu(struct reports *Rp, shint poprz_dec, shint wymag_uprawn);
char *zwolnij_baze(struct reports *R);
/* struct lista *szukaj_adr_alloc(shint kod_raportu, char *txt,
															 char *co, char *adr, shint *liczba); */
#endif
