#ifndef _WEWY1_H_INCLUDE
#define _WEWY1_H_INCLUDE
#include "../opcjesys/scr_size.h"
extern signed char _TYP_KONSOLI_;
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

/* Globalne zmienne silnika formularzy - definicje w blank/wewy.c */
struct Blok_anim
 {shint xp, yp, xm, ym;};
extern struct Blok_anim BA;
extern struct Blok_anim *blok_anim;
extern shint mouse_free;
extern char blokada_potwierdzania;
extern char ABORT, BLAD, blokada_zapisu, bez_potwierdzania;
extern shint SCROLL;
extern signed char Akt_Kolor, Klucz, arrow;
extern char haslo[LHASEL][11];
extern char kod_o[LHASEL][21];
extern unsigned int attr_o[3], attrtx[3], attr_dat[3], attr_inf;
extern shint X_0, Y_0, X_maxBlank, Y_maxBlank;
extern int dana_poczatkowa;
extern unsigned int attryb;
extern shint INSERT, HELP, ZAPIS, BEZ_HASLA, SLEDZIC;
extern struct okno *Ok;
extern shint nr_danej, liczba_danych;
extern shint lwmall, lwmallp;
extern char Uprawn, Upr_max, Uprawn_max, *Kod_operatora_max;
extern signed char NR_OPER;
extern struct blankiet *BL[LBLANK];
extern struct reports *RAP[LRAP_BL+1];
extern shint nr_blank;
extern shint x_kursor;
extern char lhasel, Uprawn_pocz, EDIT;
extern signed char KOD_raportu, wpisano_dana;
extern char Czy_Blankiet_otwarty;
extern char *(*dane_rap_bl)(shint ob_pocz, shint ob_konc, shint *rozmiar_ob);
extern void (*wpis_danych_bl)(shint ob_pocz, shint ob_konc, char *D,
                              shint rozmiar_ob, char zapis[], char *Kod_op);
/* Zmienne warstwy terminala - definicje w blank/term_ncurses.c */
extern int x_maxOkna, y_maxOkna, x_akt, y_akt, x_akt0, y_akt0;
extern int x_min, y_min;
extern unsigned int cursor, nocursor;
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
