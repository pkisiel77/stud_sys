/*####################################################################
  #                  PAMIETAJ !!! w procedurach wewy                 #
  #      WSZYSTKIE !!! ADRESOWE ZMIENNE !!! MUSZA BYC STATIC !!!!!!  #
  #                ograniczenia i liczby opcji tez !!!               #  
  ####################################################################    */
// -- Uwaga !! 1sza w linii grupa_danych MUSI miec spacje w formacie !!!!
#ifndef _WEWYBL_H_INCLUDE
#define _WEWYBL_H_INCLUDE
#include <stdarg.h>
#define BAZA_SPOJNA 0
#define BAZA_WEKTOR 1
#define BAZA_LISTOW 2
#if defined(_DOS_) || defined(_NCURSES_)
int Get_Char(void);
int Get_char(void);
#define GETchar Get_char
// --------------------------KLAWIATURA-------------------------- !!!
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

#define TERM_CLS_SCR        1

#ifdef _DOS_WC
#define HILIGHT 0x0008
#define TERM_BLINK          BLINK
#else
// #define TERM_BLINK        0x0800
// #define HILIGHT 0x0038
#endif
#define TERM_HILIGHT       HILIGHT
#define ATTR_A TERM_WHITE|HILIGHT|TERM_RED_BG //0x043f
#define MTERM_HILIGHT TERM_HILIGHT /*0*/

#ifndef _NCURSES_
/* Function declarations - not needed for ncurses as they're in term_ncurses.h */
int term_printf(int y, int x, unsigned int attr, char* format, ...);
int term_type(int y, int x, char* text, int len, unsigned int attr);
void term_cur(int y, int x);
int term_save_image(int y0, int x0, char* bufor, int dlug);
void term_restore_image(int y0, int x0, char* bufor, int dlug);
void term_flush(void);
void term_clear(int co);
void term_color(int attr);
#endif
#define TermColor(attr)\
 {int textkol, bgkol, cc, l, i;\
	textkol=attr&0x00ff;\
	bgkol=attr&0xff00;  bgkol=bgkol>>8;\
	if((attr&INVERSE)==INVERSE)\
	 {cc=bgkol; bgkol=textkol&(~INVERSE); textkol=cc;}\
	textcolor(textkol); textbackground(bgkol);\
 }
void term_fill(int attr);
/* The following block defines alternate key codes - only for non-DOS/non-ncurses platforms */
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

struct lista
{
    char* adr_nast;
    int size;
    int rekord;
};

int mscanf(char* stos, ...);
int GET_char(void);
int Get_Char(void);
int bscanf(char* text, unsigned int attrtx, char* stos, ...);
int czytaj_we(char* monit, char* text, int lz0, int liczba_znakow);
int podzial_na_slowa(char* text, char* slowa[], int max_l_slow, char f,
                     char male);
int slowa(char* monit, char text[], int liczba_znakow, char* slowo[],
          int max_l_slow, char opcje);
int podzial_na_slowa(char* text, char* slowa[], int max_l_slow, char f,
                     char male);
char* szukaj(char* text, char znak);
char* szukaj_konca(char* slowo, char f, char male);
char* szukaj_slowa(char* text, char f);
char znaki_liczby(char* slowo, char kropka);
char mgetch(char* monit);
int Term_Typef(int y, int x, char* text, int lenth, unsigned int attr);
void mAbort(void);
void bip(char mod);
void yplus(void), xplus();
int m_wherex(void);
int m_wherey(void);
void m_gotoxy(int x, int y);
void Okno(int y0, int x0, int ymax, int xmax, unsigned int attr);
void set_attr(unsigned int attr);
void clr_Okno(void);
void wewy_abort_off(void);
void wewy_abort_on(void);
void Clr_eol(void);
int wpis_float(signed char wpis, int y, int x0, int size,
               unsigned int attr_d, unsigned int attr,
               unsigned int attrtx, float* wart, char precyzja,
               char* monit_pocz, float w_min, float w_max,
               char* format_ogr, float baza, float abs_min, float abs_max);

/* ---------------------- Biblioteka wejsc i wyjsc ---------------------- */
int okno_float(signed char wpis, int y, int x0, int size,
               unsigned int attr_d, unsigned int attr,
               unsigned int attrtx, float* wart, char precyzja,
               char* monit_pocz, float w_min, float w_max);
int okno_tabl_float(signed char wpis, int y, int x0, char* monit_pocz,
                    int ind_min, int ind_max, float w_min, float w_max,
                    unsigned int attr_d, unsigned int attr,
                    unsigned int attrtx, char* format_tabl,
                    int* index, float tabl[], int dim, int L_kol,
                    int size, char prec);
int okno_ogr_wzgl(signed char wpis, int y, int x0, int size,
                  unsigned int attr_d, unsigned int attr,
                  unsigned int attrtx, float* wart, char precyzja,
                  char* monit_pocz, float w_min, float w_max,
                  char* format, float baza, float abs_min, float abs_max);
int okno_float_wzgl(signed char wpis, int y, int x0, int size,
                    unsigned int attr_d, unsigned int attr,
                    unsigned int attrtx, float* wart, char precyzja,
                    char* monit_pocz, float w_min, float w_max, float baza);
int okno_tabl_float_wzgl(signed char wpis, int y, int x0, char* monit_pocz,
                         int ind_min, int ind_max, float w_min, float w_max,
                         unsigned int attr_d, unsigned int attr,
                         unsigned int attrtx, char* format_tabl,
                         int* index, float tabl[], int dim, int size, char prec,
                         float baza);
int okno_double(signed char wpis, int y, int x0, int size,
                unsigned int attr_d, unsigned int attr,
                unsigned int attrtx, double* wart, char precyzja, char* mon,
                double w_min, double w_max);
int okno_int(signed char wpis, int y, int x0, int size,
             unsigned int attr_d, unsigned int attr,
             unsigned int attrtx, int* wart, char* monit,
             int w_min, int w_max);
int okno_int_menu(signed char wpis, int y, int x0, int size,
                  unsigned int attr_d, unsigned int attr,
                  unsigned int attrtx, int* wart, char* monit_pocz,
                  int w_min, int w_max, int l_poz_men, char** menu);
int okno_tabl_int(int wpis, int y, int x0, char* monit_pocz,
                  int ind_min, int ind_max, int w_min, int w_max,
                  unsigned int attr_d, unsigned int attr, unsigned int attrtx,
                  char* format_tabl, int* index, int* tabl, int dim,
                  int size, int l_poz_menu, char** menu);
int okno_text(signed char wpis, int y, int x0, int size,
              unsigned int attr_d, unsigned int attr,
              unsigned int attrtx, char* text, char* monit, char** help,
              int l_opcji);

int okno_text_menu(signed char wpis, int y, int x0, int size,
                   unsigned int attr_d, unsigned int attr,
                   unsigned int attrtx, char* string, char* monit, void* R,
                   char** help, int l_opcji, int skok);

int okno_znak(signed char wpis, int y, int x0,
              unsigned int attr_d, unsigned int attr,
              unsigned int attrtx, char* znak, char* monit_pocz,
              char opcje[], char* objasn[], int liczba_obj);
int okno_grupy(signed char wpis, int y, int x0, int size,
               unsigned int attr_d, unsigned int attr,
               unsigned int attrtx, int* wart, char* monit_pocz,
               int w_min, int w_max, int skok, void* R, char** help);
int okno_tabl_grup(signed char wpis, int y, int x0, char* monit_pocz,
                   int ind_min, int ind_max, int w_min, int w_max,
                   unsigned int attr_d, unsigned int attr, unsigned int attrtx,
                   char* format_tabl, int* index, int* tabl, int dim,
                   int size, int skok, char** help);
int okno_tabl_znak(signed char wpis, int y, int x0,
                   unsigned int attr_d, unsigned int attr,
                   unsigned int attrtx, int* index, char* tabl, int dim,
                   char* monit_pocz, int ind_min, int ind_max, char opcje[],
                   char* format_tabl, char* objasn[], int liczba_obj);
int okno_menu(char* opcje[], int kmax, int opcja0,
              unsigned int attr, unsigned int attro,
              int yp, int xp, int xm, char* tytul, char ramka);
void m_gotoxy(int y, int x);
int m_wherex(void);
int m_wherey(void);
void xplus(void);
/* ------------------- budowa blankietow ------------------------------- */
int otworz_blankiet(signed char kolor, signed char ramka, int y_lewy_gorny,
                    int x_lewy_gorny, int y_prawy_dolny,
                    int x_prawy_dolny, signed char kod_raportu,
                    int nr_rekordu, char czynny);
signed char zamknij_blankiet(int* nr_rekordu);
void reset_blankiet(int nr_rekordu);
int dana_float(int y, int x0, char* monit, float* w_min, float* w_max,
               float* wart, int size, char precyzja, int ochrona,
               char kod_animacji);
int dana_ogr_wzgl(int y, int x, char* monit, float* w_min, float* w_max,
                  float* wart, int size, char precyzja, int ochrona,
                  char kod_animacji, char* format, float* baza,
                  float* abs_min, float* abs_max);
int dana_float_wzgl(int y, int x, char* monit, float* w_min, float* w_max,
                    float* wart, int size, char precyzja, int ochrona,
                    char kod_animacji, float* baza);
int dana_ogr_wzgl_dec(int y, int x, char* monit, float* w_min, float* w_max,
                      float* wart, int size, char precyzja, int ochrona,
                      char kod_animacji, char* format, float* baza,
                      float* abs_min, float* abs_max, int kod_decyzji);
int dana_float_wzgl_dec(int y, int x, char* monit, float* w_min, float* w_max,
                        float* wart, int size, char precyzja, int ochrona,
                        char kod_animacji, float* baza, int kod_decyzji);
int tabl_float(int y, int x, char* monit, int ind_min, int ind_max,
               float* w_min, float* w_max, char* format_tabl,
               float* tablica, int dim, int L_kol, int size,
               int precyzja, int ochrona, char kod_animacji);
int tabl_float_dec(int y, int x, char* monit, int ind_min, int ind_max,
                   float* w_min, float* w_max, char* format_tabl,
                   float* tablica, int dim, int L_kol, int size,
                   int precyzja, int ochrona, char kod_animacji, int kod_decyzji);
int Tabl_Floatg(int y, int x, char* monit, int ind_min, int ind_max,
                float* w_min, float* w_max, char* format_tabl,
                int* index, float* tablica, int dim, int L_kol, int size,
                int precyzja, int ochrona, char kod_animacji);
int tabl_float_wzgl(int y, int x0, char* monit, int ind_min, int ind_max,
                    float* w_min, float* w_max, char* format_tabl,
                    float* tablica, int dim, int size, char precyzja,
                    int ochrona, char kod_animacji, float* baza);
int def_float(int y, int x, char* monit, int ind_min, int ind_max,
              float* w_min, float* w_max, char* format_tabl,
              int* index, float* tablica,
              int dim, int L_kol, int size, char precyzja,
              int ochrona, char kod_animacji, float* baza,
              char* format, float* abs_min, float* abs_max);
int dana_double(int y, int x0, char* monit, double* w_min, double* w_max,
                double* wart, int size, char precyzja, int ochrona,
                char kod_animacji);
int dana_int(int y, int x0, char* monit, int* w_min, int* w_max,
             int* wart, int size, int ochrona, signed char raport);
int dana_int_menu(int y, int x0, char* monit, int* w_min, int* w_max,
                  int* wart, int size, int ochrona, signed char raport,
                  int l_poz_menu, char** menu);
int dana_int_menu_dec(int y, int x0, char* monit, int* w_min, int* w_max,
                      int* wart, int size, int ochrona, signed char raport,
                      int l_poz_menu, char** menu, int kod_decyzji);
int dana_znak(int y, int x0, char* monit, char* opcje, char* objasn[],
              int liczba_obj, char* znak, int ochrona);
int tabl_znak(int y, int x0, char* monit, int ind_min, int ind_max,
              char* opcje, char* format_tabl, char* tablica, int dim,
              char* objasn[], int liczba_obj, int ochrona);
int dana_int_menu(int y, int x0, char* monit, int* w_min, int* w_max,
                  int* wart, int size, int ochrona, signed char raport,
                  int l_poz_menu, char** menu);
int dana_decyzyjna(int y, int x, char* monit, char* opcje, char* objasn[],
                   int liczba_obj, char* znak, int ochrona, int kod_decyzji);
char jaka_decyzja(int* kod, int* nr_danej);
void* get_plot_data(int nr_danej, int kod, int* ind_min,
                    int* ind_max, int* lkol, int* item_size);
int dana_text(int y, int x, char* monit, char* string, int size,
              char* help[], int l_opcji, int ochrona);
/* ---------- Uwaga !! 1sza w linii grupa MUSI miec spacje !!!! ------ */
int grupa_danych(int y, int x0, char* monit, int* w_min,
                 int* w_max, int* wart, int size, int ochrona,
                 int skok, signed char raport, char* hformat, ...);
/* -------------------------------------------------------------------- */
int tabl_grup(int y, int x, char* monit, int ind_min,
              int ind_max, int* w_min, int* w_max, char* format_tabl,
              int* tablica, int dim, int size, int ochrona,
              int skok, signed char raport, char* hformat, ...);
int tabl_grup_dec(int y, int x, char* monit, int ind_min,
                  int ind_max, int* w_min, int* w_max, char* format_tabl,
                  int* tablica, int dim, int size, int ochrona,
                  int skok, signed char raport, int kod_dec, char* hformat, ...);
int tabl_int_menu(int y, int x, char* monit, int ind_min,
                  int ind_max, int* w_min, int* w_max, char* format_tabl,
                  int* tablica, int dim, int size, int ochrona,
                  signed char raport, int l_poz_menu, char* menu[]);

int tabl_int_menu_dec(int y, int x, char* monit, int ind_min,
                      int ind_max, int* w_min, int* w_max, char* format_tabl,
                      int* tablica, int dim, int size, int ochrona,
                      signed char raport, int l_poz_menu, char* menu[], int kod_dec);

int tabl_int(int y, int x, char* monit, int ind_min,
             int ind_max, int* w_min, int* w_max, char* format_tabl,
             int* tablica, int dim, int size,
             int ochrona, signed char raport);
void reset_blankiet(int nr_rekordu);
int otworz_raporty(char nr_op);
void zamknij_raporty(void);
int zamknij_system_raportow(void);
int zamkniecie_raportow(char kontr_spojnosci);
typedef int (*def_blankiet)(int nr_rekordu, int ob_pocz,
                            int ob_konc, int x_lewy_gorny, int y_lewy_gorny,
                            int anim_pid, char* D);
int def_Report(signed char kod_raportu, int anim_pid, int ob_pocz,
               int ob_konc, int rozmiar_ob, char* tytul, signed char Kolor,
               signed char Ramka, char* D, int max_l_danych_blank, char typ_bazy,
               int (*def_blankiet)(int nr_rekordu, int ob_pocz,
                                   int ob_konc, int x_lewy_gorny,
                                   int y_lewy_gorny, int anim_pid, char* D),
               char*(*dane_rap_bl)(int ob_pocz, int ob_konc,
                                   int* rozmiar_ob),
               void (*wpis_danych_bl)(int ob_pocz, int ob_konc,
                                      char* D, int rozmiar_ob, char zapis[], char* Kod_op));
int def_raport(signed char kod_raportu, int anim_pid, int ob_pocz,
               int ob_konc, int rozmiar_ob, char* tytul, signed char Kolor,
               signed char Ramka, char* D, int max_l_danych_blank,
               int (*def_blankiet)(int nr_rekordu, int ob_pocz,
                                   int ob_konc, int x_lewy_gorny,
                                   int y_lewy_gorny, int anim_pid,
                                   char* D),
               char*(*dane_rap_bl)(int ob_pocz, int ob_konc,
                                   int* rozmiar_ob),
               void (*wpis_danych_bl)(int ob_pocz, int ob_konc,
                                      char* D, int rozmiar_ob, char zapis[], char* Kod_op));
def_blankiet czy_jest(signed char kod_raportu, int* anim_pid, int* ob_pocz,
                      int* ob_konc, int* rozmiar_ob, char** tytul,
                      signed char* Kolor, signed char* Ramka, char** D);

int blankiet(signed char* kod_raportu, int* nr_rekordu, int x_lewy_gorny, int y_lewy_gorny,
             int y_prawy_dolny, int x_prawy_dolny, char czynny,
             signed char kolor, signed char ramka, char* poziom,
             char* zapis, int* y_danej);
char* czy_zdefiniowany(signed char kod_raportu, int* ob_pocz, int* ob_konc,
                       int* rozmiar_ob);
int rap_blank(char* tytul, signed char* kod_raportu, int* nr_rekordu);

/* --------------------------------------------------------------------- */
struct okno
{
    char* format;
    int size;
    char Wpis, mod;
    int xo, xmax;
    int yo;
    char typ;
    char obowiazk;
    char yplus;
    signed char ochrona;
    signed char decyzja;
    int* index_helpu;
    int size_helpu;
    signed char blok_wpisu;
    signed char kod_anim;
    int ind_min;
    int ind_max;
    int index;
    int dim;
    char* format_tabl;
    unsigned char prec;
    char wpis;
    float* wartosc;
    float* wmin_opcje;
    float* wmax;
    int proxy;
    int L_kol;
    void* Rep;
    void* Rep_help;
    char* help[LHELP];
};

char* wpisz_format(char* format, char* monit);
int wpis_int(signed char wpis, int y, int x0, int size,
             unsigned int attr_d, unsigned int attr,
             unsigned int attrtx, char typ, int* wart, char* monit_pocz,
             int w_min, int w_max, int skok, char** help);

char* szukaj_formatow(char* format, char typ, char** f_pocz);
char* formaty_tabl(char* f_nazwy, char* monit_pocz, int* ind_min,
                   int* ind_max, int* index, int* ind_size, char* hash,
                   char* format_tabl, char* opis_danej, int* dl_zakr,
                   char** f_ind, char** fk_nazwy, int* dx_ind);
int wydruk_monitu(int wpis, char* opis_danej, int dl_zakr, char* taknie,
                  int* dltn, char* text, int* x0, int dx_ind,
                  int* dl_menu, int* size, int y, int* xp_naz,
                  int* xp, int* txl, unsigned int attrtx, int w_min,
                  int w_max, char typ, int skok, void* R, void* Rh, char* help[]);
int wpis_indexu(int y, int xp_ind, int ind_size, int* wart,
                char* index_helpu, int size_helpu,
                int w_min, int w_max, char* adr, int dim,
                int xp, int xmax, char* f_ind, char* format,
                unsigned int attrtx, unsigned int attr, long int skok,
                void* R, void* Rh, char** help, char typ, int dltn, int W_min,
                int W_max, int lsmax);
int wpis_tabl_int(signed char wpis, int y, int x0, char* monit_pocz,
                  int ind_min, int ind_max, int w_min, int w_max,
                  unsigned int attr_d, unsigned int attr, unsigned int attrtx,
                  char typ, char* format_tabl, int* index, char* tabl,
                  char* index_helpu, int size_helpu, int dim,
                  int size, int skok, void* R, void* Rh, char** help);
int wpis_tabl_float(signed char wpis, int y, int x0,
                    unsigned int attr_d, unsigned int attr,
                    unsigned int attrtx, int* index, char* tabl, int dim,
                    int L_kol, int size, char precyzja, char* monit_pocz,
                    int ind_min, int ind_max, float w_min, float w_max,
                    char* format_tabl, char* format_ogr, float baza,
                    float abs_min, float abs_max);
int v_tabl_grup(int y, int x, char* monit, int ind_min,
                int ind_max, int* w_min, int* w_max, char* format_tabl,
                int* tablica, int dim, int size, int ochrona,
                int skok, signed char raport, char* hformat, va_list* ap);
void wyjscie(struct okno* Ok, int liczba);
void wysylka(int dana, int kod, float wartosc, int proxy);
int disp_dana(signed char wp, int y, int x,
              unsigned int attr_d, unsigned int attr_o,
              unsigned int attrtx, struct okno* ok, int shift, char mod);
int obsluga_wpisu(int dana, struct okno* Ok, int liczba,
                  unsigned int attr_d[], unsigned int attr_o[],
                  unsigned int attrtx[], unsigned int attr_h,
                  unsigned int attr_inf, int* l_oblig, signed char* raport,
                  int* wpisano);
int disp_help(int y, int x, unsigned int attrtx, unsigned int attr,
              int ind, int Skok, void* R, char* help[], int max_x);
char czy_format(char* form, int* cd);
void maluj_ramke(int x_lewy_gorny, int yp, int x_prawy_dolny, int y_prawy_dolny, unsigned int attr);
void Kolory(signed char kolor, unsigned int* attr, unsigned int* tlo,
            unsigned int* at_dat, unsigned int* at_okno,
            unsigned int* at_wpis, unsigned int* at_inf, unsigned int* at_help);
int Esc_Tab_wybor_toru(int Ret, signed char* kod_raportu, int* nr_rekordu,
                       int ob_pocz, int ob_konc, char* zapis, char* poziom,
                       int* y_danej);
char* czy_jest_blankiet(signed char kod_raportu, int* anim_pid, int* nr_ob,
                        int* ob_pocz, int* ob_konc, int* rozmiar_ob,
                        char** tytul, signed char* Kolor, signed char* Ramka,
                        char czynny, int yp, int xp, int ym, int xm);
char* dane_oryginalne(int nr_rekordu, char* tytul, char** d_rek,
                      int* rozmiar, char** zmiany);

char czy_byly_zmiany(void);
char* dane_raportowanego_rekordu(int (*def_blank)(int nr_ob, int ob_pocz,
                                                  int ob_konc, int xp, int yp, int anim_pid,
                                                  char* d), int* nr_rekordu);
int wpis_koment(int yo, int xo, unsigned int attrtx, char* text);
int dana_koment(int yo, int xo, char* format, ...);
int okno_help(int Yp, int Xp, int Xm, unsigned int attr, unsigned int attr_o,
              char typ, int w_min, int w_max, int W_min, int W_max,
              char* index_help, int size_help, int size, int skok, void* R, void* Rh,
              char* help[], signed char ramka, char* tabl, int dim,
              char* f_nazwy, char* f_ind, char* fk_nazwy);
int wydruk_indexu(int y, int xp_naz, unsigned int attr,
                  unsigned int attrtx, char* f_nazwy, char* f_ind,
                  int index, char* fk_nazwy);
int objasn_grupy(int y, int x, int w_min, int w_max, int wartn,
                 unsigned int attr, unsigned int attrtx, long int skok,
                 int dltn, void* R, char** help);
void def_haslo(char* hasl, char* kod);
void przepisz(char* pod_adr, char* z_adr, int ile);
int ustaw_uprawnienia(char* tytul, char potrzebne_uprawn, char nr_oper);
void zapis_zwykly(void);
void haslo_nieaktywne(void);
void zapis_bezwzgledny(void);
void haslo_aktywne(void);
void sledzenie(void);
void sledzenie_off(void);
void ustaw_zapis(int nr_rekordu, char zapis);
int dana_float_dec(int y, int x, char* monit, float* w_min,
                   float* w_max, float* wart, int size, char precyzja,
                   int ochrona, char kod_animacji, int kod_decyzji);
int dana_double_dec(int y, int x, char* monit, double* w_min,
                    double* w_max, double* wart, int size, char precyzja,
                    int ochrona, char kod_animacji, int kod_decyzji);
int dana_text_dec(int y, int x, char* monit, char* string, int size,
                  char** help, int l_opcji, int ochrona, int kod_decyzji);
int dana_text_menu_rek(int y, int x, char* monit, char* string, int size,
                       int l_opcji, int ochrona,
                       int kod_raportu, char* hformat, ...);
int dana_text_menu(int y, int x, char* monit, char* string, int size,
                   int l_opcji, int ochrona, long int skok, char* hformat, ...);
int dana_int_dec(int y, int x, char* monit, int* w_min, int* w_max,
                 int* wart, int size, int ochrona,
                 signed char raport, int kod_decyzji);
int tabl_grup_danych(int y, int x, char* monit, int* w_min,
                     int* w_max, int* wart, char* index_helpu,
                     int size_helpu, int size, int ochrona,
                     int skok, signed char raport, char* hformat, ...);
int grupa_danych_dec(int y, int x, char* monit, int* w_min, int* w_max,
                     int* wart, int size, int ochrona, int skok,
                     signed char raport, char kod_dec, char* hformat, ...);
int grupa_posredn_danych_dec(int y, int x, char* monit, int* w_min,
                             int* w_max, int* wart, char* index_helpu,
                             int size_helpu, int size, int ochrona,
                             int skok, signed char raport, int kod_dec,
                             char* hformat, ...);
int grupa_rap_posredn_danych_dec(int y, int x, char* monit, int* w_min,
                                 int* w_max, int* wart, char* index_helpu,
                                 int size_helpu, int size, int ochrona,
                                 int skok, signed char raport, int kod_dec,
                                 char* hformat, ...);
int grupa_posredn_danych(int y, int x, char* monit, int* w_min,
                         int* w_max, int* wart, char* index_helpu,
                         int size_helpu, int size, int ochrona,
                         int skok, signed char raport, char* hformat, ...);
int grupa_rap_posredn_danych(int y, int x, char* monit, int* w_min,
                             int* w_max, int* wart, char* index_helpu,
                             int size_helpu, int size, int ochrona,
                             int skok, signed char raport, char* hformat, ...);
int grupa_dec_posredn_danych(int y, int x, char* monit, int* w_min,
                             int* w_max, int* wart, char* index_helpu,
                             int size_helpu, int size, int ochrona,
                             int skok, signed char raport, int kod_dec, char* hformat, ...);
int tabl_grup_dec_posredn(int y, int x, char* monit, int ind_min,
                          int ind_max, int* w_min, int* w_max, char* format_tabl,
                          int* tablica, int dim, char* index_helpu,
                          int size_helpu, int size, int ochrona,
                          int skok, signed char raport, int kod_dec, char* hformat, ...);
int tabl_grup_posredn(int y, int x, char* monit, int ind_min,
                      int ind_max, int* w_min, int* w_max, char* format_tabl,
                      int* tablica, int dim, char* index_helpu,
                      int size_helpu, int size, int ochrona,
                      int skok, signed char raport, char* hformat, ...);

char* tablica_zapisu(int kod_raportu);
char* stare_dane(int kod_raportu, int nr_rekordu);
void odnow_blankiet(int kod_raportu, int nr_rekordu);
void zapis_zblokowany(char tryb_zapisu);
void wpis_danych_bez_potwierdz(void);
void wpis_danych_z_potwierdz(void);
void przelacz_potwierdzanie(void);
void wylacz_potwierdzanie(void);
void ustaw_wybor_na_ost_danej(void);
void ustaw_wybor_normalnie(void);
void ustaw_wybor_na_danej(int nr);
void rekord_danych_do_naglowka(int rekord_nr);
void* calloc_rep_mem(long int size, int kod_raportu, int rekord, char* txt);
struct lista* szukaj_adr_alloc(int kod_raportu, char* txt,
                               char* co, char* adr, int* liczba);
void* free_rep_mem(void* adres, int kod_raportu, int rekord, char* txt);
void* realloc_rep_mem(void* adres, long int size, int kod_raportu,
                      int rekord, char* txt);
void tytul_raportu(int kod_raportu, char* tytul);
void wpisz_rek_konc_raportu(int kod_raportu, int rek_pocz, int rek_konc);
void wpisz_adr_danych_raportu(int kod_raportu, char* D);
char sprawdz_znak(char zn, char opcje[], int dlopt, char* sukces, int* iopt);
int czytaj_has(char* text, int lz0, int liczba_znakow);
int Zapisz_raport(int kod_raportu);
int komunikat(int y, int x, unsigned int attr, char* format, ...);
int komun_text(int y, int x, char* bufor, int l, unsigned int attr);
void begin_scroll(int dana_nr);
int czy_mozna_nowe_wykresy(void*(*akcja_grafik)(int typ_kom, int kto,
                                                char* pdane, char* Text,
                                                void*(*Dane_Wykr)(void* Rd),
                                                void* R));
int update_wewy_grafs(void* RGn, void* RGs, int report,
                      void*(*akcja_grafik)(int typ_kom, int kto,
                                           char* pdane, char* Text,
                                           void*(*Dane_Wykr)(void* Rd),
                                           void* R));
int sygnal_grafiki(pid_t pid, void* D);
void zamknij_grafiki(int report);
void zamknij_rap_kod(int kod_raportu);
/* ============== */
int Zapisz_raport(int kod_raportu);
void setcursor(unsigned int shape);
void setalarm(void);
void clearalarm(void);
int restore_var(int buf[]);
int save_var(int buf[]);
char* ustal_adres_rek(int kod_raportu, int rek_no);
char* podaj_adres_rap(int kod_raportu);
void ustaw_rek_max_raportu(int (*def_blank)(int nr_ob, int ob_pocz,
                                            int ob_konc, int xp, int yp, int anim_pid,
                                            char* d), int rek_max);
void* alloc_list(void* (*L), long int size);
void* free_list(void* (*L), void* lf);
long int ustal_skok_adr(void* Rh, int nr, int skok, char* err);
int dana_rekord_str(int y, int x0, char* monit, int* w_min,
                    int* w_max, int* wart, int size, int ochrona,
                    int kod_raportu, signed char raport, char* hformat, ...);
int dana_rekord_posredn_str(int y, int x, char* monit, int* w_min,
                            int* w_max, int* wart, char* index_helpu,
                            int kod_raportu_helpu, int size, int ochrona,
                            int kod_raportu, signed char raport, char* hformat, ...);
int dana_rekord_str_dec(int y, int x, char* monit, int* w_min, int* w_max,
                        int* wart, int size, int ochrona, int kod_raportu,
                        signed char raport, char kod_dec, char* hformat, ...);
int dana_rekord_posredn_str_dec(int y, int x, char* monit, int* w_min,
                                int* w_max, int* wart, char* index_helpu,
                                int kod_raportu_helpu, int size, int ochrona,
                                int kod_raportu, signed char raport, char kod_dec,
                                char* hformat, ...);
int GETchar(void);
int okno_menu_rekordow(char* Menu[], int w_max, int opcja0,
                       unsigned int attr, unsigned int attr_o,
                       int Yp, int Xp, int Xm, char* tytul, char ramka,
                       char* hformat, ...);
void animuj_blank(int lzd, void* ptrs[]);
int kod_rap_aktual_blank(void);
int otworz_graf_blank(int Xw_min, int Yw_min, int Xw_max,
                      int Yw_max, int Yz_max, unsigned int Attr);
int Endgraph(void);
int wczytaj_int(int y, int x, char* monit, int size, int w_def,
                int w_min, int w_max,
                unsigned int attr_monit, unsigned int attr_wpis);
float wczytaj_float(int y, int x, char* monit, int size, int prec,
                    float w_def, float w_min, float w_max,
                    unsigned int attr_monit, unsigned int attr_wpis);
char wczytaj_char(int y, int x, char* monit, unsigned char w_def,
                  unsigned int attr_monit, unsigned int attr_wpis);
char wczytaj_string(int y, int x, char* monit, char* text, int size,
                    unsigned int attr_monit, unsigned int attr_wpis);
int monit_textowy(int yp, int xp, unsigned int attr, char* text);

#endif
