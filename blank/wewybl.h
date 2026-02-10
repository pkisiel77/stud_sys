/*####################################################################
  #                  PAMIETAJ !!! w procedurach wewy                 #  
  #      WSZYSTKIE !!! ADRESOWE ZMIENNE !!! MUSZA BYC STATIC !!!!!!  #
  #                ograniczenia i liczby opcji tez !!!               #  
  ####################################################################    
/* -- Uwaga !! 1sza w linii grupa_danych MUSI miec spacje w formacie !!!! */
#ifndef _WEWYBL_H_INCLUDE
#define _WEWYBL_H_INCLUDE
#define BAZA_SPOJNA 0
#define BAZA_WEKTOR 1
#define BAZA_LISTOW 2
#if defined(_DOS_) || defined(_NCURSES_)
shint Get_Char(void);
shint Get_char(void);
#define GETchar Get_char
/*--------------------------KLAWIATURA--------------------------*/
#define DOWN     80 
#define BBS      75
#define FFR      77
#define UP       72
#define LEFT     75
#define PgUp     73
#define Home     71
#define End      79
#define KPDMINUS 163
#define KPDPLUS  167
#define PgDn     81
#define Ins      82
#define Del      83
#define Esc      27
#define CR       13
#define BS        8
#define SPEC     0
#define TAB        9
#define SHIFT_TAB 158
#define FONT1    59
#define FONT2    60
#define FONT3    61
#define FONT4    62
#define FONT5    63
#define FONT6    64
#define FONT7    65
#define FONT8    66
#define FONT9    67
#define FONT10   68
#define FONT11   69
#define FONT12   70
#define CTRL_TAB 148
/*--------------------------KOLORY-------------------------- */
#define TERM_NORMAL         0x0000
#define TERM_INVERSE        0x0000
#define TERM_FLUSH          0x0000
#define TERM_BLACK          0x0000
#define TERM_BLUE           0x0001
#define TERM_GREEN          0x0002
#define TERM_CYAN           0x0003
#define TERM_RED            0x0004
#define TERM_MAGENTA        0x0005
#define TERM_YELLOW         0x0006
#define TERM_WHITE          0x0007
#define TERM_BLACK_BG       0x0000
#define TERM_BLUE_BG        0x0100
#define TERM_GREEN_BG       0x0200
#define TERM_CYAN_BG        0x0300
#define TERM_RED_BG         0x0400
#define TERM_MAGENTA_BG     0x0500
#define TERM_YELLOW_BG      0x0600
#define TERM_WHITE_BG       0x0700
#define INVERSE             0x0040
#define BLINK               128
#define TERM_HILIGHT        HILIGHT
#define ATTR_A              TERM_WHITE|HILIGHT|TERM_RED_BG 
#define MTERM_HILIGHT       TERM_HILIGHT 
#define HILIGHT             0x0008
#define TERM_BLINK          BLINK
/*--------------------------KONIEC--------------------------*/
#endif
/*
#if !defined (_DOS_) & defined(GRAF)
#define DOWN		40
#define BBS			37  // == LEFT
#define FFR			39
#define UP			38
#define LEFT		37  // == BBS
#define PgUp		33
#define Home		36
#define End			35
#define KPDMINUS	109
#define KPDPLUS		107
#define PgDn		34
#define Ins			45
#define Del			46
#define Esc			27
#define CR			13
#define BS			8	 
#define SPEC		0
#define TAB			9
#define SHIFT_TAB	0// brak pomys�u
#define FONT1		112  
#define FONT2		113
#define FONT3		114	
#define FONT4 		115   
#define FONT5		116    
#define FONT6 		117   
#define FONT7 		118   
#define FONT8 		119   
#define FONT9 		120   
#define FONT10 		121  
#define FONT11 		122  
#define FONT12  	123 
#define CTRL_TAB    0 // brak pomys�u 
#endif
*/
#define TERM_CLS_SCR        1

/*--------------------------KOLORY-------------------------- 
#define TERM_NORMAL         0x0000
#define TERM_BLINK          0x0800//0x0080
#define TERM_HILIGHT        BACKGROUND_RED|BACKGROUND_BLUE//0x0008//BACKGROUND_RED|BACKGROUND_BLUE -- co� nie dzia�� poprawnie
#define TERM_INVERSE        0x0000
#define TERM_FLUSH          0x0000
#define TERM_BLACK          0x0000
#define TERM_BLUE           FOREGROUND_BLUE
#define TERM_GREEN          FOREGROUND_GREEN 
#define TERM_CYAN           FOREGROUND_GREEN|FOREGROUND_BLUE 
#define TERM_RED            FOREGROUND_RED
#define TERM_MAGENTA        FOREGROUND_RED|FOREGROUND_BLUE
#define TERM_YELLOW         FOREGROUND_RED|FOREGROUND_GREEN
#define TERM_WHITE          FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE 
#define TERM_BLACK_BG       0x0000
#define TERM_BLUE_BG        BACKGROUND_BLUE
#define TERM_GREEN_BG       BACKGROUND_GREEN
#define TERM_CYAN_BG        BACKGROUND_GREEN|BACKGROUND_BLUE
#define TERM_RED_BG         BACKGROUND_RED
#define TERM_MAGENTA_BG     BACKGROUND_RED|BACKGROUND_BLUE
#define TERM_YELLOW_BG      BACKGROUND_RED|BACKGROUND_GREEN
#define TERM_WHITE_BG       BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE 
#define INVERSE             2048//0x0040
#define BLINK               TERM_BLINK
#define MTERM_HILIGHT       TERM_HILIGHT 
#define ATTR_A 			    15|64//TERM_WHITE|TERM_HILIGHT|TERM_RED_BG//15|64
/*--------------------------KONIEC--------------------------*/

/*--------------------------POCZ-------------------------- 
#define TERM_NORMAL 	    RGB(0,0,0)
#define TERM_BLINK          RGB(255,255,255)
#define TERM_HILIGHT        TERM_RED|TERM_BLUE
#define TERM_INVERSE        RGB(0,0,0)
#define TERM_FLUSH          RGB(0,0,0)
#define TERM_BLACK          RGB(0,0,0)
#define TERM_BLUE           RGB(0,0,255)
#define TERM_GREEN          RGB(0,255,0)
#define TERM_CYAN           RGB(0,255,255)
#define TERM_RED            RGB(255,0,0)
#define TERM_MAGENTA        RGB(255,0,255)
#define TERM_YELLOW         RGB(255,255,0) 
#define TERM_WHITE          RGB(255,255,255)
#define TERM_BLACK_BG       TERM_BLACK
#define TERM_GREEN_BG       TERM_GREEN
#define TERM_CYAN_BG        TERM_CYAN
#define TERM_RED_BG         TERM_RED
#define TERM_MAGENTA_BG     TERM_MAGENTA
#define TERM_YELLOW_BG      TERM_YELLOW
#define TERM_WHITE_BG       TERM_WHITE
#define TERM_BLUE_BG        TERM_BLUE
#define INVERSE             OPAQUE//RGB(0,0,0)//0x0000
#define BLINK               OPAQUE//RGB(255,0,0)//128// ???
#define MTERM_HILIGHT       TERM_HILIGHT
#define ATTR_A              TERM_WHITE|MTERM_HILIGHT|TERM_RED_BG|TERM_FLUSH
/*--------------------------KONIEC--------------------------*/

#ifdef _DOS_WC
 #define HILIGHT 0x0008
 #define TERM_BLINK          BLINK
#else
 #define TERM_BLINK        0x0800
 #define HILIGHT 0x0038
#endif
 #define TERM_HILIGHT       HILIGHT
 #define ATTR_A TERM_WHITE|HILIGHT|TERM_RED_BG //0x043f
 #define MTERM_HILIGHT TERM_HILIGHT /*0*/

#ifndef _NCURSES_
/* Function declarations - not needed for ncurses as they're in term_ncurses.h */
shint term_printf(shint y, shint x, unsigned int attr, char *format,...);
shint term_type(shint y, shint x, char *text, shint len, unsigned int attr);
void term_cur(int y, int x);
int term_save_image(int y0, int x0, char *bufor, int dlug);
void term_restore_image(int y0, int x0, char *bufor, int dlug);
void term_flush(void);
void term_clear(int co);
void term_color(int attr);
#endif
#define TermColor(attr)\
 {shint textkol, bgkol, cc, l, i;\
	textkol=attr&0x00ff;\
	bgkol=attr&0xff00;  bgkol=bgkol>>8;\
	if((attr&INVERSE)==INVERSE)\
	 {cc=bgkol; bgkol=textkol&(~INVERSE); textkol=cc;}\
	textcolor(textkol); textbackground(bgkol);\
 }
void term_fill(int attr);
/* Following block defines alternate key codes - only for non-DOS/non-ncurses platforms */
#if !defined(_DOS_) && !defined(_NCURSES_)
#define GETchar GET_char
#define Malloc malloc
#define DOWN     169
#define BBS      164
#define FFR      166
#define UP       161
#define LEFT     164
#define PgUp     162
#define Home     160
#define End      168
#define KPDMINUS 163
#define KPDPLUS  167
#define PgDn     170
#define Ins      171
#define Del      172
#define Esc      27
#define CR       13
#define BS       127
#define SPEC     255
#define TAB        9
#define SHIFT_TAB 128
#define FONT1    129
#define FONT2    130
#define FONT3    131
#define FONT4    132
#define FONT5    133
#define FONT6    134
#define FONT7    135
#define FONT8    136
#define FONT9    137
#define FONT10   138
#define FONT11   139
#define FONT12   140
#define CTRL_TAB 159
#define INVERSE TERM_INVERSE
/*#define BLINK TERM_HILIGHT */
#define ATTR_A TERM_WHITE|MTERM_HILIGHT|TERM_RED_BG|TERM_FLUSH
#define MTERM_HILIGHT TERM_HILIGHT /*0*/
#endif /* End of alternate key codes block */
#define _GRAFIKA_ 31234
#define LHELP 26
#define EOS 0
#define NL '\n'
#define NLCR 10 /*'\n'*/
/* ------------------------------------------------------------------ */
#define UNDO       1126
#define UNDO_END   1127
#define ZAP_OBOWIAZK  7
#define ZAPIS_DOWOLNY 0
#define ZAPIS_KOLEJNYCH_RAPORTOW 1
#define ZAPIS_WSZYSTKICH_RAZEM   2
#define WSZYSTKIE_REKORDY   -1
#define ZAP_MIN 2
struct lista {char *adr_nast; int size; shint rekord;};
shint mscanf(char *stos,...);
shint GET_char(void);
shint Get_Char(void);
shint bscanf(char *text, unsigned int attrtx, char *stos,...);
shint czytaj_we(char *monit, char *text, shint lz0, shint liczba_znakow);
shint podzial_na_slowa(char *text, char *slowa[], shint max_l_slow, char f,
                     char male);
shint slowa(char *monit,char text[], shint liczba_znakow,char *slowo[],
          shint max_l_slow, char opcje);
shint podzial_na_slowa(char *text,char *slowa[], shint max_l_slow, char f,
                     char male);
char *szukaj(char *text, char znak);
char *szukaj_konca(char *slowo, char f, char male);
char *szukaj_slowa(char *text, char f);
char znaki_liczby(char *slowo, char kropka);
char mgetch(char *monit);
shint Term_Typef(shint y, shint x, char *text, shint lenth, unsigned int attr);
void mAbort(void);
void bip(char mod);
void yplus(void), xplus();
shint m_wherex(void);
shint m_wherey(void);
void m_gotoxy(shint x, shint y);
void Okno(shint y0, shint x0, shint ymax, shint xmax, unsigned int attr);
void set_attr(unsigned int attr);
void clr_Okno(void);
void wewy_abort_off(void);
void wewy_abort_on(void);
void Clr_eol(void);
shint wpis_float(signed char wpis, shint y, shint x0, shint size,
                 unsigned int attr_d, unsigned int attr,
                 unsigned int attrtx, float *wart, char precyzja,
                 char *monit_pocz, float w_min, float w_max,
                 char *format_ogr, float baza, float abs_min, float abs_max);

/* ---------------------- Biblioteka wejsc i wyjsc ---------------------- */
shint okno_float(signed char wpis, shint y, shint x0, shint size,
                 unsigned int attr_d, unsigned int attr,
                 unsigned int attrtx, float *wart, char precyzja,
                                 char *monit_pocz, float w_min, float w_max);
shint okno_tabl_float(signed char wpis, shint y, shint x0, char *monit_pocz,
                shint ind_min, shint ind_max, float w_min, float w_max,
                unsigned int attr_d, unsigned int attr,
                unsigned int attrtx, char *format_tabl,
                shint *index, float tabl[], shint dim, shint L_kol,
                shint size, char prec);
shint okno_ogr_wzgl(signed char wpis, shint y, shint x0, shint size,
                 unsigned int attr_d, unsigned int attr,
                 unsigned int attrtx, float *wart, char precyzja,
                 char *monit_pocz, float w_min, float w_max,
                 char *format, float baza, float abs_min, float abs_max);
shint okno_float_wzgl(signed char wpis, shint y, shint x0, shint size,
                 unsigned int attr_d, unsigned int attr,
                 unsigned int attrtx, float *wart, char precyzja,
                 char *monit_pocz, float w_min, float w_max, float baza);
shint okno_tabl_float_wzgl(signed char wpis, shint y, shint x0, char *monit_pocz,
                shint ind_min, shint ind_max, float w_min, float w_max,
                unsigned int attr_d, unsigned int attr,
                    unsigned int attrtx, char *format_tabl,
                shint *index, float tabl[], shint dim, shint size, char prec,
                float baza);
shint okno_double(signed char wpis,shint y, shint x0, shint size,
                 unsigned int attr_d, unsigned int attr,
                 unsigned int attrtx, double *wart, char precyzja, char *mon,
                 double w_min, double w_max);
shint okno_int(signed char wpis, shint y, shint x0, shint size,
               unsigned int attr_d, unsigned int attr,
               unsigned int attrtx, shint *wart, char *monit,
               shint w_min, shint w_max);
shint okno_int_menu(signed char wpis, shint y, shint x0, shint size,
               unsigned int attr_d, unsigned int attr,
                           unsigned int attrtx, shint *wart, char *monit_pocz,
               shint w_min, shint w_max, shint l_poz_men, char **menu);
shint okno_tabl_int(shint wpis, shint y, shint x0, char *monit_pocz,
               shint ind_min, shint ind_max, shint w_min, shint w_max,
               unsigned int attr_d, unsigned int attr, unsigned int attrtx,
               char *format_tabl, shint *index, shint *tabl, shint dim,
               shint size, shint l_poz_menu, char **menu);
shint okno_text(signed char wpis, shint y, shint x0, shint size,
               unsigned int attr_d, unsigned int attr,
	       unsigned int attrtx, char *text, char *monit, char **help,
	       shint l_opcji);

shint okno_text_menu(signed char wpis, shint y, shint x0, shint size,
	       unsigned int attr_d, unsigned int attr,
	       unsigned int attrtx, char *string, char *monit, void *R, 
        char **help, shint l_opcji, shint skok);

shint okno_znak(signed char wpis, shint y, shint x0,
                unsigned int attr_d, unsigned int attr,
                unsigned int attrtx, char *znak, char *monit_pocz,
                                char opcje[], char *objasn[], shint liczba_obj);
shint okno_grupy(signed char wpis, shint y, shint x0, shint size,
               unsigned int attr_d, unsigned int attr,
               unsigned int attrtx, shint *wart, char *monit_pocz,
               shint w_min, shint w_max, shint skok, void *R, char **help);
shint okno_tabl_grup(signed char wpis, shint y, shint x0, char *monit_pocz,
               shint ind_min, shint ind_max, shint w_min, shint w_max,
               unsigned int attr_d, unsigned int attr, unsigned int attrtx,
               char *format_tabl, shint *index, shint *tabl, shint dim,
	               shint size, shint skok, char **help);
shint okno_tabl_znak(signed char wpis, shint y, shint x0,
                unsigned int attr_d, unsigned int attr,
                unsigned int attrtx, shint *index, char *tabl, shint dim,
                char *monit_pocz, shint ind_min, shint ind_max, char opcje[],
                                 char *format_tabl, char *objasn[], shint liczba_obj);
shint okno_menu(char *opcje[], shint kmax, shint opcja0,
                unsigned int attr, unsigned int attro,
                shint yp, shint xp, shint xm, char *tytul, char ramka);
void m_gotoxy(shint y, shint x);
shint m_wherex(void);
shint m_wherey(void);
void xplus(void);
/* ------------------- budowa blankietow ------------------------------- */
shint otworz_blankiet(signed char kolor, signed char ramka, shint y_lewy_gorny,
                      shint x_lewy_gorny, shint y_prawy_dolny,
                      shint x_prawy_dolny, signed char kod_raportu,
                      shint nr_rekordu, char czynny);
signed char zamknij_blankiet(shint *nr_rekordu);
void reset_blankiet(shint nr_rekordu);
shint dana_float(shint y, shint x0, char *monit, float *w_min, float *w_max,
                 float *wart, shint size, char precyzja, shint ochrona,
                                 char kod_animacji);
shint dana_ogr_wzgl(shint y, shint x, char *monit, float *w_min, float *w_max,
                 float *wart, shint size, char precyzja, shint ochrona,
                 char kod_animacji, char *format, float *baza,
                 float *abs_min, float *abs_max);
shint dana_float_wzgl(shint y, shint x, char *monit, float *w_min, float *w_max,
                 float *wart, shint size, char precyzja, shint ochrona,
                 char kod_animacji, float *baza);
shint dana_ogr_wzgl_dec(shint y, shint x, char *monit, float *w_min, float *w_max,
                 float *wart, shint size, char precyzja, shint ochrona,
                 char kod_animacji, char *format, float *baza,
                 float *abs_min, float *abs_max, shint kod_decyzji);
shint dana_float_wzgl_dec(shint y, shint x, char *monit, float *w_min, float *w_max,
                 float *wart, shint size, char precyzja, shint ochrona,
                 char kod_animacji, float *baza, shint kod_decyzji);
shint tabl_float(shint y, shint x, char *monit, shint ind_min, shint ind_max,
                float *w_min, float *w_max, char *format_tabl,
                float *tablica, shint dim, shint L_kol, shint size,
                shint precyzja, shint ochrona, char kod_animacji);
shint tabl_float_dec(shint y, shint x, char *monit, shint ind_min, shint ind_max,
                float *w_min, float *w_max, char *format_tabl,
                float *tablica, shint dim, shint L_kol, shint size,
                shint precyzja, shint ochrona, char kod_animacji, shint kod_decyzji);
shint Tabl_Floatg(shint y, shint x, char *monit, shint ind_min, shint ind_max,
                float *w_min, float *w_max, char *format_tabl,
                shint *index, float *tablica, shint dim, shint L_kol, shint size,
                shint precyzja, shint ochrona, char kod_animacji);
shint tabl_float_wzgl(shint y, shint x0, char *monit, shint ind_min, shint ind_max,
                   float *w_min, float *w_max, char *format_tabl,
                   float *tablica, shint dim, shint size, char precyzja,
                   shint ochrona, char kod_animacji, float *baza);
shint def_float(shint y, shint x, char *monit, shint ind_min, shint ind_max,
               float *w_min, float *w_max, char *format_tabl, 
               shint *index, float *tablica,
               shint dim, shint L_kol, shint size, char precyzja,
               shint ochrona, char kod_animacji, float *baza,
               char *format, float *abs_min, float *abs_max);
shint dana_double(shint y, shint x0, char *monit, double *w_min, double *w_max,
                 double *wart, shint size, char precyzja, shint ochrona,
                 char kod_animacji);
shint dana_int(shint y, shint x0, char *monit, shint *w_min, shint *w_max,
                 shint *wart, shint size, shint ochrona, signed char raport);
shint dana_int_menu(shint y, shint x0, char *monit, shint *w_min, shint *w_max,
                 shint *wart, shint size, shint ochrona, signed char raport,
                 shint l_poz_menu, char **menu);
shint dana_int_menu_dec(shint y, shint x0, char *monit, shint *w_min, shint *w_max,
                 shint *wart, shint size, shint ochrona, signed char raport,
                 shint l_poz_menu, char **menu, shint kod_decyzji);
shint dana_znak(shint y, shint x0, char *monit, char *opcje, char *objasn[],
                                shint liczba_obj, char *znak, shint ochrona);
shint tabl_znak(shint y, shint x0, char *monit, shint ind_min, shint ind_max,
              char *opcje, char *format_tabl, char *tablica, shint dim,
              char *objasn[], shint liczba_obj, shint ochrona);
shint dana_int_menu(shint y, shint x0, char *monit, shint *w_min, shint *w_max,
                 shint *wart, shint size, shint ochrona, signed char raport,
                 shint l_poz_menu, char **menu);
shint dana_decyzyjna(shint y, shint x, char *monit, char *opcje, char *objasn[],
                shint liczba_obj, char *znak, shint ochrona, shint kod_decyzji);
char jaka_decyzja(shint *kod, shint *nr_danej);
void *get_plot_data(shint nr_danej, shint kod, shint *ind_min,
                    shint *ind_max, shint *lkol, shint *item_size);
shint dana_text(shint y, shint x, char *monit, char *string, shint size,
                char *help[], shint l_opcji, shint ochrona);
/* ---------- Uwaga !! 1sza w linii grupa MUSI miec spacje !!!! ------ */
shint grupa_danych(shint y, shint x0, char *monit, shint *w_min,
                   shint *w_max, shint *wart, shint size, shint ochrona,
                   shint skok, signed char raport, char *hformat,...);
/* -------------------------------------------------------------------- */
shint tabl_grup(shint y, shint x, char *monit, shint ind_min,
               shint ind_max, shint *w_min, shint *w_max, char *format_tabl,
               shint *tablica, shint dim,  shint size, shint ochrona,
               shint skok, signed char raport, char *hformat,...);
shint tabl_grup_dec(shint y, shint x, char *monit, shint ind_min,
            shint ind_max, shint *w_min, shint *w_max, char *format_tabl,
            shint *tablica, shint dim,  shint size, shint ochrona,
            shint skok, signed char raport, shint kod_dec, char *hformat,...);
shint tabl_int_menu(shint y, shint x, char *monit, shint ind_min,
               shint ind_max, shint *w_min, shint *w_max, char *format_tabl,
               shint *tablica, shint dim,  shint size, shint ochrona,
               signed char raport, shint l_poz_menu, char *menu[]);

shint tabl_int_menu_dec(shint y, shint x, char *monit, shint ind_min,
           shint ind_max, shint *w_min, shint *w_max, char *format_tabl,
           shint *tablica, shint dim,  shint size, shint ochrona,
           signed char raport, shint l_poz_menu, char *menu[], shint kod_dec);

shint tabl_int(shint y, shint x, char *monit, shint ind_min,
                           shint ind_max, shint *w_min, shint *w_max, char *format_tabl,
               shint *tablica, shint dim,  shint size,
               shint ochrona, signed char raport);
void reset_blankiet(shint nr_rekordu);
shint otworz_raporty(char nr_op);
void zamknij_raporty(void);
shint zamknij_system_raportow(void);
shint zamkniecie_raportow(char kontr_spojnosci);
typedef shint (*def_blankiet)(shint nr_rekordu, shint ob_pocz,
                   shint ob_konc, shint x_lewy_gorny, shint y_lewy_gorny,
                   shint anim_pid, char *D);
shint def_Report(signed char kod_raportu, shint anim_pid, shint ob_pocz,
                shint ob_konc, shint rozmiar_ob, char *tytul, signed char Kolor,
                signed char Ramka, char *D, shint max_l_danych_blank, char typ_bazy,
                shint (*def_blankiet)(shint nr_rekordu, shint ob_pocz,
                shint ob_konc, shint x_lewy_gorny,
                shint y_lewy_gorny, shint anim_pid,char *D),
                char *(*dane_rap_bl)(shint ob_pocz, shint ob_konc,
                                     shint *rozmiar_ob),
                void (*wpis_danych_bl)(shint ob_pocz, shint ob_konc,
                       char *D, shint rozmiar_ob, char zapis[], char *Kod_op));
shint def_raport(signed char kod_raportu, shint anim_pid, shint ob_pocz,
                shint ob_konc, shint rozmiar_ob, char *tytul, signed char Kolor,
                signed char Ramka, char *D, shint max_l_danych_blank,
                                shint (*def_blankiet)(shint nr_rekordu, shint ob_pocz,
                                      shint ob_konc, shint x_lewy_gorny,
                                      shint y_lewy_gorny, shint anim_pid,
                                      char *D),
                char *(*dane_rap_bl)(shint ob_pocz, shint ob_konc,
                                     shint *rozmiar_ob),
                void (*wpis_danych_bl)(shint ob_pocz, shint ob_konc,
                       char *D, shint rozmiar_ob, char zapis[], char *Kod_op));
def_blankiet czy_jest(signed char kod_raportu, shint *anim_pid, shint *ob_pocz,
                shint *ob_konc, shint *rozmiar_ob, char **tytul,
                signed char *Kolor, signed char *Ramka, char **D);

shint blankiet(signed char *kod_raportu, shint *nr_rekordu, shint x_lewy_gorny, shint y_lewy_gorny,
                          shint y_prawy_dolny, shint x_prawy_dolny, char czynny,
                          signed char kolor, signed char ramka, char *poziom,
                          char *zapis, shint *y_danej);
char *czy_zdefiniowany(signed char kod_raportu, shint *ob_pocz, shint *ob_konc,
                                           shint *rozmiar_ob);
shint rap_blank(char *tytul, signed char *kod_raportu, shint *nr_rekordu);
/* --------------------------------------------------------------------- */
struct okno
 {char *format;
	shint size;
	char Wpis, mod;
	shint xo, xmax;
	shint yo;
	char typ;
	char obowiazk;
	char yplus;
	signed char ochrona;
	signed char decyzja;
	shint *index_helpu;
	shint size_helpu;
	signed char blok_wpisu;
	signed char kod_anim;
	shint ind_min;
	shint ind_max;
	shint index;
	shint dim;
	char *format_tabl;
	unsigned char prec;
	char wpis;
	float *wartosc;
	float *wmin_opcje;
	float *wmax;
	shint proxy;
	shint L_kol;
	void *Rep;
	void *Rep_help;
  char *help[LHELP];
 };
char *wpisz_format(char *format, char *monit);
shint wpis_int(signed char wpis, shint y, shint x0, shint size,
               unsigned int attr_d, unsigned int attr,
                           unsigned int attrtx, char typ, shint *wart, char *monit_pocz,
                           shint w_min, shint w_max, shint skok, char **help);

char *szukaj_formatow(char *format, char typ, char **f_pocz);
char *formaty_tabl(char *f_nazwy, char *monit_pocz, shint *ind_min,
                          shint *ind_max, shint *index, shint *ind_size, char *hash,
                          char *format_tabl, char *opis_danej, shint *dl_zakr,
                          char **f_ind, char **fk_nazwy, shint *dx_ind);
shint wydruk_monitu(shint wpis, char *opis_danej, shint dl_zakr, char *taknie,
                    shint *dltn, char *text, shint *x0, shint dx_ind,
                    shint *dl_menu, shint *size, shint y, shint *xp_naz,
                    shint *xp, shint *txl, unsigned int attrtx, shint w_min,
                    shint w_max, char typ, shint skok, void *R, void *Rh, char *help[]);
shint wpis_indexu(shint y, shint xp_ind, shint ind_size, shint *wart,
                  char *index_helpu, shint size_helpu,
                  shint w_min, shint w_max, char *adr, shint dim,
                  shint xp, shint xmax, char *f_ind, char *format,
                  unsigned int attrtx, unsigned int attr, long int skok,
                  void *R, void *Rh, char **help, char typ, shint dltn, shint W_min,
                  shint W_max, shint lsmax);
shint wpis_tabl_int(signed char wpis, shint y, shint x0, char *monit_pocz,
               shint ind_min, shint ind_max, shint w_min, shint w_max,
               unsigned int attr_d, unsigned int attr, unsigned int attrtx,
               char typ, char *format_tabl, shint *index, char *tabl,
               char *index_helpu, shint size_helpu, shint dim,
               shint size, shint skok, void *R, void *Rh, char **help);
shint wpis_tabl_float(signed char wpis, shint y, shint x0,
              unsigned int attr_d, unsigned int attr,
              unsigned int attrtx, shint *index, char *tabl, shint dim,
              shint L_kol, shint size, char precyzja, char *monit_pocz,
              shint ind_min, shint ind_max, float w_min, float w_max,
              char *format_tabl, char *format_ogr, float baza,
              float abs_min, float abs_max);
shint v_tabl_grup(shint y, shint x, char *monit, shint ind_min,
               shint ind_max, shint *w_min, shint *w_max, char *format_tabl,
               shint *tablica, shint dim,  shint size, shint ochrona,
               shint skok, signed char raport, char *hformat, char **parinfo);
void wyjscie(struct okno *Ok, shint liczba);
void wysylka(shint dana, shint kod, float wartosc, int proxy);
shint disp_dana(signed char wp, shint y, shint x,
                unsigned int attr_d, unsigned int attr_o,
								unsigned int attrtx, struct okno *ok, shint shift, char mod);
shint obsluga_wpisu(shint dana, struct okno *Ok, shint liczba,
                    unsigned int attr_d[], unsigned int attr_o[],
                    unsigned int attrtx[], unsigned int attr_h,
                    unsigned int attr_inf, shint *l_oblig, signed char *raport,
                    shint *wpisano);
shint disp_help(shint y, shint x, unsigned int attrtx, unsigned int attr,
                shint ind, shint Skok, void *R, char *help[], shint max_x);
char czy_format(char *form, shint *cd);
void maluj_ramke(shint x_lewy_gorny, shint yp, shint x_prawy_dolny, shint y_prawy_dolny, unsigned int attr);
void Kolory(signed char kolor, unsigned int *attr, unsigned int *tlo,
           unsigned int *at_dat, unsigned int *at_okno,
           unsigned int *at_wpis, unsigned int *at_inf, unsigned int *at_help);
shint Esc_Tab_wybor_toru(shint Ret, signed char *kod_raportu, shint *nr_rekordu,
               shint ob_pocz, shint ob_konc, char *zapis, char *poziom,
               shint *y_danej);
char *czy_jest_blankiet(signed char kod_raportu, shint *anim_pid, shint *nr_ob,
                      shint *ob_pocz, shint *ob_konc, shint *rozmiar_ob,
                      char **tytul, signed char *Kolor, signed char *Ramka,
                      char czynny, shint yp, shint xp, shint ym, shint xm);
char *dane_oryginalne(shint nr_rekordu, char *tytul, char **d_rek,
                      shint *rozmiar, char **zmiany);

char czy_byly_zmiany(void);
char *dane_raportowanego_rekordu(shint (*def_blank)(shint nr_ob, shint ob_pocz,
                    shint ob_konc, shint xp, shint yp, shint anim_pid,
                    char *d), shint *nr_rekordu);
shint wpis_koment(shint yo, shint xo, unsigned int attrtx, char *text);
shint dana_koment(shint yo, shint xo, char *format, ...);
shint okno_help(shint Yp, shint Xp, shint Xm, unsigned int attr, unsigned int attr_o,
              char typ, shint w_min, shint w_max, shint W_min, shint W_max,
              char *index_help, shint size_help, shint size, shint skok, void *R, void *Rh,
              char *help[], signed char ramka, char *tabl, shint dim,
              char *f_nazwy, char *f_ind, char *fk_nazwy);
shint wydruk_indexu(shint y, shint xp_naz, unsigned int attr,
                    unsigned int attrtx, char *f_nazwy, char *f_ind,
                    shint index, char *fk_nazwy);
shint objasn_grupy(shint y, shint x, shint w_min, shint w_max, shint wartn,
									          unsigned int attr, unsigned int attrtx, long int skok,
                   shint dltn, void *R, char **help);
void def_haslo(char *hasl, char *kod);
void przepisz(char *pod_adr, char *z_adr, shint ile);
shint ustaw_uprawnienia(char *tytul, char potrzebne_uprawn, char nr_oper);
void zapis_zwykly(void);
void haslo_nieaktywne(void);
void zapis_bezwzgledny(void);
void haslo_aktywne(void);
void sledzenie(void);
void sledzenie_off(void);
void ustaw_zapis(shint nr_rekordu, char zapis);
shint dana_float_dec(shint y, shint x, char *monit, float *w_min,
                 float *w_max, float *wart, shint size, char precyzja,
                 shint ochrona, char kod_animacji, shint kod_decyzji);
shint dana_double_dec(shint y, shint x, char *monit, double *w_min,
                 double *w_max, double *wart, shint size, char precyzja,
                 shint ochrona, char kod_animacji, shint kod_decyzji);
shint dana_text_dec(shint y, shint x, char *monit, char *string, shint size,
		char **help, shint l_opcji, shint ochrona, shint kod_decyzji);
shint dana_text_menu_rek(shint y, shint x, char *monit, char *string, shint size,
		     shint l_opcji, shint ochrona,
		     shint kod_raportu, char *hformat,...);
shint dana_text_menu(shint y, shint x, char *monit, char *string, shint size,
		     shint l_opcji, shint ochrona, long int skok, char *hformat,...);
shint dana_int_dec(shint y, shint x, char *monit, shint *w_min, shint *w_max,
                   shint *wart, shint size, shint ochrona,
                   signed char raport, shint kod_decyzji);
shint tabl_grup_danych(shint y, shint x, char *monit, shint *w_min,
                   shint *w_max, shint *wart, char *index_helpu,
                   shint size_helpu, shint size, shint ochrona,
                   shint skok, signed char raport, char *hformat,...);
shint grupa_danych_dec(shint y, shint x, char *monit, shint *w_min,shint *w_max,
                   shint *wart, shint size, shint ochrona, shint skok,
                   signed char raport, char kod_dec, char *hformat,...);
shint grupa_posredn_danych_dec(shint y, shint x, char *monit, shint *w_min,
                   shint *w_max, shint *wart, char *index_helpu,
                   shint size_helpu, shint size, shint ochrona,
                   shint skok, signed char raport, shint kod_dec,
                   char *hformat,...);
shint grupa_rap_posredn_danych_dec(shint y, shint x, char *monit, shint *w_min,
                   shint *w_max, shint *wart, char *index_helpu,
                   shint size_helpu, shint size, shint ochrona,
                   shint skok, signed char raport, shint kod_dec,
                   char *hformat,...);
shint grupa_posredn_danych(shint y, shint x, char *monit, shint *w_min,
                   shint *w_max, shint *wart, char *index_helpu,
                   shint size_helpu, shint size, shint ochrona,
                   shint skok, signed char raport, char *hformat,...);
shint grupa_rap_posredn_danych(shint y, shint x, char *monit, shint *w_min,
                   shint *w_max, shint *wart, char *index_helpu,
                   shint size_helpu, shint size, shint ochrona,
                   shint skok, signed char raport, char *hformat,...);
shint grupa_dec_posredn_danych(shint y, shint x, char *monit, shint *w_min,
           shint *w_max, shint *wart, char *index_helpu,
           shint size_helpu, shint size, shint ochrona,
           shint skok, signed char raport, shint kod_dec, char *hformat,...);
shint tabl_grup_dec_posredn(shint y, shint x, char *monit, shint ind_min,
            shint ind_max, shint *w_min, shint *w_max, char *format_tabl,
            shint *tablica, shint dim,  char *index_helpu,
            shint size_helpu, shint size, shint ochrona,
            shint skok, signed char raport, shint kod_dec, char *hformat,...);
shint tabl_grup_posredn(shint y, shint x, char *monit, shint ind_min,
            shint ind_max, shint *w_min, shint *w_max, char *format_tabl,
            shint *tablica, shint dim,  char *index_helpu,
            shint size_helpu, shint size, shint ochrona,
            shint skok, signed char raport, char *hformat,...);

char *tablica_zapisu(shint kod_raportu);
char *stare_dane(shint kod_raportu, shint nr_rekordu);
void odnow_blankiet(shint kod_raportu, shint nr_rekordu);
void zapis_zblokowany(char tryb_zapisu);
void wpis_danych_bez_potwierdz(void);
void wpis_danych_z_potwierdz(void);
void przelacz_potwierdzanie(void);
void wylacz_potwierdzanie(void);
void ustaw_wybor_na_ost_danej(void);
void ustaw_wybor_normalnie(void);
void ustaw_wybor_na_danej(shint nr);
void rekord_danych_do_naglowka(shint rekord_nr);
void *calloc_rep_mem(long int size, shint kod_raportu, shint rekord, char *txt);
struct lista *szukaj_adr_alloc(shint kod_raportu, char *txt,
															 char *co, char *adr, shint *liczba);
void *free_rep_mem(void *adres, shint kod_raportu, shint rekord, char *txt);
void *realloc_rep_mem(void *adres, long int size, shint kod_raportu,
											shint rekord, char *txt);
void tytul_raportu(shint kod_raportu, char *tytul);
void wpisz_rek_konc_raportu(shint kod_raportu, shint rek_pocz, shint rek_konc);
void wpisz_adr_danych_raportu(shint kod_raportu, char *D);
char sprawdz_znak(char zn, char opcje[], shint dlopt, char *sukces, shint *iopt);
shint czytaj_has(char *text, shint lz0, shint liczba_znakow);
shint Zapisz_raport(shint kod_raportu);
shint komunikat(shint y, shint x, unsigned int attr, char *format,...);
shint komun_text(shint y, shint x, char *bufor, shint l, unsigned int attr);
void begin_scroll(shint dana_nr);
shint czy_mozna_nowe_wykresy(void *(*akcja_grafik)(shint typ_kom, shint kto, 
                                   char *pdane, char *Text,
                                   void *(*Dane_Wykr)(void *Rd), 
                                   void *R));
shint update_wewy_grafs(void *RGn, void *RGs, shint report,
             void *(*akcja_grafik)(shint typ_kom, shint kto, 
                                   char *pdane, char *Text,
                                   void *(*Dane_Wykr)(void *Rd), 
                                   void *R));
shint sygnal_grafiki(pid_t pid, void *D);
void zamknij_grafiki(shint report); 
void zamknij_rap_kod(shint kod_raportu);
/* ============== */
shint Zapisz_raport(shint kod_raportu);
void setcursor(unsigned int shape);
void setalarm(void);
void clearalarm(void);
shint restore_var(shint buf[]);
shint save_var(shint buf[]);
char *ustal_adres_rek(shint kod_raportu, shint rek_no);
char *podaj_adres_rap(shint kod_raportu);
void ustaw_rek_max_raportu(shint (*def_blank)(shint nr_ob, shint ob_pocz,
										shint ob_konc, shint xp, shint yp, shint anim_pid,
										char *d), shint rek_max);
void *alloc_list(void *(*L), long int size);
void *free_list(void *(*L), void *lf);
long int ustal_skok_adr(void *Rh, int nr, shint skok, char *err);
shint dana_rekord_str(shint y, shint x0, char *monit, shint *w_min,
                   shint *w_max, shint *wart, shint size, shint ochrona,
                   shint kod_raportu, signed char raport, char *hformat,...);
shint dana_rekord_posredn_str(shint y, shint x, char *monit, shint *w_min,
                   shint *w_max, shint *wart, char *index_helpu,
									          shint kod_raportu_helpu, shint size, shint ochrona,
                   shint kod_raportu, signed char raport, char *hformat,...);
shint dana_rekord_str_dec(shint y, shint x, char *monit, shint *w_min,shint *w_max,
                   shint *wart, shint size, shint ochrona, shint kod_raportu,
                   signed char raport, char kod_dec, char *hformat,...);
shint dana_rekord_posredn_str_dec(shint y, shint x, char *monit, shint *w_min,
                   shint *w_max, shint *wart, char *index_helpu,
                   shint kod_raportu_helpu, shint size, shint ochrona,
                   shint kod_raportu, signed char raport, char kod_dec,
                   char *hformat,...);
shint GETchar(void);
shint okno_menu_rekordow(char *Menu[], shint w_max, shint opcja0,
								unsigned int attr, unsigned int attr_o,
								shint Yp, shint Xp, shint Xm, char *tytul, char ramka,
								char *hformat, ...);
void animuj_blank(shint lzd, void *ptrs[]);
shint kod_rap_aktual_blank(void);
int otworz_graf_blank(shint Xw_min, shint Yw_min, shint Xw_max,
										shint Yw_max, shint Yz_max, unsigned int Attr);
shint Endgraph(void);
shint wczytaj_int(shint y, shint x, char *monit, shint size, shint w_def,
								 shint w_min, shint w_max,
								 unsigned int attr_monit, unsigned int attr_wpis);
float wczytaj_float(shint y, shint x, char *monit, shint size, shint prec,
										float w_def, float w_min, float w_max,
										unsigned int attr_monit, unsigned int attr_wpis);
char wczytaj_char(shint y, shint x, char *monit, unsigned char w_def,
										unsigned int attr_monit, unsigned int attr_wpis);
char wczytaj_string(shint y, shint x, char *monit, char *text, shint size,
										unsigned int attr_monit, unsigned int attr_wpis);
shint monit_textowy(shint yp, shint xp, unsigned int attr, char *text);

#endif
