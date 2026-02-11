//#define _QNX
#define _DOS_
#define GRAF
//#define _DOS_WC   /* tylko dla Watcom - NT */
#ifdef _QNX_
#undef _DOS_
#undef _DOS_WC
#undef GRAF
#elif defined(_NCURSES_)
#undef _DOS_
#undef GRAF
#else
#undef _TERM_QNX_
#endif
#define WEWY_MAIN
#include <stdio.h>
//#include <conio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <time.h>
/* extern char wait_for_synch_trig; */
#include "../opcjesys/opcje.h"
extern HWND handle_window_device;
/* -------------------- Headery QNX'a ---------------------------- */
#ifndef _DOS_
#define Get_char GET_char
#define Get_Char GET_char  /* Also define Get_Char for compatibility */
#ifdef _NCURSES_
char real_time = 0;  /* Definition for NCURSES */
#else
extern char real_time;  /* Declaration for QNX */
#endif
/*#include <malloc.h>
#include <sys/dev.h>
#include <sys/qnxterm.h>
#include <sys/console.h>
#include <sys/types.h>
#include <term.h>
#include <signal.h>
#include <sys/qioctl.h>
void exit(int status), clear_cur(char);
#include <process.h> */
#else
//#include <dos.h>
void clear_cur(char);
char real_time;
#define Get_Char GET_char

#ifdef _CBC_
#include <alloc.h>
#else
//#include <malloc.h>
#endif
#endif
//#include "../aj/dim.h"

#include "moje.h"

// extern unsigned int cursor = 0x0A0C, nocursor = 0x2000;

#ifndef CLK_TCK
#define CLK_TCK CLOCKS_PER_SEC
#endif
static time_t lticks_min, Ltickm = 0.2 * CLK_TCK, kbtickm = 0.02 * CLK_TCK; /* klawisz co 0.2sek */

/*
extern int xg_akt, yg_akt;
extern char alarmclock;
// extern unsigned int cursor, nocursor;

extern int x_akt, y_akt;
extern int x_akt0, y_akt0;
extern int x_maxOkna, y_maxOkna;
extern int x_min, y_min;
extern int x_max, y_max;
*/



extern int x_akt0, y_akt0;

extern int x_min, y_min;

//static time_t lticks_min, Ltickm = 0.2 * CLK_TCK, kbtickm = 0.02 * CLK_TCK; /* klawisz co 0.2sek */
void backspace(char* c);
int GETchar(void);
void ustaw_okno_graficzne();
void ramka_graficzna(int yp, int xp, int ym, int xm, unsigned int attr);
char*(*dane_rap_bl)(int ob_pocz, int ob_konc, int* rozmiar_ob);
void (*wpis_danych_bl)(int ob_pocz, int ob_konc, char* D,
                       int rozmiar_ob, char zapis[], char* Kod_op);
void przepisz(char* do_ob, char* z_ob, int ile);
void open_konsole_mouse(int fd_in);
int BUF_com_size = 0, nr_komunik = -1;
void* BUF_com = NULL;
static char wait_for_synch_trig = 0;
int igraf = 0;
/* ================Procedura czytania danych i slow z klawiatury=========*/
/*      Dane wejsciowe:  monit - komentarz do tekstu pisanego na monitor
												 text  - slowa lub dane
												 liczba_znakow - ilosc znakow
												 slowo - tablica slow
												 l_slow- liczba slow
												 f     - = 0 dla slow i danych integer
																 = 1 dla danych float                 */

/*                        Procedura SLOWA()
	 ============== Czytanie znakow i podzial na slowa  ============== */
#define L_SEP 8
#include "wewybl.h"
char separator[L_SEP] = {' ', ',', '!', ':', ';', '?', EOS, '.'};
#include "WEWY1.H"

int (*odbior_komunik)(void* b, pid_t a) = NULL;
#undef buttons
/* ---------------------- struktura dla obslugi grafiki ------------ */
#define _TYLKO_RYS_
#if defined(_TERM_QNX_) && !defined(_DOS_)
#include "../aj/graf.h"
#endif
#define MAX_GRAFS 20

struct baza_grafik
{
    int Lgrafs;
    void* Grafiki[MAX_GRAFS];
    void*(*akcja_grafik)(int typ_kom, int kto, char* pdane, char* Text,
                         void*(*Dane_Wykr)(void* Rd), void* R);
    int report[MAX_GRAFS];
} BGR;

/* ============== Czytanie znakow decyzyjnych np.:<t/n> =============== */
char mgetch(char* monit)
{
    char cox;
    unsigned int cco, co;
    co = strlen(monit);
    if (co < 3)
    {
        term_printf(y_akt, x_akt, attryb, "Brak wzorca funkcji mgetch() %s ??. Nacisnij dowolny klawisz", monit);
        Get_Char(); /* dev_mode(0,opcje,_DEV_MODES);*/
        return 0;
    }
    cox = m_wherex();
    do
    {
        x_akt = cox;
        term_printf(y_akt, x_akt, attryb, " <%s> ?? ", monit);
        x_akt += strlen(monit);
        cco = Get_Char();
        co = cco & 0x00FF;
        if (co == Esc)
        {
            if (ABORT == 1) mAbort(); /* dev_mode(0,opcje,_DEV_MODES);*/
            return co;
        }
        else
        {
            if (co == EOS)
            {
                Get_Char();
                continue;
            }
            if (co == CR) continue;
            /*      if(isalfa(co) term_type(y_akt,x_akt,&co,1,attryb); */
        }
    }
    while (co != monit[0] && co != monit[2]);
    /* dev_mode(0,opcje,_DEV_MODES);*/
    return co;
}

int slowa(char* monit, char text[], int liczba_znakow, char* slowo[],
          int max_l_slow, char opcje)
/* opcje  - opcje konwersji slowa:
			 0  - kropka jest separatorem, znaki bez zmiany
       1  - kropka nie jest separatorem
			 2  - zamienia duze litery na male
       3  - jak 1 i 2

*/
{
    int ls;
    char f = 0, male = 0;
    if ((opcje & 1) != 0) f = 1;
    if ((opcje & 2) != 0) male = 1;
    text[0] = EOS;
    czytaj_we(monit, text, 0, liczba_znakow);
    ls = podzial_na_slowa(text, slowo, max_l_slow, f, male);
    return ls;
} /*.....end slowa()...*/
/*------------------------------------------------------------------------*/
/*     MSCANF(stos) - procedura czytania dowolnej ilosci danych
                      (opisanych monitem) wprowadzanych z klawiatury       */
/*
/* postac stosu - znak :# - jesli wystepuje - dane musza byc wpisane
                          - jesli brak # - brak danych oznacza przyjecie
                                           wartosci wczesniej podstawionych
                  znak :+ - jesli wystepuje - zmiana linii po wczytaniu danych
                          - jesli brak + - bez zmiany linii
                 monit : tekst zawierajacy monit i specyfikacje formatu
												 czytania danych
                         Dopuszczalne specyfikacje:
                           %s - text (bez spacji !!!!)
                           %d - liczba calkowita,
													 %f - liczba rzeczyw.wpisana normalnie np 2.3
                           %g - liczba rzeczyw.wpisana normalnie
																lub wykladniczo np 2.3 lub 0.23e+1
                           %e - liczba double prec. w zapisie jak %g
                         Format moze zawierac dowolna liczbe specyfikacji
                         ale nastepujaca po nim lista adresow musi !!!
                         byc zgodna z odpowiadajaca lista specyfikacji
				 adresy danych : lista adresow zgodna co do ilosci i typow zmiennych
                         z lista specyfikacji

Uwaga !! Znak <Esc> wprowadz. z klaw. powoduje zakonczenie programu (po ostrzezeniu)

Przyklady:
char text[20];
 int k,j;
 float a,b;
 double c;
Dane nieobowiazkowe (monit bez #):
D: k=1; j=2; a=1.2; b=2.; c=1.e-3;    - wstepne podstwienie danych
   mscanf("\nWpisz k,j,a,b,c (np.<1 2 1.2 2 1.e-3> ?? %d%d%f%g%e",&k,&j,&a,&b,&c);
   "\nWpisz text,k,j,a,b,c (np.<Dane 1 2 1.2 2 1.e-3> ?? - monit dla danych niobowiazk.
   %s%d%d%f%g%e         - lista specyfikacji,
	 ,&k,&j,&a,&b,&c      - lista adresow danych,
Dane obowiazkowe (#) bez monitu wlasciwego:
   mscanf("#%d%d%f%g%e",&k,&j,&a,&b,&c);
Dane obowiazkowe (#) z monitem wlasciwym:
   mscanf("#\nWpisz k,j,a,b,c (np.<1 2 1.2 2 1.e-3> ?? %d%d%f%g%e",&k,&j,&a,&b,&c);

				 Program kontroluje poprawnosc danych
				 Lista separatorow:{( ), (,), (!), (:), (;), (?), (\0), (.)}
                           dwa ostatnie tylko dla specyfikacji %s

*/
int mscanf(char* stos, ...)
/* f - format konwersji slowa:
       s  - tylko teksty (slowo[]= adresy kolejnych tekstow)
			 d  - tylko liczby calkowite   (slowo[]= adresy kolejnych liczb)
       f,g- tylko liczby rzeczywiste (slowo[]= adresy kolejnych liczb)
       l  - tylko liczby  (slowo[]= adres testow stanowiacych kolejne liczby
                                    NULL gdy tekst nie jest liczba
*/
{
    int l_danych, z0, x_pocz, dlm;
    char text[80], *monit, *form, *sl, f, def = 1, nl = 0;
    char** adres_danej;
    char *forms, *ksl, m0;
    signed char ff;
    unsigned int cc;
    double a;
    text[0] = EOS;
    l_danych = 0;
    monit = stos;
    if (*monit == '+')
    {
        monit++;
        nl = 1;
    }
    if (*monit == '#')
    {
        monit++;
        def = 0;
        if (*monit == '*') monit++;
        if (*monit == '+')
        {
            monit++;
            nl = 1;
        }
    }
    forms = szukaj(monit, '%');
    if (forms == NULL)
    {
        yplus();
        x_akt = x_akt0;
        term_printf(y_akt, x_akt, attryb, "mscanf(%s)");
        yplus();
        term_printf(y_akt, x_akt, attryb, "Brak specyfikacji. Koniec ?? <q> ", monit);
        cc = Get_Char();
        f = cc & 0x00FF;
        if (f == 'q')
        {
            clear_cur(EOS);
            return 0;
        }
        yplus();
    }
    /*  larg=nargs()-1; */
    m0 = *monit;
    *forms = EOS;
    dlm = strlen(monit);
    x_pocz = x_akt;
P:
    text[0] = EOS;
    x_akt = x_pocz;
    term_cur(y_akt, x_akt);
    goto D;
B:
    x_akt = x_pocz + dlm;
    term_cur(y_akt, x_akt);
D:
    z0 = strlen(text);
    adres_danej = &stos;
    adres_danej++;
    form = forms;
    if (*monit == '\n')
    {
        yplus();
        x_akt = x_akt0;
        monit++;
    }
    if (*monit == '\r')
    {
        x_akt = x_akt0;
        monit++;
    }
    m0 = *monit;
    if (czytaj_we(monit, text, z0, x_maxOkna - x_akt0 - strlen(monit)) < -1)
    {
        text[0] = EOS;
        *forms = '%';
        *monit = m0;
        return -1;
    }
    (*forms) = '%';
    *monit = m0;
    ksl = text;
    if (*ksl == EOS)
    {
        if (nl == 1 && *monit != '\n')
        {
            yplus();
            x_akt = x_akt0;
        }
        if (def == 1)
        {
            *forms = '%';
            *monit = m0;
            return 0;
        }
        else
        {
            *forms = EOS;
            goto P;
        }
    }
    do
    {
        form++;
        f = *form;
        /*    if(l_danych > larg)
               {term_printf(yplus(),x_akt0,attryb,"Za malo danych na liscie. Koniec ?? <q> ");
                        cc=Get_Char(); f=cc&0x00FF;
                if(f=='q') {clear_cur(EOS); return 0;}
                yplus(); x_akt=x_akt0;
               }
        */
        f = *form;
        if (f == 's')
        {
            ff = -1;
            if (ksl == text) sl = ksl;
            else sl = szukaj_slowa(ksl, ff);
        }
        else
        {
            ff = 2;
            sl = szukaj_slowa(ksl, ff);
        }
        if (ff >= 0) ksl = szukaj_konca(sl, ff, 0);
        ff = 1;
        switch (f)
        {
        case 's': przepisz(*adres_danej, sl, sizeof(sl));
            break;
        case 'd':
            if (znaki_liczby(sl, 0) == 1) *((int*)(*adres_danej)) = atoi(sl);
            else
            {
                if (BLAD == 1)
                {
                    yplus();
                    term_printf(y_akt, x_akt0, attryb,
                                "Bledna dana (%s) typu %c", sl, f);
                    yplus();
                    x_akt = x_akt0;
                }
                else
                {
                    bip(1);
                    monit[0] = EOS;
                    goto B;
                }
                *forms = EOS;
                goto P;
            }
            break;
        case 'e':
            if (znaki_liczby(sl, 2) == 1)
            {
                a = atof(sl);
                *((double*)(*adres_danej)) = a;
            }
            else
            {
                if (BLAD == 1)
                {
                    yplus();
                    komunikat(MY_MAX, X_L0, attryb, "Bledna dana (%s) typu %c", sl, f);
                }
                else
                {
                    bip(1);
                    monit[0] = EOS;
                    goto B;
                }
                *forms = EOS;
                goto P;
            }
            break;
        case 'g': ff = 2;
        case 'f':
            if (znaki_liczby(sl, ff) == 1)
            {
                a = atof(sl);
                *((float*)(*adres_danej)) = (float)a;
            }
            else
            {
                if (BLAD == 1)
                {
                    yplus();
                    term_printf(y_akt, x_akt0, attryb, "Bledna dana (%s) typu %c", sl, f);
                    yplus();
                    x_akt = x_akt0;
                }
                else
                {
                    bip(1);
                    monit[0] = EOS;
                    goto B;
                }
                *forms = EOS;
                goto P;
            }
            break;
        default:
            komunikat(MY_MAX, X_L0, attryb, "Bledna specyfikacja %c formatu. Koniec ?? <q> ", f);
            cc = Get_Char();
            f = cc & 0x00FF;
            if (f == 'q') { return (0); }
            else
            {
                yplus();
                x_akt = x_akt0;
            };
        }
        adres_danej++;
        l_danych++;
        form = szukaj(form, '%');
        if (ksl == NULL && def == 0 && form != NULL)
        {
            if (nl == 1 && *monit != '\n')
            {
                yplus();
                x_akt = x_akt0;
            }
            *forms = EOS;
            goto P;
        }
    }
    while (form != NULL && ksl != NULL);
    if (nl == 1)
    {
        yplus();
        x_akt = x_akt0;
    }
    return l_danych;
} /*...end mscanf()....*/

/*--------------------------------------------------------------------- */
int bscanf(char* text, unsigned int attrtx, char* stos, ...)
/* wyjscie jak w czytaj_we:
	-2 Esc; -1 Ent; 0 ??; >0 liczba wczytanych znakow
	----------------------------------------------------------- */
/* f - format konwersji slowa:
			 s  - tylko teksty (slowo[]= adresy kolejnych tekstow)
			 d  - tylko liczby calkowite   (slowo[]= adresy kolejnych liczb)
			 f,g- tylko liczby rzeczywiste (slowo[]= adresy kolejnych liczb)
       l  - tylko liczby  (slowo[]= adres testow stanowiacych kolejne liczby
                                    NULL gdy tekst nie jest liczba
*/
{
    int l_danych, z0, x_pocz, dlm, ret;
    char *monit, *form, *sl, f, def = 1, nl = 0, ifmon = 1;
    char** adres_danej;
    char *forms, *ksl, m0;
    signed char ff;
    unsigned int cc;
    double a;
    l_danych = 0;
    z0 = strlen(text);
    if (z0 > MXR_MAX) text[MXR_MAX] = EOS;
    monit = stos;
    if (*monit == '*') monit++;
    if (*monit == '+')
    {
        monit++;
        nl = 1;
    }
    if (*monit == '#')
    {
        monit++;
        def = 0;
        if (*monit == '+')
        {
            monit++;
            nl = 1;
        }
    }
    else if (*monit == '*') monit++;
    forms = szukaj(monit, '%');
    if (forms == NULL)
    {
        yplus();
        x_akt = x_akt0;
        term_printf(y_akt, x_akt, attryb, "mscanf(%s)");
        yplus();
        term_printf(y_akt, x_akt, attryb, "Brak specyfikacji. Koniec ?? <q> ", monit);
        cc = Get_Char();
        f = cc & 0x00FF;
        if (f == 'q')
        {
            clear_cur(EOS);
            return 0;
        }
        yplus();
    }
    /*  larg=nargs()-1; */
    m0 = *monit;
    *forms = EOS;
    dlm = strlen(monit);
    x_pocz = x_akt;
P:
    x_akt = x_pocz;
    term_cur(y_akt, x_akt);
    ifmon = 1;
    goto D;
B:
    x_akt = x_pocz + dlm;
    term_cur(y_akt, x_akt);
    ifmon = 0;
D:
    z0 = strlen(text);
    adres_danej = &stos;
    adres_danej++;
    form = forms;
    if (ifmon == 1)
    {
        if (*monit == '\n')
        {
            yplus();
            x_akt = x_akt0;
            monit++;
            dlm--;
        }
        if (*monit == '\r')
        {
            x_akt = x_akt0;
            monit++;
            dlm--;
        }
        term_type(y_akt, x_akt, monit, 0, attrtx);
        x_akt = m_wherex(); /*+=strlen(monit); m_gotoxy(x_akt,y_akt); */
    }
    /*  m0=*monit; *monit=EOS; */
    ret = czytaj_we("", text, z0, x_maxOkna - x_akt + 2);
    *monit = m0;
    if (ret < -1)
    {
        text[0] = EOS;
        *forms = '%';
        return -1;
    }
    (*forms) = '%';
    ksl = text;
    if (*ksl == EOS/* || ret==-1*/)
    {
        if (nl == 1 && *monit != '\n')
        {
            yplus();
            x_akt = x_akt0;
        }
        if (def == 1)
        {
            *forms = '%';
            *monit = m0;
        }
        else
        {
            *forms = EOS;
            goto P;
        }
    }
    do
    {
        form++;
        f = *form;
        /*    if(l_danych > larg)
               {term_type(yplus(),x_akt0,"Za malo danych na liscie. Koniec ?? <q> ",0,attryb);
                        cc=Get_Char(); f=cc&0x00FF;
                if(f=='q') {/ *dev_mode(0,opcje,_DEV_MODES);* / return 0;}
                        yplus(); x_akt=x_akt0;
               }
        */
        f = *form;
        if (f == 's')
        {
            ff = -1;
            if (ksl == text) sl = ksl;
            else sl = szukaj_slowa(ksl, ff);
        }
        else
        {
            ff = 2;
            sl = szukaj_slowa(ksl, ff);
        }
        if (ff >= 0) ksl = szukaj_konca(sl, ff, 0);
        if (ff >= 0) ksl = szukaj_konca(sl, ff, 0);
        ff = 1;
        switch (f)
        {
        case 's': strcpy(*adres_danej, sl);
            break;
        case 'c': *adres_danej[0] = sl[0];
            break;
        case 'd':
            if (znaki_liczby(sl, 0) == 1) *((int*)(*adres_danej)) = atoi(sl);
            else
            {
                if (BLAD == 1)
                {
                    yplus();
                    komunikat(MY_MAX, X_L0, attryb,
                              "Bledna dana (%s) typu %c", sl, f);
                    yplus();
                    x_akt = x_akt0;
                }
                else
                {
                    bip(1);
                    monit[0] = EOS;
                    goto B;
                }
                *forms = EOS;
                /*            if(nl==1 && *monit !='\n') {yplus(); x_akt=x_akt0;}
                                        else akt=x_akt0; */
                goto P;
            }
            break;
        case 'e':
            if (znaki_liczby(sl, 2) == 1)
            {
                a = atof(sl);
                *((double*)(*adres_danej)) = a;
            }
            else
            {
                if (BLAD == 1)
                {
                    yplus();
                    komunikat(MY_MAX, X_L0, attryb, "Bledna dana (%s) typu %c", sl, f);
                }
                else
                {
                    bip(1);
                    monit[0] = EOS;
                    goto B;
                }
                *forms = EOS;
                /* else x_akt=x_akt0; */
                goto P;
            }
            break;
        case 'g': ff = 2;
        case 'f':
            if (znaki_liczby(sl, ff) == 1)
            {
                a = atof(sl);
                *((float*)(*adres_danej)) = (float)a;
            }
            else
            {
                if (BLAD == 1)
                {
                    yplus();
                    term_printf(y_akt, x_akt0, attryb, "Bledna dana (%s) typu %c", sl, f);
                    yplus();
                    x_akt = x_akt0;
                }
                else
                {
                    bip(1);
                    monit[0] = EOS;
                    goto B;
                }
                *forms = EOS;
                goto P;
            }
            break;
        default:
            komunikat(MY_MAX, X_L0, attryb, "Bledna specyfikacja %c formatu. Koniec ?? <q> ", f);
            cc = Get_Char();
            f = cc & 0x00FF;
            if (f == 'q')
            {
                clear_cur(EOS);
                return (0);
            }
            else
            {
                yplus();
                x_akt = x_akt0;
            };
        }
        adres_danej++;
        l_danych++;
        form = szukaj(form, '%');
        if (ksl == NULL && def == 0 && form != NULL)
        {
            if (nl == 1 && *monit != '\n')
            {
                yplus();
                x_akt = x_akt0;
            }
            *forms = EOS;
            goto P;
        }
    }
    while (form != NULL && ksl != NULL);
    if (nl == 1)
    {
        yplus();
        x_akt = x_akt0;
    }
    if (ret < -1) return 0;
    return l_danych;
} /*...end mscanf()....*/
/* ---------------------------------------------------------------------- */
char* szukaj(char* text, char znak)
{
    while ((*text) != EOS)
    {
        if ((*text) != znak) text++;
        else return text;
    };
    return NULL;
} /*...end szukaj().....*/
/* ----------------------------------------------------------------------
int szukaj_znak(char *text,char znak)
 {int i=0;
  do {if(*text!=znak) text++;
			else return text;
     }while(*text!=EOS);
	return NULL;
 }   ...end szukaj().....*/

/*------------------------------------------------------------------------*/
int czytaj_we(char* monit, char* text, int lz0, int liczba_znakow)
{
    int lz, nEOS = 0, lzp = 0, x, x_pocz, start = 0, ccx, i;
    int y_cursor, x_cursor, x_cur0, x_curM, y_cur0, y_curM;
    unsigned int c, cc, attr_old;
    lticks_min = kbtickm;
    if (x_maxOkna - x_akt + 1 < liczba_znakow) liczba_znakow = x_maxOkna - x_akt + 1;
    nEOS = strlen(text);
    if (nEOS >= liczba_znakow)
    {
        text[liczba_znakow] = EOS;
        nEOS = liczba_znakow;
    }
    y_cursor = y_akt;
    if (monit != NULL)
    {
        if (strlen(monit) > 0) term_type(y_cursor, x_akt, monit, 0, attryb);
    }
    x_pocz = m_wherex();
    term_type(y_akt, x_pocz, text, 0, attryb);
    if (x_kursor < 0) x_akt = m_wherex(); /*(strlen(monit)+strlen(text)); */
    else
    {
        x_akt = x_kursor;
        lz0 = x_akt - x_pocz;
        start = 1;
    }
    x_kursor = -1;
    if (x_akt > x_maxOkna) x_akt = x_maxOkna;
    if (lz0 >= liczba_znakow) lz0 = liczba_znakow - 1;
    if (HELP == 0) /* && EDIT==1) */
    {
        ccx = strlen(text) - 1;
        if (ccx > 0)
        {
            while (ccx > 0 && (text[lz0 - ccx] == ' ')) { ccx--; }
            x_akt -= ccx;
            lz0 -= ccx;
        }
        if (lz0 < 0) lz0 = 0;
    }
    /*  else {if(HELP!=0) x_akt+=lz0;} */
    cc = 0;
    lz = lz0;
    lzp = 0;
    term_cur(y_cursor, x_akt);
    do
    {
        term_flush();
        x_cursor = m_wherex();
        y_cursor = m_wherey();
        x_cur0 = x_akt0;
        x_curM = x_maxOkna;
        y_cur0 = y_akt0;
        y_curM = y_maxOkna;
        attr_old = attryb;
        /* -------------------- tu moze wejsc RTkernel ------------------------- */
        cc = GET_char();
        c = cc & 0x00FF;
        /* --------------------------------------------------------------------- */
        x_akt0 = x_cur0;
        x_maxOkna = x_curM;
        y_akt0 = y_cur0;
        y_maxOkna = y_curM;
        attryb = attr_old;
        m_gotoxy(x_cursor, y_cursor);
        /*		 term_cur(y_cursor, x_cursor); */
        /*     if(c==NLCR) c=CR/ *=GET_char()*/
        ;
        if (c == CR)
        {
            clear_cur(text[lz]);
            if (start == 0)
            {
                lticks_min = Ltickm;
                return -1;
            }
            cc = c;
            continue;
        }
        if (c == BS)
        {
            if (lz == 0)
            {
                if (nEOS == 1)
                {
                    nEOS = 0;
                    goto DEL;
                }
                else continue;
            }
            lzp = lz;
            if (nEOS < liczba_znakow)
            {
                lz--;
#ifdef GRAF
                if (igraf != 0) backspace(&text[lz]);
                else term_cur(y_akt, x_akt - 1);
#else
                term_cur(y_akt, x_akt - 1);
#endif
            }
            goto DEL;
        }
        if (c == Esc)
        {
            if (ABORT == 1)
            {
                mAbort();
                break;
            }
            else
            {
                if (EDIT == 0)
                {
                    text[0] = Esc;
                    text[1] = EOS;
                }
                lticks_min = Ltickm;
                return -2;
            }
        }

        if (c == EOS || c == SPEC)
        {
            c = Get_char(); /*c=cc&0x00FF*/
            ;
            //c=VCGetch();

            if (c == BBS || c == LEFT)
            {
                if (lz == 0) continue;
                lz--;
#ifdef GRAF
                if (igraf != 0) backspace(&text[lz]);
                else { m_gotoxy(x_pocz + lz, y_cursor); }
#else
                m_gotoxy(x_pocz + lz, y_cursor); /* term_cur(y_akt,x_akt+lz); */
#endif
                continue;
            }
            if (c == FFR)
            {
                if (lz >= nEOS) continue;
                if (x_akt >= x_maxOkna) continue;
                clear_cur(text[lz]);
                lz++;
                if (lz == liczba_znakow) lz--;
                xplus();
                term_cur(y_akt, x_akt); /* m_gotoxy(x_akt,y_cursor); */
                continue;
            }
            if (c == Home)
            {
#ifdef GRAF
                if (igraf != 0) clear_cur(text[lz]);
#endif
                x_akt = x_pocz;
                lz = 0;
                term_cur(y_akt, x_akt); /*m_gotoxy(x_akt,y_cursor); */
                continue;
            }
            if (c == End)
            {
                /*fprintf(mystderr,"lz=%d nEOS=%d, dltx=%d\n",lz,nEOS,strlen(text));*/
                clear_cur(text[lz]);
                lzp = lz;
                x_akt = x_pocz + lz;
                for (lz = lzp; lz < nEOS; lz++)
                {
                    term_type(y_akt, x_akt, &text[lz], 1, attryb);
                    xplus();
                }
                lz = nEOS;
                if (lz == liczba_znakow) lz--;
                term_cur(y_akt, x_akt);
                continue;
            }
            if (c == UP)
            {
                if (EDIT == 1)
                {
                    if (y_cursor == y_min) continue;
                    y_akt--;
                    x_kursor = m_wherex();
                    lticks_min = Ltickm;
                    return nEOS;
                }
                cc = text[lz];
                if (cc < '0' || cc > '9') continue;
                text[lz]++;
                if (text[lz] > '9') text[lz] = '0';
                goto INC;
            }
            if (c == DOWN)
            {
                if (EDIT == 1)
                {
                    if (y_cursor == y_maxOkna) continue;
                    y_akt++;
                    x_kursor = m_wherex();
                    lticks_min = Ltickm;
                    return nEOS;
                }
                cc = text[lz];
                if (cc < '0' || cc > '9') continue;
                text[lz]--;
                if (text[lz] < '0') text[lz] = '9';
            INC:
                term_type(y_akt, x_akt, &text[lz], 1, attryb);
                term_cur(y_akt, x_akt);
                start = 1;
                if (HELP == 0) continue;
                HELP = 2;
                x_kursor = m_wherex();
                lticks_min = Ltickm;
                return nEOS;
            }
            if (c == Del)
            {
                if (lz >= nEOS) continue;
            DEL:
                lzp = lz;
                x = x_akt;
                Clr_eol();
                start = 1;
                if (nEOS > 0)
                {
                    for (lz = lzp; lz < nEOS - 1; lz++, x++)
                    {
                        text[lz] = text[lz + 1];
                        term_type(y_cursor, x, &text[lz], 1, attryb);
                    }
                    nEOS--;
                    text[nEOS] = EOS;
                    lz = lzp;
                    term_cur(y_cursor, x_akt);
                }
                continue;
            }
            if (c == Ins)
            {
                INSERT = -INSERT;
                /*         if(x_maxOkna<X_maxBlank) term_type(y_akt,xmax+1,"*",1,TERM_RED|MTERM_HILIGHT);
                                 if(INSERT==-1) attryb=attryb|TERM_ULINE;
                                 else           attryb=attryb|(~TERM_ULINE);
                */
                continue;
            }
            continue;
        }
        if (lz >= liczba_znakow)
        {
            lz = liczba_znakow - 1;
            continue;
        }
        c = cc;
        if (isprint(cc))
        {
            if (INSERT < 0)
            {
                lzp = lz;
                if (nEOS >= liczba_znakow)
                {
                    nEOS = liczba_znakow;
                    if (text[0] != ' ' && text[nEOS] != ' ')
                    {
                        bip(0);
                        continue;
                    }
                    m_gotoxy(x_pocz, y_cursor);
                    /*					 term_cur(y_akt, x_akt); */
                    for (lz = 0; lz < lzp; lz++)
                    {
                        text[lz] = text[lz + 1];
                        term_type(y_cursor, x_akt, &text[lz], 1, attryb);
                        xplus();
                    }
                }
                else
                {
                    lzp = lz;
                    for (lz = nEOS; lz > lzp; lz--) text[lz] = text[lz - 1];
                    nEOS++;
                    text[nEOS] = EOS;
                    x = x_akt + 1;
                    for (lz = lzp + 1; lz < nEOS; lz++, x++) term_type(y_cursor, x, &text[lz], 1, attryb);
                }
                lz = lzp;
            }
            text[lz] = c; /* term_cur(y_cursor,x_akt); */
            term_type(y_cursor, x_akt, (char*)&cc, 1, attryb);
            xplus();
            term_cur(y_cursor, x_akt);
            lz++;
            if (lz == liczba_znakow)
            {
                nEOS = lz;
                text[nEOS] = EOS;
                lz--;
            }
            start = 1;
            /*       fprintf(mystderr," lz=%d nEOS=%d c=%c tx[lz]=%c ",lz,nEOS,c,text[lz]);*/
            if (nEOS < lz)
            {
                nEOS = lz;
                text[nEOS] = EOS;
            }
        }
    }
    while (cc != CR);
    text[nEOS] = EOS;
    lticks_min = Ltickm;
    return nEOS;
} /*....end czytaj_we()....*/

/*------------------------------------------------------------------------*/
int czytaj_has(char* text, int lz0, int liczba_znakow)
{
    int lz, nEOS = 0, lzp = 0, x, x_pocz, start = 0, i;
    int y_cursor, x_cursor, x_cur0, x_curM, y_cur0, y_curM;
    unsigned int c, cc, attr_old;
    lticks_min = kbtickm;
    if (x_maxOkna - x_akt + 1 < liczba_znakow) liczba_znakow = x_maxOkna - x_akt + 1;
    nEOS = strlen(text);
    if (nEOS >= liczba_znakow)
    {
        text[liczba_znakow] = EOS;
        nEOS = liczba_znakow;
    }
    x_pocz = m_wherex();
    /*  term_type(y_cursor,m_wherex(),text,0,attryb); / * ###### */
    if (x_kursor < 0) x_akt = m_wherex();
    else
    {
        x_akt = x_kursor;
        lz0 = x_akt - x_pocz;
        start = 1;
    }
    x_kursor = -1;
    if (x_akt > x_maxOkna) x_akt = x_maxOkna;
    if (lz0 >= liczba_znakow) lz0 = liczba_znakow - 1;
    lz = lz0;
    lzp = 0;
    term_cur(y_akt, x_akt);
    do
    {
        term_flush();
        x_cursor = x_akt;
        y_cursor = m_wherey();
        x_cur0 = x_akt0;
        x_curM = x_maxOkna;
        y_cur0 = y_akt0;
        y_curM = y_maxOkna;
        attr_old = attryb;
        cc = Get_Char();
        c = cc & 0x00FF;
        x_akt0 = x_cur0;
        x_maxOkna = x_curM;
        y_akt0 = y_cur0;
        y_maxOkna = y_curM;
        term_cur(y_cursor, x_cursor);
        attryb = attr_old;
        /*     if(c==NLCR) c=CR/ *=GET_char()*/
        ;
        if (c == CR)
        {
            clear_cur(EOS);
            if (start == 0)
            {
                lticks_min = Ltickm;
                return -1;
            }
            cc = c;
            continue;
        }
        if (c == BS)
        {
            if (lz == 0)
            {
                if (nEOS == 1)
                {
                    nEOS = 0;
                    goto DEL;
                }
                else continue;
            }
            lzp = lz;
            if (nEOS < liczba_znakow)
            {
                lz--;
#ifdef GRAF
                if (igraf != 0) backspace(&text[lz]);
                else m_gotoxy(x_akt - 1, y_akt);
#else
                m_gotoxy(x_akt - 1, y_akt); /*  term_cur(y_akt,x_akt); */
#endif
            }
            goto DEL;
        }
        if (c == Esc)
        {
            if (ABORT == 1)
            {
                mAbort();
                break;
            }
            else
            {
                text[0] = Esc;
                text[1] = EOS;
                lticks_min = Ltickm;
                return -2;
            }
        }
        if (c == EOS || c == SPEC)
        {
            c = Get_char(); /*c=cc&0x00FF*/
            ;
            if (c == BBS || c == LEFT)
            {
                if (lz == 0) continue;
                lz--;
#ifdef GRAF
                if (igraf != 0) backspace(&text[lz]);
                else
                {
                    m_gotoxy(x_pocz + lz, y_akt);
                    term_cur(y_akt, x_akt);
                }
#else
                m_gotoxy(x_pocz + lz, y_akt); term_cur(y_akt, x_akt);
#endif
                continue;
            }
            if (c == FFR)
            {
                if (lz >= nEOS) continue;
                if (x_akt >= x_maxOkna) continue;
                lz++;
                if (lz == liczba_znakow) lz--;
                xplus();
                m_gotoxy(x_akt, y_akt);
                term_cur(y_akt, x_akt);
                continue;
            }
            if (c == Home)
            {
                x_akt = x_pocz;
#ifdef GRAF
                if (igraf != 0) { for (i = lz; i >= 0; i--) backspace(&text[i]); }
#endif
                lz = 0;
                m_gotoxy(x_akt, y_akt);
                term_cur(y_akt, x_akt);
                continue;
            }
            if (c == End)
            {
                /*fprintf(mystderr,"lz=%d nEOS=%d, dltx=%d\n",lz,nEOS,strlen(text));*/
                lzp = lz;
                x_akt = x_pocz + lz;
                for (lz = lzp; lz < nEOS; lz++)
                {
                    term_type(y_akt, x_akt, "*", 1, attryb);
                    xplus();
                }
                lz = nEOS;
                if (lz == liczba_znakow) lz--;
                term_cur(y_akt, x_akt);
                continue;
            }
            if (c == UP)
            {
                cc = text[lz];
                if (cc < '0' || cc > '9') continue;
                text[lz]++;
                if (text[lz] > '9') text[lz] = '0';
                goto INC;
            }
            if (c == DOWN)
            {
                cc = text[lz];
                if (cc < '0' || cc > '9') continue;
                text[lz]--;
                if (text[lz] < '0') text[lz] = '9';
            INC:
                term_type(y_akt, x_akt, "*", 1, attryb);
                m_gotoxy(x_akt, y_akt);
                start = 1;
                if (HELP == 0) continue;
                HELP = 2;
                x_kursor = m_wherex();
                lticks_min = Ltickm;
                return nEOS;
            }
            if (c == Del)
            {
                if (lz >= nEOS) continue;
            DEL:
                lzp = lz;
                x = x_akt;
                Clr_eol();
                start = 1;
                if (nEOS > 0)
                {
                    for (lz = lzp; lz < nEOS - 1; lz++, x++)
                    {
                        text[lz] = text[lz + 1];
                        term_type(y_akt, x, "*", 1, attryb);
                    }
                    nEOS--;
                    text[nEOS] = EOS;
                    lz = lzp;
                    term_cur(y_akt, x_akt);
                }
                continue;
            }
            if (c == Ins)
            {
                INSERT = -INSERT;
                continue;
            }
            continue;
        }
        if (lz >= liczba_znakow)
        {
            lz = liczba_znakow - 1;
            continue;
        }
        c = cc;
        if (isprint(cc))
        {
            if (INSERT < 0)
            {
                lzp = lz;
                if (nEOS >= liczba_znakow)
                {
                    nEOS = liczba_znakow;
                    if (text[0] != ' ' && text[0] != EOS)
                    {
                        bip(0);
                        continue;
                    }
                    m_gotoxy(x_pocz, y_akt);
                    for (lz = 0; lz < lzp; lz++)
                    {
                        text[lz] = text[lz + 1];
                        term_type(y_akt, x_akt, "*", 1, attryb);
                        xplus();
                    }
                }
                else
                {
                    lzp = lz;
                    for (lz = nEOS; lz > lzp; lz--) text[lz] = text[lz - 1];
                    nEOS++;
                    text[nEOS] = EOS;
                    x = x_akt + 1;
                    for (lz = lzp + 1; lz < nEOS; lz++, x++) term_type(y_akt, x, "*", 1, attryb);
                }
                lz = lzp;
            }
            text[lz] = c;
            term_type(y_akt, x_akt, "*", 1, attryb);
            xplus();
            term_cur(y_akt, x_akt);
            lz++;
            if (lz == liczba_znakow)
            {
                nEOS = lz;
                text[nEOS] = EOS;
                lz--;
            }
            start = 1;
            if (nEOS < lz)
            {
                nEOS = lz;
                text[nEOS] = EOS;
            }
        }
    }
    while (cc != CR);
    text[nEOS] = EOS;
    lticks_min = Ltickm;
    return nEOS;
} /*....end czytaj_has()....*/
/*------------------------------------------------------------------------*/
int podzial_na_slowa(char* text, char* slowa[], int max_l_slow, char f, char male)
{
    int nr_slowa = 0;
    char* sl;
    do
    {
        sl = szukaj_slowa(text, f);
        text = szukaj_konca(sl, f, male);
        slowa[nr_slowa] = sl;
        nr_slowa++;
    }
    while (text != NULL && nr_slowa < max_l_slow);
    return nr_slowa;
} /*...end podzial_na_slowa()....*/
/*------------------------------------------------------------------------*/
char znaki_liczby(char* slowo, char kropka)
{
    char kr, e, z;
    kr = kropka;
    e = kr;
    if (*slowo == '+' || *slowo == '-') slowo++;
    do
    {
        z = 0;

        if (*slowo < 58 && *slowo > 47)
        {
            slowo++;
            z = 1;
            continue;
        }
        if (kr > 0 && *slowo == '.')
        {
            slowo++;
            kr = 0;
            z = 1;
            continue;
        }
        if (e == 2 && *slowo == 'e')
        {
            slowo++;
            e = 0;
            if (*slowo == '+' || *slowo == '-') slowo++;
        }
        else return 0;
    }
    while ((*slowo) != EOS);
    return z;
} /*...end znaki_liczby()....*/
/*------------------------------------------------------------------------*/
char* szukaj_slowa(char* text, char f)
{
    register int i;
    int lsep;
    lsep = L_SEP;
    if (f > 0) lsep = lsep - f;
    for (;;)
    {
        for (i = 0; i < lsep; i++) { if (*text == separator[i]) goto S; }
        return text;
    S:
        if (*text == EOS) return text;
        text++;
    }
} /*....end szukaj_slowa()....*/
/*------------------------------------------------------------------------*/
char* szukaj_konca(char* slowo, char f, char male)
{
    register int i;
    int lsep;
    lsep = L_SEP;
    if (f > 0) lsep = lsep - f;
    for (;;)
    {
        for (i = 0; i < lsep; i++) { if (*slowo == separator[i]) goto S; }
        if (*slowo == EOS) return NULL;
        if (male == 1 && (*slowo > 64 && *slowo < 91)) (*slowo) += 32;
        slowo++;
    }
S:
    if (*slowo == EOS) return NULL;
    *slowo = EOS;
    slowo++;
    return slowo;
} /*...end szukaj_konca().....*/
/*------------------------------------------------------------------------*/
void mAbort()
{
    yplus();
    x_akt = x_akt0;
    term_printf(y_akt, x_akt, attryb, "���������������������������������������ͻ");
    yplus();
    x_akt = x_akt0;
    term_printf(y_akt, x_akt, attryb, "�   ABORT programu !!! ?? TAK - 't'     �");
    yplus();
    x_akt = x_akt0;
    term_printf(y_akt, x_akt, attryb, "���������������������������������������ͼ");
    yplus();
    x_akt = x_akt0;
    if (Get_Char() == 't') exit(0);
    term_printf(y_akt, x_akt, attryb, "Wpisz odpowiedz na poprzednie pytanie");
}

/*------------------------------------------------------------------------*/
void bip(char mod)
{
#ifndef _TERM_QNX_
    if (mod == 0)
    {
        sound(500);
        delay(100);
    }
    else
    {
        sound(1000);
        delay(200);
    }
    nosound();
#endif
}

/* -----------------------------------------------------------------------*/
void yplus(void)
{
    if (y_akt < y_maxOkna) y_akt++;
    return;
}

/* -----------------------------------------------------------------------*/
void xplus(void)
{
    if (x_akt < x_maxOkna) x_akt++;
    return;
}


/*  ======================== Procedury dla DOS'a ========================*/
#if !defined(_TERM_QNX_) && !defined(_NCURSES_)
#include "../blank/term_dos.c"

#ifndef _CVC_
int Get_char(void) { return getch(); }
#else

int Get_char(void) { return term_new_getch_char(-1); }
#endif


void zamknij_grafiki(int a)
{
};

int czy_mozna_nowe_wykresy(void*(*akcja_grafik)(int typ_kom, int kto,
                                                char* pdane, char* Text, void*(*Dane_Wykr)(void* Rd), void* R))
{
    return 0;
}


#elif defined(_TERM_QNX_)
void clear_cur(char znak) { ; }
void setcursor(unsigned int cursor) { ; }
/* int GETchar(void){return getchar();} */
int Get_Char(void) { return getchar(); }
#define L_EVENT 2
#define SPEC1 255
int GET_char(void) /* ======= odczyt znaku z myszki lub klawiatury === */
{
    static int znak = -2;
    static int buf_znak = -2;
    int siz;
    int l_events, armed, i;
    pid_t proxy = 0;
    char** buf_adr = NULL;
    //  if(Mouse_ctrl==NULL) return getchar();
    /* fprintf(mystderr,"\nwewy.c - GetChar(): buf_znak=%d znak=%d",buf_znak,znak); */
    if (buf_znak > -2)
    {
        znak = buf_znak;
        buf_znak = -2;
        /* fprintf(mystderr," RETURN %d/%c",znak,znak); */
        return znak;
    }
    buf_znak = -2;
    znak = -2;
    do
    {
        if (odbior_komunik != NULL)
        {
            M_ev = BUF_com;
            siz = BUF_com_size;
        }
        else
        {
            M_ev = &Mouse_ev[0];
            siz = sizeof(struct sd1);
            if (siz < M_BUF_SIZ) siz = M_BUF_SIZ;
        }
        buf_adr = (char**)M_ev;
        buf_adr[0] = NULL;
        buf_adr[1] = NULL;
        w_pid = Receive(0, M_ev, siz); //M_BUF_SIZ);
        /* fprintf(mystderr,"\nwewy-Receive: w_pid=%d Mprx=%d kprx=%d odbior_komunik=%d siz=%d wait_for_synch_trig=%d",w_pid,Mproxy,kproxy,odbior_komunik,siz,wait_for_synch_trig); */
        if (Mouse_ctrl != NULL && w_pid == Mproxy)
        {
            armed = 0;
            while (!armed)
            {
                l_events = mouse_read(Mouse_ctrl, M_ev, MAX_EVENTS, Mproxy, &armed);
                /* fprintf(mystderr," l_events=%d",l_events); */
                for (i = 0; i < l_events; i++, M_ev++)
                {
                    if (M_ev->dy > 6) buf_znak = UP;
                    if (M_ev->dy < -6) buf_znak = DOWN;
                    if (buf_znak > -2) break;
                    if (M_ev->dx < -10)
                    {
                        buf_znak = LEFT;
                        znak = BS;
                        buf_znak = 0;
                    }
                    if (M_ev->dx > 10) buf_znak = FFR;
                    if (buf_znak > -2) break;
                    if (M_ev->buttons > 0)
                    {
                        znak = CR;
                        buf_znak = 0;
                    }
                    if (buf_znak > -2) break;
                }
                if (buf_znak > -2) break;
            }
            mouse_flush(Mouse_ctrl);
            /* fprintf(mystderr, "znak=%d/%c buf_znak=%d/%c",znak,znak,buf_znak,buf_znak); */
            if (buf_znak > 0) znak = SPEC;
        }
        else
        {
            if (w_pid == kproxy)
            {
                int k = 0;
                armed = 0;
                while (!armed)
                {
                    l_events = dev_read(0, kbuf, LCHAR_DEV, 1, 0, 0, kproxy, &armed);
                    for (i = 0; i < l_events; i++, k++)
                    {
                        if (k == 0) znak = kbuf[i];
                        else buf_znak = kbuf[i];
                        /* fprintf(mystderr,"\nDEV: i=%d znak=%c/%d buf_znak=%d armed=%d wait_syn=%d",i,znak,znak,buf_znak,armed,wait_for_synch_trig); */
                    }
                    if ((znak == SPEC || znak == SPEC1) && buf_znak == -2)
                    {
                        armed = 0;
                        continue;
                    }
                }
            }
            else
            {
                znak = sygnal_grafiki(w_pid, &Mouse_ev[0]);
                if (znak > 0)
                {
                    buf_znak = -2;
                    znak = -2;
                } // obsluzono grafiki - dalej czekac
                else
                {
                    if (odbior_komunik != NULL)
                    {
                        nr_komunik = odbior_komunik(BUF_com, w_pid); //GetMessage(w_pid,(char *)buf);
                        /* fprintf(mystderr,"\nwewy.c Po: nr_komunik=%d lwmall=%d",nr_komunik,lwmall); */
                        if (nr_komunik > 0)
                        {
                            if (nr_komunik == CL_READY) wait_for_synch_trig = 0;
                            znak = 0;
                            continue;
                        }
                        if (nr_komunik == TIMEOUT)
                        {
                            komunikat(MY_MAX, X_L0, ATTR_A,
                                      " Brak zadania synchronizowanego z klawiatura !!! <q - exit> ");
                            if (getchar() == 'q') exit(1);
                            else
                            {
                                wait_for_synch_trig = 0;
                                znak = -1;
                                break;
                            }
                        }
                        else
                        {
                            if (nr_komunik == COM_PROGR)
                            {
                                /*fprintf(mystderr," zwoln.");*/
                                znak = 0;
                                continue;
                            }
                            Reply(w_pid, &BUF_com, sizeof(void*));
                            wait_for_synch_trig = 0;
                            znak = -1;
                            break; //continue;
                        }
                    }
                }
            }
        }
        /* fprintf(mystderr,"\nKon: buf_znak=%d znak=%d wait=%d",buf_znak,znak,wait_for_synch_trig);  */
    }
    while (buf_znak == -2 && znak == -2 || wait_for_synch_trig == 1);
    if (znak != SPEC && znak != SPEC1) buf_znak = -2;
    /* fprintf(mystderr," RETURN %d/%c",znak,znak); */
    return znak;
    /* ------- z /usr/sys/mouse.h
     SIGIO
    #define _MOUSE_LEFT     0x0004L
    #define _MOUSE_MIDDLE   0x0002L
    #define _MOUSE_RIGHT    0x0001L
    #define _MOUSE_BUT4     0x0008L
    #define _MOUSE_BUT5     0x0010L
    #define _MOUSE_BUT6     0x0020L
    #define _MOUSE_BUT7     0x0040L
    #define _MOUSE_BUT8     0x0080L
    #define _MOUSE_ABS_MIN      0
    #define _MOUSE_ABS_MAX  16383
    #define _MSEFLG_REL_ARB         0x0000      / * Mouse * /
    #define _MSEFLG_ABS_SCREEN      0x0009      / * Touch screen * /
    #define _MSEFLG_ABS_WORLD       0x000A      / * Digitizing tablet * /
    #define _MSEFLG_TYPE_MASK       0x000F      /* Type mask * /
    #define _MSEFLG_UNCALIBRATED    0x0010      / * Raw coordinates * /
    #define _MSEFLG_Z_DATA          0x0020      / * Some touch screen, pressure * /
    / * /  mouse_event.mode, _mouse_ctrl.mode and _mouse_param.mode explanation * /
    0x0001 RO   Screen related
    0x0002 RO   World related
    0x0008 RO   Absolute Coordinates (not Relative)
    0x0010 RW   Uncalibrated data (for Touch Screen and Digitizing Tablets)
    0x0020 RO   Z Data available
     _ _ _ _._ _ _ _._ _ _ _._ __ _
                                            | | | | | |screen related
                                            | | | | |world related
                                            | | | |undef.
                                            | | |0-relative 1-absolute
                                            | |Uncalibrated data
                                            |Z available
    struct mouse_event
         {short       dx;
            short       dy;
            long        buttons;
            long        timestamp;
            short       dz;
            unsigned short mode;
         } ;

    struct _mouse_param
         {char                    name[32];
            unsigned short          mode;
            long                    buttons;
            short                   threshold;
            short                   gain;
            unsigned short          zero[11];
            };
    */
}
#ifndef _NCURSES_
int m_wherex(void) { return term_state.col; }
int m_wherey(void) { return term_state.row; }
void set_attr(unsigned int attr) { attryb = attr; }
void m_gotoxy(int x, int y)
{
    if (x < x_akt0) x = x_akt0;
    if (x > x_maxOkna) x = x_maxOkna;
    if (y < y_akt0) y = y_akt0;
    if (y > y_maxOkna) y = y_maxOkna;
    x_akt = x;
    y_akt = y;
    /*   term_state.row=y_akt; term_state.col=x_akt; !!!! To ZAKAZANE !!!! */
    term_cur(y_akt, x_akt); /* term_flush(); */
    return;
}
#else
void set_attr(unsigned int attr) { attryb = attr; }
#endif


/*  ------------------------------------------------------------------
/ / ---------------------- Baza grafik ------------------------------- */
int czy_mozna_nowe_wykresy(void*(*akcja_grafik)(int typ_kom, int kto,
                                                char* pdane, char* Text,
                                                void*(*Dane_Wykr)(void* Rd),
                                                void* R))
{
    if (BGR.akcja_grafik != NULL)
    {
        if (BGR.akcja_grafik != akcja_grafik)
        {
            komunikat(MY_MAX, X_L0, ATTR_A, "Wykresy_dla_modeli(): Zadeklarowano zla funkcje dla obsugi grafiki");
            getchar();
            return -BGR.Lgrafs;
        }
    }
    if (BGR.Lgrafs == MAX_GRAFS) return -BGR.Lgrafs;
    return BGR.Lgrafs;
}

int update_wewy_grafs(void* RGn, void* RGs, int raport,
                      void*(*akcja_grafik)(int typ_kom, int kto,
                                           char* pdane, char* Text,
                                           void*(*Dane_Wykr)(void* Rd),
                                           void* R))
{
    int i;
    if (RGn == RGs) return BGR.Lgrafs;
    if (RGn == NULL) /* anulowano wykresy */
    {
        for (i = 0; i < BGR.Lgrafs; i++) { if (BGR.Grafiki[i] == RGs) break; }
        if (i < BGR.Lgrafs)
        {
            BGR.report[i] = -1;
            BGR.Lgrafs--;
            for (; i < BGR.Lgrafs; i++)
            {
                BGR.Grafiki[i] = BGR.Grafiki[i + 1];
                BGR.report[i] = BGR.report[i + 1];
            }
        }
        return BGR.Lgrafs;
    }
    if (BGR.akcja_grafik != NULL)
    {
        if (BGR.akcja_grafik != akcja_grafik)
        {
            komunikat(MY_MAX, X_L0, ATTR_A, "Wykresy_dla_modeli(): Zadeklarowano zla funkcje dla obsugi grafiki");
            getchar();
            return -BGR.Lgrafs;
        }
    }
    if (BGR.Lgrafs < MAX_GRAFS)
    {
        BGR.Grafiki[BGR.Lgrafs] = RGn;
        BGR.report[BGR.Lgrafs] = raport;
        BGR.akcja_grafik = akcja_grafik;
        BGR.Lgrafs++;
        /* fprintf(mystderr,"\n wewy: Nowy RG=%d Lgrafs=%d",RGn,BGR.Lgrafs); */
        return BGR.Lgrafs;
    }
    komunikat(MY_MAX, X_L0, ATTR_A, "Wykresy_dla_modeli(): Brak miejsca na nowe wykresy ");
    getchar();
    return -BGR.Lgrafs;
}

int sygnal_grafiki(pid_t pid, void* D)
{
    int i, n;
    struct sd1* pd1;
    RYS *R, *Rn;
    pd1 = (struct sd1*)D;
    for (i = 0; i < BGR.Lgrafs; i++)
    {
        R = BGR.Grafiki[i];
        for (n = R->rys_min; n <= R->rys_max; n++)
        {
            if (R->aktywny[n] < 0) continue;
            if (R->w_pid[n] == pid)
            {
                Reply(pid, D, 20);
                if (BGR.akcja_grafik != NULL)
                    Rn = BGR.akcja_grafik(pd1->D1.rs_par, getpid(), pd1->BOX.textbx, NULL, NULL, R);
                if (Rn == NULL) update_wewy_grafs(Rn, R, -1, BGR.akcja_grafik);
                break;
            }
        }
        if (n <= R->rys_max) return 1;
    }
    return 0;
}

void zamknij_grafiki(int report)
{
    int i;
    /* fprintf(mystderr,"\nZamknij: Lgrafs=%d report=%d",BGR.Lgrafs,report); */
    for (i = BGR.Lgrafs - 1; i >= 0; i--)
    {
        if (BGR.Grafiki[i] != NULL)
        /* fprintf(mystderr," Gafiki[%d]=%d",i,BGR.Grafiki[i]); */
        {
            if (report < 0 || BGR.report[i] == report)
            {
                if (BGR.akcja_grafik != NULL)
                    BGR.Grafiki[i] = BGR.akcja_grafik(-1, getpid(), NULL, NULL, NULL, BGR.Grafiki[i]);
                BGR.Grafiki[i] = NULL;
                BGR.report[i] = -1;
                BGR.Lgrafs--;
            }
        }
    }
    /* fprintf(mystderr,"\n wewy: Koniec Lgrafs=%d akcja=%d",BGR.Lgrafs, BGR.akcja_grafik); */
    if (BGR.Lgrafs <= 0)
    {
        BGR.Lgrafs = 0;
        BGR.akcja_grafik = NULL;
        BGR.Grafiki[0] = NULL;
    }
    return;
}

#elif defined(_NCURSES_)
/* NCURSES implementations - most functions are in term_ncurses.c */
void zamknij_grafiki(int a)
{
};

int czy_mozna_nowe_wykresy(void*(*akcja_grafik)(int typ_kom, int kto,
                                                char* pdane, char* Text, void*(*Dane_Wykr)(void* Rd), void* R))
{
    return 0;
}

int zamknij_jeden_wykres(void* R, pid_t pid)
{
    return 0;
}

#endif
/*====================================================================== */
int Term_Typef(int y, int x, char* text, int lenth, unsigned int attr)
{
    int cc, l, i;
    /*  if(x<X_L0) x=X_L0; if(x>MX_MAX) x=MX_MAX;
        if(y<Y_G0) y=Y_G0; if(y>MY_MAX) y=MY_MAX;
    */
    if (lenth == 0)
    {
        term_type(y, x, text, 0, attr);
        lenth = strlen(text);
    }
    else
    {
        if (lenth == 1) term_type(y, x, &text[0], 1, attr);
        else
        {
            l = strlen(text);
            if (l >= lenth) term_type(y, x, text, lenth, attr);
            else
            {
                term_type(y, x, text, 0, attr);
                for (i = l; i < lenth; i++) term_type(y, x + i, " ", 1, attr);
            }
        }
    }
    return lenth;
}

/************************************************************************/
int okno_menu(char* opcje[], int kmax, int opcja0,
              unsigned int attr, unsigned int attr_o,
              int Yp, int Xp, int Xm, char* tytul, char ramka)
{
    unsigned int att;
    int k_inv, i, y, dl_tyt, l_linii, k, tx_size, ret, ip, k_ost;
    int xp, xm, yp, ym, Ym, y_kur, x_kur, n, ik, x, ip_old;
    char znak, **buf, *text, *koniec = "<Esc>", jest;
    if (Xp < X_L0) Xp = X_L0;
    if (Xm > MXR_MAX) Xm = MXR_MAX;
    if (Yp < Y_G0) Yp = Y_G0;
    tx_size = Xm - Xp;
    dl_tyt = 0;
    if (tytul != NULL) dl_tyt = strlen(tytul);
    if (tx_size < dl_tyt)
    {
        for (i = 0; i < kmax; i++)
        {
            y = strlen(opcje[i]);
            if (y > tx_size) tx_size = y;
        }
        k = strlen(koniec);
        if (tx_size < k) tx_size = k;
        if (tx_size < dl_tyt) tx_size = dl_tyt;
        tx_size += 2;
        if (ramka > 0) tx_size += 2;
        if (tx_size > MXR_MAX)
        {
            komun_text(MY_MAX, X_L0, " Za dlugi tekst opcji w menu. <Ent>", 0,ATTR_A);
            GET_char();
        }
        Xm = Xp + tx_size - 1;
    }
    if (Xm > MXR_MAX)
    {
        i = Xm - MXR_MAX;
        Xp -= i;
        Xm = MXR_MAX;
        if (Xp < X_L0)
        {
            tx_size -= Xp;
            Xm -= Xp;
            Xp = X_L0;
        }
    }
    xp = Xp;
    yp = Yp;
    xm = Xm;
    if (ramka > 0)
    {
        xp++;
        xm--;
    }
    Ym = Yp + kmax;
    ym = Ym;
    if (dl_tyt > 0)
    {
        Ym++;
        yp++;
        ym++;
    }
    if (ramka > 0 && dl_tyt > 0) Ym++;
    if (dl_tyt == 0 && ramka > 0)
    {
        yp++;
        ym++;
        Ym += 2;
    }
    if (Ym > MYR_MAX)
    {
        Ym = MYR_MAX;
        ym = MYR_MAX;
        if (ramka > 0) ym = MYR_MAX - 1;
    }
    l_linii = Ym - Yp + 1;
    blok_anim->xp = Xp;
    blok_anim->yp = Yp;
    blok_anim->xm = Xm;
    blok_anim->ym = Ym;
    buf = Malloc(l_linii * sizeof(char**));
    if (buf == NULL)
    {
        komun_text(MY_MAX, 1, " Brak pamieci dla menu. <Ent>", 0,ATTR_A);
        GET_char();
        return -1;
    }
    lwmall++;
    attr = attr & (~TERM_FLUSH);
    attr_o = attr_o & (~TERM_FLUSH);
    tx_size++;
    k = term_save_image(yp, xp, "a", 0);
    for (y = Yp; y <= Ym; y++)
    {
        n = y - Yp;
        buf[n] = (char*)Malloc(tx_size * k + 1);
        if (buf[n] == NULL)
        {
            komun_text(MY_MAX, 1, " Brak pamieci dla menu. <Ent>", 0,ATTR_A);
            for (k = 0; k < n; k++)
            {
                Free(buf[k]);
                lwmall--;
            }
            Free(buf);
            lwmall--;
            GET_char();
            return -1;
        }
        lwmall++;
        term_save_image(y, Xp, buf[n], tx_size);
    }
    /*fprintf(mystderr,"\nokno_menu(przed Okno): Yp,Xp,Ym,Xm=%d %d %d %d; x_maxOkna=%d y_maxOkna=%d; X_L0,Y_G0,MXR_MAX,MY_MAX=%d %d %d %d",Yp,Xp,Ym,Xm,x_maxOkna,y_maxOkna,X_L0,Y_G0,MXR_MAX,MY_MAX); */
    Okno(Yp, Xp, Ym, Xm, attr);
    if (ramka == 1) maluj_ramke(Yp, Xp, Ym, Xm, attr);
    if (dl_tyt > 0)
    {
        i = xp + (xm - xp + 1 - dl_tyt) / 2;
        term_type(Yp, i, tytul, 0, attr);
    }
    if (opcja0 < 0) opcja0 = 0;
    if (kmax > 0) opcja0 = opcja0 % kmax;
    else opcja0 = 0;
    ip = 0;
    k_inv = opcja0;
    l_linii = ym - yp;
    ip_old = -1;
    jest = 0;
    k_ost = k_inv;
    ik = ip + l_linii;
    if (k_inv > ik) ip += (k_inv - ik);
    do
    {
        ik = ip + l_linii;
        y = yp;
        for (i = ip; (i <= ik) || (jest != 0); i++, y++)
        {
            if (ip == ip_old)
            {
                switch (jest)
                {
                case 0: i = k_ost;
                    y = yp + i - ip;
                    jest = 1;
                    break;
                case 1: i = k_inv;
                    y = yp + i - ip;
                    jest = 2;
                    break;
                case 2: jest = 0;
                    goto W;
                }
            }
            att = attr;
            if (i == k_inv)
            {
                att = attr_o;
                x_kur = xp + 2;
                y_kur = y;
            }
            if (i < kmax) text = opcje[i];
            else text = koniec;
            znak = EOS;
            k = strlen(text);
            if (k + xp > xm - 1)
            {
                znak = text[xm - xp - 1];
                text[xm - xp - 1] = EOS;
            }
            term_type(y, xp, " ", 1, att);
            term_type(y, xp + 1, text, 0, att);
            if (znak != EOS) text[xm - xp - 1] = znak;
            for (x = m_wherex(); x <= xm; x++) term_type(y, x, " ", 1, att);
        }
        /* fprintf(mystderr," y_cur=%d x_cur=%d k_inv=%d",y_kur,x_kur,k_inv); */
    W:
        do
        P:
            {
                term_cur(y_kur, x_kur);
                k_ost = k_inv;
                term_flush();

                ret = GET_char();
                ret = ret & 0x00ff;

                switch (ret)
                {
                case NLCR: ret = CR/*Get_Char()*/;
                case CR: if (k_inv == kmax) ret = Esc;
                    goto D;
                case Esc: ret = Esc;
                    goto D;
                case BS: k_inv--; /*if(k_inv<0) k_inv=0;*/
                    break;
                case SPEC:
                    ret = GETchar();
                    ret = ret & 0x00ff; /*  printf(" drugi=%d",ret); */
                    switch (ret)
                    {
                    case Home: k_inv = 0;
                        break;
                    case End: k_inv = kmax;
                        break;
                    case PgUp: k_inv -= 5;
                        break;
                    case PgDn: k_inv += 5;
                        break;
                    case LEFT:
                    case UP: k_inv--;
                        break;
                    case FFR:
                    case DOWN: k_inv++;
                        break;
                    default: goto P;
                    }
                    break;
                default: goto P;
                }
                if (k_inv < 0) k_inv = 0;
                if (k_inv > kmax) k_inv = kmax;
                if (k_inv < ip) ip = k_inv;
                if (k_inv > ik) ip += (k_inv - ik);
            }while (k_inv == k_ost);
    D:
        continue;
    }
    while (ret != Esc && ret != CR);
    if (ret == Esc) k_inv = -1;
    for (y = Yp; y <= Ym; y++)
    {
        n = y - Yp;
        term_restore_image(y, Xp, buf[n], tx_size);
        Free(buf[n]);
        lwmall--;
    }
    Free(buf);
    lwmall--;
    blok_anim->ym = -1;
    return (k_inv);
}

/* ===================================================================== */
void maluj_ramke(int yp, int xp, int ym, int xm, unsigned int attr)
{
    int i;
    if (xp < X_L0) xp = X_L0;
    if (yp < Y_G0) yp = Y_G0;
    if (xm > MXR_MAX) xm = MXR_MAX;
    if (ym > MYR_MAX) ym = MYR_MAX;
#ifdef GRAF
    if (igraf != 0) ramka_graficzna(yp, xp, ym, xm, attr);
    else
#endif
    {
        term_type(yp, xp, "�", 1, attr);
        for (i = xp + 1; i < xm; i++) term_type(yp, i, "�", 1, attr);
        term_type(yp, xm, "�", 1, attr);
        for (i = yp + 1; i < ym; i++)
        {
            term_type(i, xp, "�", 1, attr);
            term_type(i, xm, "�", 1, attr);
        }
        term_type(ym, xp, "�", 1, attr);
        for (i = xp + 1; i < xm; i++) term_type(ym, i, "�", 1, attr);
        term_type(ym, xm, "�", 1, attr);
    }
}

/* ====================================================================== */
void Okno(int y0, int x0, int ymax, int xmax, unsigned int attr)
{
    if (x0 < X_L0) x0 = X_L0;
    if (x0 >= (MXR_MAX)) x0 = MXR_MAX - 1;
    if (y0 < Y_G0) y0 = Y_G0;
    if (y0 > MY_MAX) y0 = MY_MAX;
    if (xmax < X_L0) xmax = X_L0;
    if (xmax > MXR_MAX) xmax = MXR_MAX;
    if (ymax < Y_G0) ymax = Y_G0;
    if (ymax > MY_MAX) ymax = MY_MAX;
    x_akt0 = x0;
    x_akt = x0;
    y_akt = y0;
    y_akt0 = y0;
    y_maxOkna = ymax;
    x_maxOkna = xmax;
    x_min = x0;
    y_min = y0;
    attryb = attr;
    /*fprintf(mystderr," X_maxBlank=%d/%d ymax=%d y_maxOkna=%d Y_maxBlankO2=%d",X_maxBlank,x_maxOkna,ymax,y_maxOkna,Y_maxBlank); */
#ifdef GRAF
    if (igraf != 0) ustaw_okno_graficzne();
#endif
    /*fprintf(mystderr," X_maxBlank=%d/%d ymax=%d y_maxOkna=%d Y_maxBlankOK=%d",X_maxBlank,x_maxOkna,ymax,y_maxOkna,Y_maxBlank);*/
}

/* ---------------------------------------------------------------------- */
void clr_Okno(void)
{
    int x, y;
#ifdef GRAF
    if (igraf != 0)
    {
        term_color(attryb);
        bar(x0_cur, y0_cur, xmax_cur + char_width, ymax_cur + char_height); //clearviewport();
    }
    else
#endif
    {
        for (y = y_akt0; y <= y_maxOkna; y++)
        {
            for (x = x_akt0; x <= x_maxOkna; x++) term_type(y, x, " ", 1, attryb);
        }
        m_gotoxy(x_akt0, y_akt0);
    }
}

/* ---------------------------------------------------------------------- */
int komunikat(int y, int x, unsigned int attr, char* format, ...)
{
    int l, i = 0;
    char bufor[120], *text = NULL;
    va_list arg;
    va_start(arg, format);
    vsprintf(bufor, format, arg);
    va_end(arg);
    l = strlen(bufor);
    if (x + l >= MX_MAX) bufor[MX_MAX - x - l] = 0;
    for (i = x + l; i < MX_MAX; i++) term_type(y, i, " ", 1, attr);
    term_type(y, x, bufor, 0, attr);
    term_color(TERM_WHITE | TERM_BLACK_BG);
    term_type(y, MXR_MAX, " ", 1,TERM_WHITE | TERM_BLACK_BG);
    return m_wherex();
}

int komun_text(int y, int x, char* bufor, int lz, unsigned int attr)
{
    int l, i = 0;
    l = strlen(bufor);
    if (x + l >= MX_MAX) bufor[MX_MAX - x - l] = 0;
    for (i = x + l; i < MX_MAX; i++) term_type(y, i, " ", 1, attr);
    term_type(y, x, bufor, lz, attr);
    term_color(TERM_WHITE | TERM_BLUE_BG);
    term_type(y, MXR_MAX, " ", 1,TERM_WHITE | TERM_BLACK_BG);
    return m_wherex();
}


void Clr_eol(void)
{
    int x;
    y_akt = m_wherey();
    x_akt = m_wherex();
    for (x = x_akt; x <= x_maxOkna; x++) term_type(y_akt, x, " ", 1, attryb);
    m_gotoxy(x_akt, y_akt);
}

void wylacz_potwierdzanie(void)
{
    blokada_potwierdzania = 1;
    bez_potwierdzania = 1;
}

void wpis_danych_bez_potwierdz(void) { bez_potwierdzania = 1; }

void wpis_danych_z_potwierdz(void)
{
    if (blokada_potwierdzania == 0) bez_potwierdzania = 0;
}

void zapis_zblokowany(char tryb_zapisu) { blokada_zapisu = tryb_zapisu; }
void haslo_nieaktywne(void) { BEZ_HASLA = 1; }
void zapis_bezwzgledny(void) { ZAPIS = 2; }
void haslo_aktywne(void) { BEZ_HASLA = 0; }

void wewy_abort_off(void)
{
    ABORT = 0;
    BLAD = 0;
}

void wewy_abort_on(void)
{
    ABORT = 1;
    BLAD = 1;
}

void sledzenie_off(void) { SLEDZIC = 0; }
void sledzenie(void) { SLEDZIC = 1; }
void ustaw_wybor_na_ost_danej(void) { dana_poczatkowa = 9999; }
void ustaw_wybor_normalnie(void) { dana_poczatkowa = 0; }

void ustaw_wybor_na_danej(int nr)
{
    if (nr < 0) nr = 0;
    dana_poczatkowa = nr;
}

void przepisz(char* do_ob, char* z_ob, int ile)
{
    char* adr_max;
    adr_max = z_ob + ile;
    for (; z_ob < adr_max; do_ob++, z_ob++) *do_ob = *z_ob;
}

char* szukaj_formatow(char* format, char typ, char** zp)
{
    char *z, *c, k;
    int i = 0, zn;
    z = format;
    k = 0;
    if (z == NULL) return z;
    do
    {
        z = szukaj(z, '%');
        if (z != NULL)
        {
            zn = czy_format(z, &i);
            if (zn == EOS)
            {
                z++;
                continue;
            }
            if (typ != zn)
            {
                komunikat(MY_MAX, 1,ATTR_A, " Bledny format %s w okno_tabl_..(). <Ent> ",
                          format);
                GET_char();
                z = NULL;
                break;
            }
            if (k == 0) *zp = z;
            c = z + i;
            k++;
            z = c; /* szukamy nowego formatu od adresu z */
        }
    }
    while (z != NULL && k < 2);
    return z;
}

void setalarm(void) { alarmclock = 1; }
void clearalarm(void) { alarmclock = 0; }

int save_var(int buf[])
{
    int siz = 0;
    if (buf == NULL) return 31 + sizeof(struct Blok_anim) / sizeof(int) + 1;
    buf[0] = X_0;
    buf[1] = Y_0;
    buf[2] = X_maxBlank;
    buf[3] = Y_maxBlank;
    buf[4] = x_maxOkna;
    buf[5] = y_maxOkna;
    buf[6] = x_akt;
    buf[7] = y_akt;
    buf[8] = x_akt0;
    buf[9] = y_akt0;
    buf[10] = x_min;
    buf[11] = y_min;
    buf[12] = Uprawn_max;
    buf[13] = Uprawn_pocz;
    buf[14] = bez_potwierdzania;
    buf[15] = ZAPIS;
    buf[16] = blokada_potwierdzania;
    buf[17] = Uprawn_max;
    buf[18] = Uprawn_pocz;
    buf[19] = bez_potwierdzania;
    buf[20] = blokada_potwierdzania;
    buf[21] = BEZ_HASLA;
    buf[22] = EDIT;
    buf[23] = SLEDZIC;
    buf[24] = blokada_zapisu;
    buf[25] = dana_poczatkowa;
    buf[26] = attryb;
    buf[27] = m_wherex();
    buf[28] = m_wherey();
    buf[29] = xg_akt;
    buf[30] = yg_akt;
    siz = sizeof(struct Blok_anim);
    przepisz((char*)&buf[31], (char*)blok_anim, siz);
    siz = 31 + siz / sizeof(int) + 1;
    return siz;
}

int restore_var(int buf[])
{
    int siz;
    if (buf == NULL) return -1;
    X_0 = buf[0];
    Y_0 = buf[1];
    X_maxBlank = buf[2];
    Y_maxBlank = buf[3];
    x_maxOkna = buf[4];
    y_maxOkna = buf[5];
    x_akt = buf[6];
    y_akt = buf[7];
    x_akt0 = buf[8];
    y_akt0 = buf[9];
    x_min = buf[10];
    y_min = buf[11];
    Uprawn_max = buf[12];
    Uprawn_pocz = buf[13];
    bez_potwierdzania = buf[14];
    ZAPIS = buf[15];
    blokada_potwierdzania = buf[16];
    Uprawn_max = buf[17];
    Uprawn_pocz = buf[18];
    bez_potwierdzania = buf[19];
    blokada_potwierdzania = buf[20];
    BEZ_HASLA = buf[21];
    EDIT = buf[22];
    SLEDZIC = buf[23];
    blokada_zapisu = buf[24];
    dana_poczatkowa = buf[25];
    attryb = buf[26];
    term_cur(buf[28], buf[27]);
    xg_akt = buf[29];
    yg_akt = buf[30];
    siz = sizeof(struct Blok_anim);
    przepisz((char*)blok_anim, (char*)&buf[31], siz);
    return 31 + siz / sizeof(int) + 1;
}


char czy_format(char* form, int* cd)
{
    int i = 1, j, k = 0;
    *cd = 2;
    if (form[i] == 's' || form[i] == 'S' || form[i] == 'd' || form[i] == 'c') return form[i];
    if (form[i] == 'f' || form[i] == 'i' || form[i] == 'e') return form[i];
    if (form[i] == ' ') return EOS;
    if (form[i] == '+' || form[i] == '-')
    {
        i = 2;
        *cd = 3;
    }
    for (j = i; j < 10; j++, (*cd)++)
    {
        if (form[j] >= '0' && form[j] <= '9') continue;
        if (form[j] == 's' || form[j] == 'S' || form[j] == 'd' || form[j] == 'i' || form[i] == 'c')
        {
            if (k == 0) return form[j];
            else
            {
                komunikat(5, 1,ATTR_A, " Bledny format %s. <Ent>", form);
                GET_char();
            }
            *cd = j + 1;
            return EOS;
        }
        if (form[j] == 'f' || form[j] == 'e')
        {
            if (k == 1)
            {
                *cd = j + 1;
                return form[j];
            }
            else
            {
                komunikat(5, 1,ATTR_A, " Bledny format %s. <Ent>", form);
                GET_char();
            }
            return EOS;
        }
        if (form[j] == '.')
        {
            if (k == 0)
            {
                k = 1;
                continue;
            }
            else
            {
                komunikat(5, 1,ATTR_A, " Bledny format %s. <Ent>", form);
                GET_char();
            }
            return EOS;
        }
        return EOS;
    }
    return EOS;
}

/* ------------------------------------------------------------------- */
/* =================================================================== */
int otworz_raporty(char nr_op)
{
    int i, zn;
    /*  delay(800); */
    blok_anim->ym = -1;
    Uprawn_max = 0;
    Uprawn_pocz = 0;
    Kod_operatora_max = NULL;
    bez_potwierdzania = 0;
    blokada_potwierdzania = 0;
    X_0 = X_L0;
    Y_0 = Y_G0;
    X_maxBlank = MXR_MAX;
    Y_maxBlank = MY_MAX;
    x_maxOkna = MXR_MAX;
    y_maxOkna = MY_MAX;
    x_akt = X_0;
    y_akt = Y_0;
    x_akt0 = X_0;
    y_akt0 = Y_0;
    x_min = X_0;
    y_min = Y_0;
    wewy_abort_off();
    dana_poczatkowa = 0;
    SCROLL = -1;
    arrow = 27;
    if (BUF_com == NULL || BUF_com_size == 0 || odbior_komunik == NULL)
    {
        BUF_com = NULL;
        BUF_com_size = 0;
        if (real_time == 0) odbior_komunik = NULL;
    }
#ifdef _TERM_QNX_
	else
    {
        if (Mouse_ctrl != NULL) mouse_read(Mouse_ctrl, BUF_com, 0, Mproxy, NULL);
    }
    if (_TYP_KONSOLI_ == 2) arrow = 26;
    /*  ......  if(_TYP_KONSOLI_==2) Mouse_ctrl=NULL; */
    if (kproxy < 0)
    {
        open_konsole_mouse(0);
        if (kproxy > 0) mouse_free = 1;
    }
    if (real_time == 1)
    {
        BUF_com = &Mouse_ev[0];
        BUF_com_size = M_BUF_SIZ;
    }
    wait_for_synch_trig = 0;
    BGR.Lgrafs = 0; BGR.akcja_grafik = NULL; BGR.Grafiki[0] = NULL; BGR.report[0] = -1;
#endif
    lhasel = 0;
    ZAPIS = 0;
    BEZ_HASLA = 0;
    EDIT = 0;
    SLEDZIC = 0;
    blokada_zapisu = ZAPIS_DOWOLNY;
    if (Klucz != -1)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Nie zamkniete raporty poprzedniej sesji    <Ent>    ");
        GET_char();
        return -1;
    }
    for (i = 0; i < LHASEL; i++)
    {
        haslo[i][0] = EOS;
        kod_o[i][0] = EOS;
    }
    if (lwmall > 0)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Pozostaly alokacje pamieci z poprzedniej sesji (%d) <Ent>  ",
                  lwmall);
        GET_char();
    }
    lwmallp = lwmall;
    if (nr_blank >= 0 || Czy_Blankiet_otwarty != 0)
    {
        komunikat(MY_MAX, 1,ATTR_A,
                  "Nie zamkniete blankiety (ostatni %d, stan %d) z poprzedniej sesji: <q> - koniec; <d> - dalej ?? ",
                  nr_blank, Czy_Blankiet_otwarty);
        do
        {
            zn = GET_char();
            zn = zn & 0x00ff;
        }
        while (zn != 'q' && zn != 'd');
        if (zn == 'q') return -1;
        do
        {
            i = BL[nr_blank]->nr_rekordu;
            zamknij_blankiet(&i);
        }
        while (nr_blank >= 0);
    }
    Czy_Blankiet_otwarty = 0;
    nr_blank = -1;
    for (i = 0; i <= LRAP_BL; i++)
    {
        RAP[i] = NULL;
        if (RAP[i] != NULL)
        {
            komunikat(MY_MAX, 1,ATTR_A, "Nie zamkniety raport %d z poprzedniej sesji: <q> - koniec; <d> - dalej ?? ",
                      i);
            do
            {
                zn = GET_char();
                zn = zn & 0x00ff;
            }
            while (zn != 'q' && zn != 'd');
            if (zn == 'q') return -1;
        }
    }
    for (i = 0; i < LBLANK; i++) BL[i] = NULL;
    NR_OPER = nr_op;
    Klucz = 0;
#ifdef _DOS_
#ifndef GRAF
    window(1, 1, 80, 25);
#else
    if (igraf == 0) window(1, 1, 80, 25);
#endif
#endif
    return i;
}

/* ------------------------------------------------------------------- */
signed char zamknij_blankiet(int* nr_rekordu)
{
    int i, k;
    signed char nr_raportu = -1, ramka;
    struct okno *ok, *Okn;
    i = 0;
    k = 0;
    Okn = Ok;
    Czy_Blankiet_otwarty = 0;
    do
    {
        if (nr_blank < 0)
        {
            if (Ok != NULL)
            {
                ok = Ok;
                for (i = 0; i < liczba_danych; i++, ok++)
                {
                    if (ok->typ == 'k' && ok->format != NULL)
                    {
                        Free(ok->format);
                        lwmall--;
                    }
                }
                Free(Ok);
                lwmall--;
            }
            Ok = NULL;
            return -1;
        }
        i = 0;
        if (BL[nr_blank] == NULL)
        {
            komunikat(5, 1,ATTR_A, " Zla specyfikacja blankietu %d: <Ent>", nr_blank);
            nr_blank--;
            i = 1;
            k = 1;
            GET_char();
        }
    }
    while (i == 1);
    Okn = BL[nr_blank]->Okbl;
    if (Ok == NULL) Ok = Okn;
    if (Ok != Okn)
    {
        komunikat(5, 1,ATTR_A, " Blankiet %d Ok=%d: <Ent>", nr_blank, Ok);
        ok = Ok;
        for (i = 0; i < liczba_danych; i++, ok++)
        {
            if (ok->typ == 'k' && ok->format != NULL)
            {
                Free(ok->format);
                lwmall--;
            }
        }
        Free(Ok);
        lwmall--;
        GET_char();
    }
    Czy_Blankiet_otwarty = 2;
    if (k == 0)
    {
        ok = Ok;
        for (i = 0; i < liczba_danych; i++, ok++)
        {
            if (ok->typ == 'k' && ok->format != NULL)
            {
                Free(ok->format);
                lwmall--;
            }
        }
        Free(Ok);
        Ok = NULL;
        lwmall--;
        if (*nr_rekordu != BL[nr_blank]->nr_rekordu)
        {
            komunikat(MY_MAX, X_L0,ATTR_A,
                      " Niezgodnosc rekordu dekl.(%d) z zapisem (%d) w zamykanym blankiecie %d. <ENT> ",
                      *nr_rekordu, BL[nr_blank]->nr_rekordu, nr_blank);
            GET_char();
        }
        if (nr_blank == 0)
        {
        }
        Free(BL[nr_blank]);
        BL[nr_blank] = NULL;
        lwmall--;
        nr_blank--;
        nr_danej = 0;
        X_0 = X_L0;
        Y_0 = Y_G0;
        X_maxBlank = MXR_MAX;
        Y_maxBlank = MY_MAX;
        HELP = 0;
    }
    if (nr_blank >= 0)
    {
        Czy_Blankiet_otwarty = 2;
        Ok = BL[nr_blank]->Okbl;
        ramka = BL[nr_blank]->ramka;
        Akt_Kolor = BL[nr_blank]->Kolor;
        X_maxBlank = BL[nr_blank]->X_max;
        X_0 = BL[nr_blank]->X_0;
        Y_maxBlank = BL[nr_blank]->Y_max;
        Y_0 = BL[nr_blank]->Y_0;
        Uprawn = BL[nr_blank]->Uprawn;
        nr_danej = BL[nr_blank]->nr_danej;
        nr_raportu = BL[nr_blank]->nr_raportu;
        *nr_rekordu = BL[nr_blank]->nr_rekordu;
        if (nr_raportu < 0 || nr_raportu > LRAP_BL)
        {
            komunikat(5, 1,ATTR_A, " Zla specyfikacja blankietu %d (nr_raportu=%d): <Ent>",
                      nr_blank, nr_raportu);
            if (nr_blank >= 0) nr_blank--;
            GET_char();
            return -1;
        }
        if (RAP[nr_raportu] != NULL) return RAP[nr_raportu]->kod_raportu;
        else return -1;
    }
    Czy_Blankiet_otwarty = 0;
    return -1;
}

/* ------------------ Nowa biblioteczka wczytaj_..liczbe() ---------- */
/* ===================================================================== */
#define WLMAX 80
#include <ctype.h>
/* ---------------- Macro do wczytywania liczb ------------------- */
/* == bscanf zwaraca: -2 <Esc>; -1 <Ent>; 0 pusty; >0 l.znakow w buf === */
#define macro_wczyt(form)\
	if(oblig==0)\
	 {sprintf(buf,format,liczba);\
		term_type(y,x0,buf,size,attryb); term_cur(y,x0);\
	 }\
	else {buf[0]=0; if(size>1) INSERT=-1;}\
	while(1)\
	 {ret=bscanf(buf,attryb,form,&liczba);\
		if(ret==-1 && oblig==1)\
		 {term_type(y,x0-1,"?",1,ATTR_A); term_cur(y,x0); continue;}\
		if(ret<-1) liczba=w_def;\
		break;\
	 }\
	if(liczba<w_min) liczba=w_min; if(liczba>w_max) liczba=w_max;\
	term_printf(y,x0,attryb,format,liczba);\
	attryb=attr_old;	term_color(attryb);
/* --------------------- KONIEC MACRA ----------------------------- */
char ustaw_buf_wpisu(int y, int x, char* monit, int* size,
                     unsigned int attr_monit, unsigned int attr_wpis)
{
    int dl, dlug, x0;
    char znak = 0;
    wewy_abort_off();
    INSERT = 1;
    if (attr_monit == 0) attr_monit = attryb;
    if (attr_wpis == 0) attr_wpis = attryb;
    if (y < 0) y = m_wherey();
    if (x < 0) x = m_wherex();
    if (y < Y_G0) y = Y_G0;
    if (x < X_L0) x = X_L0;
    if (y > MY_MAX) y = MY_MAX;
    if (x > MX_MAX) x = MX_MAX;
    dlug = strlen(monit);
    if ((*size) < 1) (*size) = 1;
    dl = MX_MAX - dlug - (*size);
    if (dl < 0) dlug -= dl;
    znak = monit[dlug];
    monit[dlug] = znak;
    if ((dlug + (*size) + x) > MX_MAX)
    {
        if (y == MY_MAX) x = X_L0;
        else
        {
            y++;
            x = X_L0;
        }
    }
    if (dlug > 0)
    {
        term_type(y, x, monit, dlug, attr_monit);
        x = m_wherex();
    }
    else { term_cur(y, x); }
    if (znak != 0) monit[dlug] = znak;
    Okno(y, x, y, x + (*size) - 1, attr_wpis);
    clr_Okno();
    return m_wherex();
}

/* ---------------------------------------------------------------- */
int wczytaj_int(int y, int x, char* monit, int size, int w_def,
                int w_min, int w_max,
                unsigned int attr_monit, unsigned int attr_wpis)
{
    int x0, ret, i;
    char buf[WLMAX], format[20], oblig = 0;
    int liczba;
    unsigned int attr_old;
    if (size >= WLMAX) size = WLMAX - 1;
    attr_old = attryb;
    if (size >= WLMAX) size = WLMAX - 1;
    if (w_def < w_min)
    {
        w_def = w_min;
        oblig = 1;
    }
    if (w_def > w_max)
    {
        w_def = w_max;
        oblig = 1;
    }
    liczba = w_def;
    if (monit[0] == '#') oblig = 1;
    x0 = ustaw_buf_wpisu(y, x, monit, &size, attr_monit, attr_wpis);
    y = m_wherey();
    sprintf(format, "%%%dd", size);
    macro_wczyt("%d");
    return liczba;
}

/* ---------------------------------------------------------------- */
float wczytaj_float(int y, int x, char* monit, int size, int prec,
                    float w_def, float w_min, float w_max,
                    unsigned int attr_monit, unsigned int attr_wpis)
{
    int x0, ret, i;
    char buf[WLMAX], format[20], oblig = 0;
    float liczba;
    unsigned int attr_old;
    if (size >= WLMAX) size = WLMAX - 1;
    attr_old = attryb;
    if (w_def < w_min)
    {
        w_def = w_min;
        oblig = 1;
    }
    if (w_def > w_max)
    {
        w_def = w_max;
        oblig = 1;
    }
    liczba = w_def;
    if (monit[0] != '#') oblig = 1;
    x0 = ustaw_buf_wpisu(y, x, monit, &size, attr_monit, attr_wpis);
    y = m_wherey();
    if (prec >= size) prec = size - 1;
    if (prec < 0) prec = 0;
    sprintf(format, "%%%d.%df", size, prec);
    macro_wczyt("%f")
    return liczba;
}

/* ---------------------------------------------------------------- */
char wczytaj_char(int y, int x, char* monit, unsigned char w_def,
                  unsigned int attr_monit, unsigned int attr_wpis)
{
    unsigned int attr_old;
    unsigned char format[5], buf[2], liczba, w_min = 0, w_max = 'z';
    int oblig = 0, size = 1, x0, ret, i;
    if (!isalnum(w_def)) { w_def = w_max; }
    attr_old = attryb;
    liczba = w_def;
    if (monit[0] == '#') oblig = 1;
    x0 = ustaw_buf_wpisu(y, x, monit, &size, attr_monit, attr_wpis);
    y = m_wherey();
    sprintf(format, "%%c");
    macro_wczyt("%c");
    return liczba;
}

char wczytaj_string(int y, int x, char* monit, char* text, int size,
                    unsigned int attr_monit, unsigned int attr_wpis)
{
    int x0, ret, strl, i;
    char buf[WLMAX], *format, oblig = 0;
    unsigned int attr_old;
    unsigned char liczba, w_min = 0, w_max = 128;
    if (text == NULL) return 0;
    if (size >= WLMAX) size = WLMAX - 1;
    attr_old = attryb;
    if (monit[0] == '#') oblig = 1;
    x0 = ustaw_buf_wpisu(y, x, monit, &size, attr_monit, attr_wpis);
    y = m_wherey();
    strl = strlen(text);
    /*	if(strl==0)
         {for(i=0; i<size;i++) text[i]=' ';
            text[size]=EOS; strl=size; oblig=1;
         } */
    if (strl > size)
    {
        text[size] = EOS;
        strl = size;
        oblig = 1;
    }
    /*	macro_wpis("%c"); */
    if (oblig == 0)
    {
        sprintf(buf, "%s", text);
        term_type(y, x0, buf, size, attryb);
        term_cur(y, x0);
    }
    else
    {
        buf[0] = EOS;
        if (size > 1) INSERT = -1;
    }
    while (1)
    {
        ret = czytaj_we(NULL, buf, strlen(buf), size);
        /* == bscanf zwaraca: -2 <Esc>; -1 <Ent>; 0 pusty; >0 l.znakow w buf === */
        if (ret == -1 && oblig == 1) /* <Ent> */
        {
            term_type(y, x0 - 1, "?", 1,ATTR_A);
            term_cur(y, x0);
            continue;
        }
        if (ret < -1)
        {
            attryb = attr_old;
            term_color(attryb);
            term_cur(y, x0);
            return 0;
        } /* <Esc> */
        for (i = 0; i <= strlen(buf); i++) text[i] = buf[i];
        break;
    }
    term_printf(y, x0, attryb, "%s", text);
    strl = strlen(text);
    attryb = attr_old;
    term_color(attryb);
    term_cur(y, x0 + strl);
    return strl;
}

int monit_textowy(int yp, int xp, unsigned int attr, char* text)
{
    int k, tx_size, ret, dl;
    char *buf, znak = 0;
    int xkursora, ykursora;
    unsigned int old_attr;
    xkursora = m_wherex();
    ykursora = m_wherey();
    setcursor(nocursor);
    old_attr = attryb;
    k = term_save_image(yp, xp, "a", 0);
    tx_size = strlen(text);
    dl = MX_MAX - (xp + tx_size);
    if (dl < 0)
    {
        tx_size += dl;
        znak = text[tx_size];
        text[tx_size] = EOS;
    }
    buf = (char*)Malloc(tx_size * k + 1);
    if (buf != NULL)
    {
        lwmall++;
        term_save_image(yp, xp, buf, tx_size);
    }
    term_type(yp, xp, text, 0, attr);
    ret = GET_char();
    if (buf != NULL)
    {
        term_restore_image(yp, xp, buf, tx_size);
        Free(buf);
        lwmall--;
    }
    text[tx_size] = znak;
    attryb = old_attr;
    setcursor(cursor);
    m_gotoxy(xkursora, ykursora);
    return ret;
}

#define WEWY_INCL
/* =========================== include dla wersji dos'owej =========
#include "wewy1.c"
#include "wewy2.c"
#include "wewy3.c"
#include "wewy4.c"
#include "wewy5.c"
 ================================================================== */
#ifndef WEWY0_INCL
/* ================### Tu zaczyna sie plik wewy1.c ================== */
#ifndef WEWY_INCL
#define _DOS_
#include <stdio.h>
#include <process.h>
#include <conio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "../opcjesys/intdef.h"
#ifdef _DOS_
#include <alloc.h>
#include <dos.h>
#define Malloc farmalloc
#define Free farfree
#else
#define Free free
#include <malloc.h>
#include <sys/qnxterm.h>
#endif
#include "wewybl.h"
#define WEW_EXT extern
#include "WEWY1.H"
extern int (*odbior_komunik)(void* b, pid_t a);
extern int BUF_com_size, nr_komunik;
extern void* BUF_com;
extern char real_time;
#endif
/* =================================================================== */
int okno_znak(signed char wpis, int y, int x0,
              unsigned int attr_d, unsigned int attr,
              unsigned int attrtx, char* znak, char* monit_pocz,
              char opcje[], char* objasn[], int liczba_obj)
{
    int index = 0;
    return okno_tabl_znak(wpis, y, x0, attr_d, attr, attrtx, &index, znak, 0,
                          monit_pocz, 0, 0, opcje, NULL, objasn, liczba_obj);
}

int objasn_grupy(int y, int x, int w_min, int w_max, int wartn,
                 unsigned int attr, unsigned int attrtx, long int skok,
                 int dltn, void* R, char** help)
{
    int x_k_help;
    if (wartn < w_min || wartn > w_max)
    {
        disp_help(y, x, attrtx, attr, w_min, skok, R, help, X_maxBlank - dltn + 1); /*  ### +1 */
        x_k_help = m_wherex() - 1;
        Okno(y, x, y, x_k_help, attr);
        clr_Okno();
        term_type(y, x, " ", 1, attrtx);
        Term_Typef(y, x + 1, " ??? ", 5, attr);
    }
    else
    {
        disp_help(y, x, attrtx, attr, wartn, skok, R, help, X_maxBlank - dltn + 1);
        x_k_help = m_wherex();
    }
    /*fprintf(mystderr,"\nPo grupie"); */
    return x_k_help;
}

/* ------------------------------------------------------------------- */
int wydruk_monitu(int wpis, char* opis_danej, int dl_zakr, char* taknie,
                  int* dltn, char* text, int* x0, int dx_ind,
                  int* dl_menu, int* size, int y, int* xp_naz,
                  int* xp, int* txl, unsigned int attrtx, int w_min,
                  int w_max, char typ, int skok, void* R, void* Rh, char* help[])
{
    char* fmon;
    int ret, i, ms, xmax, k, l;
    *dltn = 0;
    if (bez_potwierdzania == 0) *dltn = strlen(taknie);
    i = dl_zakr;
    if (i > 0) i--;
    fmon = &opis_danej[i];
    ms = strlen(fmon);
    text[0] = EOS;
    for (i = ms - 2; i >= 0; i--)
    {
        if (fmon[i + 1] == '?' && fmon[i] == ' ')
        {
            for (k = i; k <= ms; k++) text[k - i] = fmon[k];
            fmon[i + 1] = EOS;
            fmon[i] = EOS;
            break;
        }
    }
    ms = strlen(opis_danej);
    *xp_naz = (*x0) + ms;
    *xp = *xp_naz + dx_ind;
    xmax = (*xp) + (*size) - 1;
    for (*txl = 0; *txl < MXR_MAX; (*txl)++) { if (text[*txl] == EOS) break; }
    ret = xmax + 4;
    if (wpis != 2) ret = xmax + (*dltn);
    k = 0;
    if (help != NULL)
    {
        if (typ != 'g')
        {
            if (typ == 'z')
            {
                w_min = 0;
                w_max = skok - 1;
            }
            if (typ == 'i' && w_max > skok - 1) w_max = skok - 1;
            for (i = w_min; i <= w_max; i++)
            {
                l = strlen(help[i]);
                if (l > k) k = l;
            }
            if (typ == 'z')
            {
                k--;
                if (k < 6) k = 6;
            }
        }
        *dl_menu = k;
    }
    k = ret + (*dl_menu);
    if (k > X_maxBlank)
    {
        if (y <= Y_maxBlank && y < MY_MAX) return 0;
        (*x0) += (X_maxBlank - k);
        if ((*x0) < X_L0 && ms >= -(*x0))
        {
            opis_danej[ms - (*x0)] = EOS;
            (*x0) = X_L0;
        }
        if ((*x0) < X_L0 && ms < -(*x0))
        {
            opis_danej[0] = EOS;
            (*x0) = X_L0;
        }
        *xp_naz = (*x0) + ms;
        *xp = *xp_naz + dx_ind;
        xmax = (*xp) + (*size) - 1;
    }
    term_type(y, (*x0), opis_danej, 0, attrtx);
    return xmax;
}

int wydruk_indexu(int y, int xp_naz, unsigned int attr,
                  unsigned int attrtx, char* f_nazwy, char* f_ind,
                  int index, char* fk_nazwy)
{
    int xp_ind;
    term_type(y, xp_naz, f_nazwy, 0, attrtx);
    xp_ind = m_wherex();
    term_printf(y, xp_ind, attr, f_ind, index);
    term_type(y, m_wherex(), fk_nazwy, 0, attrtx);
    return xp_ind;
}

int wpis_indexu(int y, int xp_ind, int ind_size, int* wart,
                char* index_helpu, int size_helpu, int w_min,
                int w_max, char* adr, int dim, int xp,
                int xmax, char* f_ind, char* format, unsigned int attrtx,
                unsigned int attr, long int skok, void* R, void* Rh,
                char** help, char typ, int dltn, int W_min, int W_max, int lsmax)
{
    char form[6], bufor[30], c_tab, *zle, *menu, ogr, err = 0;
    int ret, wartn, l_tab, i, xk, nr_zm, ins, Ww_max, l_help;
    float w_tab;
    zle = " ?? ";
    wartn = *wart;
    sprintf(bufor, f_ind, wartn);
    sprintf(form, " d");
    form[0] = '%';
    xk = xp_ind + ind_size - 1;
    nr_zm = *wart;
    do
    {
        ins = INSERT;
        INSERT = 1;
        HELP = 1;
        Okno(y, xp_ind, y, xk, attr);
        nr_zm = wartn;
        ret = bscanf(bufor, attrtx, form, &wartn);
        INSERT = ins;
        if (ret < 0)
        {
            HELP = 0;
            if (wartn < w_min) wartn = w_min;
            if (wartn > w_max) wartn = w_max;
            *wart = wartn;
            term_printf(y, xp_ind, attr, f_ind, wartn);
        }
        if (ret == 0) HELP = 0;
        ogr = 0;
        if (wartn < w_min)
        {
            wartn = w_min;
            term_printf(y, xp_ind, attr, f_ind, wartn);
            ogr = 1;
        }
        if (wartn > w_max)
        {
            wartn = w_max;
            term_printf(y, xp_ind, attr, f_ind, wartn);
            ogr = 1;
        }
        if (wartn != nr_zm || ogr == 1)
        {
            sprintf(bufor, f_ind, wartn);
            switch (typ)
            {
            case 'i':
                l_tab = *((int*)(adr + wartn * dim));
                term_printf(y, xp, attr, format, l_tab);
                if (help != NULL)
                {
                    term_type(y, m_wherex(), " ", 1, attrtx);
                    /*              lsmax=0;
                                  for(i=0;i<skok;i++)
                                   {ls=strlen(help[i]); if(ls>lsmax) lsmax=ls;} */
                    menu = zle;
                    if (l_tab >= 0 || l_tab < skok) menu = help[l_tab];
                    Term_Typef(y, m_wherex(), menu, lsmax, attr);
                }
                break;
            case 'g':
                l_tab = *(int*)(adr + wartn * dim);
                Ww_max = W_max;
                l_help = l_tab;
                err = 0;
                if (size_helpu > 0)
                {
                    long int skok_sum;
                    skok_sum = ustal_skok_adr(Rh, l_tab, size_helpu, &err);
                    if (err == 0)
                        l_help = *(int*)((unsigned long int)index_helpu + skok_sum);
                    Ww_max = 9999;
                }
                term_printf(y, xp, attr, format, l_tab);
                objasn_grupy(y, xmax + 1, W_min, Ww_max, l_help, attr, attrtx, skok, dltn, R, help);
                break;
            case 'f':
                {
                    int nrk, xpp;
                    for (nrk = 0, xpp = xp; nrk < size_helpu; nrk++)
                    {
                        w_tab = *((float*)((adr + (w_max + 1) * dim * nrk) + wartn * dim));
                        term_printf(y, xpp, attr, format, w_tab);
                        xpp = m_wherex() + 1;
                    }
                }
                break;
            case 'z':
                c_tab = *(adr + wartn * dim);
                if (help == NULL) term_type(y, xp, &c_tab, 1, attr);
                else
                {
                    menu = zle; /*lsmax=0; */
                    for (i = 0; i < skok; i++)
                    {
                        /*ls=strlen(help[i]); if(ls>lsmax) lsmax=ls; */
                        if (c_tab == help[i][0])
                        {
                            menu = &help[i][2];
                            break;
                        }
                    }
                    i = lsmax + xp;
                    if (i > X_maxBlank) lsmax = (X_maxBlank - xp);
                    term_type(y, xp, &c_tab, 1, attr);
                    term_type(y, xp + 1, " ", 1, attrtx);
                    Term_Typef(y, xp + 2, menu, lsmax - 1, attr);
                }
                break;
            case 't':
                menu = adr + wartn * dim;
                term_type(y, xp, menu, 0, attr);
                break;
            default: break;
            }
            if (HELP == 2) HELP = 1;
            else HELP = 0;
        } /* koniec obslugi (wartn!=nr_zm)  */
        else break;
        if (ret < 0) return -1;
    }
    while (HELP != 0);
    HELP = 0;
    if (wartn == *wart) return 0;
    *wart = wartn;
    return 1;
}


char* wpisz_format(char* format, char* monit)
{
    return monit;
    /*   if(format!=NULL) {Free(format); lwmall--;}
       for(i=0;;i++) {if(monit[i]==EOS) break;}; i+=(i%4);
       format=(char *)Malloc(i*sizeof(char));

       if(format==NULL)
            {komunikat(5,1,ATTR_A," Brak pamieci dla formatu danej %d: <Ent>", nr_danej);
         nr_danej--;
         GET_char();
         return NULL;
        }
       format[i-1]=EOS;  lwmall++;
       for(l=0;l<=i;l++) {format[l]=monit[l];}
      return format;
    */
}

char* formaty_tabl(char* f_nazwy, char* monit_pocz, int* ind_min,
                   int* ind_max, int* index, int* ind_size, char* hash,
                   char* format_tabl, char* opis_danej, int* dl_zakr,
                   char** f_ind, char** fk_nazwy, int* dx_ind)
{
    char *fmon, *z, *c, im, zn, *fp, *f_pocz;
    int i, j, k, l;
    fmon = monit_pocz;
    im = 0;
    if (monit_pocz[0] == '#' || monit_pocz[1] == '#') *hash = '#';
    if (fmon[0] == '#' || fmon[0] == '*' || fmon[0] == '+') im = 1;
    if (fmon[1] == '#' || fmon[1] == '*' || fmon[1] == '+') im = 2;
    fmon += im;
    *dl_zakr = 0;
    opis_danej[0] = EOS;
    f_nazwy[0] = EOS;
    /* ----------------- wstepne operacje dla tablicy -------------------- */
    if (*ind_max <= 0 || *ind_max <= *ind_min)
    {
        *ind_min = 0;
        *ind_size = 0;
    }
    else
    {
        *ind_size = 1;
        if (*ind_max > 9) *ind_size = 2;
        if (*ind_max > 99) *ind_size = 3;
    }
    if (*index < *ind_min) *index = *ind_min;
    if (*index > *ind_max) *index = *ind_max;
    *dx_ind = 0; /* *f_pocz=NULL; */
    if (*ind_size > 0) /* =========== obsluga tablicy =========== */
    {
        z = szukaj_formatow(fmon, 'd', &f_pocz);
        if (z != NULL)
        {
            c = szukaj(z, ' ');
            if (c != NULL)
            {
                c++;
                zn = *c;
                *c = EOS;
            }
            *dl_zakr = sprintf(opis_danej, fmon, *ind_min, *ind_max);
            if (c != NULL)
            {
                *c = zn;
                fmon = c;
            }
            else fmon = NULL;
        }
        l = 1;
        if (format_tabl != NULL) /* wydzielamy 3 formaty opisujace tablice */
        {
            fp = format_tabl; /* tj. f_nazwy, f_ind, fk_nazwy */
            l = 0;
            j = 0;
            k = 0; /* np.: " h[", "%3d", "]="  */
            do
            {
                for (z = fp; *z != '%' && *z != EOS; z++, j++)
                {
                    if (l < 32)
                    {
                        f_nazwy[l] = format_tabl[j];
                        l++;
                    }
                    else
                    {
                        *z = EOS;
                        break;
                    }
                }
                if ((*z) == '%')
                {
                    zn = czy_format(z, &i); /* prawdop. specyfikacja */
                    if (zn == EOS)
                    {
                        fp = z + 1;
                        continue;
                    } /* nie jest to specyfik. */
                    if (k == 0) /* znaleziono specyfikacje */
                    {
                        f_nazwy[l] = EOS;
                        l++;
                        *f_ind = &f_nazwy[l];
                        j += i; /* przeskakujemy specyfikacje formatu */
                        l += 4;
                        (*f_ind)[3] = EOS; /* zapelnimy format pozniej */
                        *fk_nazwy = &f_nazwy[l];
                        k++;
                        fp = z + 1;
                        continue;
                    }
                    else /* druga specyfikacja */
                    {
                        if (czy_format(z, &i) == EOS)
                        {
                            fmon = z + 1;
                            continue;
                        }
                        k++;
                        f_nazwy[l] = EOS; /* konczymy format fk_nazwy */
                    }
                } /* koniec obslugi   znaku %  */
                else
                {
                    f_nazwy[l] = EOS;
                    l++;
                    if (k == 0)
                    {
                        *f_ind = &f_nazwy[l];
                        l += 4;
                        *f_ind[3] = EOS; /* zapelnimy format pozniej */
                        *fk_nazwy = &f_nazwy[l];
                        *fk_nazwy[0] = ' ';
                        *fk_nazwy[1] = EOS;
                    }
                }
            }
            while (k < 2 && *z != EOS);
        }
        else /* brak formatu opisujacego tablice */
        {
            f_nazwy[0] = EOS;
            *f_ind = &f_nazwy[1];
            *fk_nazwy = *f_ind + 4;
            (*fk_nazwy)[0] = ' ';
            (*fk_nazwy)[1] = EOS;
        }
        sprintf(*f_ind, " %dd", *ind_size);
        (*f_ind)[0] = '%';
        *dx_ind = (*ind_size) + strlen(f_nazwy) + strlen(*fk_nazwy);
    } /* koniec obsugi tablicy */
    return fmon;
}

/************************************************************************/
#define FLOAT 0
#define ZNAK  1
#define INT   2
#define GRUPA 3

long int ustal_skok_adr(void* Rh, int nr, int skok, char* err)
{
    struct reports* R;
    char** baza;
    long int skok_sum;
    *err = 0;
    if (Rh == NULL) return skok * nr;
    R = (struct reports*)Rh;
    if (R->typ_bazy == BAZA_WEKTOR)
    {
        baza = (char**)(R->D);
        if (baza[nr] == NULL)
        {
            *err = 1;
            return 0;
        }
        skok_sum = (long int)((unsigned long int)baza[nr] - (unsigned long int)baza[0]);
        return skok_sum;
    }
    {
        void *l, *next, *lp = NULL, *(*L);
        int k;
        L = (void**)(&(R->D));
        if ((*L) == NULL)
        {
            *err = 1;
            return 0;
        }
        l = (*L);
        next = *(void**)l;
        for (k = 0; k <= nr; k++)
        {
            if (next == NULL) break;
            lp = l;
            l = next;
            next = *(void**)l;
        }
        if (next == NULL && k < R->ob_konc)
        {
            *err = 1;
            return 0;
        }
        return (long int)((unsigned long int)lp - (unsigned long int)(*L));
    }
}


int okno_help(int Yp, int Xp, int Xm, unsigned int attr, unsigned int attr_o,
              char typ, int w_min, int w_max, int W_min, int W_max,
              char* index_helpu, int size_helpu, int size, int skok,
              void* R, void* Rh, char* help[], signed char ramka, char* tabl, int dim,
              char* f_nazwy, char* f_ind, char* fk_nazwy)
{
    unsigned int att;
    int k_inv, i, id, y, l_linii, k, tx_size, ret, ip, kmax, nr, k_ost, nrh;
    int xp, xpf, xm, yp, ym, yd, Ym, y_kur, x_kur, n, ik, x, ind, ip_old, ind_size;
    char znak, **buf, *koniec = "<Esc>", form_dat[10], *form_d, *menu, *zle, jest;
    int dl_tyt = 0, opcja0 = 0;
    float w, *wart;
    char err = 0;
    struct dane_wpisu* dw;
    dw = (struct dane_wpisu*)index_helpu;
    zle = " ??? ";
    switch (typ)
    {
    case 'f': typ = FLOAT;
        break;
    case 'z': typ = ZNAK;
        break;
    case 'i': typ = INT;
        break;
    case 'g': typ = GRUPA;
        break;
    default: typ = INT;
        break;
    }
    if (size > 0)
    {
        form_d = &form_dat[0];
        form_d[0] = '%';
        sprintf(&form_d[1], "%dd", size);
        if (typ == FLOAT)
            sprintf(&form_d[1], "%d.%df", size, skok);
        ind_size = 1;
        if (w_max > 9) ind_size = 2;
        if (w_max > 99) ind_size = 3;
    }
    else
    {
        form_d = NULL;
        ind_size = 0;
    }
    if (Xp < X_L0) Xp = X_L0;
    if (Xm > MXR_MAX) Xm = MXR_MAX;
    if (Yp < Y_G0) Yp = Y_G0;
    xp = Xp;
    yp = Yp;
    xm = Xm;
    if (ramka > 0)
    {
        xp++;
        xm--;
    }
    tx_size = Xm - Xp;
    kmax = w_max - w_min + 1;
    Ym = Yp + kmax;
    ym = Ym;
    if (ramka > 0)
    {
        yp++;
        ym++;
        Ym += 2;
    }
    if (Ym > MYR_MAX)
    {
        Ym = MYR_MAX;
        ym = MYR_MAX;
        if (ramka > 0) ym = MYR_MAX - 1;
    }
    blok_anim->xp = Xp;
    blok_anim->yp = Yp;
    blok_anim->xm = Xm;
    blok_anim->ym = Ym;
    l_linii = Ym - Yp + 1;
    buf = Malloc(l_linii * sizeof(char**));
    if (buf == NULL)
    {
        komun_text(MY_MAX, X_L0, " Brak pamieci dla okno_help(). <Ent>", 0,ATTR_A);
        GET_char();
        return -1;
    }
    lwmall++;
    attr = attr & (~TERM_FLUSH);
    attr_o = attr_o & (~TERM_FLUSH);
    tx_size += 2;
    k = term_save_image(yp, xp, "a", 0);
    for (y = Yp; y <= Ym; y++)
    {
        n = y - Yp;
        buf[n] = (char*)Malloc(tx_size * k + 1);
        if (buf[n] == NULL)
        {
            komun_text(MY_MAX, 1, " Brak pamieci dla menu. <Ent>", 0,ATTR_A);
            for (k = 0; k < n; k++)
            {
                Free(buf[k]);
                lwmall--;
            }
            Free(buf);
            lwmall--;
            GET_char();
            return -1;
        }
        lwmall++;
        term_save_image(y, Xp, buf[n], tx_size);
    }
    Okno(Yp, Xp, Ym, Xm, attr);
    if (ramka == 1) maluj_ramke(Yp, Xp, Ym, Xm, attr);
    if (R != NULL)
    {
        struct reports* Rd;
        Rd = (struct reports*)R;
        if (Rd->kod_raportu < 0) /* jest to menu_rekordow */
        {
            dl_tyt = strlen(Rd->tytul);
            if (dl_tyt > 0)
            {
                i = xp + (xm - xp + 1 - dl_tyt) / 2;
                term_type(Yp, i, Rd->tytul, 0, attr);
            }
            opcja0 = Rd->ob_pocz;
            if (opcja0 < 0) opcja0 = 0;
            if (w_max > 0) opcja0 = opcja0 % w_max;
            else opcja0 = 0;
        }
    }
    ip = 0;
    k_inv = ip;
    ip_old = -1;
    l_linii = ym - yp, jest = 0;
    k_ost = k_inv;
    yd = yp;
    id = ip;
    do
    {
        ik = ip + l_linii;
        y = yp;
        for (i = ip; (i <= ik) || (jest != 0); i++, y++)
        {
            if (ip == ip_old)
            {
                switch (jest)
                {
                case 0: i = k_ost;
                    y = yp + i - ip;
                    jest = 1;
                    break;
                case 1: i = k_inv;
                    y = yp + i - ip;
                    jest = 2;
                    yd = y;
                    id = i;
                    break;
                case 2: jest = 0;
                    goto W;
                }
            }
            att = attr;
            if (i == k_inv)
            {
                att = attr_o;
                y_kur = y;
            }
            nr = i + w_min;
            term_type(y, xp, " ", 1, attr);
            x = m_wherex();
            if (i < kmax)
            {
                x_kur = x + size - 1;
                if (tabl != NULL)
                {
                    ind = nr;
                    if (f_ind != NULL) wydruk_indexu(y, xp + 1, att, attr, f_nazwy, f_ind, ind, fk_nazwy);
                    x = m_wherex();
                    if (f_ind != NULL) x_kur = xp + strlen(f_nazwy) + ind_size;
                    else x_kur = xp;
                    if (typ > ZNAK) /* liczby calkowite */
                        nr = *(int*)(tabl + ind * dim);
                    if (typ == FLOAT)
                    {
                        int xpp, kol;
                        for (kol = 0, xpp = x;;)
                        {
                            w = *(float*)((tabl + (w_max + 1) * dim * kol) + ind * dim);
                            if (form_d != NULL) term_printf(y, xpp, att, form_d, w);
                            kol++;
                            if (kol == size_helpu) break;
                            xpp = m_wherex();
                            term_type(y, xpp, " ", 1, attr);
                            xpp++;
                        }
                        xpf = x;
                        goto R;
                    }
                    if (typ == ZNAK) /* znaki */
                    {
                        nr = *(tabl + ind * dim);
                        term_type(y, x, (char*)&nr, 1, att);
                        menu = zle;
                        if (help != NULL)
                        {
                            for (k = 0; k < skok; k++)
                            {
                                if (help[k][0] == nr)
                                {
                                    menu = &help[k][2];
                                    break;
                                }
                            }
                            goto PISZ;
                        }
                        else goto R;
                    }
                }
                if (typ == GRUPA)
                {
                    if (size > 0) term_printf(y, x, att, form_d, nr);
                    if (nr < W_min || nr > W_max)
                    {
                        if (size > 0) term_type(y, x + size, " ", 1, attr);
                        term_type(y, x + size + 1, zle, 0, att);
                    }
                    else
                    {
                        long int skok_sum;
                        nrh = nr;
                        err = 0;
                        if (size_helpu > 0)
                        {
                            skok_sum = ustal_skok_adr(Rh, nrh, size_helpu, &err);
                            if (err == 0)
                                nrh = *(int*)((unsigned long int)index_helpu + skok_sum);
                        }
                        /* dla grupy posredn. size_helu>0      */
                        if (err > 0) term_type(y, x + size + 1, zle, 0, att);
                        else { disp_help(y, x + size, attr, att, nrh, skok, R, help, xm - 1); }
                    }
                    goto R;
                }
                menu = zle;
                if (typ == INT)
                {
                    if (form_d != NULL) term_printf(y, x, att, form_d, nr);
                    if (help != NULL && nr < skok) menu = help[nr];
                }
                if (typ == ZNAK)
                {
                    if (nr < skok && help != NULL)
                    {
                        term_type(y, x, &help[nr][0], 1, att);
                        menu = &help[nr][2];
                    }
                    else term_type(y, x, " ", 1, att);
                }
                if (help != NULL)
                PISZ:
                    {
                        term_type(y, m_wherex(), " ", 1, attr);
                        term_type(y, m_wherex(), menu, 0, att);
                    }
            }
            else /* i>=kmax */
            {
                znak = EOS;
                k = strlen(koniec);
                if (k + xp > xm - 1)
                {
                    znak = koniec[xm - xp - 1];
                    koniec[xm - xp - 1] = EOS;
                }
                term_type(y, xp + 1, koniec, 0, att);
                if (znak != EOS) koniec[xm - xp - 1] = znak;
                att = attr;
            }
        R:
            for (x = m_wherex(); x < xm; x++) term_type(y, x, " ", 1, att);
            term_type(y, x, " ", 1, attr);
        }
    W:
        ip_old = ip;
        ind = k_inv + w_min;
        id = k_inv;
        do
        P:
            {
                term_cur(y_kur, x_kur);
                k_ost = k_inv;
                term_flush();
                ret = GET_char();
                ret = ret & 0x00ff; /*printf("\n znak=%d",ret); */
                switch (ret)
                {
                case NLCR: ret = CR/*Get_Char()*/;
                case CR:
                    if (k_inv == kmax) ret = Esc;
                    else
                    {
                        if (typ == FLOAT && dw != NULL)
                        {
                            int kol, index;
                            index = id + w_min;
                            for (kol = size_helpu - 1; kol >= 0; kol--)
                            {
                                wart = (float*)((tabl + (w_max + 1) * dim * kol) + index * dim);
                                dw->wartn[kol] = *wart;
                            }
                            if (dw->iobl == 0)
                                sprintf(dw->bufor, dw->format, *wart);
                            else dw->bufor[0] = EOS;
                            wpisanie_danych_fl(dw->x0, xpf, yd, dw->xmax, xm, 0,
                                               dw->dltn, dw->poz_wart_abs, dw->dlh_ogr, &dw->format,
                                               dw->format_bazy, dw->format_tx, dw->form,
                                               dw->format_wyn, dw->help_ogr, dw->format_ogr, dw->text,
                                               dw->zakres, dw->zakr_abs, dw->bufor, dw->taknie,
                                               attr_o, attr, dw->iobl, size_helpu, size, dim, w_max,
                                               index, tabl, wart, dw->wartn, dw->baza, dw->w_min,
                                               dw->w_max, dw->abs_min, dw->abs_max);
                            ret = DOWN;
                            goto SW;
                        };
                    }
                    goto D;
                case Esc: ret = Esc;
                    goto D;
                case BS: k_inv--; /*if(k_inv<0) k_inv=0;*/
                    break;
                case SPEC:
                    ret = GETchar();
                    znak = znak & 0x00ff; /*  printf(" drugi=%d",ret); */
                SW:
                    switch (ret)
                    {
                    case Home: k_inv = 0;
                        break;
                    case End: k_inv = kmax;
                        break;
                    case PgUp: k_inv -= 5;
                        break;
                    case PgDn: k_inv += 5;
                        break;
                    case LEFT:
                    case UP: k_inv--;
                        break;
                    case FFR:
                    case DOWN: k_inv++;
                        break;
                    default: goto P;
                    }
                    break;
                default: goto P;
                }
                if (k_inv < 0) k_inv = 0;
                if (k_inv > kmax) k_inv = kmax;
                if (k_inv < ip) ip = k_inv;
                if (k_inv > ik) ip += (k_inv - ik);
            }while (k_inv == k_ost);
    D:
        continue;
    }
    while (ret != Esc && ret != CR);
    if (ret == Esc) k_inv = -1;
    for (y = Yp; y <= Ym; y++)
    {
        n = y - Yp;
        term_restore_image(y, Xp, buf[n], tx_size);
        Free(buf[n]);
        lwmall--;
    }
    Free(buf);
    lwmall--;
    blok_anim->ym = -1;
    return (k_inv);
}

/* ------------------------------------------------------------------- */
int okno_tabl_znak(signed char wpis, int y, int x0,
                   unsigned int attr_d, unsigned int attr,
                   unsigned int attrtx, int* index, char* tabl, int dim,
                   char* monit_pocz, int ind_min, int ind_max, char opcje[],
                   char* format_tabl, char* objasn[], int liczba_obj)
{
    int ret, size, dlopt, txl, xp_ind, xp_naz;
    char text[80], znakp;
    char znk, znp, znf, sukces, *znak, hash = '*';
    char *buffer, opis_danej[80], nznak, *taknie = "<t/n>? ";
    char f_nazwy[40], *f_ind = NULL, *fk_nazwy = NULL, *fmon;
    int xmax, xp, i, k, x_konc, zn, znw, xd, lopt, iopt;
    int iobl, dltn = 0, dx_ind = 0, x, ind_size, dl_zakr;
    unsigned int att;
    char blok = 0;
    if (wpis < -2)
    {
        blok = 1;
        wpis = -wpis;
    }
    znak = (tabl + (*index) * dim);
    fmon = formaty_tabl(f_nazwy, monit_pocz, &ind_min, &ind_max, index,
                        &ind_size, &hash, format_tabl, opis_danej, &dl_zakr, &f_ind,
                        &fk_nazwy, &dx_ind);
    if (fmon != NULL)
        sprintf(&opis_danej[dl_zakr], fmon, opcje);
    else
        sprintf(&opis_danej[dl_zakr], "[%s] ", opcje);
    lopt = liczba_obj;
    k = 1; /* rozm_pola znaku */
    xmax = wydruk_monitu(wpis, opis_danej, dl_zakr, taknie, &dltn, text, &x0,
                         dx_ind, &size, &k, y, &xp_naz, &xp, &txl, attrtx,
                         0, 0, 'z', liczba_obj, NULL, NULL, objasn);
    if (xmax == 0) return 0;
    dltn = 0;
    if (bez_potwierdzania == 0) dltn = strlen(taknie);
    iobl = 0;
    if (hash == '#') iobl = 1;
    if (iobl == 1 && wpis != 2) nznak = '?';
    else nznak = *znak;
    znakp = nznak;
    iopt = 0;
    xd = xp;
    dlopt = 0;
    for (i = 0; i < 100; i++) { if (opcje[i] == EOS) break; }
    if (i == 100) i = 0;
    dlopt = i;
    for (iopt = 0; iopt < dlopt; iopt += 2)
    {
        znk = opcje[iopt];
        if (znk == nznak) break;
        else
        {
            if (opcje[iopt + 1] == '-')
            {
                znp = znk + 1;
                znf = opcje[iopt + 2];
                for (znk = znp; znk < znf; znk++) { if (znk == nznak) break; }
                if (znk < znf) break;
            }
        }
    }
    if (iopt >= dlopt)
    {
        iopt = 0;
        znakp = '?';
    }
    xp_ind = xp;
    if (ind_size > 0)
    {
        xp_ind = wydruk_indexu(y, xp_naz, attr, attrtx, f_nazwy, f_ind, *index, fk_nazwy);
        Okno(y, xp_ind, y, xmax, attr);
        ret = xp_ind;
        term_type(y, xp + 1, fk_nazwy, 0, attrtx);
    }
    xmax += size;
OP:
    term_type(y, xp, &nznak, 1, attr_d); /* wydruk objasnienia znaku */
    sukces = 0;
    xd = m_wherex() - 1;
    if (size > 0 && nznak != ' ')
    {
        term_type(y, m_wherex(), " ", 1, attrtx);
        for (i = 0; i < liczba_obj; i++)
        {
            if (objasn[i][0] == EOS)
            {
                Term_Typef(y, m_wherex(), " ??? ", size - 1, attr);
                k = 5;
                break;
            }
            if (objasn[i][0] == nznak)
            {
                Term_Typef(y, m_wherex(), &objasn[i][2], size - 1, attr);
                sukces = 2;
                break;
            }
        }
        if (i == liczba_obj)
        {
            Term_Typef(y, m_wherex(), " ??? ", size - 1, attr);
            k = 5;
        }
    }
    if (wpis == 2)
    {
        for (i = 0; i < txl; i++)
        {
            if (i + xmax + 1 < X_maxBlank) term_type(y, xmax + 1 + i, " ", 1, attrtx);
        }
        return m_wherex();
    }
    att = attrtx;
    if (sukces == 0)
    {
        text[1] = '!';
        att = ATTR_A;
    }
    for (i = 0; i < txl; i++)
    {
        if (i + xmax + 1 < X_maxBlank) term_type(y, xmax + 1 + i, &text[i], 1, att);
    }
    if (sukces == 0) text[1] = '?';
    x_konc = m_wherex();
    if (wpis == 0) return x_konc;
    /* ---------------------------- menu ------------------------------- */
    if (wpis > 2)
    {
        if (y < MY_MAX)
        {
            if (wpis == 3 && objasn != NULL)
            {
                do
                {
                    ret = okno_help(y + 1, xp - 1, xmax + 1, attrtx, attr, 'z',
                                    0, liczba_obj - 1, 0, 0, NULL, 0, 1,
                                    liczba_obj, NULL, NULL, objasn, 0, NULL, 0, NULL, NULL, NULL);
                    /*ret=okno_menu(objasn, liczba_obj, 0, attrtx, attr, y+1, xp-1, xmax+1,
                                   NULL, 0); */
                    if (blok == 1) ret = -1;
                    if (ret >= 0)
                    {
                        if (*znak == objasn[ret][0]) return 0;
                        else
                        {
                            nznak = objasn[ret][0]; /* zn=objasn[ret][0]; */
                            nznak = sprawdz_znak(nznak, opcje, dlopt, &sukces, &iopt);
                            if (sukces == 0) nznak = znakp;
                            *znak = nznak;
                        }
                    }
                    else
                    {
                        x_konc = -1;
                        break;
                    }
                }
                while (sukces == 0);
            }
            if (wpis == 4 && ind_size > 0)
            {
                ret = okno_help(y + 1, xp_naz - 1, xmax + 1, attrtx, attr, 'z',
                                ind_min, ind_max, 0, 0, NULL, 0, 1, liczba_obj,
                                NULL, NULL, objasn, 0, tabl, dim, f_nazwy, f_ind, fk_nazwy);
                if (ret >= 0)
                {
                    if (ret + ind_min == (*index)) x_konc = 0;
                    else *index = ret + ind_min;
                }
                else x_konc = -1;
            }
        }
        wpis = 1;
        return x_konc;
    }
    /* ----------------------------------------------------------------- */
    /* -------------------- Obsluga wpisu indeksu ---------------------- */
    if (ind_size > 0 && wpis == -1)
    {
        i = *index;
        ret = wpis_indexu(y, xp_ind, ind_size, index, (char*)index, 0, ind_min, ind_max, tabl,
                          dim, xp, xp + 1, f_ind, NULL, attrtx, attr, liczba_obj, NULL, NULL,
                          objasn, 'z', dltn, 0, 0, size);
        if (ret < 0)
        {
            *index = i;
            return -5;
        } /* tu bylo -1; */
        if (*index != i)
        {
            znak = tabl + (*index) * dim;
            nznak = *znak;
        }
        wpis = 1;
    }
    /* ----------------------------------------------------------------- */
WP:
    do
    {
        do
        {
            /* m_gotoxy(xd,y); */
            Okno(y, xp, y, xp, attr_d);
            term_cur(y, xp);
            term_flush();
            zn = GET_char();
            zn = zn & 0x00ff;
            if (zn == Esc)
            {
                if (sukces == 0)
                {
                    *znak = opcje[0];
                    *znak = znakp;
                }
                return -x_konc;
            }
            znw = zn;
            i = dlopt;
            if (zn == NLCR) zn = CR/*Get_Char()*/;
            if (zn == CR)
            {
                if (nznak != '?') zn = nznak;
                if (sukces == 1) break;
            }
            if (zn == EOS || zn == SPEC)
            {
                zn = GETchar();
                zn = zn & 0x00ff;
                if (zn == UP)
                {
                    if (lopt > 0)
                    {
                        if (opcje[iopt + 1] == '-')
                        {
                            nznak++;
                            if (nznak < opcje[iopt + 2]) goto OP;
                        } /* OP wydruk opcji */
                        iopt += 2;
                        if (iopt >= dlopt) iopt = 0;
                        nznak = opcje[iopt];
                        goto OP;
                    }
                }
                if (zn == DOWN)
                {
                    if (lopt > 0)
                    {
                        znk = nznak - 1;
                        if (iopt > 1 && opcje[iopt - 1] == '-')
                        {
                            if (znk > opcje[iopt - 2])
                            {
                                nznak = znk;
                                goto OP;
                            }
                        }
                        if (opcje[iopt + 1] == '-')
                        {
                            if (znk > opcje[iopt])
                            {
                                nznak = znk;
                                goto OP;
                            }
                        }
                        iopt -= 2;
                        if (iopt < 0) iopt = dlopt - 1;
                        nznak = opcje[iopt];
                        goto OP;
                    }
                }
            }
            else nznak = sprawdz_znak(zn, opcje, dlopt, &sukces, &iopt);
        }
        while (sukces == 0);
        term_type(y, xp, &nznak, 1, attr_d);
        if (objasn != NULL && sukces != 2)
        {
            x = xp + 2;
            for (i = 0; i < liczba_obj; i++)
            {
                if (objasn[i][0] == EOS)
                {
                    Term_Typef(y, x, " ??? ", size - 1, attr);
                    k = 5;
                    break;
                }
                if (objasn[i][0] == nznak)
                {
                    Term_Typef(y, x, &objasn[i][2], size - 1, attr);
                    break;
                }
            }
        }
    }
    while (znw != CR);
    if (*znak == nznak) return 0;
    if (bez_potwierdzania == 1)
    {
        znw = 't';
        goto TAK;
    }
    if (xmax + dltn > X_maxBlank)
    {
        i = X_maxBlank - xmax;
        taknie[i] = EOS;
    }
    k = term_save_image(y, xmax + 1, taknie, 0);
    do
    {
        buffer = (char*)Malloc(dltn * k + 1);
        if (buffer != NULL)
        {
            term_save_image(y, xmax + 1, buffer, dltn);
            lwmall++;
        }
        term_type(y, xmax + 1, taknie, 0, attrtx);
        i = m_wherex();
        if (i < MXR_MAX) i--;
        i--;
        term_cur(y, i);
        term_flush();
        znw = GET_char();
        znw = znw & 0x00ff;
        if (buffer != NULL)
        {
            term_restore_image(y, xmax + 1, buffer, dltn);
            Free(buffer);
            lwmall--;
        }
        if (znw == 'n')
        {
            wpis = 1;
            nznak = zn;
            goto WP;
        }
        if (znw == 't')
        TAK:
            *znak = zn;
        if (znw == Esc) return -x_konc;
    }
    while (znw != 't');
    return x_konc;
}

char sprawdz_znak(char zn, char opcje[], int dlopt, char* sukces, int* iopt)
{
    char znk, znp, znf, nznak;
    int i;
    *sukces = 0;
    for (i = 0; i < dlopt; i += 2)
    {
        znk = opcje[i];
        if (znk == zn) break;
        else
        {
            if (opcje[i + 1] == '-')
            {
                znp = znk + 1;
                znf = opcje[i + 2];
                for (znk = znp; znk < znf; znk++) { if (znk == zn) break; }
                if (znk < znf) break;
            }
        }
    }
    if (i < dlopt)
    {
        nznak = zn;
        *sukces = 1;
        *iopt = i;
    }
    return nznak;
}

/* =================================================================== */
int disp_help(int y, int x, unsigned int attrtx, unsigned int attr,
              int ind, int Skok, void* R, char* help[], int max_x)
{
    int i, k, nrzm, typ, xk, dlfor, l_opcji, koniec;
    long int skok, shift;
    int dnrz = 0, nrz;
    char *format, *dana, *form, *fm, znak, text[80], *fdanej, kod, **menu;
    char *brak[1] = {" !? "}, *zn, f_bledu[10], lenth, wzor;
    unsigned long int Dana;
    format = (char*)help[0];
    max_x++;
    /* fprintf(mystderr,"\nind=%d Skok=%d %s",ind,Skok,format); */
    nrzm = 0;
    form = format;
    if (Skok > 0)
    {
        char err = 0;
        skok = ustal_skok_adr(R, ind, Skok, &err);
        if (err > 0)
        {
            Term_Typef(y, x, " Brak !! ", 0, attr);
            return m_wherex();
        }
    }
    do
    {
        do
        {
            fm = szukaj(form, '%');
            if (fm == NULL)
            {
                dlfor = strlen(form);
                if (dlfor > 0)
                {
                    xk = x + dlfor;
                    if (xk > max_x)
                    {
                        i = max_x - x;
                        k = format[i];
                        format[i] = EOS;
                    }
                    term_type(y, x, format, 0, attrtx);
                }
                /* fprintf(mystderr,"\nret.szukaj: format=%s %d",format,m_wherex()); */
                return m_wherex();
            }
            else;
            form = fm + 1;
            typ = czy_format(fm, &koniec);
        }
        while (typ == EOS);
        nrzm++;
        if (nrzm == LHELP) break;
        fdanej = fm;
        dana = (char*)help[nrzm];
        Dana = (unsigned long int)dana;
        if (typ == 's' || typ == 'S')
        {
            zn = form;
            if (*zn == '-' || *zn == '+') zn++;
            if (*zn < '0' || *zn > '9') lenth = 80;
            else
            {
                fm[koniec - 1] = EOS;
                lenth = atoi(zn);
            }
            fm[koniec - 1] = 's';
        }
        form = &fm[koniec]; /* adres poczatku nastepnego formatu */
        fm = form; /* adres konca analizowanego formatu */
        znak = *fm;
        menu = NULL;
        /* ----- Dodano 19.08.1998 ----------------------------------------- */
        if (Skok <= 0) /* menu z tablic param.objasniajacych */
        {
            switch (typ)
            {
            case 'S':
            case 's': skok = ind * sizeof(char*);
                typ = 'S';
                break;
            case 'f': skok = ind * sizeof(float);
                break;
            case 'i':
            case 'd': skok = ind * sizeof(int);
                break;
            case 'e': skok = ind * sizeof(double);
                break;
            case 'c': skok = ind * sizeof(char);
                break;
            default: break;
            }
        }
        shift = skok;
        if (dana == NULL) shift = -1; /* wskazywany obiekt do prezent. nie istnieje */
        /* ----------------------------------------------------------------- */
        dnrz = 0;
        if (form[0] == '[' && form[1] == '%' && form[2] == 'a')
        {
            for (zn = &form[3], k = 4;; zn++, k++)
            {
                if ((*zn) == EOS) break;
                if ((*zn) == ']') break;
                if ((*zn) < '0' || (*zn) > '9')
                {
                    if (k > 4) *zn = EOS;
                    break;
                }
            }
            /* fprintf(mystderr,"\naform=%s k=%d zn=%c",form,k,*zn); */
            if (*zn == EOS)
            {
                komunikat(MY_MAX, X_L0,ATTR_A, " Blad formatu %s w grupie (zn=%c). <Ent>", form, *zn);
                GET_char();
                break;
            }
            if (k == 4)
            {
                if (*zn == '%' && *(zn + 1) == 'l' && *(zn + 2) == ']')
                {
                    dnrz = 1;
                    nrz = nrzm + 2;
                    /* fprintf(mystderr,"\n form=zn%d=%c %s (nrz=%d, help=%d)",k,*zn,form, nrz,help[nrz]); */
                    if (nrz >= LHELP || help[nrz] == NULL)
                    {
                        komunikat(MY_MAX, X_L0,ATTR_A, " Brak liczby danych dla spec. %s w grupie - zn=%c k=%d. <Ent>",
                                  form, *zn, k);
                        *zn = EOS;
                        GET_char();
                        break;
                    }
                    else
                    {
                        l_opcji = *(int*)help[nrz];
                        zn += 2;
                        k = 6;
                    }
                }
                else
                {
                    komunikat(5, 1,ATTR_A, " Brak liczby danych dla spec. %s w grupie. <Ent>", form);
                    /* fprintf(mystderr,"\n zly form=%s",form); */
                    GET_char();
                    break;
                }
            }
            else l_opcji = atoi(&form[3]);
            *zn = ']';
            nrzm++;
            if (nrzm == LHELP) break;
            if (Skok <= 0) skok = ind * sizeof(int);
            if (help[nrzm] == NULL || (char*)((unsigned long int)help[nrzm] + skok) == NULL) shift = -1;
            else
                shift = *(int*)((unsigned long int)help[nrzm] + skok);
            /* fprintf(mystderr, " nrzm=%d help=%d hel[nrzm]=%d shift=%d l_opcji=%d", nrzm,help,help[nrzm],shift,l_opcji); */
            znak = *fm;
            *fm = EOS; /*tu jest zamykany kompletny format */
            form += k; /* przesuwamy poczatek nastepnego formatu */
            nrzm += dnrz;
            dnrz = 0;
            if (shift < 0 || shift >= l_opcji) shift = -1; /* zly wskaznik */
            else /* poprawny wskaznik */
            {
                switch (typ)
                {
                case 'S':
                case 's': shift = shift * sizeof(char*);
                    typ = 'S';
                    break;
                case 'f': shift = shift * sizeof(float);
                    break;
                case 'i':
                case 'd': shift = shift * sizeof(int);
                    break;
                case 'e': shift = shift * sizeof(double);
                    break;
                case 'c': shift = shift * sizeof(char);
                    break;
                default: break;
                }
            }
        }
        else
        {
            if (form[0] == '[' && form[1] == '%' && (form[2] == 'z' || form[2] == 'Z' || form[2] == 'c'))
            {
                wzor = form[2];
                for (zn = &form[3], k = 4;; zn++, k++)
                {
                    if ((*zn) == EOS) break; /*  komunikat(MY_MAX,X_L0-8+2*k,ATTR_A," %c",*zn); */
                    if ((*zn) == ']') break;
                    if ((*zn) < '0' || (*zn) > '9')
                    {
                        *zn = EOS;
                        break;
                    }
                }
                if (*zn == EOS)
                {
                    komunikat(5, 1,ATTR_A, " Blad formatu %s w grupie. <Ent>", form);
                    GET_char();
                    break;
                }
                if (k == 4)
                {
                    komunikat(5, 1,ATTR_A, " Brak liczby stringow w menu dla spec. %s w grupie. <Ent>", form);
                    GET_char();
                    break;
                }
                if (typ != 's' && typ != 'S')
                {
                    komunikat(5, 1,ATTR_A, " Blad formatu %s w grupie. <Ent>", form);
                    GET_char();
                    break;
                }
                typ = 'S';
                *zn = EOS;
                l_opcji = atoi(&form[3]);
                /*  komunikat(MY_MAX,m_wherex(),ATTR_A," %s(%d)",&form[3],l_opcji); */
                *zn = ']';
                menu = (char**)help[nrzm]; /*  to jest tez tablica danych 'dana' */
                nrzm++;
                if (nrzm == LHELP)
                {
                    komunikat(5, 1,ATTR_A, " Za duzo danych w grupie. <Ent>");
                    GET_char();
                    break;
                }
                if (Skok <= 0)
                {
                    if (wzor == 'c') skok = ind * sizeof(char);
                    else skok = ind * sizeof(char*);
                }
                /* if(wzor=='z' || wzor=='Z')
                {komunikat(MY_MAX,m_wherex(),ATTR_A," Help=%d menu=%d %c%c%c ind=%d Skok=%d skok=%d l_opc=%d kod=%c ",help[nrzm],menu,wzor,form[3],form[4],ind,Skok,skok,l_opcji,kod); getchar();}
                fprintf(mystderr,"\nTyp=%c: dana=%d l_opcji=%d shift=%d (dana+shift)=%d skok=%d help[nrzm]=%d menu=%d",typ,dana,l_opcji,shift,dana+shift,skok,help[nrzm],menu);
                 */
                if (skok < 0) shift = l_opcji;
                else
                {
                    if (help[nrzm] == NULL || menu == NULL) goto ERR_Z;
                    if ((char*)((unsigned long int)help[nrzm] + skok) == NULL) goto ERR_Z;
                    if (wzor == 'Z')
                    {
                        if ((*(char**)((unsigned long int)help[nrzm] + skok)) == NULL) goto ERR_Z;
                        kod = (*(char**)((unsigned long int)help[nrzm] + skok))[0];
                    }
                    else kod = *(char*)((unsigned long int)help[nrzm] + skok); /*((char*)(help[nrzm]+skok))[0];*/
                    /*if(wzor=='z' || wzor=='Z')
                    komunikat(MY_MAX,m_wherex(),ATTR_A," %c%c%c ind=%d Skok=%d skok=%d l_opc=%d kod=%c ",wzor,form[3],form[4],ind,Skok,skok,l_opcji,kod);
                    */
                    for (shift = 0; shift < l_opcji; shift++, menu++)
                    {
                        if ((*(char**)menu) == NULL)
                        {
                            shift = l_opcji;
                            break;
                        }
                        /*if(wzor=='z' || wzor=='Z') {komunikat(MY_MAX,m_wherex(),ATTR_A," %s% ",(*(char**)menu));  getchar();} */
                        if ((*(char**)menu)[0] == kod) break;
                    }
                }
                if (shift == l_opcji || dana == NULL)
                {
                ERR_Z:
                    typ = 'S';
                    dana = (char*)brak;
                    menu = NULL;
                    shift = 0;
                }
                shift = shift * sizeof(char*);
                znak = *fm;
                *fm = EOS; /*tu jest zamykany kompletny format */
                form += k; /* przesuwamy poczatek nastepnego formatu */
            }
        } /* koniec else po analizie klamry [%a] */
        *fm = EOS; /*tu jest kompletny format do wydruku zmiennej*/
        *fdanej = EOS; /*wyciecie formatu tekstu */
        k = EOS;
        dlfor = strlen(format);
        if (dlfor > 0)
        {
            xk = x + dlfor;
            if (xk > max_x)
            {
                i = max_x - x;
                k = format[i];
                format[i] = EOS;
            }
            term_type(y, x, format, 0, attrtx);
            x = m_wherex();
        }
        *fdanej = '%'; /* rekonstrukcja oryginalnego formatu text+dana */
        if (k != EOS)
        {
            format[i] = k;
            *fm = znak; /*fprintf(mystderr,"\nret.k!=EOS");*/
            return x;
        }
        /* fprintf(mystderr,"\nTyp=%c: dana=%d shift=%d (dana+shift)=%d",typ,dana, shift,dana+shift); */
        if (shift >= 0 || R != NULL) dana = (char*)(Dana + shift); /* wszystko OK !! */
        else /* niewlasciwy indeks */
        {
            shift = 0;
            switch (typ)
            {
            case 'c': dana = f_bledu;
                f_bledu[0] = '?';
                break;
            case 'S':
            case 's': dana = (char*)brak;
                typ = 'S';
                menu = NULL;
                shift = 0;
                break;
            default:
                f_bledu[0] = '%';
                dana = (char*)brak;
                for (zn = &fdanej[1], k = 1; k < 4; zn++, k++)
                {
                    if ((*zn) < '0' || (*zn) > '9') { if (k > 1 || (*zn) != '-') break; }
                    f_bledu[k] = fdanej[k];
                }
                if (k == 1)
                {
                    f_bledu[1] = '4';
                    k++;
                }
                f_bledu[k] = 'S';
                f_bledu[k + 1] = EOS;
                fdanej = f_bledu;
                typ = 'S';
                break;
            } /*  koniec obslugi innych formatow (nie %c i nie %s)  */
            /* -----if(typ=='c')
                                 fprintf(mystderr,"\nPo poprawce: typ=%c %c",typ,dana[0]);
                    else {if(typ=='S')
                                                    fprintf(mystderr,"\nPo poprawce: typ=%c %s",typ, *(char**)dana);
                                             else fprintf(mystderr,"\nPo poprawce: typ=%c ",typ, f_bledu, dana[0]);
                         }
             ------------------------------------ */
        } /*  koniec obslugi niewlasciwego indeksu */
        switch (typ)
        {
        case 'i':
        case 'd':
            sprintf(text, fdanej, *(int*)(dana));
            break;
        case 'f':
            sprintf(text, fdanej, *(float *)(dana));
            break;
        case 'e':
            sprintf(text, fdanej, *(double *)(dana));
            break;
        case 'c':
            sprintf(text, fdanej, *(dana));
            break;
        case 'S':
            if (lenth > max_x) lenth = max_x;
            if ((*(char**)dana) == NULL)
            {
                dana = (char*)brak;
                i = strlen(dana);
            }
            /* fprintf(mystderr,"\n Dana S=%s fdanej=%s",*(char**)dana,fdanej); */
            else { i = strlen((char*)(*(char**)dana)); }
            /* fprintf(mystderr," dlug=%d",i); */
            if (menu != NULL)
            {
                lenth += 2;
                if (i > lenth)
                {
                    k = (*(char**)dana)[lenth];
                    (*(char**)dana)[lenth] = EOS;
                }
                sprintf(text, fdanej, &(*(char**)dana)[2]);
                /* fprintf(mystderr," text=%s###",text); */
            }
            else
            {
                if (i > lenth)
                {
                    k = (*(char**)dana)[lenth];
                    (*(char**)dana)[lenth] = EOS;
                }
                sprintf(text, fdanej, (*(char**)dana));
            }
            if (k != EOS)
            {
                (*(char**)dana)[lenth] = k;
                k = EOS;
            }
            fdanej[koniec - 1] = 'S';
            break;
        case 's':
            if (lenth > max_x) lenth = max_x;
            i = strlen((char*)dana);
            /*          if(menu!=NULL)
                       {lenth+=2;
                        if(i>lenth) {k=((char*)dana)[lenth]; ((char*)dana)[lenth]=EOS;}
                        sprintf(text,fdanej,&((char*)dana)[2]);
                       }
                      else
            */
            {
                if (i > lenth)
                {
                    k = ((char*)dana)[lenth];
                    ((char*)dana)[lenth] = EOS;
                }
                sprintf(text, fdanej, (char*)dana);
            }
            if (k != EOS)
            {
                ((char*)dana)[lenth] = k;
                k = EOS;
            }
            break;
        default:
            komunikat(5, 1,ATTR_A, " Blad formatu %s w disp_help(). <Ent>", format);
            GET_char();
            break;
        }
        i = strlen(text);
        xk = x + i;
        if (xk > max_x)
        {
            i = max_x - x;
            text[i] = EOS;
            i = 0;
        }
        term_type(y, x, text, 0, attr);
        x = m_wherex();
        format = form;
        *fm = znak; /* wyciecie nastepnego formatu */
        if (i == 0) return x;
    }
    while (i != 0 && nrzm < LHELP);
    /* fprintf(mystderr,"\nreturn KONIEC"); */
    return m_wherex();
}

/* ----------------------------------------------------------------- */

int wpisanie_danych_fl(int x0, int xp, int y, int xmax, int x_konc,
                       char iffree, int dltn, int poz_wart_abs, int dlh_ogr,
                       char** format, char* format_bazy, char* format_tx, char* form,
                       char* format_wyn, char help_ogr[], char* format_ogr, char text[],
                       char zakres[], char* zakr_abs, char* bufor, char* taknie,
                       unsigned int attr, unsigned int attrtx, int iobl, int L_kol,
                       int size, int dim, int ind_max, int index, char* tabl,
                       float* wart, float wartn[], float baza, float w_min, float w_max,
                       float abs_min, float abs_max)
{
    float w_abs, wmin, wmax;
    int xpp, nrkol, ins, ll, i, k, xa, ret, llmax, zn = ' ';
    char *zakr[2], *buffer, zmiana = 0; /* *fh, */
    /* ## proba przerobki dla aktywnego help'u (brak w_min, w_max)
      --------------------------------------------------------------------- */
    HELP = 0;
WPIS:
    xpp = xp;
    nrkol = 0;
    zmiana = 0;
    do
    WPS:
        {
            ins = INSERT;
            INSERT = 1;
            do
            {
                Okno(y, xpp, y, xmax, attr);
                if (dlh_ogr > 0) HELP = 1;
                ret = bscanf(bufor, attrtx, form, &wartn[nrkol]);
                INSERT = ins;
                if (ret < 0)
                {
                    if (iffree == 1)
                    {
                        Free(*format);
                        *format = NULL;
                        lwmall--;
                        HELP = 0;
                    }
                    return -x_konc;
                }
                if (HELP == 2)
                {
                    /* zmiana=1;  */
                    if (fabs(baza) > 1.e-4)
                        term_printf(y, poz_wart_abs, attr, format_wyn, baza * wartn[nrkol]);
                }
            }
            while (HELP == 2);
            HELP = 0;
            zakr[0] = zakres;
            zakr[1] = zakr_abs;
            wmin = w_min;
            wmax = w_max;
            w_abs = wartn[nrkol];
            llmax = 1;
            if (dlh_ogr > 0) llmax = 2;
            for (ll = 0; ll < llmax; ll++)
            {
                if (w_abs < wmin || w_abs > wmax)
                {
                    sprintf(text, *format, wartn[nrkol]);
                    i = strlen(text);
                    if (ll == 1)
                    {
                        i += sprintf(&text[i], help_ogr);
                        i += sprintf(&text[i], format_bazy, baza);
                        i += sprintf(&text[i], format_tx);
                        i += sprintf(&text[i], format_wyn, baza*wartn[nrkol]);
                    }
                    if (zakr[ll][0] == EOS)
                        sprintf(&text[i], " ZAKRES !! <ENT>-popraw ");
                    else
                    {
                        if (ll == 0)
                            sprintf(&text[i], zakres, " poza ", wmin, wmax, " <ENT> ");
                        if (ll == 1)
                            sprintf(&text[i], " poza%s <ENT> ", zakr_abs);
                    }
                    i = strlen(text) + 1;
                    if (x0 + i > X_maxBlank)
                    {
                        i = X_maxBlank - x0;
                        text[i] = EOS;
                    }
                    xa = x_konc - i;
                    if (xa < x0) xa = x0;
                    k = term_save_image(y, xa, text, 0);
                    i = strlen(text) + 1;
                    buffer = (char*)Malloc(i * k + 1);
                    if (buffer != NULL)
                    {
                        lwmall++;
                        term_save_image(y, xa, buffer, i);
                        term_type(y, xa, " ", 1,ATTR_A);
                        term_type(y, xa + 1, text, 0,ATTR_A);
                    }
                    GET_char();
                    if (buffer != NULL)
                    {
                        term_restore_image(y, xa, buffer, i);
                        Free(buffer);
                        lwmall--;
                    }
                    wartn[nrkol] = *wart;
                    goto WPS;
                }
                wmin = abs_min;
                wmax = abs_max;
                w_abs = wartn[nrkol] * baza;
                /* fh=format_ogr; */
            }
            if ((*wart) != wartn[nrkol])
            {
                zmiana = 1;
                wpisano_dana = 1;
            }
            /*       if(ret==0 && zmiana==0 || *wart==wartn[nrkol]) *wart=wartn[nrkol];
                   else zmiana=1; */
            nrkol++;
            if (nrkol < L_kol)
            {
                wart = (float*)((tabl + (ind_max + 1) * dim * nrkol) + index * dim);
                if (iobl == 0)
                    sprintf(bufor, *format, wartn[nrkol]);
                else bufor[0] = EOS;
                xpp = xmax + 1;
                xmax = xpp + size;
                xpp++;
            }
        }while (nrkol < L_kol);
    if (zmiana == 0)
    {
        if (iffree == 1)
        {
            Free(*format);
            *format = NULL, lwmall--;
        }
        return 0; /*x_konc;*/
    }
    else ret = x_konc;
    xpp = xp;
    for (i = 0; i < L_kol; i++, xpp += (size + 1))
    {
        term_printf(y, xpp, attr, *format, wartn[i]);
        x_konc = m_wherex();
        if (dlh_ogr > 0)
        {
            term_type(y, x_konc, help_ogr, 0, attrtx);
            term_printf(y, m_wherex(), attrtx, format_bazy, baza);
            term_type(y, m_wherex(), format_tx, 0, attrtx);
            term_printf(y, m_wherex(), attr, format_wyn, baza * wartn[i]);
            term_type(y, m_wherex(), zakr_abs, 0, attrtx);
            x_konc = m_wherex();
        }
    }
    if (bez_potwierdzania == 1)
    {
        zn = 't';
        goto TAK;
    }
    if (x_konc + dltn >= X_maxBlank)
    {
        i = X_maxBlank - x_konc;
        taknie[i] = EOS;
    }
    k = term_save_image(y, x_konc, taknie, 0);
    do
    {
        buffer = (char*)Malloc(dltn * k + 1);
        if (buffer != NULL)
        {
            term_save_image(y, x_konc, buffer, dltn);
            lwmall++;
        }
        term_type(y, x_konc, taknie, 0, attrtx);
        i = m_wherex();
        if (i < MXR_MAX) i--;
        i--;
        term_cur(y, i);
        term_flush();
        zn = GET_char();
        zn = zn & 0x00ff;
        if (buffer != NULL)
        {
            term_restore_image(y, x_konc, buffer, dltn);
            Free(buffer);
            lwmall--;
        }
        if (zn == 'n')
        {
            wpisano_dana = 0;
            goto WPIS;
        }
        if (zn == 't')
        {
        TAK:
            for (i = 0; i < L_kol; i++)
            {
                wart = (float*)((tabl + (ind_max + 1) * dim * i) + index * dim);
                *wart = wartn[i];
            }
        }
    }
    while (zn != Esc && zn != 't');
    return x_konc;
}

/*#define WEWY_INCL */
/* ================### Tu zaczyna sie plik wewy2.c ================== */
#ifndef WEWY_INCL
#define _DOS_
#include <stdio.h>
#include <process.h>
#ifdef _DOS_
#include <alloc.h>
#define Malloc farmalloc
#define Free farfree
#include <dos.h>
#include "../blank/intdef.h"
#else
#define Malloc malloc
#define Free free
#include <malloc>
#include <sys/qnxterm.h>
#include "../blank/intdef.h"
#endif
#include <conio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "wewybl.h"
#define WEW_EXT extern
#include "WEWY1.H"
#endif
/* ====================================================================== */
int okno_text(signed char wpis, int y, int x0, int size,
              unsigned int attr_d, unsigned int attr,
              unsigned int attrtx, char* string, char* monit, char** help,
              int l_opcji)
{
    return okno_text_menu(wpis, y, x0, size, attr_d, attr, attrtx, string,
                          monit, NULL, help, l_opcji, 0);
}

int okno_text_menu(signed char wpis, int y, int x0, int size,
                   unsigned int attr_d, unsigned int attr,
                   unsigned int attrtx, char* string, char* monit, void* R,
                   char** help, int l_opcji, int skok)
{
    int ret, pznak;
    char *bufor, *format = NULL, text[80], *form = NULL, ms;
    char textn[80], *buffer, *taknie = "<t/n>? ";
    int xmax, xp, ins, i, k, im = 0, x_konc, zn, txl, dltn = 0, dlstr;
    char blok = 0;
    void* Rh = NULL; /*, *Rh=NULL; / * ???? */
    if (wpis < -2)
    {
        blok = 1;
        wpis = -wpis;
    }
    dlstr = strlen(string);
    if (dlstr > size)
    {
        dlstr = size;
        string[dlstr] = EOS;
    }
    if (bez_potwierdzania == 0) dltn = strlen(taknie);
    im = 0;
    if (monit[0] == '#' || monit[0] == '*' || monit[0] == '+') im = 1;
    if (monit[1] == '#' || monit[1] == '*' || monit[1] == '+') im = 2;
    ms = strlen(monit);
    text[0] = EOS;
    pznak = 0;
    for (i = ms - 2; i >= im; i--)
    {
        if (monit[i + 1] == '?' && monit[i] == ' ')
        {
            for (k = i; k <= ms; k++) text[k - i] = monit[k];
            pznak = i;
            monit[i] = EOS;
            break;
        }
    }
    ms = strlen(monit) - im;
    xp = x0 + ms;
    if (size < 1) size = 1;
    xmax = xp + size - 1;
    for (txl = 0; txl < MXR_MAX; txl++) { if (text[txl] == EOS) break; }
    k = xmax + 4;
    if (wpis != 2) k = xmax + dltn;
    if (k > X_maxBlank)
    {
        if (y <= Y_maxBlank && y < MY_MAX)
        {
            if (pznak > 0) monit[pznak] = ' ';
            return 0;
        }
        x0 += (X_maxBlank - k);
        if (x0 < X_L0 && ms >= -x0)
        {
            monit[ms + im - x0] = EOS;
            x0 = X_L0;
        }
        if (x0 < X_L0 && ms < -x0)
        {
            monit[im] = EOS;
            x0 = X_L0;
        }
        xmax = x0 + ms + size - 1;
    }
    if (string == NULL)
    {
        term_type(y, x0, &monit[im], 0, attrtx);
        term_type(y, m_wherex(), " NULL ", 0,ATTR_A);
        return m_wherex();
    }
    ms = strlen(monit);
    format = (char*)Malloc((xmax - x0 + 14) * sizeof(char));
    if (format == NULL)
    {
        if (pznak > 0) monit[pznak] = ' ';
        return -1;
    }
    lwmall++;
    form = &format[8];
    format[7] = EOS;
    bufor = &form[ms + 3 * sizeof(char)];
    bufor[0] = EOS;
    sprintf(format, " -%ds", size);
    format[0] = '%';
    textn[0] = EOS;
    if (monit[0] != '#' && monit[1] != '#' || wpis == 2)
        przepisz(bufor, string, dlstr + 1); /* bylo sprintf(bufor,format,string);*/
    Okno(y, x0, y, xmax, attr);
    x_konc = xmax + txl;
    if (wpis != 1)
    {
        term_type(y, x0, &monit[im], 0, attrtx);
        x_konc = m_wherex();
        Term_Typef(y, x_konc, bufor, size, attr_d);
        if (wpis == 0 || wpis == 3)
        {
            for (i = 0; i < txl; i++)
            {
                if (i + xmax + 1 < X_maxBlank) term_type(y, xmax + 1 + i, &text[i], 1, attrtx);
            }
            x_konc = m_wherex();
        }
        else
        {
            for (i = 0; i < txl; i++)
            {
                if (i + xmax + 1 < X_maxBlank) term_type(y, xmax + 1 + i, " ", 1, attrtx);
            }
            x_konc = m_wherex();
        }
        if (wpis == 3)
        {
            if (help == NULL || l_opcji <= 0 || y >= MY_MAX)
            {
                wpis = 1;
                goto D;
            }
            if (skok == 0)
            {
                ret = okno_menu((char**)help[0], l_opcji, 0, attrtx, attr, y + 1, xmax - size, xmax + 1,
                                NULL, 0);
            }
            else
            {
                int w_min, w_max, size_helpu = 0, sizh, k, xmax_h;
                char *buf = NULL, index_helpu = 0;
                w_min = 0;
                w_max = l_opcji - 1;
                k = term_save_image(y + 1, xmax - size, taknie, 0);
                sizh = MX_MAX - xmax + size + 1;
                xmax_h = MX_MAX;
                buf = (char*)Malloc(sizh * k + 1);
                w_min = 0;
                if (buf != NULL)
                {
                    term_save_image(y + 1, xmax - size, buf, sizh);
                    lwmall++;
                    for (k = 0; k < w_max; k++)
                    {
                        objasn_grupy(y + 1, xmax - size, k, k, k, attr, attrtx, skok, dltn,NULL, help);
                        xmax_h = m_wherex();
                        if (w_min < xmax_h) w_min = xmax_h;
                    }
                    xmax_h = w_min;
                    term_restore_image(y + 1, xmax - size, buf, sizh);
                    Free(buf);
                    lwmall--;
                }
                ret = okno_help(y + 1, xmax - size, xmax_h, attrtx, attr, 'g', w_min = 0, w_max,
                                w_min = 0, w_max, NULL /*&index_helpu*/, size_helpu = 0,
                                sizh = 0, skok, R, NULL, help, 0, NULL, 0, NULL, NULL, NULL); /* tu powinno byc R */
            }
            if (blok == 1) ret = -1;
            if (ret >= 0)
            {
                int j;
                char *opcja, **tabl_opcji;
                k = 0;
                if (skok == 0)
                {
                    tabl_opcji = (char**)help[0];
                    opcja = tabl_opcji[ret];
                }
                else opcja = help[1] + ret * skok;
                for (i = 0, j = strlen(string); j < size; i++, j++)
                {
                    string[j] = opcja[i];
                    k = x_konc;
                    if (opcja[i] == EOS)
                    {
                        string[j] = EOS;
                        break;
                    }
                }
                string[size] = EOS;
                x_konc = k;
            }
            wpis = 1;
            goto D;
        }
    }
    else /* wpis=1 */
    {
        for (i = 0; i < txl; i++)
        {
            if (i + xmax + 1 < X_maxBlank) term_type(y, xmax + 1 + i, &text[i], 1, attrtx);
        }
        x_konc = m_wherex();
        /*D:*/
        sprintf(form, "%s s", monit);
        form[ms] = '%';
    WPIS:
        ins = INSERT;
        INSERT = 1;
        Okno(y, x0, y, xmax, attr);
        clr_Okno();
        ret = bscanf(bufor, attrtx, form, textn);
        INSERT = ins;
        if (ret < 0)
        {
            Free(format);
            lwmall--;
            if (pznak > 0) monit[pznak] = ' ';
            return -5;
        }
        if (ret > 0)
        {
            for (i = 0; i <= size; i++)
            {
                if (string[i] != textn[i]) break;
                if (string[i] == 0)
                {
                    i = size + 1;
                    break;
                }
            }
            if (i > size) ret = 0;
        }
        if (ret == 0)
        {
            strncpy(string, textn, size+1);
            Free(format);
            lwmall--;
            if (pznak > 0) monit[pznak] = ' ';
            return 0;
        }
        Term_Typef(y, xmax - size + 1, textn, size, attr);
        if (bez_potwierdzania == 1)
        {
            zn = 't';
            goto TAK;
        }
        if (xmax + dltn > X_maxBlank)
        {
            i = X_maxBlank - xmax;
            taknie[i] = EOS;
        }
        k = term_save_image(y, xmax + 1, taknie, 0);
        do
        {
            buffer = (char*)Malloc(dltn * k + 1);
            if (buffer != NULL)
            {
                term_save_image(y, xmax + 1, buffer, dltn);
                lwmall++;
            }
            term_type(y, xmax + 1, taknie, 0, attrtx);
            i = m_wherex();
            if (i < MXR_MAX) i--;
            i--;
            term_cur(y, i);
            term_flush();
            zn = GET_char();
            zn = zn & 0x00ff;
            if (buffer != NULL)
            {
                term_restore_image(y, xmax + 1, buffer, dltn);
                Free(buffer);
                lwmall--;
            }
            if (zn == 'n') goto WPIS;
            if (zn == 't')
            TAK:
                {
                    strncpy(string, textn, size+1);
                    Term_Typef(y, xmax - size + 1, string, size, attr);
                    wpisano_dana = 1;
                }
        }
        while (zn != Esc && zn != 't');
    } /* else (wpis==1) */
D: Free(format);
    lwmall--;
    if (pznak != 0) monit[pznak] = ' ';
    return x_konc;
}


void def_haslo(char* hasl, char* kod)
{
    char l;
    if (Klucz != 0)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Nie otwarte raporty. Uzyj funkcji otworz_raporty() !!!!!  <Ent>   ");
        GET_char();
        return;
    }
    if (lhasel < LHASEL)
    {
        l = strlen(hasl);
        if (l > 10)
        {
            hasl[10] = EOS;
            l = 10;
        }
        przepisz(haslo[lhasel], hasl, l + 1);
        l = strlen(kod);
        if (l > 20)
        {
            kod[20] = EOS;
            l = 20;
        }
        przepisz(kod_o[lhasel], kod, l + 1);
        lhasel++;
    }
}

char wpisz_haslo(char** kod_operat, signed char potrzebne_uprawn,
                 char nr_oper, struct reports* Rp)
{
    char poziom_upr, *tytul;
    char buf[10], znak, *form, sukces;
    int i, l, j, ret, lh;
    unsigned attr_o, attrtx;
    tytul = *kod_operat;
    attrtx = TERM_WHITE | MTERM_HILIGHT | TERM_GREEN_BG;
    attr_o = TERM_WHITE | TERM_MAGENTA_BG;
    poziom_upr = 0;
    if ((potrzebne_uprawn % 2) == 1 && nr_oper != 0)
    {
        komun_text(MY_MAX, X_L0, " Zmiany mozna wprowadzic tylko Z TERMINALA GLOWNEGO !! Zostana ANULOWANE. <Ent> ", 0,
                   ATTR_A);
        GET_char();
        return 0;
    }
    if (nr_oper == 0) poziom_upr = 1;
    if (poziom_upr < Uprawn_max) poziom_upr = Uprawn_max;
    if (Rp != NULL && potrzebne_uprawn >= ZAP_MIN) /* zamykamy raporty */
    {
        Okno(MY_MAX, X_L0, MY_MAX, MXR_MAX, attrtx);
        clr_Okno(); /*term_flush(); */
        komun_text(MY_MAX, X_L0, tytul, 0,TERM_WHITE | INVERSE | TERM_BLACK_BG);
        form = "  <Ent> - zapis danych; <Esc> - anulowanie zmian: ??     ";
        i = strlen(form) + 1;
        Term_Typef(MY_MAX, MXR_MAX - i, form, i,TERM_WHITE | MTERM_HILIGHT | TERM_MAGENTA_BG);
        term_flush();
        do
        {
            znak = GET_char();
            znak = znak & 0x00ff;
        }
        while (znak != CR && znak != Esc);
        if (znak == Esc) return 0;
    }
    while (potrzebne_uprawn > Uprawn_max)
    {
        do
        {
            Okno(MY_MAX, X_L0, MY_MAX, MXR_MAX, attrtx);
            clr_Okno();
            komun_text(MY_MAX, X_L0, tytul, 0,TERM_WHITE | INVERSE | TERM_BLACK_BG);
            form = " WPISZ HASLO i zatwierdz je <Ent>: ?? ";
            l = strlen(form);
            i = MXR_MAX - 10 - l;
            Term_Typef(MY_MAX, i, form, l, attrtx);
            term_flush();
            Okno(MY_MAX, MXR_MAX - 11, MY_MAX, MXR_MAX, attr_o);
            clr_Okno(); /*term_flush(); */
            term_cur(MY_MAX, MXR_MAX - 10);
            komun_text(MY_MAX, MXR_MAX - 10, " ", 0, attr_o);
            m_gotoxy(MXR_MAX - 10, MY_MAX);
            buf[0] = EOS;
            ret = czytaj_has(buf, 0, 10);
            if (ret < -1) return 0;
            for (i = 0; i < lhasel; i++)
            {
                sukces = 1;
                if (haslo[i] == NULL) continue;
                lh = strlen(haslo[i]);
                if (lh == 0) continue;
                for (j = 0; j <= lh; j++)
                {
                    if (buf[j] != haslo[i][j])
                    {
                        sukces = 0;
                        break;
                    }
                    if (buf[j] == EOS) break;
                }
                if (sukces == 1) break;
            }
            if (sukces == 0)
            {
                komun_text(MY_MAX, X_L0,
                           " NIEWLASCIWE HASLO: <Ent> - powtorka; <Esc> - wyjscie z anulowaniem zmian: ??  ", 0,ATTR_A);
                do
                {
                    znak = GET_char();
                    znak = znak & 0x00ff;
                }
                while (znak != CR && znak != Esc);
                if (znak == Esc) return 0;
            }
        }
        while (sukces == 0);
        if (i == 0) ret = 4;
        else ret = 2;
        if (ret + poziom_upr < potrzebne_uprawn)
        {
            komun_text(MY_MAX, X_L0,
                       "  ZA NISKIE UPRAWNIENIA: <Ent> -powtorka; <Esc> -wyjscie z anulow.zmian ?? ", 0,ATTR_A);
            do
            {
                znak = GET_char();
                znak = znak & 0x00ff;
            }
            while (znak != CR && znak != Esc);
            if (znak == Esc) return 0;
        }
        if (poziom_upr + ret > Uprawn_max)
        {
            Uprawn_max = poziom_upr + ret;
            Kod_operatora_max = *kod_operat;
        }
    } /* koniec while()  */
    return 1;
}

int ustaw_uprawnienia(char* tytul, char potrzebne_uprawn, char nr_oper)
{
    char *h, buf[10], znak, *form, sukces;
    int i, l, j, ret, lh, poziom_upr;
    unsigned int attrtx = TERM_WHITE | MTERM_HILIGHT | TERM_GREEN_BG;
    unsigned int attr_o = TERM_WHITE | TERM_MAGENTA_BG;
    poziom_upr = 0;
    if ((potrzebne_uprawn % 2) == 1 && nr_oper != 0)
    {
        komun_text(MY_MAX, X_L0, " Poziom uprawnien %d wymaga TERMINALA GLOWNEGO !! <Ent> ", 0,ATTR_A);
        GET_char();
        return -1;
    }
    if (nr_oper == 0) poziom_upr = 1;
    if (poziom_upr < Uprawn_pocz) poziom_upr = Uprawn_pocz;
    if (poziom_upr >= potrzebne_uprawn) return 1;
    while (potrzebne_uprawn > Uprawn_pocz)
    {
        do
        {
            Okno(MY_MAX, X_L0, MY_MAX, MXR_MAX, attrtx);
            clr_Okno();
            komun_text(MY_MAX, X_L0, tytul, 0,TERM_WHITE | INVERSE | TERM_BLACK_BG);
            form = " WPISZ HASLO i zatwierdz je <Ent>: ?? ";
            l = strlen(form);
            i = MXR_MAX - 11 - l;
            Term_Typef(MY_MAX, i, form, l, attrtx);
            term_flush();
            Okno(MY_MAX, MXR_MAX - 10, MY_MAX, MXR_MAX, attr_o);
            clr_Okno();
            term_flush();
            buf[0] = EOS;
            term_cur(MY_MAX, MXR_MAX - 10);
            komun_text(MY_MAX, MXR_MAX - 10, " ", 0, attr_o);
            m_gotoxy(MXR_MAX - 10, MY_MAX);
            ret = czytaj_has(buf, 0, 10);
            if (ret < -1) return -1;
            for (i = 0; i < lhasel; i++)
            {
                sukces = 1;
                if (haslo[i] == NULL) continue;
                lh = strlen(haslo[i]);
                if (lh == 0) continue;
                for (j = 0; j <= lh; j++)
                {
                    if (buf[j] != haslo[i][j])
                    {
                        sukces = 0;
                        break;
                    }
                    if (buf[j] == EOS) break;
                }
                if (sukces == 1) break;
            }
            if (sukces == 0)
            {
                komun_text(MY_MAX, X_L0,
                           " NIEWLASCIWE HASLO: <Ent> - powtorka; <Esc> - wyjscie z procedury: ??   ", 0,ATTR_A);
                do
                {
                    znak = GET_char();
                    znak = znak & 0x00ff;
                }
                while (znak != CR && znak != Esc);
                if (znak == Esc) return -1;
            }
        }
        while (sukces == 0);
        if (i == 0) ret = 4;
        else ret = 2;
        if (ret + poziom_upr < potrzebne_uprawn)
        {
            komun_text(MY_MAX, X_L0,
                       "ZA NISKIE UPRAWNIEN: <Ent> -powtorka; <Esc> -wyjscie z anulowaniem zmian: ?? ", 0,ATTR_A);
            do
            {
                znak = GET_char();
                znak = znak & 0x00ff;
            }
            while (znak != CR && znak != Esc);
            if (znak == Esc) return 0;
        }
        if (poziom_upr + ret > Uprawn_max)
        {
            Uprawn_max = poziom_upr + ret;
            Kod_operatora_max = NULL;
            Uprawn_pocz = Uprawn_max;
        }
    } /* koniec while()  */
    return 1;
}

int def_raport(signed char kod_raportu, int anim_pid, int ob_pocz,
               int ob_konc, int rozmiar_ob, char* tytul, signed char Kolor,
               signed char Ramka, char* D, int max_l_danych_blank,
               int (*def_blankiet)(int nr_ob, int ob_pocz,
                                   int ob_konc, int xp, int yp,
                                   int anim_pid, char* D),
               char*(*dane_rap_bl)(int ob_pocz, int ob_konc, int* rozmiar_ob),
               void (*wpis_rap_bl)(int ob_pocz, int ob_konc, char* D,
                                   int rozmiar_ob, char zapis[], char* Kod_op))
{
    return def_Report(kod_raportu, anim_pid, ob_pocz, ob_konc, rozmiar_ob,
                      tytul, Kolor, Ramka, D, max_l_danych_blank, BAZA_SPOJNA,
                      def_blankiet, dane_rap_bl, wpis_rap_bl);
}

int def_Report(signed char kod_raportu, int anim_pid, int ob_pocz,
               int ob_konc, int rozmiar_ob, char* tytul, signed char Kolor,
               signed char Ramka, char* D, int max_l_danych_blank, char typ_bazy,
               int (*def_blankiet)(int nr_ob, int ob_pocz,
                                   int ob_konc, int xp, int yp,
                                   int anim_pid, char* D),
               char*(*dane_rap_bl)(int ob_pocz, int ob_konc, int* rozmiar_ob),
               void (*wpis_rap_bl)(int ob_pocz, int ob_konc, char* D,
                                   int rozmiar_ob, char zapis[], char* Kod_op))
{
    int i, j, l, k;
    struct reports* R;
    if (Klucz != 0)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Nie otwarte raporty. Uzyj funkcji otworz_raporty() !!!!!  <Ent>    ");
        GET_char();
        return -1;
    }
    if (typ_bazy != BAZA_SPOJNA)
    {
        ob_konc = ob_konc - ob_pocz;
        ob_pocz = 0;
    }
    if (ob_konc < ob_pocz)
    {
        komunikat(MY_MAX, 1,ATTR_A, "%s. Brak obiektow raportu !! <Ent> ", tytul);
        GET_char();
        return -1;
    }
    for (i = 0; i < LRAP_BL; i++)
    {
        if (RAP[i] == NULL) break;
        if (RAP[i]->kod_raportu == kod_raportu) break;
    }
    if (i == LRAP_BL) return -1;
    R = RAP[i];
    if (R != NULL) /* przedefiniowanie raportu */
    {
        if (R->D != NULL)
        {
            if (R->zapis != NULL)
            {
                zapis_raportu(R, 0, 0); /* !!! to moze dzialac zle, gdy UNDO wymaga kontroli */
                if (R->d_oryg != NULL && (R->free_mem & 0x01) == 0x01)
                {
                    l = R->ob_konc - R->ob_pocz + 1;
                    for (j = 0; j < l; j++)
                    {
                        if (R->d_oryg[j] != NULL)
                        {
                            Free(R->d_oryg[j]);
                            R->d_oryg[j] = NULL;
                            lwmall--;
                        }
                    }
                    Free(R->d_oryg);
                    lwmall--;
                }
                if ((R->free_mem & 0x01) == 0x01)
                {
                    Free(R->zapis);
                    lwmall--;
                    R->free_mem &= 0xfe;
                }
            }
            if ((R->free_mem & 0x02) == 0x02) R->D = zwolnij_baze(R);
        }
        zamknij_grafiki(R->kod_raportu);
        free_all_rep_mem(R);
    }
    else RAP[i] = (struct reports*)Malloc(sizeof(struct reports));
    R = RAP[i];
    if (R == NULL)
    {
        int zn = 0, l = strlen(tytul);
        if (l > 40)
        {
            zn = tytul[40];
            tytul[40] = 0;
        }
        komunikat(MY_MAX, 1,ATTR_A, "%s. Brak miejsca na dane !! <Ent> ",
                  tytul);
        if (zn != 0) tytul[40] = zn;
        GET_char();
        return -1;
    }
    lwmall++;
    R->typ_bazy = typ_bazy;
    R->kod_raportu = kod_raportu;
    R->anim_pid = anim_pid;
    R->ob_pocz = ob_pocz;
    R->ob_konc = ob_konc;
    R->rozmiar_ob = rozmiar_ob;
    R->tytul = tytul;
    R->Kolor = Kolor;
    R->Ramka = Ramka;
    R->D = NULL;
    R->bufs = NULL;
    R->free_mem = 0;
    R->zapis = NULL;
    R->uprawnienia = 0;
    k = LRAP_BL;
    /*komunikat(MY_MAX,X_L0,attryb,"Raport %s zgloszony na poz.%d",R->tytul,i); */
    if (D != NULL)
    {
        struct reports* r;
        int pocz;
        for (k = 0; k < LRAP_BL; k++)
        {
            if (RAP[k] == NULL) continue;
            r = RAP[k];
            if (r->D != D) continue;
            if ((r->free_mem & 0x01) != 0x01) continue;
            if (r->rozmiar_ob != rozmiar_ob) continue;
            if (r->ob_konc < ob_pocz) continue;
            pocz = ob_pocz; /* mamy podrapot - dane juz sa */
            if (r->ob_pocz > pocz) pocz = r->ob_pocz;
            R->D = D;
            R->typ_bazy = r->typ_bazy;
            /*      R->D=D-(r->ob_pocz-pocz)*rozmiar_ob; */
            R->zapis = r->zapis - (r->ob_pocz - pocz);
            R->d_oryg = r->d_oryg - (r->ob_pocz - pocz) * sizeof(char**);
            R->free_mem = 0;
            ob_pocz = pocz;
            if (r->ob_konc < ob_konc) ob_konc = r->ob_konc;
            break;
        }
        if (k == LRAP_BL) /* nie jest to podraport */
        {
            l = ob_konc - ob_pocz + 1;
            R->D = D;
            R->zapis = Malloc(l * sizeof(char));
            if (R->zapis == NULL)
            {
                int l = strlen(tytul), zn = 0;
                if (l > 30) zn = tytul[30];
                tytul[30] = 0;
                komunikat(MY_MAX, X_L0,ATTR_A, "%s. Brak pamieci na notatnik. Bedzie tylko bierny. <ENT> ",
                          tytul);
                if (zn != 0) tytul[30] = zn;
                R->uprawnienia = -1;
                GET_char();
            }
            else
            {
                R->free_mem = 0x01;
                lwmall++;
                for (i = 0; i < l; i++) R->zapis[i] = 0;
                R->d_oryg = Malloc(l * sizeof(char**));
                if (R->d_oryg == NULL)
                {
                    int l = strlen(tytul), zn = 0;
                    if (l > 30) zn = tytul[30];
                    tytul[30] = 0;
                    komunikat(MY_MAX, X_L0,ATTR_A, "%s. Brak pamieci na notatnik. Bedzie tylko bierny. <ENT> ",
                              tytul);
                    if (zn != 0) tytul[30] = zn;
                    Free(R->zapis);
                    R->free_mem &= 0xfe;
                    R->zapis = NULL;
                    lwmall--;
                    R->uprawnienia = -1;
                    GET_char();
                }
                else
                {
                    lwmall++;
                    for (i = 0; i < l; i++) R->d_oryg[i] = NULL;
                }
            }
        }
    } /*  ================ koniec dla D!=NULL  */
    R->max_l_danych_blank = max_l_danych_blank;
    R->dane_rap_bl = dane_rap_bl;
    R->wpis_danych_bl = wpis_rap_bl;
    R->def_blankiet = def_blankiet;
    R->ob_pocz = ob_pocz;
    R->ob_konc = ob_konc;
    /* komunikat(MY_MAX,m_wherex(),attryb," R=%p <Ent>",R); GET_char(); */
    return i;
}

char* zwolnij_baze(struct reports* R)
{
    if ((R->free_mem & 0x02) != 0x02) return R->D;
    if (R->typ_bazy == BAZA_WEKTOR)
    {
        int k;
        char** baza;
        baza = (char**)(R->D);
        for (k = 0; k <= R->ob_konc; k++)
        {
            if (baza[k] != NULL)
            {
                Free(baza[k]);
                lwmall--;
            }
        }
    }
    if (R->typ_bazy == BAZA_WEKTOR)
    {
        void* L;
        L = (void**)(R->D);
        while (L != NULL) { L = free_list(&L, L); }
        R->free_mem &= 0xfd;
        return NULL;
    }
    Free(R->D);
    R->free_mem &= 0xfd;
    lwmall--;
    return NULL;
}

/* ----------------- 28.08.00 ----------------------------------- */
void zamknij_rap_kod(int kod_raportu)
{
    int i = 0, j, lrek, ret = 0, Ret, poprz_dec = 0, wymag_uprawn, zmiany;
    struct reports* R;
    for (i = 0; i < LRAP_BL; i++)
    {
        R = RAP[i];
        if (R == NULL) continue;
        if (R->kod_raportu == kod_raportu) break;
    }
    if (i < LRAP_BL)
    {
        if (R->D != NULL)
        {
            if (R->zapis != NULL && (R->free_mem & 0x01) == 0x01)
            {
                if (R->d_oryg != NULL)
                {
                    lrek = R->ob_konc - R->ob_pocz + 1;
                    for (j = 0; j < lrek; j++)
                    {
                        if (R->d_oryg[j] != NULL)
                        {
                            Free(R->d_oryg[j]);
                            R->d_oryg[j] = NULL;
                            lwmall--;
                        }
                    }
                    Free(R->d_oryg);
                    lwmall--;
                }
                Free(R->zapis);
                lwmall--;
            }
            R->D = zwolnij_baze(R);
        }
        zamknij_grafiki(R->kod_raportu);
        free_all_rep_mem(R);
        /* komunikat(MY_MAX,X_L0,ATTR_A," Zamyk.raport nr %d R=%p ",i, R); GET_char(); */
        Free(R);
        lwmall--;
        RAP[i] = NULL;
    }
}

/* -------------------------------------------------------------- */
void zamknij_raporty(void) { zamkniecie_raportow(0); }

int zamknij_system_raportow(void)
{
    char opcja = 1, i, k;
    if (blokada_zapisu == ZAPIS_WSZYSTKICH_RAZEM) opcja = 0;
    return zamkniecie_raportow(opcja);
}

int zamkniecie_raportow(char kontr_spojnosci)
{
    int i = 0, j, lrek, ret = 0, Ret, poprz_dec = 0, wymag_uprawn, zmiany;
    struct reports* R;
    if (nr_blank >= 0)
    {
        do
        {
            i = BL[nr_blank]->nr_rekordu;
            /* komunikat(MY_MAX,X_L0,ATTR_A," Zamkn.bl.%d rek.%d ",nr_blank, i); GET_char(); */
            zamknij_blankiet(&i);
        }
        while (nr_blank >= 0);
        /* komunikat(MY_MAX,X_L0,ATTR_A," Wszystk.zamkn: nr_blank=%d ",nr_blank); GET_char(); */
    }
    Czy_Blankiet_otwarty = 0;
    nr_blank = -1;
    ret = 1;
    Ret = 1;
    if (blokada_zapisu == ZAPIS_WSZYSTKICH_RAZEM)
    {
        if (Klucz != 0) poprz_dec = UNDO; /* tego nie powinno byc bo tu jest tylko jedno wywolanie */
        else
        {
            poprz_dec = -1;
            wymag_uprawn = -1;
            kontr_spojnosci = 0;
            for (i = 0; i < LRAP_BL; i++)
            {
                R = RAP[i];
                if (R == NULL) continue;
                zmiany = 0;
                if (R->zapis != NULL && (R->free_mem & 0x01) == 0x01)
                {
                    lrek = R->ob_konc - R->ob_pocz + 1;
                    for (j = 0; j < lrek; j++)
                    {
                        if (R->zapis[j] >= ZAP_MIN)
                        {
                            zmiany = 1;
                            break;
                        }
                    }
                    if (zmiany == 1 && R->uprawnienia > wymag_uprawn) wymag_uprawn = R->uprawnienia;
                }
            }
        }
    }
    Klucz = 1;
    for (i = 0; i < LRAP_BL; i++)
    {
        R = RAP[i];
        if (R == NULL) continue;
        if (R->D != NULL)
        {
            if (R->zapis != NULL && (R->free_mem & 0x01) == 0x01)
            {
                if (poprz_dec == 0) wymag_uprawn = R->uprawnienia;
                ret = zapis_raportu(R, poprz_dec, wymag_uprawn);
                if (ret < 0) Ret = ret;
                if (blokada_zapisu == ZAPIS_WSZYSTKICH_RAZEM && poprz_dec == -1)
                {
                    if (ret < 0) poprz_dec = UNDO;
                    else poprz_dec = ZAP_OBOWIAZK;
                }
            }
        }
    }
    zamknij_grafiki(-1);
    if (Ret < 0 && kontr_spojnosci == 1) return Ret;
    for (i = 0; i < LRAP_BL; i++)
    {
        R = RAP[i];
        if (R == NULL) continue;
        /*  if(R->anim_pid>0) zabic zadanie(R->anim_pid) */
        if (R->D != NULL)
        {
            if (R->zapis != NULL && (R->free_mem & 0x01) == 0x01)
            {
                if (R->d_oryg != NULL)
                {
                    lrek = R->ob_konc - R->ob_pocz + 1;
                    for (j = 0; j < lrek; j++)
                    {
                        if (R->d_oryg[j] != NULL)
                        {
                            Free(R->d_oryg[j]);
                            R->d_oryg[j] = NULL;
                            lwmall--;
                        }
                    }
                    Free(R->d_oryg);
                    lwmall--;
                }
                Free(R->zapis);
                lwmall--;
            }
            R->D = zwolnij_baze(R);
        }
        zamknij_grafiki(R->kod_raportu);
        free_all_rep_mem(R);
        /* komunikat(MY_MAX,X_L0,ATTR_A," Zamyk.raport nr %d R=%p ",i, R); GET_char(); */
        Free(R);
        lwmall--;
        RAP[i] = NULL;
    }
    if (lwmall != lwmallp)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Przybylo alokacji pamieci w tej sesji: bylo %d jest %d <Ent>  ",
                  lwmallp, lwmall);
        GET_char();
    }
    /* komunikat(MY_MAX,X_L0,ATTR_A," Wszystkie rap. zamkn"); GET_char(); */
    lwmallp = lwmall;
    NR_OPER = -1;
    Uprawn_max = 0;
    Kod_operatora_max = NULL;
    ZAPIS = 0;
    BEZ_HASLA = 0;
    EDIT = 0;
    SLEDZIC = 0;
    blokada_zapisu = ZAPIS_DOWOLNY;
    bez_potwierdzania = 0;
    Klucz = -1;
    blokada_potwierdzania = 0;
#ifdef _TERM_QNX_
    if (mouse_free > 0)
    {
        if (kproxy > 0) qnx_proxy_detach(kproxy);
        kproxy = -1;
        if (Mouse_ctrl != NULL)
        {
            mouse_close(Mouse_ctrl);
            Mouse_ctrl = NULL;
            if (Mproxy > 0) qnx_proxy_detach(Mproxy);
            Mproxy = -1;
        }
    }
#endif
    mouse_free = 0;
    return 1;
}

def_blankiet czy_jest(signed char kod_raportu, int* anim_pid, int* ob_pocz,
                      int* ob_konc, int* rozmiar_ob, char** tytul,
                      signed char* Kolor, signed char* Ramka, char** D)
{
    int i;
    struct reports* R;
    for (i = 0; i < LRAP_BL; i++)
    {
        R = RAP[i];
        if (R == NULL) break;
        if (R->kod_raportu != kod_raportu) continue;
        *rozmiar_ob = R->rozmiar_ob;
        if (strukt_danych_raportu(R, rozmiar_ob, i) == NULL) return NULL;
        *anim_pid = R->anim_pid;
        *ob_pocz = R->ob_pocz;
        *ob_konc = R->ob_konc;
        *rozmiar_ob = R->rozmiar_ob;
        *tytul = R->tytul;
        *Kolor = R->Kolor;
        *Ramka = R->Ramka;
        *D = ustal_adres_rek(kod_raportu, 0);
        if ((*D) != NULL) return R->def_blankiet;
        else break;
    }
    return NULL;
}

void* alloc_list(void* (*L), long int size)
{
    void *l, *next;
    if ((*L) == NULL)
    {
        (*L) = (void*)Malloc(size);
        if ((*L) == NULL) return NULL;
        *((void**)(*L)) = NULL;
        lwmall++;
        return (*L);
    }
    l = (*L);
    next = *(void**)l;
    while (next != NULL)
    {
        l = (void*)next;
        next = *(void**)l;
    }
    next = Malloc(size);
    if (next == NULL) return next;
    *(void**)l = next;
    lwmall++;
    *(void**)next = NULL;
    return next;
}

void* free_list(void* (*L), void* lf)
{
    void *l, *next, *lp = NULL;
    if ((*L) == NULL) return NULL;
    l = (*L);
    next = *(void**)l;
    while (next != NULL && l != lf)
    {
        lp = l;
        l = next;
        next = *(void**)l;
    }
    Free(l);
    lwmall--;
    if (lp != NULL) *((void**)lp) = next;
    else *L = next;
    return *L;
}

char* podaj_adres_rap(int kod_raportu)
{
    int i;
    struct reports* R;
    for (i = 0; i < LRAP_BL; i++)
    {
        R = RAP[i];
        if (R == NULL)
        {
            komunikat(MY_MAX, X_L0,ATTR_A, " Brak raportu %d !!!!!!!!!!!!!!! <Ent> ", kod_raportu);
            //			term_color(TERM_WHITE|TERM_BLACK_BG);
            return NULL;
        }
        if (R->kod_raportu != kod_raportu) continue;
        return R->D;
    }
    return NULL;
}

void ustaw_rek_max_raportu(int (*def_blank)(int nr_ob, int ob_pocz,
                                            int ob_konc, int xp, int yp, int anim_pid,
                                            char* d), int rek_max)
{
    int i;
    struct reports* R;
    for (i = 0; i < LRAP_BL; i++)
    {
        R = RAP[i];
        if (R == NULL) continue;
        if (R->def_blankiet != def_blank) continue;
        /*		if(R->kod_raportu!=kod_raportu) continue; */
        if (rek_max < R->ob_pocz) return;
        R->ob_konc = rek_max;
        return;
    }
}

char* ustal_adres_rek(int kod_raportu, int rek_no)
{
    int i;
    struct reports* R;
    char* D;
    for (i = 0; i < LRAP_BL; i++)
    {
        R = RAP[i];
        if (R == NULL) goto ERR_BRAK_RAP;
        if (R->kod_raportu != kod_raportu) continue;
        if (rek_no < R->ob_pocz) rek_no = R->ob_pocz;
        if (R->typ_bazy == BAZA_SPOJNA) D = R->D + R->rozmiar_ob * (rek_no - R->ob_pocz);
        else
        {
            if (R->typ_bazy == BAZA_WEKTOR) /* adresy rekordow sa w tablicy */
            {
                char **baza, k;
                baza = (char**)(R->D);
                k = rek_no;
                /*       for(k=rek_no; k<=R->ob_konc; k++) {if(baza[k]!=NULL) break;}
                                 if(k>R->ob_konc)
                                     {for(k=R->ob_pocz; k<rek_no; k++) {if(baza[k]!=NULL) break;}
                                      if(k==rek_no) goto ERR_DANE;
                                     } */
                D = baza[k];
            }
            else /* adresy rekordow sa lista */
            {
                void *l, *next, *lp = NULL, *(*L);
                int k;
                L = (void**)(R->D);
                if ((*L) == NULL) goto ERR_DANE;
                l = (*L);
                next = *(void**)l;
                for (k = 0; k <= rek_no; k++)
                {
                    if (next == NULL) break;
                    lp = l;
                    l = next;
                    next = *(void**)l;
                }
                if (next == NULL && k < R->ob_konc) goto ERR_DANE;
                D = (char*)lp;
            }
        }
        return D;
    }
ERR_DANE:
    komunikat(MY_MAX, X_L0,ATTR_A, " Brak danych raportu %d !!!!!!!!!!!!!!! <Ent> ", kod_raportu);
    return NULL;
ERR_BRAK_RAP:
    komunikat(MY_MAX, X_L0,ATTR_A, " Brak raportu %d !!!!!!!!!!!!!!! <Ent> ", kod_raportu);
    return NULL;
}


char* strukt_danych_raportu(struct reports* R, int* rozmiar_ob, int i)
{
    int l;
    if (R->zapis == NULL) /* dane nie sa z zewnatrz */
    {
        l = R->ob_konc - R->ob_pocz + 1;
        R->zapis = Malloc(l * sizeof(char));
        if (R->zapis == NULL)
        {
            int l = strlen(R->tytul), zn = 0;
            if (l > 30) zn = R->tytul[30];
            R->tytul[30] = 0;
            komunikat(MY_MAX, X_L0,ATTR_A, "%s. Brak pamieci na notatnik. Bedzie tylko bierny. <ENT> ",
                      R->tytul);
            if (zn != 0) R->tytul[30] = zn;
            R->uprawnienia = 0;
            R->d_oryg = NULL;
            GET_char();
        }
        else
        {
            R->free_mem |= 0x01;
            lwmall++;
            for (i = 0; i < l; i++) R->zapis[i] = 0;
            R->d_oryg = Malloc(l * sizeof(char**));
            if (R->d_oryg == NULL)
            {
                int l = strlen(R->tytul), zn = 0;
                if (l > 30) zn = R->tytul[30];
                R->tytul[30] = 0;
                komunikat(MY_MAX, X_L0,ATTR_A, "%s. Brak pamieci na notatnik. Bedzie tylko bierny. <ENT> ",
                          R->tytul);
                if (zn != 0) R->tytul[30] = zn;
                if ((R->free_mem & 0x01) == 1 && R->zapis != NULL)
                {
                    Free(R->zapis);
                    lwmall--;
                    R->free_mem &= 0xfe;
                }
                R->uprawnienia = -1;
                GET_char();
            }
            else
            {
                lwmall++;
                for (i = 0; i < l; i++) R->d_oryg[i] = NULL;
            }
        }
    }
    if (R->D == NULL)
    {
        R->D = (R->dane_rap_bl)(R->ob_pocz, R->ob_konc, rozmiar_ob);
        if (R->D == NULL)
        {
            int l = strlen(R->tytul), zn = 0;
            if (l > 30) zn = R->tytul[30];
            R->tytul[30] = 0;
            komunikat(MY_MAX, X_L0,ATTR_A, "%s. Brak pamieci dla danych. <ENT> ",
                      R->tytul);
            zamknij_grafiki(R->kod_raportu);
            free_all_rep_mem(R);
            if (zn != 0) R->tytul[30] = zn;
            if ((R->free_mem & 0x01) == 1 && R->zapis != NULL)
            {
                Free(R->zapis);
                lwmall--;
                R->free_mem &= 0xfe;
                if (R->d_oryg != NULL)
                {
                    Free(R->d_oryg);
                    lwmall--;
                }
            }
            Free(R);
            RAP[i] = NULL;
            lwmall--;
            GET_char();
            return NULL;
        }
        lwmall++;
        R->free_mem |= 0x02;
    }
    if (R->rozmiar_ob > 0 && R->rozmiar_ob != *rozmiar_ob)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, "%s. Struktura danych niezgodna z deklarowana. (%d %d) <ENT> ",
                  R->tytul, R->rozmiar_ob, *rozmiar_ob);
        if (R->zapis != NULL && (R->free_mem & 0x01) == 0x01)
        {
            Free(R->zapis);
            R->zapis = NULL;
            lwmall--;
            R->free_mem &= 0xfe;
            if (R->d_oryg != NULL)
            {
                Free(R->d_oryg);
                lwmall--;
            }
        }

        R->D = zwolnij_baze(R);
        zamknij_grafiki(R->kod_raportu);
        free_all_rep_mem(R);
        Free(R);
        lwmall--;
        RAP[i] = NULL;
        GET_char();
        return NULL;
    }
    R->rozmiar_ob = *rozmiar_ob;
    return R->D;
}

char* dane_raportowanego_rekordu(int (*def_blank)(int nr_ob, int ob_pocz,
                                                  int ob_konc, int xp, int yp, int anim_pid,
                                                  char* d), int* nr_rekordu)
{
    struct reports* R;
    if (nr_blank < 0) goto P;
    if (BL[nr_blank] == NULL)
    {
    P:
        komunikat(MY_MAX, X_L0,ATTR_A, " Otworz blankiet dla rekordu %d. <ENT> ", *nr_rekordu);
        GET_char();
        *nr_rekordu = -1;
        return NULL;
    }
    R = RAP[BL[nr_blank]->nr_raportu];
    if (R == NULL)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Otworz raport %d dla rekordu %d. <ENT> ",
                  BL[nr_blank]->nr_raportu, *nr_rekordu);
        GET_char();
        *nr_rekordu = -1;
        return NULL;
    }
    if (R->def_blankiet != def_blank)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Niezgodnosc funkcji generujacej %s. <ENT> ", R->tytul);
        GET_char();
        *nr_rekordu = -1;
        return NULL;
    }
    *nr_rekordu = BL[nr_blank]->nr_rekordu;
    return ustal_adres_rek(R->kod_raportu, (*nr_rekordu));
}

char* czy_zdefiniowany(signed char kod_raportu, int* ob_pocz, int* ob_konc,
                       int* rozmiar_ob)
{
    int i;
    char* D;
    struct reports* R;
    *rozmiar_ob = 0;
    for (i = 0; i < LRAP_BL; i++)
    {
        R = RAP[i];
        if (R == NULL) continue;
        if (R->kod_raportu != kod_raportu) continue;
        if (strukt_danych_raportu(R, &R->rozmiar_ob, i) == NULL) return NULL;
        *ob_pocz = R->ob_pocz;
        *ob_konc = R->ob_konc;
        *rozmiar_ob = R->rozmiar_ob;
        D = R->D;
        return ustal_adres_rek(kod_raportu, 0);
    }
    return NULL;
}

/* ================================================================= */
void zapis_zwykly(void) { ZAPIS = 0; }

void przelacz_potwierdzanie(void)
{
    if (bez_potwierdzania == 1 && blokada_potwierdzania == 0) bez_potwierdzania = 0;
    else bez_potwierdzania = 1;
}

/* ---------------------------------------------------------------------- */
char* tablica_zapisu(int kod_raportu)
{
    int i;
    for (i = 0; i < LRAP_BL; i++)
    {
        if (RAP[i] == NULL) continue;
        if (RAP[i]->kod_raportu != kod_raportu) continue;
        break;
    }
    if (i < LRAP_BL) return RAP[i]->zapis;
    else return NULL;
}


char* stare_dane(int kod_raportu, int nr_rekordu)
{
    int i;
    char *d_oryg_r, *d_rek;
    for (i = 0; i < LRAP_BL; i++)
    {
        if (RAP[i] == NULL) continue;
        if (RAP[i]->kod_raportu != kod_raportu) continue;
        break;
    }
    if (i >= LRAP_BL) return NULL;
    if (nr_rekordu < RAP[i]->ob_pocz || nr_rekordu > RAP[i]->ob_konc) return NULL;
    d_rek = RAP[i]->D;
    if (d_rek == NULL) return NULL;
    d_rek = d_rek + (nr_rekordu - RAP[i]->ob_pocz) * RAP[i]->rozmiar_ob;
    if (RAP[i]->d_oryg == NULL) return d_rek;
    d_oryg_r = RAP[i]->d_oryg[nr_rekordu - RAP[i]->ob_pocz];
    if (d_oryg_r == NULL) return d_rek;
    return d_oryg_r;
}

void odnow_blankiet(int kod_raportu, int nr_rekordu)
{
    int i, nr_bl, nr_rap;
    struct blankiet* B;
    struct reports* R = NULL;
    for (i = 0; i < LRAP_BL; i++)
    {
        if (RAP[i] == NULL) continue;
        if (RAP[i]->kod_raportu != kod_raportu) continue;
        R = RAP[i];
        nr_rap = i;
        for (nr_bl = 0; nr_bl < LBLANK; nr_bl++)
        {
            B = BL[nr_bl];
            if (B == NULL) continue;
            if (B->nr_raportu != nr_rap) continue;
            if (nr_rekordu >= 0)
            {
                if (B->nr_rekordu != nr_rekordu) continue;
                if (nr_rekordu < R->ob_pocz) continue;
                if (nr_rekordu > R->ob_konc) continue;
            }
            B->reset = 1;
            /* fprintf(mystderr,"\nodnow_blank(%d): B=%d RAPORT=%d/%d nr_rek=%d",nr_bl,B,kod_raportu,nr_rap,nr_rekordu); */
        }
    }
}

void tytul_raportu(int kod_raportu, char* tytul)
{
    int i, nr_bl, nr_rap;
    struct reports* R = NULL;
    for (i = 0; i < LRAP_BL; i++)
    {
        if (RAP[i] == NULL) continue;
        if (RAP[i]->kod_raportu != kod_raportu) continue;
        R = RAP[i];
        nr_rap = i;
        R->tytul = tytul;
    }
}

void wpisz_rek_konc_raportu(int kod_raportu, int rek_pocz, int rek_konc)
{
    int i, nr_bl, nr_rap;
    struct reports* R = NULL;
    for (i = 0; i < LRAP_BL; i++)
    {
        if (RAP[i] == NULL) continue;
        if (RAP[i]->kod_raportu != kod_raportu) continue;
        R = RAP[i];
        if (rek_konc > R->ob_konc) rek_konc = R->ob_konc;
        if (rek_pocz > R->ob_pocz) rek_pocz = R->ob_pocz;
        if (rek_pocz < 0) rek_pocz = 0;
        R->ob_konc = rek_konc;
        R->ob_pocz = rek_pocz;
    }
}

#define TXM 15
static int total_size = 0;
/*struct lista {char *adr_nast; int size; int rekord;}; w wewybl.h */
void* calloc_rep_mem(long int size, int kod_raportu, int rekord, char* txt)
{
    long int i, ds;
    char *buf, *adr_buf, **poprz_adr, znak = 0, k;
    struct reports* R = NULL;
    struct lista* l;
    if (strlen(txt) > TXM)
    {
        znak = txt[TXM];
        txt[TXM] = 0;
    }
    for (i = 0; i < LRAP_BL; i++)
    {
        if (RAP[i] == NULL) continue;
        if (RAP[i]->kod_raportu != kod_raportu) continue;
        R = RAP[i];
        break;
    }
    if (i == LRAP_BL)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Brak raportu %d do alokacji bufora %s(%d) i=%d <Ent>    ",
                  kod_raportu, txt, size, i);
        /*		for(k=0;k<=i;k++)
                 {komunikat(MY_MAX-k,X_L0,TERM_WHITE|TERM_BLUE_BG," RAP[%d]=%p",k,RAP[k]);
        //			if(RAP[k]!=NULL) komunikat(MY_MAX-k,m_wherex(),TERM_WHITE|TERM_BLUE_BG," %s",RAP[k]->tytul);
                 } */
        GET_char(); /*term_color(TERM_WHITE|TERM_BLUE_BG); */
        if (znak != 0) txt[TXM] = znak;
        return NULL;
    }
    /*  if((R->free_mem&0x02)!=0x02)
         {komunikat(MY_MAX,X_L0,ATTR_A," Raport %d nie zwalnia pamieci - alokacja %s(%d) niemozliwa " ,
                                    kod_raportu, txt, size);
            GET_char(); if(znak!=0) txt[TXM]=znak; return NULL;
         }
    */
    if (size <= 0)
    {
        if (strlen(txt) > 0)
        {
            komunikat(MY_MAX, X_L0,ATTR_A, " Niewlasciwy rozmiar=%d bufora %s dla raportu %d  <Ent>    ",
                      size, txt, kod_raportu);
            GET_char();
        }
        if (znak != 0) txt[TXM] = znak;
        return NULL;
    }
    ds = sizeof(struct lista);
    buf = (char*)Malloc(size + ds);
    if (buf == NULL)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Brak pamieci na %s(%d) dla raportu %d (tot.siz=%d) <Ent> ",
                  txt, size, kod_raportu, total_size);
        GET_char();
        if (znak != 0) txt[TXM] = znak;
        return NULL;
    }
    lwmall++;
    l = (struct lista*)&R->bufs;
    while (l->adr_nast != NULL) { l = (struct lista*)(l->adr_nast); }
    l->adr_nast = buf;
    l = (struct lista*)buf;
    l->adr_nast = NULL;
    l->size = size;
    l->rekord = rekord;
    total_size += (size + ds);
    for (i = 0, adr_buf = buf + ds; i < size; i++, adr_buf++) *adr_buf = 0;
    if (znak != 0) txt[TXM] = znak;
    return (buf + ds);
}

struct lista* szukaj_adr_alloc(int kod_raportu, char* txt,
                               char* co, char* adr, int* liczba)
{
    int i, ds;
    char znak = 0;
    struct reports* R = NULL;
    struct lista* l;
    *liczba = 0;
    if (strlen(txt) > TXM)
    {
        znak = txt[TXM];
        txt[TXM] = 0;
    }
    for (i = 0; i < LRAP_BL; i++)
    {
        if (RAP[i] == NULL) continue;
        if (RAP[i]->kod_raportu != kod_raportu) continue;
        R = RAP[i];
        break;
    }
    if (i == LRAP_BL)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Brak raportu %d do %s %s <Ent>    ",
                  kod_raportu, co, txt);
        GET_char();
        if (znak != 0) txt[TXM] = znak;
        return NULL;
    }
    l = (struct lista*)&R->bufs;
    ds = sizeof(struct lista);
    while (l->adr_nast != NULL)
    {
        if ((l->adr_nast + ds) == adr) break;
        l = (struct lista*)(l->adr_nast);
        (*liczba)++;
    }
    /* fprintf(mystderr,"\nszukaj_mem(): adr=%d l->adr=%d",adr,l->adr_nast); */
    return l;
}

void* free_rep_mem(void* adres, int kod_raportu, int rekord, char* txt)
{
    int i, liczba;
    char* buf;
    struct lista *l, *lp;
    lp = szukaj_adr_alloc(kod_raportu, txt, "free", adres, &liczba);
    if (lp == NULL || liczba < 0) return adres;
    l = (struct lista*)lp->adr_nast;
    lp->adr_nast = l->adr_nast;
    total_size -= (l->size + sizeof(struct lista));
    Free(l);
    lwmall--;
    return NULL;
}

void* free_all_rep_mem(struct reports* R)
{
    int i, liczba, ds;
    char *buf, *adr_poprz, znak = 0;
    struct lista *l, *lp;
    lp = (struct lista*)&R->bufs;
    ds = sizeof(struct lista);
    while (lp->adr_nast != NULL)
    {
        l = (struct lista*)(lp->adr_nast);
        lp->adr_nast = l->adr_nast;
        total_size -= (l->size + ds);
        Free(l);
        lwmall--;
    }
    return NULL;
}

void* realloc_rep_mem(void* adres, long int size, int kod_raportu,
                      int rekord, char* txt)
{
    int i, liczba;
    char znak = 0, *bufn, *dalej;
    struct reports* R = NULL;
    struct lista *l, *lp;
    if (strlen(txt) > TXM)
    {
        znak = txt[TXM];
        txt[TXM] = 0;
    }
    lp = szukaj_adr_alloc(kod_raportu, txt, "realloc", adres, &liczba);
    if (lp == NULL || liczba < 0) return adres;
    if (lp->adr_nast == NULL)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Brak wskazanego adresu na liscie: %s(%d) dla rap.%d (tot.siz=%d) <Ent> ",
                  txt, size, kod_raportu, total_size);
        GET_char();
        if (znak != 0) txt[TXM] = znak;
        return adres;
    }
    l = (struct lista*)lp->adr_nast;
    i = l->size;
    dalej = l->adr_nast;
    bufn = realloc(lp->adr_nast, size + sizeof(struct lista));
    if (bufn == NULL)
    {
        lp->adr_nast = dalej;
        total_size -= i;
        lwmall--;
        komunikat(MY_MAX, X_L0,ATTR_A, " Brak pamieci na realok.%s(%d) dla rap.%d (tot.siz=%d) <Ent> ",
                  txt, size, kod_raportu, total_size);
        GET_char();
        if (znak != 0) txt[TXM] = znak;
        return NULL;
    }
    l = (struct lista*)bufn;
    lp->adr_nast = bufn;
    total_size += (size - i);
    l->size = size;
    return (bufn + sizeof(struct lista));
}

void wpisz_adr_danych_raportu(int kod_raportu, char* D)
{
    int i;
    struct reports* R = NULL;
    for (i = 0; i < LRAP_BL; i++)
    {
        if (RAP[i] == NULL) continue;
        if (RAP[i]->kod_raportu != kod_raportu) continue;
        RAP[i]->D = D;
    }
}

void rekord_danych_do_naglowka(int rekord_nr) { BL[nr_blank]->rek_danych_blank = rekord_nr; }

/* ------------------------------------------------------------------- */
int Zapisz_raport(int kod_raportu)
{
    struct reports* R;
    int i, poprz_dec = -1, wymag_uprawn = 0;
    for (i = 0; i < LRAP_BL; i++)
    {
        if (RAP[i] == NULL) continue;
        if (RAP[i]->kod_raportu != kod_raportu) continue;
        R = RAP[i];
        break;
    }
    if (i == LRAP_BL)
    {
        komunikat(MY_MAX, 0,ATTR_A, " Brak raportu %d do zapisu !!! Nacisnij <Ent> ", kod_raportu);
        GET_char();
        return 0;
    }
    return zapis_raportu(R, poprz_dec, wymag_uprawn);
}

/* ------------------------------------------------------------------- */
int zapis_raportu(struct reports* Rp, int poprz_dec, int wymag_uprawn)
{
    signed char nr_raportu, i;
    char zmiany = 0, *kod_operatora, *text = " WSZYSTKIE RAPORTY !!!!!!!   ";
    struct reports* R;
    int lrek, ret = 0;
    if (Uprawn < 1 && wymag_uprawn > 0)
    {
        komunikat(MY_MAX, 0,ATTR_A, " Za niskie uprawnienia (Uprawn=%d) <Ent> ", Uprawn);
        GET_char();
        return 0;
    }
    if (poprz_dec < 0)
    {
        if (BEZ_HASLA == 0 && Rp != NULL)
        {
            kod_operatora = text;
            zmiany = wpisz_haslo(&kod_operatora, wymag_uprawn, NR_OPER, Rp);
            if (zmiany == 0) poprz_dec = UNDO;
            else poprz_dec = ZAP_OBOWIAZK;
        }
        else poprz_dec = ZAP_OBOWIAZK;
    }
    R = Rp;
    if (R == NULL) /* aktualny raport  */
    {
        if (nr_blank < 0)
        {
            if (poprz_dec == UNDO) return -1;
            {
                komunikat(MY_MAX, 0,ATTR_A, " Brak blankietu. Zastosuj fkcje: Zapisz_raport(kod_raportu,%d,%d); <Ent> ",
                          poprz_dec, wymag_uprawn);
                GET_char();
                return 0;
            }
        }
        nr_raportu = BL[nr_blank]->nr_raportu;
        if (nr_raportu < 0)
        {
            komunikat(MY_MAX, X_L0,ATTR_A, " Bledny numer raportu %d w blank.%d. <Ent> ",
                      nr_raportu, nr_blank);
            GET_char();
            if (poprz_dec == UNDO) return -1;
            return 0;
        }
        R = RAP[nr_raportu];
        if (R == NULL)
        {
            komunikat(MY_MAX, X_L0,ATTR_A, " Brak raportu %d w blank.%d. <Ent> ",
                      nr_raportu, nr_blank);
            GET_char();
            if (poprz_dec == UNDO) return -1;
            return 0;
        }
    }
    if (R->zapis == NULL)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Brak rejestracji zmian w raporcie %d, blank.%d. <Ent> ",
                  nr_raportu, nr_blank);
        GET_char();
        if (poprz_dec == UNDO) return -1;
        return 0;
    }
    lrek = R->ob_konc - R->ob_pocz + 1;
    zmiany = 0;
    ret = 0;
    kod_operatora = NULL;
    for (i = 0; i < lrek; i++)
    {
        if (R->zapis[i] >= ZAP_MIN)
        {
            zmiany = 1;
            break;
        }
    }
    if (zmiany == 0 && poprz_dec != ZAP_OBOWIAZK)
    {
        if (poprz_dec == UNDO) return -1;
        else return 0;
        /*     {komunikat(MY_MAX,X_L0,ATTR_A," Brak zmian danych w raporcie <Ent> ");
              GET_char();
              return 0;
             }
        */
    }
    else
    {
        char zap_obowiazk;
        kod_operatora = R->tytul;
        ret = 1;
        /* ======================= Kwestia hasla ============================= */
        if (poprz_dec == 0)
        {
            if (BEZ_HASLA == 0) zmiany = wpisz_haslo(&kod_operatora, R->uprawnienia, NR_OPER, Rp);
            else kod_operatora = NULL;
        }
        zap_obowiazk = ZAP_OBOWIAZK;
        if (poprz_dec == UNDO)
        {
            zmiany = 0;
            zap_obowiazk = 127;
            ret = -1;
        }
        if (zmiany == 0) /* ZAP_OBOWIAZK wynika z powiazan miedzy raportami */
        {
            for (i = 0; i < lrek; i++) /* == ta petla gwarantuje spojnosc danych w raporcie ==*/
            {
                if (R->zapis[i] >= ZAP_MIN && R->d_oryg[i] != NULL)
                {
                    if (R->zapis[i] >= zap_obowiazk)
                    {
                        zmiany = 1;
                        continue;
                    }
                    else
                    {
                        przepisz(R->D + i * R->rozmiar_ob, R->d_oryg[i], R->rozmiar_ob);
                        R->zapis[i] = 0;
                        ret = -1; /* jest UNDO */
                        /*          if(R->d_oryg[i]!=NULL) {Free(R->d_oryg[i]); lwmall--;} */
                    }
                }
            }
        }
        if (zmiany > 0)
        {
            (R->wpis_danych_bl)(R->ob_pocz, R->ob_konc, R->D, R->rozmiar_ob,
                                R->zapis, Kod_operatora_max);
            for (i = 0; i < lrek; i++)
            {
                R->zapis[i] = 0;
                if (R->d_oryg != NULL)
                {
                    if (R->d_oryg[i] != NULL)
                    {
                        Free(R->d_oryg[i]);
                        R->d_oryg[i] = NULL;
                        lwmall--;
                    }
                }
            }
        }
    }
    return ret; /* -1 UNDO; 0 zapis byl zbedny; 1 wpisano dane; 2 blad */
}

/* --------------------------------------------------------------------- */
void reset_blankiet(int nr_rekordu)
{
    int i, l;
    struct okno* ok;
    struct reports* R;
    ok = Ok;
    for (i = 0; i < liczba_danych; i++, ok++)
    {
        if (ok->typ == 'k' && ok->format != NULL)
        {
            Free(ok->format);
            lwmall--;
        }
        ok->format = NULL;
        ok->wpis = 0;
        for (l = 0; l < LHELP; l++) ok->help[l] = NULL;
    }
    R = RAP[BL[nr_blank]->nr_raportu];
    nr_danej = 0;
    BL[nr_blank]->nr_danej = 0;
    if (nr_rekordu < R->ob_pocz) nr_rekordu = R->ob_pocz;
    if (nr_rekordu > R->ob_konc) nr_rekordu = R->ob_konc;
    BL[nr_blank]->nr_rekordu = nr_rekordu;
    BL[nr_blank]->reset = 0;
    liczba_danych = R->max_l_danych_blank;
    X_maxBlank = BL[nr_blank]->X_max;
    X_0 = BL[nr_blank]->X_0;
    Y_maxBlank = BL[nr_blank]->Y_max;
    Y_0 = BL[nr_blank]->Y_0;
    Uprawn = BL[nr_blank]->Uprawn;
    Akt_Kolor = BL[nr_blank]->Kolor;
    Czy_Blankiet_otwarty = 1;
}

/* ========================= Tu zaczyna sie plik wewy3.c =========== */
void dane_helpu(char* hformat, char* help[], char** parinfo);
#ifndef WEWY_INCL
#define _DOS_
#include <stdio.h>
#include <process.h>
#ifdef _DOS_
#include <alloc.h>
#include <dos.h>
#define Malloc farmalloc
#define Free farfree
#include "../blank/intdef.h"
#else
#include <malloc.h>
#include <sys/qnxterm.h>
#include "../blank/intdef.h"
#define Malloc malloc
#define Free free
#endif
#include <conio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "wewybl.h"
#define WEW_EXT extern
#include "WEWY1.H"
#endif
/* ========================================================== */
int okno_menu_rekordow(char* Menu[], int w_max, int opcja0,
                       unsigned int attr, unsigned int attr_o,
                       int Yp, int Xp, int Xm, char* tytul, char ramka,
                       char* hformat, ...)
{
    struct reports Rd, *R;
    char **parinfo, *help[LHELP];
    int size,/* wydruk nr'u rekordu gdy >0 */dim = 0, xk, dl_tyt, tx_size, i;
    parinfo = &hformat + 1;
    R = &Rd;
    R->D = (char*)Menu;
    R->typ_bazy = BAZA_WEKTOR;
    R->tytul = tytul;
    R->kod_raportu = -1;
    R->ob_pocz = opcja0;
    R->ob_konc = w_max;
    help[0] = hformat;
    dane_helpu(hformat, help, parinfo);
    tx_size = -1;
    if (tytul != NULL)
    {
        int dl_tyt;
        dl_tyt = strlen(tytul);
        tx_size = Xp + dl_tyt + 2;
    }
    for (i = 0; i < w_max; i++)
    {
        xk = objasn_grupy(Yp, Xp, 0, w_max, 0, attr, attr, 1, 0, R, help);
        xk = m_wherex();
        if (tx_size < xk) tx_size = xk;
    }
    xk = tx_size + 2;
    if (xk > Xm) Xm = xk;
    return okno_help(Yp, Xp, Xm, attr, attr_o, 'g', 0, w_max,
                     0, w_max, NULL, 0, size = 3, 1, R, NULL, help, ramka,
                     NULL, dim = 0, NULL, NULL, NULL);
}

/* -------------------------------------------------------------------- */
int dana_znak(int y, int x, char* monit, char* opcje, char* objasn[],
              int liczba_obj, char* znak, int ochrona)
{
    int ret;
    ret = tabl_znak(y, x, monit, 0, 0, opcje, NULL, znak, 0, objasn,
                    liczba_obj, ochrona);
    return ret;
}

int tabl_znak(int y, int x, char* monit, int ind_min, int ind_max,
              char* opcje, char* format_tabl, char* tablica, int dim,
              char* objasn[], int liczba_obj, int ochrona)
{
    struct okno* ok;
    if (Ok == NULL)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Brak adresu dla danej %d blankietu. <Ent> ",
                  nr_danej);
        GET_char();
        return -1;
    }
    if (nr_danej >= liczba_danych)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Za duzo danych w blankiecie: nr_danej=%d l_danych=%d. <Ent> ",
                  nr_danej, liczba_danych);
        GET_char();
        return -1;
    }
    ok = Ok + nr_danej;
    nr_danej++;
    ok->format = wpisz_format(ok->format, monit);
    if (ok->format == NULL) return -1;
    ok->typ = 'z';
    ok->ind_min = ind_min;
    ok->ind_max = ind_max;
    if (ind_min < ind_max && ind_min >= 0)
    {
        ok->typ = 'Z';
        ok->index = 0;
        ok->ind_min = ind_min;
        ok->ind_max = ind_max;
        ok->format_tabl = format_tabl;
        ok->dim = dim;
    }
    ok->wartosc = (float*)tablica;
    ok->kod_anim = -1;
    ok->size = 1; /*size;*/
    ok->prec = liczba_obj;
    ok->wmin_opcje = (float*)opcje;
    ok->wmax = (float*)objasn;
    ok->proxy = -1;
    ok->yo = y;
    ok->xo = x;
    ok->obowiazk = 0;
    if (monit[0] == '#' || monit[1] == '#') ok->obowiazk = 1;
    ok->yplus = 0;
    if (monit[0] == '+' || monit[1] == '+') ok->yplus = 1;
    ok->blok_wpisu = 0;
    if (ochrona == -1) ok->blok_wpisu = 1;
    ok->ochrona = ochrona;
    if (ochrona < 0 || BL[nr_blank]->czynny == 0) ok->ochrona = Upr_max;
    return nr_danej;
}


int dana_double_dec(int y, int x, char* monit, double* w_min,
                    double* w_max, double* wart, int size, char precyzja,
                    int ochrona, char kod_animacji, int kod_decyzji)
{
    int ret;
    ret = dana_double(y, x, monit, w_min, w_max, wart, size, precyzja, ochrona,
                      kod_animacji);
    (Ok + nr_danej - 1)->decyzja = kod_decyzji;
    return ret;
}

int dana_double(int y, int x, char* monit, double* w_min, double* w_max,
                double* wart, int size, char precyzja, int ochrona,
                char kod_animacji)
{
    struct okno* ok;
    if (Ok == NULL) return -1;
    if (nr_danej >= liczba_danych)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Za duzo danych w blankiecie: nr_danej=%d l_danych=%d. <Ent> ",
                  nr_danej, liczba_danych);
        GET_char();
        return -1;
    }
    ok = Ok + nr_danej;
    nr_danej++;
    ok->format = wpisz_format(ok->format, monit);
    if (ok->format == NULL) return -1;
    ok->typ = 'e';
    ok->wartosc = (float*)wart;
    ok->kod_anim = kod_animacji;
    ok->size = size;
    ok->decyzja = -1;
    ok->prec = precyzja;
    ok->wmin_opcje = (float*)w_min;
    ok->wmax = (float*)w_max;
    if (kod_animacji > 0) ok->proxy = BL[nr_blank]->anim_pid;
    if (kod_animacji == 0) ok->proxy = 0;
    ok->yo = y;
    ok->xo = x;
    ok->obowiazk = 0;
    if (monit[0] == '#' || monit[1] == '#') ok->obowiazk = 1;
    ok->yplus = 0;
    if (monit[0] == '+' || monit[1] == '+') ok->yplus = 1;
    ok->ochrona = ochrona;
    ok->blok_wpisu = 0;
    if (ochrona == -1) ok->blok_wpisu = 1;
    if (ochrona < 0 || BL[nr_blank]->czynny == 0) ok->ochrona = Upr_max;
    return nr_danej;
}

int tabl_grup_dec(int y, int x, char* monit, int ind_min,
                  int ind_max, int* w_min, int* w_max, char* format_tabl,
                  int* tablica, int dim, int size, int ochrona,
                  int skok, signed char raport, int kod_dec, char* hformat, ...)
{
    char** parinfo;
    parinfo = &hformat + 1;
    (Ok + nr_danej)->decyzja = kod_dec;
    (Ok + nr_danej)->Rep = NULL;
    (Ok + nr_danej)->Rep_help = NULL;
    return v_tabl_grup(y, x, monit, ind_min, ind_max, w_min, w_max,
                       format_tabl, tablica, dim, size, ochrona, skok, raport,
                       hformat, parinfo);
}

int tabl_grup(int y, int x, char* monit, int ind_min,
              int ind_max, int* w_min, int* w_max, char* format_tabl,
              int* tablica, int dim, int size, int ochrona,
              int skok, signed char raport, char* hformat, ...)
{
    char** parinfo;
    parinfo = &hformat + 1;
    (Ok + nr_danej)->Rep = NULL;
    (Ok + nr_danej)->Rep_help = NULL;
    return v_tabl_grup(y, x, monit, ind_min, ind_max, w_min, w_max,
                       format_tabl, tablica, dim, size, ochrona, skok, raport,
                       hformat, parinfo);
}

int tabl_grup_posredn(int y, int x, char* monit, int ind_min,
                      int ind_max, int* w_min, int* w_max, char* format_tabl,
                      int* tablica, int dim, char* index_helpu,
                      int size_helpu, int size, int ochrona,
                      int skok, signed char raport, char* hformat, ...)
{
    char** parinfo;
    parinfo = &hformat + 1;
    (Ok + nr_danej)->index_helpu = (int*)index_helpu;
    (Ok + nr_danej)->size_helpu = size_helpu;
    (Ok + nr_danej)->decyzja = -1;
    (Ok + nr_danej)->Rep = NULL;
    (Ok + nr_danej)->Rep_help = NULL;
    (Ok + nr_danej)->proxy = -1; /* to wskaznik raportu strukt.helpu */
    return v_tabl_grup(y, x, monit, ind_min, ind_max, w_min, w_max,
                       format_tabl, tablica, dim, size, ochrona, skok, raport,
                       hformat, parinfo);
}

int grupa_rap_posredn_danych(int y, int x, char* monit, int* w_min,
                             int* w_max, int* wart, char* index_helpu,
                             int size_helpu, int size, int ochrona,
                             int skok, signed char raport, char* hformat, ...)
{
    char** parinfo;
    parinfo = &hformat + 1;
    (Ok + nr_danej)->index_helpu = (int*)index_helpu;
    (Ok + nr_danej)->size_helpu = size_helpu;
    (Ok + nr_danej)->decyzja = -1;
    (Ok + nr_danej)->Rep = NULL;
    (Ok + nr_danej)->Rep_help = NULL;
    (Ok + nr_danej)->proxy = 0; /* to wskaznik raportu strukt.helpu */
    return v_tabl_grup(y, x, monit, 0, 0, w_min, w_max, NULL, wart, 0, size,
                       ochrona, skok, raport, hformat, parinfo);
}

int grupa_rap_posredn_danych_dec(int y, int x, char* monit, int* w_min,
                                 int* w_max, int* wart, char* index_helpu,
                                 int size_helpu, int size, int ochrona, int skok,
                                 signed char raport, int kod_dec, char* hformat, ...)
{
    char** parinfo;
    parinfo = &hformat + 1;
    (Ok + nr_danej)->index_helpu = (int*)index_helpu;
    (Ok + nr_danej)->size_helpu = size_helpu;
    (Ok + nr_danej)->decyzja = kod_dec;
    (Ok + nr_danej)->Rep = NULL;
    (Ok + nr_danej)->Rep_help = NULL;
    (Ok + nr_danej)->proxy = 0; /* to wskaznik raportu strukt.helpu */
    return v_tabl_grup(y, x, monit, 0, 0, w_min, w_max, NULL, wart, 0, size,
                       ochrona, skok, raport, hformat, parinfo);
}


int dana_rekord_str(int y, int x0, char* monit, int* w_min,
                    int* w_max, int* wart, int size, int ochrona,
                    int kod_raportu, signed char raport, char* hformat, ...)
{
    char** parinfo;
    struct reports* R;
    int i;
    for (i = 0; i < LRAP_BL; i++)
    {
        R = RAP[i];
        if (R == NULL) return nr_danej;
        if (R->kod_raportu == kod_raportu) break;
    }
    if (i == LRAP_BL) return nr_danej;
    (Ok + nr_danej)->Rep = R;
    if (R->typ_bazy == BAZA_SPOJNA) (Ok + nr_danej)->Rep = NULL;
    (Ok + nr_danej)->Rep_help = NULL;
    parinfo = &hformat + 1;
    (Ok + nr_danej)->index_helpu = wart;
    (Ok + nr_danej)->size_helpu = 0;
    /*  (Ok+nr_danej)->decyzja=-1; */
    return v_tabl_grup(y, x0, monit, 0, 0, w_min, w_max, NULL, wart, 0, size,
                       ochrona, R->rozmiar_ob, raport, hformat, parinfo);
}

int grupa_danych(int y, int x, char* monit, int* w_min,
                 int* w_max, int* wart, int size, int ochrona,
                 int skok, signed char raport, char* hformat, ...)
{
    char** parinfo;
    parinfo = &hformat + 1;
    (Ok + nr_danej)->index_helpu = wart;
    (Ok + nr_danej)->size_helpu = 0;
    (Ok + nr_danej)->Rep = NULL;
    (Ok + nr_danej)->Rep_help = NULL;
    /*  (Ok+nr_danej)->decyzja=-1; */
    return v_tabl_grup(y, x, monit, 0, 0, w_min, w_max, NULL, wart, 0, size,
                       ochrona, skok, raport, hformat, parinfo);
}

int dana_rekord_posredn_str(int y, int x, char* monit, int* w_min,
                            int* w_max, int* wart, char* index_helpu,
                            int kod_raportu_helpu, int size, int ochrona,
                            int kod_raportu, signed char raport, char* hformat, ...)
{
    char** parinfo;
    struct reports* R;
    int i, skok;
    for (i = 0; i < LRAP_BL; i++)
    {
        R = RAP[i];
        if (R == NULL) return nr_danej;
        if (R->kod_raportu == kod_raportu) break;
    }
    if (i == LRAP_BL) return nr_danej;
    (Ok + nr_danej)->Rep = R;
    skok = R->rozmiar_ob;
    if (R->typ_bazy == BAZA_SPOJNA) (Ok + nr_danej)->Rep = NULL;
    for (i = 0; i < LRAP_BL; i++)
    {
        R = RAP[i];
        if (R == NULL) return nr_danej;
        if (R->kod_raportu == kod_raportu_helpu) break;
    }
    if (i == LRAP_BL) return nr_danej;
    (Ok + nr_danej)->Rep_help = R;
    if (R->typ_bazy == BAZA_SPOJNA) (Ok + nr_danej)->Rep_help = NULL;
    parinfo = &hformat + 1;
    (Ok + nr_danej)->index_helpu = (int*)index_helpu;
    (Ok + nr_danej)->size_helpu = R->rozmiar_ob;
    (Ok + nr_danej)->decyzja = -1;
    return v_tabl_grup(y, x, monit, 0, 0, w_min, w_max, NULL, wart, 0, size,
                       ochrona, skok, raport, hformat, parinfo);
}

int grupa_posredn_danych(int y, int x, char* monit, int* w_min,
                         int* w_max, int* wart, char* index_helpu,
                         int size_helpu, int size, int ochrona,
                         int skok, signed char raport, char* hformat, ...)
{
    char** parinfo;
    parinfo = &hformat + 1;
    (Ok + nr_danej)->index_helpu = (int*)index_helpu;
    (Ok + nr_danej)->size_helpu = size_helpu;
    (Ok + nr_danej)->decyzja = -1;
    (Ok + nr_danej)->Rep = NULL;
    (Ok + nr_danej)->Rep_help = NULL;
    return v_tabl_grup(y, x, monit, 0, 0, w_min, w_max, NULL, wart, 0, size,
                       ochrona, skok, raport, hformat, parinfo);
}

int dana_rekord_str_dec(int y, int x, char* monit, int* w_min, int* w_max,
                        int* wart, int size, int ochrona, int kod_raportu,
                        signed char raport, char kod_dec, char* hformat, ...)
{
    char** parinfo;
    struct reports* R;
    int i;
    for (i = 0; i < LRAP_BL; i++)
    {
        R = RAP[i];
        if (R == NULL) return nr_danej;
        if (R->kod_raportu == kod_raportu) break;
    }
    if (i == LRAP_BL) return nr_danej;
    if (R->typ_bazy != BAZA_SPOJNA) (Ok + nr_danej)->Rep = R;
    else (Ok + nr_danej)->Rep = NULL;
    (Ok + nr_danej)->Rep_help = NULL;
    parinfo = &hformat + 1;
    (Ok + nr_danej)->index_helpu = wart;
    (Ok + nr_danej)->size_helpu = 0;
    (Ok + nr_danej)->decyzja = kod_dec;
    return v_tabl_grup(y, x, monit, 0, 0, w_min, w_max, NULL, wart, 0, size,
                       ochrona, R->rozmiar_ob, raport, hformat, parinfo);
}


int grupa_danych_dec(int y, int x, char* monit, int* w_min, int* w_max,
                     int* wart, int size, int ochrona, int skok,
                     signed char raport, char kod_dec, char* hformat, ...)
{
    char** parinfo;
    parinfo = &hformat + 1;
    (Ok + nr_danej)->index_helpu = wart;
    (Ok + nr_danej)->size_helpu = 0;
    (Ok + nr_danej)->decyzja = kod_dec;
    (Ok + nr_danej)->Rep = NULL;
    (Ok + nr_danej)->Rep_help = NULL;
    return v_tabl_grup(y, x, monit, 0, 0, w_min, w_max, NULL, wart, 0, size,
                       ochrona, skok, raport, hformat, parinfo);
}

int dana_rekord_posredn_str_dec(int y, int x, char* monit, int* w_min,
                                int* w_max, int* wart, char* index_helpu,
                                int kod_raportu_helpu, int size, int ochrona,
                                int kod_raportu, signed char raport, char kod_dec,
                                char* hformat, ...)
{
    char** parinfo;
    struct reports* R;
    int i, skok;
    for (i = 0; i < LRAP_BL; i++)
    {
        R = RAP[i];
        if (R == NULL) return nr_danej;
        if (R->kod_raportu == kod_raportu) break;
    }
    if (i == LRAP_BL) return nr_danej;
    (Ok + nr_danej)->Rep = R;
    skok = R->rozmiar_ob;
    if (R->typ_bazy == BAZA_SPOJNA) (Ok + nr_danej)->Rep = NULL;
    for (i = 0; i < LRAP_BL; i++)
    {
        R = RAP[i];
        if (R == NULL) return nr_danej;
        if (R->kod_raportu == kod_raportu_helpu) break;
    }
    if (i == LRAP_BL) return nr_danej;
    (Ok + nr_danej)->Rep_help = R;
    if (R->typ_bazy == BAZA_SPOJNA) (Ok + nr_danej)->Rep_help = NULL;
    parinfo = &hformat + 1;
    (Ok + nr_danej)->index_helpu = (int*)index_helpu;
    (Ok + nr_danej)->size_helpu = R->rozmiar_ob;
    (Ok + nr_danej)->decyzja = kod_dec;
    return v_tabl_grup(y, x, monit, 0, 0, w_min, w_max, NULL, wart, 0, size,
                       ochrona, skok, raport, hformat, parinfo);
}

int grupa_posredn_danych_dec(int y, int x, char* monit, int* w_min,
                             int* w_max, int* wart, char* index_helpu,
                             int size_helpu, int size, int ochrona,
                             int skok, signed char raport, int kod_dec,
                             char* hformat, ...)
{
    char** parinfo;
    parinfo = &hformat + 1;
    (Ok + nr_danej)->index_helpu = (int*)index_helpu;
    (Ok + nr_danej)->size_helpu = size_helpu;
    (Ok + nr_danej)->decyzja = kod_dec;
    (Ok + nr_danej)->Rep = NULL;
    (Ok + nr_danej)->Rep_help = NULL;
    return v_tabl_grup(y, x, monit, 0, 0, w_min, w_max, NULL, wart, 0, size,
                       ochrona, skok, raport, hformat, parinfo);
}

/* ----------------------------------------------------------------- */
int v_tabl_grup(int y, int x, char* monit, int ind_min,
                int ind_max, int* w_min, int* w_max, char* format_tabl,
                int* tablica, int dim, int size, int ochrona,
                int skok, signed char raport, char* hformat, char** parinfo)
{
    struct okno* ok;
    int k, lzm;
    char *f = EOS, *form, typ, *zn;
    /*  va_list parinfo; */
    if (Ok == NULL)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Brak adresu dla danej %d blankietu. <Ent> ",
                  nr_danej);
        GET_char();
        return -1;
    }
    if (nr_danej >= liczba_danych)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Za duzo danych w blankiecie: nr_danej=%d l_danych=%d. <Ent> ",
                  nr_danej, liczba_danych);
        GET_char();
        return -1;
    }
    ok = Ok + nr_danej;
    nr_danej++;
    ok->format = wpisz_format(ok->format, monit);
    if (ok->format == NULL) return -1;
    ok->help[0] = wpisz_format(ok->help[0], hformat);
    if (ok->help[0] == NULL) return -1;
    dane_helpu(hformat, ok->help, parinfo);
    /*va_end(parinfo); */
    ok->typ = 'g';
    if (ind_min < ind_max && ind_min >= 0)
    {
        ok->typ = 'G';
        ok->index = 0;
        ok->ind_min = ind_min;
        ok->ind_max = ind_max;
        ok->format_tabl = format_tabl;
        ok->dim = dim;
    }
    ok->wartosc = (float*)tablica;
    ok->kod_anim = raport;
    ok->size = size;
    ok->prec = skok;
    ok->wmin_opcje = (float*)w_min;
    ok->wmax = (float*)w_max;
    if (raport < 0 || ok->size_helpu <= 0) ok->proxy = -1;
    ok->yo = y;
    ok->xo = x;
    ok->obowiazk = 0;
    if (monit[0] == '#' || monit[1] == '#') ok->obowiazk = 1;
    ok->yplus = 0;
    if (monit[0] == '+' || monit[1] == '+') ok->yplus = 1;
    ok->ochrona = ochrona;
    if (ochrona < 0 || BL[nr_blank]->czynny == 0) ok->ochrona = Upr_max;
    ok->blok_wpisu = 0;
    if (ochrona == -1) ok->blok_wpisu = 1;
    return nr_danej;
}

/* ----------------------------------------------------------------- */
void dane_helpu(char* hformat, char* help[], char** parinfo)
{
    char *f = NULL, *form = NULL, typ, *zn;
    int lzm, k, lz, dlz = 0;
    lzm = 0;
    form = hformat;
    /*    va_start(parinfo,hformat); * /=====  parinfo=&hformat+1; */
    do
    {
        do
        {
            f = szukaj(form, '%');
            if (f == NULL) return;
            form = f + 1;
            typ = czy_format(f, &k);
        }
        while (typ == EOS);
        lzm++;
        if (lzm < LHELP)
        {
            help[lzm] = (char*)(*parinfo);
            parinfo++; /*(char *)va_arg(parinfo,char *);*/
        }
        else break;
        form = &f[k];
        k = strlen(form);
        if (k == 0) return;
        if (k < 4) continue;
        dlz = 0;
        if (form[0] == '[' && form[1] == '%' && form[2] == 'a')
        {
            for (zn = &form[3], k = 4;; zn++, k++)
            {
                if ((*zn) == EOS) break;
                if ((*zn) == ']') break;
                if ((*zn) < '0' || (*zn) > '9')
                {
                    if (k > 4) *zn = EOS;
                    break;
                }
            }
            if (*zn == EOS)
            {
                komunikat(MY_MAX, X_L0,ATTR_A, " Blad formatu %s w grupie (zn=%c, k=%d). <Ent>", form, *zn, k);
                GET_char();
                break;
            }
            if (k == 4)
            {
                /*fprintf(mystderr,"form=%s",zn); */
                if (*zn == '%' && *(zn + 1) == 'l' && *(zn + 2) == ']')
                {
                    lz = lzm + 2;
                    dlz = 1;
                    /* fprintf(mystderr,"\n dane_form=%s",zn); */
                    if (lz >= LHELP || (*(parinfo + 1)) == NULL)
                    {
                        komunikat(MY_MAX, X_L0,ATTR_A, " Brak liczby danych dla spec. %s w grupie (zn=%c k=%d). <Ent>",
                                  form, *zn, k);
                        GET_char();
                        break;
                    }
                    help[lz] = (char*)(*(parinfo + 1));
                    zn += 2;
                    k = 6;
                }
                else
                {
                    komunikat(MY_MAX, X_L0,ATTR_A, " Brak liczby danych dla spec. %s w grupie - zn=%c k=%d. <Ent>",
                              form, *zn, k);
                    /*fprintf(mystderr,"\n zle dane_form=%s",zn); */
                    GET_char();
                    break;
                }
            }
            /* ------------------------- kawalek wymieniony 23.08.1998 -------------
                  if(form[0]=='['&&form[1]=='%'&&form[2]=='a')
                   {for(zn=&form[3],k=4;;zn++,k++)
                        {if((*zn)==EOS) break;
                      if((*zn)==']') break;
                      if((*zn)<'0' || (*zn)>'9') {*zn=EOS; break;}
                     }
                     if(*zn==EOS)
                         {komunikat(MY_MAX, X_L0,ATTR_A," Brak klamry ] w formacie %s grupy (k=%d). <Ent>", form,k);
                      GET_char();
                      break;
                     }
                    if(k==4)
                         {komunikat(MY_MAX, X_L0,ATTR_A," Brak liczby danych dla spec. %s w grupie. <Ent>", form);
                      GET_char();
                      break;
                     }
            ----------------------------------- */
            lzm++;
            if (lzm < LHELP)
            {
                help[lzm] = (char*)(*parinfo);
                parinfo++; /*(char *)va_arg(parinfo,char *); */
                lzm += dlz;
                parinfo += dlz;
                dlz = 0;
            }
            else break;
            form += k;
        }
        else
        {
            if (form[0] == '[' && form[1] == '%' && (form[2] == 'z' || form[2] == 'Z'))
            {
                for (zn = &form[3], k = 4;; zn++, k++)
                {
                    if ((*zn) == EOS) break;
                    if ((*zn) == ']') break;
                    if ((*zn) < '0' || (*zn) > '9')
                    {
                        *zn = EOS;
                        break;
                    }
                }
                if (*zn == EOS)
                {
                    komunikat(MY_MAX, X_L0,ATTR_A,
                              " Brak klamry ] lub liczby stringow w formacie %s grupy (k=%d). <Ent>", form, k);
                    GET_char();
                    break;
                }
                if (typ != 's' && typ != 'S')
                {
                    komunikat(MY_MAX, X_L0,ATTR_A, " Blad (s) formatu %s w grupie. <Ent>", form);
                    GET_char();
                    break;
                }
                lzm++;
                if (lzm == LHELP)
                {
                    komunikat(MY_MAX, X_L0,ATTR_A, " Za duzo danych w grupie. <Ent>");
                    GET_char();
                    break;
                }
                help[lzm] = (char*)(*parinfo);
                parinfo++;
                /*          help[lzm]=(char *)va_arg(parinfo,char *); */
                form += k; /* przesuwamy poczatek nastepnego formatu */
            }
        } /* koniec else po analizie klamry [%a] */
    }
    while (f != NULL);
    return;
}


int tabl_int_menu(int y, int x, char* monit, int ind_min,
                  int ind_max, int* w_min, int* w_max, char* format_tabl,
                  int* tablica, int dim, int size, int ochrona,
                  signed char raport, int l_poz_menu, char* menu[])
{
    int ret;
    struct okno* ok;
    ret = tabl_int(y, x, monit, ind_min, ind_max, w_min, w_max,
                   format_tabl, tablica, dim, size, ochrona, raport);
    if (ret > 0 && l_poz_menu > 0 && menu != NULL)
    {
        ok = Ok + ret - 1;
        ok->help[0] = (char*)menu;
        ok->prec = l_poz_menu;
    }
    return ret;
}

int tabl_int_menu_dec(int y, int x, char* monit, int ind_min,
                      int ind_max, int* w_min, int* w_max, char* format_tabl,
                      int* tablica, int dim, int size, int ochrona,
                      signed char raport, int l_poz_menu, char* menu[], int kod_dec)
{
    int ret;
    struct okno* ok;
    ret = tabl_int(y, x, monit, ind_min, ind_max, w_min, w_max,
                   format_tabl, tablica, dim, size, ochrona, raport);
    if (ret > 0 && l_poz_menu > 0 && menu != NULL)
    {
        ok = Ok + ret - 1;
        ok->help[0] = (char*)menu;
        ok->prec = l_poz_menu;
        ok->decyzja = kod_dec;
    }
    return ret;
}

int tabl_int(int y, int x, char* monit, int ind_min,
             int ind_max, int* w_min, int* w_max, char* format_tabl,
             int* tablica, int dim, int size,
             int ochrona, signed char raport)
{
    struct okno* ok;
    if (Ok == NULL)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Brak adresu dla danej %d blankietu. <Ent> ",
                  nr_danej);
        GET_char();
        return -1;
    }
    if (nr_danej >= liczba_danych)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Za duzo danych w blankiecie: nr_danej=%d l_danych=%d. <Ent> ",
                  nr_danej, liczba_danych);
        GET_char();
        return -1;
    }
    ok = Ok + nr_danej;
    nr_danej++;
    ok->format = wpisz_format(ok->format, monit);
    if (ok->format == NULL) return -1;
    ok->typ = 'i';
    if (ind_min < ind_max && ind_min >= 0)
    {
        ok->typ = 'I';
        ok->index = 0;
        ok->ind_min = ind_min;
        ok->ind_max = ind_max;
        ok->format_tabl = format_tabl;
        ok->dim = dim;
    }
    ok->wartosc = (float*)tablica;
    ok->kod_anim = raport;
    ok->size = size;
    ok->prec = 0;
    ok->wmin_opcje = (float*)w_min;
    ok->wmax = (float*)w_max;
    ok->proxy = -1;
    /*    if(kod_animacji>0) ok->proxy=BL[nr_blank]->anim_pid; */
    ok->yo = y;
    ok->xo = x;
    ok->obowiazk = 0;
    if (monit[0] == '#' || monit[1] == '#') ok->obowiazk = 1;
    ok->yplus = 0;
    if (monit[0] == '+' || monit[1] == '+') ok->yplus = 1;
    ok->ochrona = ochrona;
    ok->blok_wpisu = 0;
    if (ochrona == -1) ok->blok_wpisu = 1;
    if (ochrona < 0 || BL[nr_blank]->czynny == 0) ok->ochrona = Upr_max;
    return nr_danej;
}

int dana_int_menu(int y, int x, char* monit, int* w_min, int* w_max,
                  int* wart, int size, int ochrona, signed char raport,
                  int l_poz_menu, char* menu[])
{
    int ret;
    struct okno* ok;
    ret = dana_int(y, x, monit, w_min, w_max, wart, size, ochrona, raport);
    if (ret > 0 && l_poz_menu > 0 && menu != NULL)
    {
        ok = Ok + ret - 1;
        ok->help[0] = (char*)menu;
        ok->prec = l_poz_menu;
    }
    return ret;
}

int dana_int_menu_dec(int y, int x, char* monit, int* w_min, int* w_max,
                      int* wart, int size, int ochrona, signed char raport,
                      int l_poz_menu, char* menu[], int kod_decyzji)
{
    int ret;
    struct okno* ok;
    ret = dana_int(y, x, monit, w_min, w_max, wart, size, ochrona, raport);
    if (ret > 0 && l_poz_menu > 0 && menu != NULL)
    {
        ok = Ok + ret - 1;
        ok->help[0] = (char*)menu;
        ok->prec = l_poz_menu;
    }
    (Ok + nr_danej - 1)->decyzja = kod_decyzji;
    return ret;
}

int dana_int_dec(int y, int x, char* monit, int* w_min, int* w_max,
                 int* wart, int size, int ochrona,
                 signed char raport, int kod_decyzji)
{
    int ret;
    ret = dana_int(y, x, monit, w_min, w_max, wart, size, ochrona, raport);
    (Ok + nr_danej - 1)->decyzja = kod_decyzji;
    return ret;
}

int dana_int(int y, int x, char* monit, int* w_min, int* w_max,
             int* wart, int size, int ochrona, signed char raport)
{
    struct okno* ok;
    if (Ok == NULL)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Brak adresu dla danej %d blankietu. <Ent> ",
                  nr_danej);
        GET_char();
        return -1;
    }
    if (nr_danej >= liczba_danych)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Za duzo danych w blankiecie: nr_danej=%d l_danych=%d. <Ent> ",
                  nr_danej, liczba_danych);
        GET_char();
        return -1;
    }
    ok = Ok + nr_danej;
    nr_danej++;
    ok->decyzja = -1;
    ok->format = wpisz_format(ok->format, monit);
    if (ok->format == NULL) return -1;
    ok->typ = 'i';
    ok->wartosc = (float*)wart;
    ok->kod_anim = raport;
    ok->size = size;
    ok->prec = 0;
    ok->wmin_opcje = (float*)w_min;
    ok->wmax = (float*)w_max;
    ok->proxy = -1;
    /*    if(kod_animacji>0) ok->proxy=BL[nr_blank]->anim_pid; */
    ok->yo = y;
    ok->xo = x;
    ok->obowiazk = 0;
    if (monit[0] == '#' || monit[1] == '#') ok->obowiazk = 1;
    ok->yplus = 0;
    if (monit[0] == '+' || monit[1] == '+') ok->yplus = 1;
    ok->blok_wpisu = 0;
    if (ochrona == -1) ok->blok_wpisu = 1;
    ok->ochrona = ochrona;
    if (ochrona < 0 || BL[nr_blank]->czynny == 0) ok->ochrona = Upr_max;
    return nr_danej;
}


int dana_decyzyjna(int y, int x, char* monit, char* opcje, char* objasn[],
                   int liczba_obj, char* znak, int ochrona, int kod_decyzji)
{
    int ret;
    ret = dana_znak(y, x, monit, opcje, objasn, liczba_obj, znak, ochrona);
    (Ok + nr_danej - 1)->decyzja = kod_decyzji;
    return ret;
}

/*
int dana_znak(int y, int x, char *monit, char *opcje, char *objasn[],
                int liczba_obj, char *znak, int ochrona)
 {struct okno *ok;
  int i,l;
   if(Ok==NULL)
		{komunikat(MY_MAX,X_L0,ATTR_A," Brak adresu dla danej %d blankietu. <Ent> ",
           nr_danej);
     GET_char(); return -1;
    }
   if(nr_danej>=liczba_danych)
		{komunikat(MY_MAX,X_L0,ATTR_A," Za duzo danych w blankiecie: nr_danej=%d l_danych=%d. <Ent> ",
           nr_danej,liczba_danych);
     GET_char(); return -1;
    }
   ok=Ok+nr_danej; nr_danej++;
   ok->format=wpisz_format(ok->format, monit); if(ok->format==NULL) return -1;
    ok->typ='z';
		ok->wartosc=(float *)znak;
        ok->kod_anim=-1;
    ok->size=1;
    ok->prec=liczba_obj;
    ok->wmin_opcje=(float *)opcje;
    ok->wmax=(float *)objasn;
    ok->proxy=-1;
				ok->kod_anim=-1;
    ok->yo=y;
    ok->xo=x;
    ok->obowiazk=0;
    if(monit[0]=='#' || monit[1]=='#') ok->obowiazk=1;
    ok->yplus=0;
    if(monit[0]=='+' || monit[1]=='+') ok->yplus=1;
    ok->blok_wpisu=0; if(ochrona==-1) ok->blok_wpisu=1;
    ok->ochrona=ochrona;
		if(ochrona<0 || BL[nr_blank]->czynny==0) ok->ochrona=Upr_max;
   return nr_danej;
 }
*/

int dana_text_dec(int y, int x, char* monit, char* string, int size,
                  char** help, int l_opcji, int ochrona, int kod_decyzji)
{
    int ret;
    ret = dana_text(y, x, monit, string, size, help, l_opcji, ochrona);
    (Ok + nr_danej - 1)->decyzja = kod_decyzji;
    return ret;
}

int dana_text(int y, int x, char* monit, char* string, int size,
              char** help, int l_opcji, int ochrona)
{
    struct okno* ok;
    if (Ok == NULL)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Brak adresu dla danej %d blankietu. <Ent> ",
                  nr_danej);
        GET_char();
        return -1;
    }
    if (nr_danej >= liczba_danych)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Za duzo danych w blankiecie: nr_danej=%d l_danych=%d. <Ent> ",
                  nr_danej, liczba_danych);
        GET_char();
        return -1;
    }
    ok = Ok + nr_danej;
    nr_danej++;
    ok->format = wpisz_format(ok->format, monit);
    if (ok->format == NULL) return -1;
    ok->typ = 's';
    ok->wartosc = (float*)string;
    ok->kod_anim = -1;
    ok->size = size;
    ok->prec = 0;
    ok->decyzja = -1;
    ok->wmin_opcje = NULL;
    ok->help[0] = (char*)help;
    if (help != NULL) ok->prec = l_opcji;
    ok->wmax = NULL;
    ok->proxy = -1;
    ok->yo = y;
    ok->xo = x;
    ok->ind_min = 0;
    ok->obowiazk = 0;
    if (monit[0] == '#' || monit[1] == '#') ok->obowiazk = 1;
    ok->yplus = 0;
    if (monit[0] == '+' || monit[1] == '+') ok->yplus = 1;
    ok->blok_wpisu = 0;
    if (ochrona == -1) ok->blok_wpisu = 1;
    ok->ochrona = ochrona;
    if (ochrona < 0 || BL[nr_blank]->czynny == 0) ok->ochrona = Upr_max;
    return nr_danej;
}

int wpis_danej_text_menu(int y, int x, char* monit, char* string, int size,
                         int l_opcji, int ochrona,
                         long int skok, void* R, char* hformat, char** parinfo);

int dana_text_menu_rek(int y, int x, char* monit, char* string, int size,
                       int l_opcji, int ochrona,
                       int kod_raportu, char* hformat, ...)
{
    char** parinfo;
    struct reports* R = NULL;
    int i, skok;
    parinfo = &hformat + 1;
    for (i = 0; i < LRAP_BL; i++)
    {
        R = RAP[i];
        if (R == NULL) return nr_danej;
        if (R->kod_raportu == kod_raportu) break;
    }
    if (i == LRAP_BL) return nr_danej;
    skok = R->rozmiar_ob;
    if (R->typ_bazy == BAZA_SPOJNA) R = NULL;
    return wpis_danej_text_menu(y, x, monit, string, size, l_opcji, ochrona, skok, R, hformat, parinfo);
}

int dana_text_menu(int y, int x, char* monit, char* string, int size,
                   int l_opcji, int ochrona,
                   long int skok, char* hformat, ...)
{
    char** parinfo;
    parinfo = &hformat + 1;
    return wpis_danej_text_menu(y, x, monit, string, size, l_opcji, ochrona, skok, NULL, hformat, parinfo);
}

int wpis_danej_text_menu(int y, int x, char* monit, char* string, int size,
                         int l_opcji, int ochrona,
                         long int skok, void* R, char* hformat, char** parinfo)
{
    struct okno* ok;
    /*   char **parinfo;
       parinfo=&hformat+1; */
    if (Ok == NULL)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Brak adresu dla danej %d blankietu. <Ent> ",
                  nr_danej);
        GET_char();
        return -1;
    }
    if (nr_danej >= liczba_danych)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Za duzo danych w blankiecie: nr_danej=%d l_danych=%d. <Ent> ",
                  nr_danej, liczba_danych);
        GET_char();
        return -1;
    }
    ok = Ok + nr_danej;
    nr_danej++;
    ok->Rep_help = R;
    ok->format = wpisz_format(ok->format, monit);
    if (ok->format == NULL) return -1;
    ok->help[0] = wpisz_format(ok->help[0], hformat);
    if (ok->help[0] == NULL) return -1;
    dane_helpu(hformat, ok->help, parinfo);
    ok->typ = 's';
    ok->wartosc = (float*)string;
    ok->kod_anim = -1;
    ok->size = size;
    ok->prec = 0;
    ok->ind_min = skok;
    ok->decyzja = -1;
    ok->wmin_opcje = NULL;
    ok->prec = l_opcji;
    ok->wmax = NULL;
    ok->proxy = -1;
    ok->yo = y;
    ok->xo = x;
    ok->obowiazk = 0;
    if (monit[0] == '#' || monit[1] == '#') ok->obowiazk = 1;
    ok->yplus = 0;
    if (monit[0] == '+' || monit[1] == '+') ok->yplus = 1;
    ok->blok_wpisu = 0;
    if (ochrona == -1) ok->blok_wpisu = 1;
    ok->ochrona = ochrona;
    if (ochrona < 0 || BL[nr_blank]->czynny == 0) ok->ochrona = Upr_max;
    return nr_danej;
}

int dana_koment(int yo, int xo, char* format, ...)
{
    int l, i = 0;
    char bufor[120], *text = NULL;
    va_list arg;
    struct okno* ok;
    va_start(arg, format);
    if (format[0] == '+') i = 1;
    vsprintf(bufor, &format[i], arg);
    va_end(arg);
    l = strlen(bufor) + 1;
    text = (char*)Malloc(l * sizeof(char));
    if (text == NULL)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Brak pamieci na komentarz: %s. <Ent> ",
                  bufor);
        GET_char();
        return -1;
    }
    lwmall++;
    przepisz(text, bufor, l);
    if (Ok == NULL)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Brak adresu dla danej %d blankietu. <Ent> ",
                  nr_danej);
        GET_char();
        return -1;
    }
    if (nr_danej >= liczba_danych)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Za duzo danych w blankiecie: nr_danej=%d l_danych=%d. <Ent> ",
                  nr_danej, liczba_danych);
        GET_char();
        return -1;
    }
    ok = Ok + nr_danej;
    nr_danej++;
    ok->format = text;
    ok->help[0] = NULL;
    ok->typ = 'k';
    ok->kod_anim = -1;
    ok->size = 0;
    ok->prec = 0;
    ok->yo = yo;
    ok->xo = xo;
    ok->obowiazk = 0;
    ok->yplus = 0;
    if (format[0] == '+') ok->yplus = 1;
    ok->ochrona = Upr_max;
    ok->blok_wpisu = 0;
    return nr_danej;
}

/* -------------------------------------------------------------------- */
int blankiet(signed char* kod_raportu, int* nr_ob, int xp,
             int yp, int ym, int xm, char czynny, signed char kolor,
             signed char ramka, char* poziom, char* zapis, int* y_danej)
{
    int ret, Ret, rozmiar_ob, ob_pocz, ob_konc;
    signed char Kolor, Ramka;
    int anim_pid;
    char *tytul, *D;
    /* ----------------------------------------------------------------------- */
    Kolor = kolor;
    Ramka = ramka;
    for (;;)
    {
        D = czy_jest_blankiet(*kod_raportu, &anim_pid, nr_ob, &ob_pocz, &ob_konc,
                              &rozmiar_ob, &tytul, &Kolor, &Ramka, czynny, yp, xp, ym, xm);
        if (D == NULL) return -1;
        Ret = rap_blank(tytul, kod_raportu, nr_ob);
        /* ============= Interpretacja wyjscia Ret: =========================
                -1 - wyjscie z bledem
             TAB - zostalo cos do zapisania
             Esc - nie ma nic do zapisania
             FONT1 - nowy raport
             FONT6 - decyzja

        *  ================================================================== */
        *zapis = 1;
        if (Ret != FONT6)
        {
            if (Ret == Esc || Ret == -CR || Ret == UNDO || Ret == TAB) *zapis = 0; /* juz zapisano */
            if (Ret == UNDO) return UNDO;
            if (nr_blank < 0) break;
            if ((*kod_raportu) < 0)
            {
                Ret = -CR;
                break;
            }
            if (Ret == -1)
            {
                /* Wyjscie z bledami */
                /* przepisz(d+(*nr_ob)*rozmiar_ob, Dr, rozmiar_ob); */
                *kod_raportu = zamknij_blankiet(nr_ob);
                break;
            }
            {
                char format_inny_rek[32];
                if (ob_pocz == ob_konc)
                    sprintf(format_inny_rek, "� Chcesz WYJSC z RAPORTU ??? ");
                else
                    sprintf(format_inny_rek, "� <%c,%c,%c,%c,Home,End>-inny rek", 24, 25, 26, arrow);

#ifndef _DOS_
                if (*zapis == 1)
                {
                    term_printf(MY_MAX, 0, TERM_CYAN | INVERSE | TERM_FLUSH,
                                "�<Tab>-powrot do edycji � <%c,%c,%c,%c,Home,End>-inny rek�<F2>-ZAPIS �<Esc>-wyjscie �",
                                24, 25, 26, arrow);
                    term_printf(MY_MAX, 24, ATTR_A, "%s%s", format_inny_rek, "�<F2>-ZAPIS�");
                }
                else
                {
                    term_printf(MY_MAX, 0, TERM_CYAN | INVERSE | TERM_FLUSH,
                                "� <Tab> - powrot do edycji raportu � <%c,%c,%c,%c,Home,End>-inny rek�<Esc>-wyjscie �",
                                24, 25, 26, arrow);
                    term_printf(MY_MAX, X_L0 + 35, ATTR_A, "%s%s", format_inny_rek, "�");
                }
      }
                if (Ret == -CR)
                    komun_text(MY_MAX, 0, "�PREZENTACJA DANYCH bez prawa zmian", 0, TERM_CYAN | INVERSE | TERM_FLUSH);
#else
                if (*zapis == 1)
                {
                    term_printf(MY_MAX, 0,TERM_CYAN | INVERSE | TERM_FLUSH,
                                "�<Tab>-powrot do edycji � <%c,%c,%c,%c,Home,End>-inny rek�<F2>-ZAPIS�<Esc>-wyjscie�",
                                24, 25, 26, arrow);
                    term_printf(MY_MAX, X_L0 + 24,ATTR_A, "%s%s", format_inny_rek, "�<F2>-ZAPIS�");
                }
                else
                {
                    term_printf(MY_MAX, 0,TERM_CYAN | INVERSE | TERM_FLUSH,
                                "� <Tab> - powrot do edycji raportu � <%c,%c,%c,%c,Home,End>-inny rek�<Esc>-wyjscie�",
                                24, 25, 26, arrow);
                    term_printf(MY_MAX, X_L0 + 35,ATTR_A, "%s%s", format_inny_rek, "�");
                }
            }
            if (Ret == -CR)
                komun_text(MY_MAX, 0, "�PREZENTACJA DANYCH bez prawa zmian", 0,TERM_CYAN | INVERSE | TERM_FLUSH);
#endif
        }
        /* ================== WYBOR OPCJI JW =========================== */
        ret = Esc_Tab_wybor_toru(Ret, kod_raportu, nr_ob, ob_pocz, ob_konc,
                                 zapis, poziom, y_danej);
        /* zapytac, czy rezygnuje z zapisu */
        /*      if(ret==1) przepisz(d_ob, Dr, rozmiar_ob);*/
        switch (ret)
        {
        case TAB: continue; /*goto BL;     / * Ponowne wejscie do raportu   */
        case Esc:
            /*        if((*poziom)==0) {Ret=Esc; break;} */
            continue; /* ODTWORZENIE NIZSZEGO RAPORTU */
        case 1: return TAB; /* INNY REKORD raportu    */
        case FONT1: return FONT1;
        case FONT6: return FONT6; /* decyzja */
        case FONT2: *zapis = 0;
            continue;
        default: return Esc;
        }
    } /* koniec for(;;) dla jednego raportu na jednym rekordzie  (ret!=1) */
    return Ret;
}

int Esc_Tab_wybor_toru(int Ret, signed char* kod_raportu, int* nr_ob,
                       int ob_pocz, int ob_konc, char* zapis, char* poziom,
                       int* y_danej)
{
    int ret;
    *poziom = nr_blank;
    if (nr_blank >= 0)
    {
        ret = BL[nr_blank]->nr_danej;
        *y_danej = (Ok + ret)->yo + Y_0;
    }
    if (Ret == FONT1)
    {
        Czy_Blankiet_otwarty = 0; /* Trzeba otworzyc nowy blankiet */
        return FONT1;
    }
    if (Ret == FONT6)
    {
        Czy_Blankiet_otwarty = 1; /* Wychodzimy, aby cos policzyc */
        return FONT6;
    }
    do
    {
        ret = GET_char(); /*fprintf(mystderr,"\nznak=%d", ret); */
        switch (ret)
        {
        case Esc:
            *kod_raportu = zamknij_blankiet(nr_ob);
            *poziom = nr_blank;
            *nr_ob = -1;
            if (nr_blank >= 0) *nr_ob = BL[nr_blank]->nr_rekordu;
            if (*nr_ob < 0 || *kod_raportu < 0) return -Esc; /* KONIEC */
            return Esc; /* wychodzimy do raportu nizszego */
        case TAB: if (Ret == -CR) continue;
            return TAB;
        /*        case CR:  goto F2;  */
        case BS: if (ob_pocz == ob_konc) continue;
            (*nr_ob)--;
            if ((*nr_ob) < ob_pocz) (*nr_ob) = ob_konc;
            ret = 1;
            break;
        case SPEC:
            ret = GETchar(); /*fprintf(mystderr," %d", ret); */
            if (ob_pocz == ob_konc && ret != FONT2) continue;
            switch (ret)
            {
            case Home: (*nr_ob) = ob_pocz;
                ret = 1;
                break;
            case End: (*nr_ob) = ob_konc;
                ret = 1;
                break;
            case PgUp: (*nr_ob) += 5;
                if ((*nr_ob) > ob_konc) (*nr_ob) = ob_pocz + ((*nr_ob) - ob_konc - 1);
                ret = 1;
                break;
            case PgDn: (*nr_ob) -= 5;
                if ((*nr_ob) < ob_pocz) (*nr_ob) = ob_konc + ((*nr_ob) - ob_pocz + 1);
                ret = 1;
                break;
            case LEFT:
            case DOWN: (*nr_ob)--;
                if ((*nr_ob) < ob_pocz) (*nr_ob) = ob_konc;
                ret = 1;
                break;

            case FFR:
            case UP: (*nr_ob)++;
                if ((*nr_ob) > ob_konc) (*nr_ob) = ob_pocz;
                ret = 1;
                break;
            case FONT2:
                if (Ret == -CR) continue;
                if (*zapis == 0) continue;
                zapis_raportu(NULL, 0, 0);
                *zapis = 0;
                break;
            default: break;
            }
            break;
        default: break;
        }
    }
    while (ret != 1);
    Czy_Blankiet_otwarty = 1;
    /*    reset_blankiet(*nr_ob);  / * czy to jest potrzebne ??? */
    /* nowy rekord na tym samym blankiecie
                               Bedzie reset(blankiet) */
    /* Wychodzi do raportu na wyzszym poziomie (FONT1) lub calkowicie */
    return ret;
}

char jaka_decyzja(int* kod, int* nr_danej)
{
    *kod = -1;
    *nr_danej = -1;
    if (nr_blank >= 0)
    {
        *nr_danej = BL[nr_blank]->nr_danej;
        *kod = (Ok + (*nr_danej))->decyzja;
        if (((Ok + (*nr_danej))->typ == 'F' || (Ok + (*nr_danej))->typ == 'f' || (Ok + (*nr_danej))->typ == 'd' || (Ok +
            (*nr_danej))->typ == 'w' || (Ok + (*nr_danej))->typ == 'W' || (Ok + (*nr_danej))->typ == 'o') && (Ok + (*
            nr_danej))->proxy == 0)
        {
            *kod = _GRAFIKA_;
            return (Ok + (*nr_danej))->typ;
        }
    }
    return (char)(*(char*)(Ok + (*nr_danej))->wartosc);
}

void* get_plot_data(int nr_danej, int kod, int* ind_min,
                    int* ind_max, int* lkol, int* item_size)
{
    struct okno* ok;
    ok = Ok + nr_danej;
    if (kod != _GRAFIKA_ || ok->proxy != 0) return NULL;
    *ind_min = ok->ind_min;
    *lkol = ok->L_kol;
    *ind_max = ok->ind_max;
    *item_size = ok->dim;
    return ok->wartosc;
}

/* void bez_hasla(void) {BEZ_HASLA=1;} */
/* ===================== Tu zaczyna sie plik wewy4.c ============= */
#ifndef WEWY_INCL
#define _DOS_
#include <stdio.h>
#include <process.h>
#ifdef _DOS_
#include <alloc.h>
#include <dos.h>
#include "../blank/intdef.h"
#define Malloc farmalloc
#define Free farfree
#else
#include "../blank/intdef.h"
#include <malloc.h>
#include <sys/qnxterm.h>
#define Malloc malloc
#define Free free
#endif
#include <conio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "wewybl.h"
#define WEW_EXT extern
#include "WEWY1.H"
#endif

/*   ================================================================ */
int rap_blank(char* tytul, signed char* kod_raportu, int* nr_rekordu)
{
    int no, y, x, yp, xp, ret, wp, l_oblig = 0, dana = 0, znak, d0, y_shift;
    struct okno* ok;
    signed char nr_raportu;
    int rozmiar_ob, l_danych, ypocz;
    char x_tyt, y_tyt, *d_oryg = NULL, *d_rek = NULL, *zmiany;
    static int wpisano = 0;
    unsigned int attr, tlo, attr_ok, attr_wp;
    unsigned int text_a, attr_h, attr_d;
    if (nr_blank < 0 || BL[nr_blank] == NULL || Ok == NULL)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Blankiet %d nie otwarty. <Ent>", nr_blank);
        GET_char();
        return -1;
    }
    liczba_danych = BL[nr_blank]->liczba_danych;
    l_danych = liczba_danych;
    if (l_danych == 0)
    {
        komunikat(5, 1,ATTR_A, " Brak danych blankietu %d: <Ent>", nr_blank);
        GET_char();
        return -1 /*zamknij_blankiet()*/;
    }
    /*  nr_danej=0; */
    ABORT = 0;
    BLAD = 0;
    Kolory(Akt_Kolor, &text_a, &tlo, &attr_d, &attr_ok, &attr_wp, &attr_inf, &attr_h);
    y_tyt = Y_0 - 1;
    if (BL[nr_blank]->ramka == 1)
    {
        Okno(Y_0 - 1, X_0 - 1, Y_maxBlank + 1, X_maxBlank + 1, text_a | tlo);
        maluj_ramke(Y_0 - 1, X_0 - 1, Y_maxBlank + 1, X_maxBlank + 1, text_a | tlo);
        y_tyt = Y_0 - 1;
    }
    else { if (tytul != NULL && strlen(tytul) > 0 && y_tyt == Y_0) Y_0++; }
    if (tytul != NULL) x_tyt = X_0 + (X_maxBlank - X_0 - strlen(tytul) - 2) / 2;
    term_flush();
    tlo = tlo & (~TERM_FLUSH);
    attr_ok = attr_ok & (~TERM_FLUSH);
    attr_wp = attr_wp & (~TERM_FLUSH);
    attr_d = attr_d & (~TERM_FLUSH);
    attr_inf = attr_inf & (~TERM_FLUSH);
    text_a = text_a & (~TERM_FLUSH);
    attr_o[0] = attr_ok;
    attr_o[1] = attr_o[0];
    attr_dat[0] = attr_d;
    attr_dat[1] = attr_dat[0];
    attr_dat[2] = attr_dat[0];
    attr_o[2] = attr_wp; /*attr_d|TERM_MAGENTA_BG; */
    attr_dat[2] = attr_o[2];
    attrtx[0] = text_a | tlo;
    attrtx[1] = text_a | tlo | INVERSE;
    attrtx[2] = attrtx[1];
    /* ------------------------------------------------------------------ */
    if (BL[nr_blank]->rek_danych_blank >= 0)
        term_printf(y_tyt, x_tyt, attr_o[1], "%s %d ", tytul, BL[nr_blank]->rek_danych_blank);
    else term_printf(y_tyt, x_tyt, attrtx[1], "%s", tytul);
    Okno(Y_0, X_0, Y_maxBlank, X_maxBlank, text_a | tlo);
    clr_Okno();
    /* -------------------------------------------------------------------
         wp=0 - prezentacja danych poczatkowych
                        (text czarny, ?? na koncu i zielone okno)
         wp=1 - wpis wartosci w oknie (tekst INVERSE i okno MAGENTA, ?? na koncu)
         wp=2 - prezentacja danej po wpisie (text czarny, okno zielone, bez ?? )
         ------------------------------------------------------------------- */
    /*  wpisano=0; */
    ypocz = m_wherey();
    wp = 0;
    ok = Ok;
    ret = 0;
    l_oblig = 0; /* wyswietlenie calego blankietu */
    ret = X_0; /* -1;bylo do grudn.97 */
    m_gotoxy(X_0, Y_0);
    y_shift = 0;
    /*fprintf(mystderr,"\nOK=%d Ok->form=%s !!!",Ok,Ok->format); */
    for (no = 0; no < l_danych; no++, ok++)
    {
        if (ok->format == NULL)
        {
            l_danych = no;
            break;
        }
        /*    Okno(Y_0,X_0,Y_maxBlank,X_maxBlank,text_a|tlo);  / * to popr. z grudn.97 */
        yp = m_wherey();
        xp = ret; /* +1; tak bylo do grudn.97  */
        x = ok->xo + X_0;
        y = ok->yo + Y_0;
        if (y < yp) y = yp;
        if (x < X_0) x = xp;
        else { if (x < xp) y = yp + 1; }
        if (y < ok->yo + Y_0) y = ok->yo + Y_0;
        if (y > Y_maxBlank)
        {
            if (BL[nr_blank]->scroll < 0)
            {
                l_danych = no + 1;
                y = Y_maxBlank;
            }
            else y_shift = Y_maxBlank - y;
            /*fprintf(mystderr,"\nSCROLL: y=%d Y_maxBlank=%d l_danych=%d/%d",y,Y_maxBlank,l_danych,liczba_danych); */
        }
        if (y < Y_0) y = yp;
        wp = 0;
        attr = attr_o[0];
        if (ok->ochrona > Uprawn)
        {
            attr = attr_inf;
            wp = 2;
        }
        if (ok->blok_wpisu == 1) attr = attr_o[0];
        /* to odblokowac dla sledzenia */
        if (SLEDZIC == 1)
        {
            term_flush();
            komunikat(MY_MAX, X_L0,ATTR_A, " no=%d ld=%d typ=%c x=%d y=%d Ymax=%d/%d X0=%d ok->x=%d ok->y=%d <Ent> ",
                      no, l_danych, ok->typ, x, y, Y_maxBlank, MY_MAX, X_L0, ok->xo, ok->yo);
            GET_char();
        }
        setcursor(nocursor);
        do
        {
            ret = disp_dana(wp, y - y_shift, x, attr_d, attr, attrtx[0], ok, 0, 0);
            if (Y_maxBlank < 0) Y_maxBlank = -Y_maxBlank;
            if (ret == -1)
            {
                wyjscie(Ok, liczba_danych);
                return -1;
            }
            if (ret <= 0)
            {
                if (y == yp)
                {
                    y++;
                    x = X_0;
                    if (y >= Y_maxBlank)
                    {
                        if (BL[nr_blank]->scroll < 0)
                        {
                            l_danych = no;
                            break;
                        }
                        else y_shift = Y_maxBlank - y;
                    }
                }
                else
                {
                    if (x > X_0) x = X_0;
                    else Y_maxBlank = -Y_maxBlank;
                }
            }
        }
        while (ret <= 0);
        setcursor(cursor); //term_cur(y_cursor, x_cursor);
        if (l_danych == no) break;
        if (ok->obowiazk == 1 && ok->ochrona <= Uprawn) l_oblig++;
        ok->yo = y - Y_0;
        ok->xo = x - X_0;
        ok->xmax = ret;
        if (ok->yplus == 1)
        {
            y++;
            if (y > Y_maxBlank) y = Y_maxBlank;
            ret = X_0; /*-1; do grudn.97*/
            term_cur(y, X_0);
        }
        /*fprintf(mystderr, " no=%d y=%d form=%s;",no,ok->yo,ok->format); */
    }
    if (wpisano == UNDO) /* Po anulowaniu zmian  */
    {
        wyjscie(Ok, liczba_danych);
        wpisano = 0;
        return Esc;
    }
    wpisano = 0;
    nr_raportu = BL[nr_blank]->nr_raportu;
    *kod_raportu = RAP[nr_raportu]->kod_raportu;
    *nr_rekordu = BL[nr_blank]->nr_rekordu;
    dana = BL[nr_blank]->nr_danej;
    if (dana >= l_danych) dana = l_danych - 1;
    ok = Ok + dana;
    if (BL[nr_blank]->scroll >= 0)
    {
        BL[nr_blank]->y_scroll = (Ok + BL[nr_blank]->scroll)->yo;
    }
    if (ok->ochrona > Uprawn)
    {
        for (d0 = dana + 1; d0 != dana; d0++)
        {
            if (d0 >= l_danych) d0 = 0;
            if (d0 == dana) break;
            ok = Ok + d0;
            if (ok->ochrona <= Uprawn || ok->blok_wpisu == 1) break;
        }
        if (d0 == dana)
        {
            ustaw_zapis(*nr_rekordu, ZAPIS);
            /*      if(ZAPIS>0)
                         {d_oryg=dane_oryginalne(*nr_rekordu,tytul,&d_rek, &rozmiar_ob, &zmiany);
                    if(zmiany!=NULL) *zmiany=ZAPIS;
                   }
            */
            wyjscie(Ok, liczba_danych);
            return -CR;
        }
        dana = d0;
        BL[nr_blank]->nr_danej = d0;
    }
    /* ------- tu zrobimy kopie oryg.danych i udost.wskaznik do zapis[] --- */
    d_oryg = dane_oryginalne(*nr_rekordu, tytul, &d_rek, &rozmiar_ob, &zmiany);
    if (zmiany == NULL) return -CR;
    wp = 0;
    nr_danej = dana;
    for (;;) /* obsluga raportu az do wyjscia */
    {
        l_oblig = 0;
        wpisano = ZAPIS;
        dana = BL[nr_blank]->nr_danej;
        znak = obsluga_wpisu(dana, Ok, l_danych, attr_dat, attr_o, attrtx, attr_h,
                             attr_inf, &l_oblig, kod_raportu, &wpisano);
        /* ---------------- Wyjscia PROCEDURY -------------------------------
                znak= FONT1  - ma byc raport na wyzszym poziomie: wyjscie do blankiet()
            znak= Esc    - wyjscie z blankietu do menu: czytanie dalszych polecen
             ------------------------------------------------------------------ */
        if (wpisano < ZAPIS) wpisano = ZAPIS;
        dana = BL[nr_blank]->nr_danej;
        *nr_rekordu = BL[nr_blank]->nr_rekordu;
        if (wpisano < (*zmiany)) wpisano = (*zmiany); /* odtwarzamy stare zapisy */
        else *zmiany = wpisano; /* notujemy, ze byly zmiany w rekordzie */
        if (blokada_zapisu != ZAPIS_DOWOLNY)
        {
            ret = Esc;
            wpisano = 0;
        }
        if (znak == FONT1)
        {
            if ((Ok + dana)->proxy == 0)
            {
                if ((Ok + dana)->typ == 'g' || (Ok + dana)->typ == 'G') /* raport dotyczy helpu */
                    (*nr_rekordu) = *(int*)((char*)(Ok + dana)->index_helpu
                        + (*(int*)(Ok + dana)->wartosc) * (Ok + dana)->size_helpu);
                else znak = FONT6;
            }
            else *nr_rekordu = *(int*)(Ok + dana)->wartosc;
        }
        if (znak == FONT6) return znak;
        /* ============================================================== */
        if (wpisano < ZAP_MIN)
        {
            wyjscie(Ok, liczba_danych);
            return znak; /* bylo ret */
        }
        /* ----------------- cos zostalo wpisane ---------------------------- */
#ifndef _DOS_
        term_type(MY_MAX, X_L0,
                  "�<Tab>-powrot do danych�<F2> -ZAPIS�<q> -WYJSCIE�<Esc> -wyjscie Z ANULOW.ZMIAN �", 0, attr_h);
#else
        term_type(MY_MAX, X_L0,
                  "�<Tab>-powrot do danych�<F2> -ZAPIS�<q> -WYJSCIE�<Esc> -wyjscie Z ANULOW.ZMIAN�", 0, attr_h);
        term_type(MY_MAX, 24, "�<F2> -ZAPIS�<q> -WYJSCIE�", 0,ATTR_A);
#endif
        do
        {
            znak = GET_char();
            znak = znak & 0x00ff;
            if (znak == Esc) /* porzucenie blankietu, do menu   */
            {
                if (wpisano >= ZAP_MIN)
                {
                    term_type(MY_MAX, X_L0,
                              "�  Chcesz zakonczyc raport BEZ WPISU !!! <t/n> ?? �", 0,ATTR_A);
                    do
                    {
                        znak = GET_char();
                        znak = znak & 0x00ff;
                    }
                    while (znak != 't' && znak != 'n');
                    if (znak == 'n')
                    {
                        znak = TAB;
                        continue;
                    }
                    if (znak == 't')
                    {
                        przepisz(d_rek, d_oryg, rozmiar_ob);
                        *zmiany = 0;
                        if (wpisano > ZAP_MIN + 1) /* #### ???? ##### */
                            return FONT6; /* zmieniono decyzje - powrot do poprzedniej */
                        term_cur(ypocz, X_0);
                        wpisano = UNDO;
                        return UNDO; /* wyswietlenie poprawionego raportu i wyjscie */
                    }
                }
                wyjscie(Ok, liczba_danych);
                /* na zewnatrz mozna wejsc na inny rekord  */
                return Esc; /* dalej bedzie bez F2 */
            }
            if (znak == 'q') /* wyjscie bez zapisu, ale bez anul.danych */
            {
                wyjscie(Ok, liczba_danych);
                return TAB; /*Esc; / * bylo TAB */
            }
            if (znak == SPEC || znak == EOS)
            {
                znak = GETchar();
                znak = znak & 0x00ff;
                if (znak == SHIFT_TAB || znak == CTRL_TAB)
                {
                    przelacz_potwierdzanie();
                    continue;
                }
                if (znak == FONT2 && wpisano >= ZAP_MIN)
                {
                    if (l_oblig == 0)
                    {
                        wyjscie(Ok, liczba_danych);
                        ret = zapis_raportu(NULL, 0, 0);
                        if (ret < 0)
                        {
                            wpisano = UNDO;
                            return UNDO;
                        }
                        return Esc;
                    }
                    term_type(MY_MAX, X_L0,
                              "�Wpisz dane OBLIGATORYJNE (ze znakiem ?? ) -<Ent>; lub wyjdz bez zapisu -<q>�", 0,
                              ATTR_A);
                    znak = GET_char();
                    znak = znak & 0x00ff;
                    if (znak == CR) znak = TAB; /* powrot do edycji */
                    if (znak == 'q')
                    {
                        wyjscie(Ok, liczba_danych);
                        return TAB;
                    }
                }
            }
        }
        while (znak != TAB); /* TAB - powrot do tego samego raportu */
    } /* koniec raportu */
    /* ============= Interpretacja wyjsc: ===============================
       TAB   - zostalo cos do zapisania
         Esc   - nie ma nic do zapisania
         FONT1 - nowy raport
       FONT6 - decyzja
       ================================================================== */
}

/* ------------------------------------------------------------ */
void ustaw_zapis(int nr_rekordu, char zapis)
{
    signed char nr_raportu;
    struct reports* R;
    if (nr_blank >= 0)
    {
        nr_raportu = BL[nr_blank]->nr_raportu;
        R = RAP[nr_raportu];
        if (nr_rekordu >= R->ob_pocz && nr_rekordu <= R->ob_konc && R->zapis != NULL)
            R->zapis[nr_rekordu - R->ob_pocz] = zapis;
    }
}

/* ------------------------------------------------------------ */
char* dane_oryginalne(int nr_rekordu, char* tytul, char** d_rek,
                      int* rozmiar, char** zmiany)
{
    signed char nr_raportu;
    char* d_or = NULL;
    struct reports* R;
    nr_raportu = BL[nr_blank]->nr_raportu;
    R = RAP[nr_raportu];
    *rozmiar = 0;
    if (R->zapis == NULL || nr_rekordu < R->ob_pocz || nr_rekordu > R->ob_konc)
    {
        *zmiany = NULL;
        return NULL;
    }
    *zmiany = &R->zapis[nr_rekordu - R->ob_pocz];
    if (Uprawn < 1) return NULL;
    *d_rek = ustal_adres_rek(R->kod_raportu, nr_rekordu);
    d_or = R->d_oryg[nr_rekordu - R->ob_pocz];
    if (d_or == NULL)
    {
        d_or = (char*)Malloc(R->rozmiar_ob * sizeof(char));
        if (d_or == NULL)
        {
            int zn = 0, l = strlen(tytul);
            if (l > 30)
            {
                zn = tytul[30];
                tytul[30] = 0;
            }
            komunikat(MY_MAX, 1,ATTR_A, "%s. Brak miejsca na dane. Bedzie bierny  !! <Ent> ",
                      tytul);
            if (zn != 0) tytul[30] = zn;
            GET_char();
            *rozmiar = 0;
            *zmiany = NULL;
            return NULL;
        }
        R->d_oryg[nr_rekordu - R->ob_pocz] = d_or;
        lwmall++;
        przepisz(d_or, *d_rek, R->rozmiar_ob);
    }
    *rozmiar = R->rozmiar_ob;
    return d_or;
}


char* czy_jest_blankiet(signed char kod_raportu, int* anim_pid, int* nr_ob,
                        int* ob_pocz, int* ob_konc, int* rozmiar_ob,
                        char** tytul, signed char* Kolor, signed char* Ramka,
                        char czynny, int yp, int xp, int ym, int xm)
{
    int ret, ob_p, ob_k, nr_rap, kod_rap, nr_rek;
    signed char kolor, ramka, nrdan = -1;
    char* Dane;
    int (*def_blankiet)(int, int, int, int, int, int, char*);

    /* ----------- Zmienna globalna Czy_Blankiet_otwarty:
            0 - nalezy otworzyc nowy blankiet
            1 - nalezy resetowac poprzedni i zdefiniowac dane blankietu
            2 - blankiet gotowy do prezentacji (np po usunieciu wyzszego blankietu)
         ----------------------------------------------------------------------- */
    /*fprintf(mystderr,"\nCZY_JEST_BL: kod_rap=%d nr_ob=%d ob_p=%d ob_k=%d Czy_bl=%d",
                               kod_raportu, *nr_ob,*ob_pocz,*ob_konc,Czy_Blankiet_otwarty);
    */
    if (Czy_Blankiet_otwarty > 0)
    {
        if (nr_blank < 0) Czy_Blankiet_otwarty = 0;
        else
        {
            nr_rap = BL[nr_blank]->nr_raportu;
            kod_rap = RAP[nr_rap]->kod_raportu;
            nr_rek = BL[nr_blank]->nr_rekordu;
            if (BL[nr_blank]->reset > 0)
            {
                nrdan = BL[nr_blank]->nr_danej;
                BL[nr_blank]->reset = 0;
                /*fprintf(mystderr,"\nRESETOWANIE_BL(%d/%d): BL=%d nrdan=%d nr_rek=%d nr_rap=%d",
                                                                     nr_blank, Czy_Blankiet_otwarty, BL[nr_blank],nrdan,nr_rek,nr_rap);
                */
                Czy_Blankiet_otwarty = 1;
                BL[nr_blank]->nr_danej = dana_poczatkowa; /* ## 30.09.98 */
            }
            if (kod_rap != kod_raportu)
            {
                Czy_Blankiet_otwarty = 0;
                nrdan = -1;
            }
            else
            {
                if (nr_rek != *nr_ob) Czy_Blankiet_otwarty = 1;
                else nrdan = BL[nr_blank]->nr_danej;
            }
        }
    }
    /*  if(Czy_Blankiet_otwarty==2) nr_danej=BL[nr_blank]->nr_danej; */
    /* ---------------- To byla zmiana blankietu po decyzji ------------- */
D:
    def_blankiet = czy_jest(kod_raportu, anim_pid, ob_pocz, ob_konc,
                            rozmiar_ob, tytul, &kolor, &ramka, &Dane);
    if (Dane == NULL) Czy_Blankiet_otwarty = 0;
    /*fprintf(mystderr,"\nCzy_jest(%d/%d)%s D=%d ob_p=%d ob_k=%d DEF_BL=%d",
                               nr_blank, Czy_Blankiet_otwarty, *tytul, Dane,*ob_pocz,*ob_konc,def_blankiet);
    */
    ob_p = *ob_pocz;
    ob_k = *ob_konc;
    if ((*nr_ob) < ob_p) *nr_ob = ob_p;
    if ((*nr_ob) > ob_k) *nr_ob = ob_k;
    switch (Czy_Blankiet_otwarty)
    {
    case 0:
        if (ramka >= 0) *Ramka = ramka;
        if (kolor >= 0) *Kolor = kolor;
        kolor = *Kolor;
        ramka = *Ramka;
        ret = otworz_blankiet(kolor, ramka, yp, xp, ym, xm, kod_raportu, *nr_ob,
                              czynny);
        if (ret == -1)
        {
            term_type(MY_MAX, X_L0,
                      " Nie mozna otworzyc blankietu !! <Ent> ", 0,ATTR_A);
            zamknij_raporty();
            GET_char();
            return NULL;
        }
        if (ret == 0)
        {
            zamknij_raporty();
            return NULL;
        }
        if (ret == -2)
        {
            Czy_Blankiet_otwarty = 2;
            kod_raportu = RAP[BL[nr_blank]->nr_raportu]->kod_raportu;
            goto D;
        }
        break;
    case 1:
        reset_blankiet(*nr_ob);
        if ((*nr_ob) < ob_p) *nr_ob = ob_p;
        if ((*nr_ob) > ob_k) *nr_ob = ob_k;
        /*fprintf(mystderr,"\nCzy_jest_po_reset_bl(%d): nr_danej=%d D=%d nr_ob=%d ob_p=%d ob_k=%d xp=%d yp=%d anim_pid=%d DEF_BL=%d",
                                   nr_blank, nr_danej, Dane,*nr_ob,ob_p, ob_k, xp, yp,*anim_pid,def_blankiet);
        */
        break;
    case 2: nr_danej = BL[nr_blank]->nr_danej;
        return Dane;
    default:
        komunikat(MY_MAX, 1,ATTR_A, " Czy_Blankiet_otwarty=%d ?? <Ent> ",
                  Czy_Blankiet_otwarty);
        GET_char();
        return NULL; /* break; */
    }
    BL[nr_blank]->y_scroll = -1;
    BL[nr_blank]->scroll = -1;
    ret = def_blankiet(*nr_ob, ob_p, ob_k, xp, yp, kod_raportu, Dane);
    if (SCROLL >= 0)
    {
        BL[nr_blank]->scroll = SCROLL;
        SCROLL = -1;
    }
    /*fprintf(mystderr,"\nreset_po_def_bl(%d/%d): D=%d ret=%d nr_ob=%d nr_rap=%d",
                               nr_blank, Czy_Blankiet_otwarty, Dane,ret,*nr_ob,nr_rap);
    */
    if (ret < 0)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Brak danych w blankiecie %d: nr_danej=%d nr_ob=%d . <Ent> ",
                  nr_blank, nr_danej, *nr_ob);
        GET_char();
        zamknij_raporty();
        return NULL;
    }
    Czy_Blankiet_otwarty = 2;
    if (nrdan >= 0)
    {
        nr_danej = nrdan;
        BL[nr_blank]->nr_danej = nrdan;
    }
    else nr_danej = 0;
    /* if((*nr_ob)>0) {komunikat(MY_MAX,X_L0,ATTR_A," nr_ob=%d (dana %d)",*nr_ob,nrdan); GET_char();} */
    /*fprintf(mystderr,"\nKONIEC czy_jest_blankiet()"); */
    return Dane;
}

char czy_byly_zmiany(void)
{
    signed char nr_raportu, i;
    char zmiany = 0;
    struct reports* R;
    int lrek;
    if (Uprawn < 1) return 0;
    if (nr_blank < 0) return 0;
    nr_raportu = BL[nr_blank]->nr_raportu;
    if (nr_raportu < 0) return 0;
    R = RAP[nr_raportu];
    if (R == NULL) return 0;
    if (R->zapis == NULL) return 0;
    lrek = R->ob_konc - R->ob_pocz + 1;
    for (i = 0; i < lrek; i++)
    {
        if (R->zapis[i] >= ZAP_MIN && zmiany < R->zapis[i]) zmiany = R->zapis[i];
    }
    return zmiany;
}

void wyjscie(struct okno* Ok, int liczba)
{
    int i;
    struct okno* ok;
    ok = Ok;
    for (i = 0; i < liczba; i++, ok++)
    {
        if (ok->wpis > 0)
        {
            if (ok->format[0] == '*' && ok->obowiazk == 1) ok->format[0] = '#';
            if (ok->format[1] == '*' && ok->obowiazk == 1) ok->format[1] = '#';
        }
    }
    return;
}

/* ------------------------------------------------------------ */
void wysylka(int dana, int kod, float wartosc, int proxy)
{
    dana = dana;
    kod = kod;
    wartosc = wartosc;
    proxy = proxy;
};
/* =================================================================== */
int obsluga_wpisu(int dana, struct okno* Ok, int liczba, unsigned int attr_dat[],
                  unsigned int attr_o[], unsigned int attrtx[], unsigned int attr_h,
                  unsigned int attr_inf, int* loblig, signed char* raport,
                  int* wpisano)
{
    int znak, stara, nowa, ret, l_oblig, z_menu, y, x, yo, xo, ind, y_shift = 0;
    char mod, help, Tabl = 0;
    signed char wp;
    struct okno* ok;
    /* -------------------------------------------------------------------
             obsluga pozycji dana procedura disp_dana():
            mod=0 wydruk starej danej:
                  bez ?? (wp=2)- gdy byla wpisana; z ?? (wp=0) - gdy nie
            mod=1 podswietlenie nowej (wp=0, tj z ' ?? ')
            mod=2 wpis nowej po jej wybraniu (CR) z wp=1
       -------------------------------------------------------------------  */
    y_shift = 0; /*BL[nr_blank]->y_shift; */
    l_oblig = *loblig;
    /*   *raport=-1; */
    if (dana >= liczba) dana = liczba - 1;
    nowa = dana;
    mod = 1;
    ok = Ok + dana;
    znak = 0;
    wp = 0;
    help = 0;
    do /* petla po mod (po zmianie danej) do nastepnej zmiany */
    {
    Q:
        ok = Ok + dana; /* ustawienie na pozycji dana */
        if (mod == 0)
        {
            wp = 0;
            if (ok->wpis == 1 || ok->blok_wpisu == 1) wp = 2;
        }
        /*if(ok->typ=='F') fprintf(mystderr,"\nwp=%d %s ",wp,ok->format); */
        z_menu = 0;
    P:
        ret = disp_dana(wp, ok->yo + Y_0, ok->xo + X_0, attr_dat[mod], attr_o[mod], attrtx[mod], ok, y_shift, mod);
        if (wp < -2) wp = -wp;
        if (ret == -1 && wp < 3) return ret;
        if (wp == 1 || wp == -1 || (wp == 4 && wpisano_dana > 0))
        {
            if (ret > 0 || z_menu > 0 || wpisano_dana > 0)
            {
                ret += z_menu;
                *wpisano = 1;
                if (ok->ochrona > 0)
                {
                    *wpisano = 2;
                    if ((*wpisano) < ZAP_MIN) (*wpisano) = ZAP_MIN;
                }
                if (ok->decyzja >= 0)
                {
                    if (ok->ochrona > 0) *wpisano = 3;
                    return FONT6;
                }
            }
        }
        if (wp == -2) wp = 0;
        if (wp > 2)
        {
            if (ret >= 0)
#ifndef _DOS_
            {
                term_printf(MY_MAX, X_L0, attr_h,
                            "�  Wpis danej: <Esc> - wyjscie�  <Ent> - akceptacja � <%c, %c> - inkrementacja   �",
                            24, 25);
#else
            {
                term_printf(MY_MAX, X_L0, attr_h,
                            "�  Wpis danej: <Esc> - wyjscie�  <Ent> - akceptacja � <%c, %c> - inkrementacja  �",
                            24, 25);
#endif
                term_cur(MY_MAX, X_L0 + 1);
                if (ok->blok_wpisu == 1)
                {
                    wp = 0;
                    mod = 1;
                    goto Q;
                } /* ### dodane 22.01.98 */
                else
                {
                    term_type(MY_MAX, X_L0 + 1, "  Wpis danej ", 0, attr_o[2]);
                    wp = 1;
                    mod = 2;
                    z_menu = ret;
                    goto P;
                }
            }
            else
            {
                wp = 0;
                mod = 1;
                goto Q;
            }
        }
        switch (mod)
        {
        case 0: /* po odnowieniu poprzedniej danej */
            dana = nowa;
            mod = 1;
            wp = 0;
            break; /* gdy znak=Esc lub TAB - wyjscie do menu glownego */
        case 1: /* po podswietleniu nowej danej */
            nowa = dana;
            stara = dana;
            BL[nr_blank]->nr_danej = dana;
            /*          for(i=stara+1;i!=stara;i++)
                       {if(i==liczba) i=0;
                        ok=Ok+i; if(ok->ochrona<Upr_max) break;
                       }
                      if(i==stara) {i=1; k=4;}
            */
            ok = Ok + dana;
            Tabl = 0;
            help = 0; /* help=: 1- raport; 2-menu; 4- tablica */
            if (ok->typ == 'W' || ok->typ == 'F')
            {
                help = 4;
                Tabl = 1;
                if (ok->proxy == 0) help = 14;
            }
            if ((ok->typ == 'o' || ok->typ == 'f' || ok->typ == 'd') && ok->proxy == 0) help = 16;
            if (ok->typ == 'Z' || ok->typ == 'I' || ok->typ == 'G')
            {
                help = 4;
                Tabl = 1;
                if (ok->prec > 0) help = 6;
                else { if (ok->typ == 'G') help = 6; }
            }
            if (ok->prec > 0)
            {
                if (ok->typ == 's' || ok->typ == 'z' || ok->typ == 'i' || ok->typ == 'g')
                    help = 2;
            }
            else { if (ok->typ == 'g') help = 2; }
            if (ok->yo >= MYR_MAX) help = 0;
            if ((ok->typ == 'i' || ok->typ == 'g') && ok->kod_anim >= 0) help += 1;
            if ((ok->typ == 'I' || ok->typ == 'G') && ok->kod_anim >= 0) help += 1;
#ifndef _DOS_
            switch (help)
            {
            case 7: /* help= 1+2+4- raport+menu+tablica */
                term_printf(MY_MAX, X_L0, attr_h,
                            "�<%c,%c,%c,%c>-dana�<F1>-raport�<F3>-menu�<F4>-tabl",
                            24, 25, 26, arrow);
                if (ok->blok_wpisu == 0) term_type(MY_MAX, m_wherex(), "�<ENT>-edycja �<Esc,Tab>-wyjscie �", 0, attr_h);
                else term_type(MY_MAX, m_wherex(), "ica  � <Esc,Tab>-wyjscie z rap.  �", 0, attr_h);
                break;
            case 6: /* help= 2+4- menu+tablica */
                term_printf(MY_MAX, X_L0, attr_h,
                            "�<%c,%c,%c,%c>-wybor danej�<F3>-menu �<F4>-tablica",
                            24, 25, 26, arrow);
                if (ok->blok_wpisu == 0) term_type(MY_MAX, m_wherex(), "�<ENT>-edycja �<Esc,Tab>-wyjscie �", 0, attr_h);
                else term_type(MY_MAX, m_wherex(), "  � <Esc,Tab>-wyjscie z raportu  �", 0, attr_h);

                break;
            case 5: /* help= 1+4- raport+tablica */
                term_printf(MY_MAX, X_L0, attr_h,
                            "�<%c,%c,%c,%c>-wybor danej�<F1>-raport�<F4>-tablica",
                            24, 25, 26, arrow);
                if (ok->blok_wpisu == 0) term_type(MY_MAX, m_wherex(), "�<ENT>-edycja �<Esc,Tab>-wyjscie �", 0, attr_h);
                else term_type(MY_MAX, m_wherex(), "  � <Esc,Tab>-wyjscie z raportu  �", 0, attr_h);

                break;
            case 4: /* help= 4- tablica */
                term_printf(MY_MAX, X_L0, attr_h,
                            "�<%c,%c,%c,%c,Home,End>-wybor danej �<F4>- tablica",
                            24, 25, 26, arrow);
                if (ok->blok_wpisu == 0) term_type(MY_MAX, m_wherex(), "�<ENT>-edycja �<Esc,Tab>-wyjscie �", 0, attr_h);
                else term_type(MY_MAX, m_wherex(), " � <Esc,Tab> - wyjscie z raportu �", 0, attr_h);
                break;
            case 14: /* help= 14- tablica i grafika */
                term_printf(MY_MAX, X_L0, attr_h,
                            "�<%c,%c,%c,%c,Home,End>-wybor�<F1>-wykr.�<F4>-tabl",
                            24, 25, 26, arrow);
                if (ok->blok_wpisu == 0) term_type(MY_MAX, m_wherex(), "�<ENT>-edycja �<Esc,Tab>-wyjscie �", 0, attr_h);
                else term_type(MY_MAX, m_wherex(), " � <Esc,Tab> - wyjscie z raportu �", 0, attr_h);
                break;
            case 3:
                term_printf(MY_MAX, X_L0, attr_h,
                            "�<%c,%c,%c,%c>-wybor danej�<F1>-raport �<F3>-menu ",
                            24, 25, 26, arrow);
                if (ok->blok_wpisu == 0) term_type(MY_MAX, m_wherex(), "�<ENT>-edycja �<Esc,Tab>-wyjscie �", 0, attr_h);
                else term_type(MY_MAX, m_wherex(), "� <Esc,Tab> - wyjscie z raportu  �", 0, attr_h);
                break;
            case 2:
                term_printf(MY_MAX, X_L0, attr_h,
                            "�<%c,%c,%c,%c,Home,End>-wybor danej �<F1,F3>-menu ",
                            24, 25, 26, arrow);
                if (ok->blok_wpisu == 0) term_type(MY_MAX, m_wherex(), "�<ENT>-edycja �<Esc,Tab>-wyjscie �", 0, attr_h);
                else term_type(MY_MAX, m_wherex(), " � <Esc,Tab> - wyjscie z raportu �", 0, attr_h);
                break;
            case 1:
                term_printf(MY_MAX, X_L0, attr_h,
                            "�<%c,%c,%c,%c,Home,End>-wybor danej �<F1>-raport ",
                            24, 25, 26, arrow);
                if (ok->blok_wpisu == 0) term_type(MY_MAX, m_wherex(), "�<ENT>-edycja �<Esc,Tab>-wyjscie �", 0, attr_h);
                else term_type(MY_MAX, m_wherex(), " � <Esc,Tab> - wyjscie z raportu �", 0, attr_h);
                break;
            case 0:
                term_printf(MY_MAX, X_L0, attr_h, /*TERM_CYAN|INVERSE|TERM_FLUSH,*/
                            "�<%c,%c,%c,%c, Home, End> - wybor danej ",
                            24, 25, 26, arrow);
                if (ok->blok_wpisu == 0)
                    term_type(MY_MAX, m_wherex(), "�<ENT> - edycja danej �<Esc,Tab> - wyjscie �",
                              0, attr_h);
                else term_type(MY_MAX, m_wherex(), "do prezent.� <Esc,Tab> - wyjscie z raportu �", 0, attr_h);
                break;
            case 16:
                term_printf(MY_MAX, X_L0, attr_h, /*TERM_CYAN|INVERSE|TERM_FLUSH,*/
                            "�<%c,%c,%c,%c,Home,End>-wybor�<F1>-wykr.",
                            24, 25, 26, arrow);
                if (ok->blok_wpisu == 0)
                    term_type(MY_MAX, m_wherex(), "�<ENT> - edycja danej �<Esc,Tab> - wyjscie �",
                              0, attr_h);
                else term_type(MY_MAX, m_wherex(), "do prezent.� <Esc,Tab> - wyjscie z raportu �", 0, attr_h);
            default: break;
            }
#else
            switch (help)
            {
            case 7: /* help= 1+2+4- raport+menu+tablica */
                term_printf(MY_MAX, X_L0, attr_h,
                            "�<%c,%c,%c,%c>-dana�<F1>-raport�<F3>-menu�<F4>-tabl",
                            24, 25, 26, arrow);
                if (ok->blok_wpisu == 0) term_type(MY_MAX, m_wherex(), "�<ENT>-edycja�<Esc,Tab>-wyjscie�", 0, attr_h);
                else term_type(MY_MAX, m_wherex(), "ica  � <Esc,Tab>-wyjscie z rap.�", 0, attr_h);
                break;
            case 6: /* help= 2+4- menu+tablica */
                term_printf(MY_MAX, X_L0, attr_h,
                            "�<%c,%c,%c,%c>-wybor danej�<F3>-menu �<F4>-tablica",
                            24, 25, 26, arrow);
                if (ok->blok_wpisu == 0) term_type(MY_MAX, m_wherex(), "�<ENT>-edycja�<Esc,Tab>-wyjscie�", 0, attr_h);
                else term_type(MY_MAX, m_wherex(), "  � <Esc,Tab>-wyjscie z raportu�", 0, attr_h);

                break;
            case 5: /* help= 1+4- raport+tablica */
                term_printf(MY_MAX, X_L0, attr_h,
                            "�<%c,%c,%c,%c>-wybor danej�<F1>-raport�<F4>-tablica",
                            24, 25, 26, arrow);
                if (ok->blok_wpisu == 0) term_type(MY_MAX, m_wherex(), "�<ENT>-edycja�<Esc,Tab>-wyjscie�", 0, attr_h);
                else term_type(MY_MAX, m_wherex(), "  � <Esc,Tab>-wyjscie z raportu�", 0, attr_h);

                break;
            case 4: /* help= 4- tablica */
                term_printf(MY_MAX, X_L0, attr_h,
                            "�<%c,%c,%c,%c,Home,End>-wybor danej �<F4>- tablica",
                            24, 25, 26, arrow);
                if (ok->blok_wpisu == 0) term_type(MY_MAX, m_wherex(), "�<ENT>-edycja�<Esc,Tab>-wyjscie�", 0, attr_h);
                else term_type(MY_MAX, m_wherex(), " �<Esc,Tab> - wyjscie z raportu�", 0, attr_h);
                break;
            case 14: /* help= 14- tablica i grafika */
                term_printf(MY_MAX, X_L0, attr_h,
                            "�<%c,%c,%c,%c,Home,End>-wybor�<F1>-wykr.�<F4>-tabl",
                            24, 25, 26, arrow);
                if (ok->blok_wpisu == 0) term_type(MY_MAX, m_wherex(), "�<ENT>-edycja �<Esc,Tab>-wyjscie�", 0, attr_h);
                else term_type(MY_MAX, m_wherex(), " � <Esc,Tab> - wyjscie z raportu�", 0, attr_h);
                break;
            case 3:
                term_printf(MY_MAX, X_L0, attr_h,
                            "�<%c,%c,%c,%c>-wybor danej�<F1>-raport �<F3>-menu ",
                            24, 25, 26, arrow);
                if (ok->blok_wpisu == 0) term_type(MY_MAX, m_wherex(), "�<ENT>-edycja �<Esc,Tab>-wyjscie�", 0, attr_h);
                else term_type(MY_MAX, m_wherex(), "� <Esc,Tab> - wyjscie z raportu �", 0, attr_h);
                break;
            case 2:
                term_printf(MY_MAX, X_L0, attr_h,
                            "�<%c,%c,%c,%c,Home,End>-wybor danej �<F1,F3>-menu ",
                            24, 25, 26, arrow);
                if (ok->blok_wpisu == 0) term_type(MY_MAX, m_wherex(), "�<ENT>-edycja �<Esc,Tab>-wyjscie�", 0, attr_h);
                else term_type(MY_MAX, m_wherex(), "� <Esc,Tab> - wyjscie z raportu �", 0, attr_h);
                break;
            case 1:
                term_printf(MY_MAX, X_L0, attr_h,
                            "�<%c,%c,%c,%c,Home,End>-wybor danej �<F1>-raport ",
                            24, 25, 26, arrow);
                if (ok->blok_wpisu == 0) term_type(MY_MAX, m_wherex(), "�<ENT>-edycja �<Esc,Tab>-wyjscie�", 0, attr_h);
                else term_type(MY_MAX, m_wherex(), " � <Esc,Tab> - wyjscie z raportu�", 0, attr_h);
                break;
            case 0:
                term_printf(MY_MAX, X_L0, attr_h, /*TERM_CYAN|INVERSE|TERM_FLUSH,*/
                            "�<%c,%c,%c,%c, Home, End> - wybor danej ",
                            24, 25, 26, arrow);
                if (ok->blok_wpisu == 0)
                    term_type(MY_MAX, m_wherex(), "�<ENT> - edycja danej �<Esc,Tab> - wyjscie�", 0,
                              attr_h);
                else term_type(MY_MAX, m_wherex(), "do prezent.� <Esc,Tab> - wyjscie z raportu�", 0, attr_h);
                break;
            case 16:
                term_printf(MY_MAX, X_L0, attr_h, /*TERM_CYAN|INVERSE|TERM_FLUSH,*/
                            "�<%c,%c,%c,%c,Home,End>-wybor�<F1>-wykr.",
                            24, 25, 26, arrow);
                if (ok->blok_wpisu == 0)
                    term_type(MY_MAX, m_wherex(), "�<ENT> - edycja danej �<Esc,Tab> - wyjscie�", 0,
                              attr_h);
                else term_type(MY_MAX, m_wherex(), "do prezent.� <Esc,Tab> - wyjscie z raportu�", 0, attr_h);
                break;
            default: break;
            }
#endif
            /* klawisze: <UP, DOWN, RIGHT, LEFT, HOME, END, CR, Esc, TAB>  */
            znak = 0;
            for (;;) /* czytanie znaku z klawiatury */
            {
                znak = GET_char();
                znak = znak & 0x00ff;
                if (znak == Esc) /* porzucenie blankietu, do menu   */
                {
                    mod = 0;
                    wp = 0;
                    if (ok->wpis == 1 || ok->blok_wpisu == 1) wp = 2;
                    goto Q;
                }
                if (znak == NLCR) znak = CR /*Get_Char()*/;
                if (znak == CR) /* !!! wpis nowej wartosci */
                {
                    if (ok->blok_wpisu == 1)
                    {
                        /*fprintf(mystderr,"\nTabl=%d ok->help[2]=%d ",Tabl,ok->help[2]); */
                        if (Tabl == 1 && ok->help[2] != NULL)
                        {
                            wp = -2;
                            help = 0;
                            Tabl = 0;
                        }
                        else continue;
                    }
                    else
                    {
                        mod = 2;
                        wp = 1;
                        if (Tabl == 1) wp = -1;
                        help = 0;
                        Tabl = 0;
                    }
#ifndef _DOS_
                    term_printf(MY_MAX, X_L0, attr_h, /*TERM_CYAN|INVERSE|TERM_FLUSH,*/
                                "�  Wpis danej: <Esc> - wyjscie�  <Ent> - akceptacja � <%c, %c> - inkrementacja   �",
                                24, 25);
#else
                    term_printf(MY_MAX, X_L0, attr_h, /*TERM_CYAN|INVERSE|TERM_FLUSH,*/
                                "�  Wpis danej: <Esc> - wyjscie�  <Ent> - akceptacja � <%c, %c> - inkrementacja  �",
                                24, 25);
#endif
                    term_type(MY_MAX, X_L0 + 1, "  Wpis danej ", 0, attr_o[2]);
                    break;
                }
                if (znak == BS) goto W_LEWO;
                if (znak == TAB)
                {
                    mod = 0;
                    wp = 0;
                    if (ok->wpis == 1 || ok->blok_wpisu == 1) wp = 2;
                    goto Q;
                }
                if (znak == EOS || znak == SPEC)
                {
                    znak = GETchar();
                    znak = znak & 0x00ff;
                    if (znak == SHIFT_TAB || znak == CTRL_TAB)
                    {
                        przelacz_potwierdzanie();
                        goto DALEJ;
                    }
                    if (znak == PgUp)
                    {
                        nowa -= 4;
                        znak = UP;
                        goto W_LEWO;
                    }
                    if (znak == PgDn)
                    {
                        nowa += 4;
                        znak = DOWN;
                        goto W_PRAWO;
                    }
                    if (znak == Home)
                    {
                        nowa = -1;
                        znak = DOWN;
                        goto W_PRAWO;
                    }
                    if (znak == End)
                    {
                        nowa = liczba;
                        znak = UP;
                        goto W_LEWO;
                    }
                    if (znak == DOWN)
                    {
                        ok = Ok + nowa;
                        yo = ok->yo;
                        xo = ok->xo; /* przechodz.do nast.linii */
                        do
                        {
                            nowa++;
                            if (nowa >= liczba) nowa -= liczba;
                            ok = Ok + nowa;
                            /*fprintf(mystderr," nowa=%d stara=%d",nowa,stara); */
                        }
                        while ((ok->ochrona > Uprawn && ok->blok_wpisu == 0 || ok->yo == yo) && nowa != stara);
                        if (stara == nowa) break;
                        yo = ok->yo;
                        x = abs(ok->xo - xo);
                        for (ind = nowa + 1; ind < liczba; ind++)
                        {
                            ok = Ok + ind;
                            if (ok->yo != yo) break;
                            if (ok->ochrona > Uprawn && ok->blok_wpisu == 0) continue;
                            if (abs(ok->xo - xo) < x)
                            {
                                x = abs(ok->xo - xo);
                                nowa = ind;
                            }
                            else break;
                        }
                        goto DALEJ;
                    }
                    if (znak == FFR)
                    W_PRAWO:
                        {
                            do
                            {
                                nowa++;
                                if (nowa >= liczba) nowa -= liczba;
                                ok = Ok + nowa;
                                /*fprintf(mystderr," nowa=%d stara=%d",nowa,stara); */
                            }
                            while (ok->ochrona > Uprawn && ok->blok_wpisu == 0 && nowa != stara);
                            goto DALEJ;
                        }
                    if (znak == UP)
                    {
                        ok = Ok + nowa;
                        yo = ok->yo;
                        xo = ok->xo; /* przechodz.do nast.linii */
                        do
                        {
                            nowa--;
                            if (nowa < 0) nowa = liczba + nowa;
                            ok = Ok + nowa;
                            /*fprintf(mystderr," nowa=%d stara=%d",nowa,stara); */
                        }
                        while ((ok->ochrona > Uprawn && ok->blok_wpisu == 0 || ok->yo == yo) && nowa != stara);
                        if (stara == nowa) break;
                        yo = ok->yo;
                        x = abs(ok->xo - xo);
                        for (ind = nowa - 1; ind >= 0; ind--)
                        {
                            ok = Ok + ind;
                            if (ok->yo != yo) break;
                            if (ok->ochrona > Uprawn && ok->blok_wpisu == 0) continue;
                            if (abs(ok->xo - xo) < x)
                            {
                                x = abs(ok->xo - xo);
                                nowa = ind;
                            }
                            else break;
                        }
                        goto DALEJ;
                    }
                    if (znak == BBS)
                    {
                    W_LEWO:
                        do
                        {
                            nowa--;
                            if (nowa < 0) nowa = liczba + nowa;
                            ok = Ok + nowa;
                            /*fprintf(mystderr," nowa=%d stara=%d",nowa,stara); */
                        }
                        while (ok->ochrona > Uprawn && ok->blok_wpisu == 0 && nowa != stara);
                    DALEJ:
                        if (nowa == stara) break; /* bylo continue; */
                        mod = 0;
                        dana = stara;
                        wp = 0;
                        if (ok->wpis == 1 || ok->blok_wpisu == 1) wp = 2;
                        break;
                    }
                    if (help > 0)
                    {
                        if ((help & 0x01) == 1 && znak == FONT1)
                        {
                            *raport = ok->kod_anim;
                            help = 0;
                            BL[nr_blank]->nr_danej = dana;
                            return FONT1;
                        }
                        if (help == 14 || help == 16) /* FONT1 dla grafiki gdy sa dane 'f' lub 'F' lub 'd' */
                        {
                            if ((ok->typ == 'f' || ok->typ == 'F' || ok->typ == 'd') && ok->proxy == 0 && znak == FONT1)
                            {
                                help = 0;
                                BL[nr_blank]->nr_danej = dana;
                                return FONT1;
                            }
                        }
                        if ((help & 0x02) == 2 && (znak == FONT3 || znak == FONT1))
                        {
                            mod = 2;
                            wp = 3;
                            if (ok->blok_wpisu == 1) wp = -3;
#ifndef _DOS_
                            term_printf(MY_MAX, X_L0, attr_h,
                                        "�<%c,%c,%c,%c,Home,End>-wybor opcji  � <Ent> - akceptacja � <Esc> - wyjscie z menu  �",
                                        24, 25, 26, arrow);
#else
                            term_printf(MY_MAX, X_L0, attr_h,
                                        "�<%c,%c,%c,%c,Home,End>-wybor opcji  � <Ent> - akceptacja � <Esc> - wyjscie z menu�",
                                        24, 25, 26, arrow);
#endif
                            term_printf(MY_MAX, X_L0 + 1, attr_o[2], "<%c,%c,%c,%c,Home,End> - wybor opcji",
                                        24, 25, 26, arrow);
                        }
                        if ((help & 0x04) == 4 && (znak == FONT4))
                        {
                            mod = 2;
                            wp = 4; /* if(ok->blok_wpisu==1) wp=-4; ### */
                            if (ok->blok_wpisu == 1) wp = 5; /* wpis w tabl.dodane 27.01.98 */
#ifndef _DOS_
                            term_printf(MY_MAX, X_L0, attr_h,
                                        "�<%c,%c,%c,%c,Home,End> - wybierz do edycji � <Ent> - akceptacja � <Esc> - wyjscie �",
                                        24, 25, 26, arrow);
#else
                            term_printf(MY_MAX, X_L0, attr_h,
                                        "�<%c,%c,%c,%c,Home,End> - wybierz do edycji � <Ent> - akceptacja � <Esc> - wyjscie�",
                                        24, 25, 26, arrow);
#endif
                            if (ok->blok_wpisu == 1)
                                term_type(MY_MAX, X_L0 + 1, " Przeglad danych bez prawa ich edycji  ", 0, attr_o[2]);
                            else
                                term_printf(MY_MAX, X_L0 + 1, attr_o[2], "<%c,%c,%c,%c,Home,End> - wybierz do edycji ",
                                            24, 25, 26, arrow);
                        }
                        znak = CR;
                        help = 0;
                        break;
                    } /* koniec helpow */
                } /* koniec analizy dla znakow specjalnych */
            } /* koniec for() wyboru po podswietleniu danej */
            break;
        case 2: /* po wczytaniu danej */
            wp = 0;
            mod = 1;
            if (ret >= 0)
            {
                ok->wpis = 1;
                wp = 2;
                if (ret > 0)
                {
                    if (ok->proxy > 0)
                        wysylka(dana, ok->kod_anim, *(ok->wartosc),
                                ok->proxy);
                    if (ok->wpis == 0 && ok->obowiazk == 1) l_oblig--;
                    if (ok->format[0] == '#') ok->format[0] = '*';
                    if (ok->format[1] == '#') ok->format[1] = '*';
                    aktualiz_blank(dana, liczba, ok, attr_dat, attr_o, attrtx, attr_inf);
                }
            }
            break;
        default: komunikat(MY_MAX, X_L0,ATTR_A, " !! Cos zle w blankiecie: mod=%d", mod);
            mod = 1;
        } /* koniec switch(mod) */
    }
    while (znak != TAB && znak != Esc); /* przejscie do menu sterujacego */
    *loblig = l_oblig;
    return znak;
}

void aktualiz_blank(int dana, int liczba, struct okno* okd,
                    unsigned int attr_dat[], unsigned int attr_o[],
                    unsigned int attrtx[], unsigned int attr_inf)
{
    int no, wp, y_shift = 0;
    struct okno* ok;
    unsigned int attr, attr_d;
    if (liczba == 1) return;
    /*  if(##tu bedzie scroll */
    /*fprintf(mystderr,"\n%s ",okd->format); */
    for (no = dana + 1; no != dana; no++)
    {
        if (no == liczba) no = 0;
        if (no == dana) break;
        ok = Ok + no;
        if (ok->typ == 'g' || ok->typ == 'G') goto DISP;
        if (okd->typ <= 'a' && okd->typ != 'F') continue; /* bez aktualizacji bo tablica */
        if (okd->wartosc == ok->wmin_opcje) goto DISP;
        if (okd->wartosc == ok->wmax) goto DISP;
        if (okd->wartosc == ok->wartosc) goto DISP;
        /* zabl.17.10.98    if(ok->typ !='w' && ok->typ !='W' && ok->typ!='o' && okd->typ!='F') continue; */
        if ((float*)(ok->help[1]) == okd->wartosc) goto DISP;
        if ((float*)(ok->help[2]) == okd->wartosc) goto DISP;
        if ((int*)(ok->help[2]) == (int*)(okd->help[2])) goto DISP;
        /*    if(ok->help[2])==(char *)okd->wartosc) goto DISP; */
        if ((float*)(ok->help[3]) != okd->wartosc) continue;
    DISP:
        wp = 0;
        attr = attr_o[0];
        attr_d = attr_dat[0];
        if (ok->ochrona > Uprawn)
        {
            attr = attr_inf;
            wp = 2;
        }
        if (ok->blok_wpisu == 1) attr = attr_o[0];
        disp_dana(wp, ok->yo + Y_0, ok->xo + X_0, attr_d, attr, attrtx[0], ok, y_shift, 0);
    }
}

void animuj_blank(int liczba, void* ptrs[])
{
    int no, wp, i, y_shift = 0, xkursor, ykursor, yd, mod;
    int X_akt0, XX_max, Y_akt0, YY_max;
    struct okno *ok, *Ok0;
    unsigned int attr, attr_d, attr_old;
    if (nr_blank < 0) return;
    xkursor = m_wherex();
    ykursor = m_wherey();
    attr_old = attryb;
    setcursor(nocursor);
    X_akt0 = x_akt0;
    XX_max = x_maxOkna;
    Y_akt0 = y_akt0;
    YY_max = y_maxOkna;
    //fprintf(mystderr,"\nX_akt0=%d XX_max=%d Y_akt0=%d YY_max=%d xkursor=%d ykursor=%d", X_akt0,XX_max,Y_akt0,Y_max,xkursor,ykursor);
    Ok0 = BL[nr_blank]->Okbl;
    for (no = 0; no < liczba_danych; no++)
    {
        ok = Ok + no;
        if (ok == NULL) break;
        if (ok->typ == 'k') continue;
        if (ok->wartosc == NULL) break;
        if (ok->Wpis == 1) continue; /* wlasnie jest wpis danej */
        if (abs(ok->Wpis) > 2) continue; /* wlasnie jest obsluga menu */
        if (ok->typ == 'g' || ok->typ == 'G') goto DISP;
        if (ok->typ == 'F') continue; /* bez aktualizacji bo tablica */
        if (ok->typ == 's' && ok->typ == 'S') continue; /* bez aktualizacji bo tablica */
        if (ok->typ == 'z' && ok->typ == 'Z') continue; /* bez aktualizacji bo tablica */
        for (i = 0; i < liczba; i++)
        {
            if (ptrs[i] == NULL) continue;
            if (ptrs[i] == ok->wmin_opcje) goto DISP;
            if (ptrs[i] == ok->wmax) goto DISP;
            if (ptrs[i] == ok->wartosc) goto DISP;
            /* zabl.17.10.98    if(ok->typ !='w' && ok->typ !='W' && ok->typ!='o' && okd->typ!='F') continue; */
            if ((float*)(ok->help[1]) == ptrs[i]) goto DISP;
            if ((float*)(ok->help[2]) == ptrs[i]) goto DISP;
            /*    if(ok->help[2])==(char *)okd->wartosc) goto DISP; */
            if ((float*)(ok->help[3]) != ptrs[i]) continue;
        DISP:
            wp = ok->Wpis;
            mod = ok->mod;
            yd = ok->yo + Y_0;
            if (blok_anim->ym < yd) goto DD; /* ponizej ramki */
            if (blok_anim->yp > yd) goto DD; /* powyzej ramki */
            if (ok->xo + X_0 > blok_anim->xm) goto DD; /* po prawe str.ramki */
            if (ok->xmax < blok_anim->xp) goto DD; /* po lewej str.ramki */
            continue;
        DD:
            attr = attr_o[mod];
            attr_d = attr_dat[mod];
            if (ok->ochrona > Uprawn)
            {
                attr = attr_inf;
                wp = 2;
            }
            if (ok->blok_wpisu == 1) attr = attr_o[0];
            /*			xd=x_akt; yd=y_akt; */
            disp_dana(wp, ok->yo + Y_0, ok->xo + X_0, attr_d, attr, attrtx[mod], ok, y_shift, mod);
            break;
        }
    }
    /*	term_cur(yd,xd); */
    attryb = attr_old;
    setcursor(cursor);
    x_akt0 = X_akt0;
    x_maxOkna = XX_max;
    y_akt0 = Y_akt0;
    y_maxOkna = YY_max;
    //fprintf(mystderr,"\nKoniec: X_akt0=%d XX_max=%d Y_akt0=%d YY_max=%d xkursor=%d ykursor=%d", X_akt0,XX_max,Y_akt0,Y_max,xkursor,ykursor);
    term_cur(ykursor, xkursor); /*m_gotoxy(xkursor,ykursor); */
}

int kod_rap_aktual_blank(void)
{
    if (nr_blank < 0) return -1;
    //	if(nie_animuj==1) return -1;
    return BL[nr_blank]->nr_raportu;
}

int otworz_blankiet(signed char kolor, signed char ramka, int yp,
                    int xp, int ym, int xm, signed char kod_raportu,
                    int nr_rekordu, char czynny)
{
    int i, l, ret, nr_rap, l_danych;
    unsigned int attr, attr_bl, tlo, att, att_d;
    struct okno* ok;
    struct reports* R;
    char* kod_upr[6] = {
        "0 tylko pokaz danych",
        "1 zapis z termin.glown.",
        "2 zapis po podaniu hasla",
        "3 zapis z term.gl.+haslo",
        "4 operator glowny",
        "5 oper.glowny i term.gl."
    };
    char znak, *zakres = "0-4";
    for (i = 0; i < LRAP_BL; i++)
    {
        if (RAP[i] == NULL)
        {
            i = LRAP_BL;
            break;
        }
        if (RAP[i]->kod_raportu == kod_raportu) break;
    }
    if (i == LRAP_BL)
    {
        komunikat(MY_MAX, X_L0, ATTR_A, "Nie zdefiniowany raport %d. <ENT> ", kod_raportu);
        GET_char();
        return -1;
    }
    R = RAP[i];
    nr_rap = i;
    if (xp < X_L0) xp = X_L0;
    if (xm > MXR_MAX) xm = MXR_MAX;
    if (yp < Y_G0) yp = Y_G0;
    if (ym > MYR_MAX) ym = MYR_MAX;
    Uprawn = 0;
    if (R->uprawnienia >= 0)
    {
        l = 4;
        if (NR_OPER == 0) l = 5;
        Uprawn = Uprawn_pocz;
        if (R->uprawnienia < Uprawn_pocz) R->uprawnienia = Uprawn_pocz;
        i = R->uprawnienia;
        if (i > l) i = l;
        Uprawn = i;
        if (i < l)
        {
            znak = '0' + i;
            zakres[0] = znak;
            zakres[2] = '0' + l;
            Kolory(kolor, &attr_bl, &tlo, &att_d, &att, &attr, &att, &att);
            /*  attr=TERM_WHITE|MTERM_HILIGHT|TERM_MAGENTA_BG|TERM_FLUSH; */
            if (czynny != 0)
            {
                if (BEZ_HASLA == 1) Uprawn = 5;
                else
                {
                    Okno(MY_MAX, X_L0, MY_MAX, MXR_MAX, attr_bl | tlo | INVERSE | TERM_FLUSH);
                    clr_Okno();
                    /* fprintf(mystderr,"\n Przed okno_znak() "); */
                    term_type(MY_MAX, X_L0, R->tytul, 0, attr | TERM_FLUSH);
                    ret = okno_znak(1, MY_MAX, 40, attr, attr/*TERM_MAGENTA*/ | TERM_FLUSH,
                                    attr_bl | tlo/*TERM_WHITE*/ | INVERSE | TERM_FLUSH, &znak,
                                    " Podaj kod uprawn [%s] ?? ", zakres, kod_upr, 6);
                    if (ret < 0) return 0;
                    Uprawn = znak - '0';
                }
            }
        }
        if (Uprawn > R->uprawnienia) R->uprawnienia = Uprawn;
    }
    nr_blank++;
    if (nr_blank == LBLANK)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Przekroczony limit blankietow (%d). <Ent>",LBLANK);
        nr_blank--;
        GET_char();
        return -2;
    }
    BL[nr_blank] = (struct blankiet*)Malloc(sizeof(struct blankiet));
    if (BL[nr_blank] == NULL)
    {
        int zn = 0, l = strlen(R->tytul);
        if (l > 30)
        {
            zn = R->tytul[30];
            R->tytul[30] = 0;
        }
        komunikat(MY_MAX, X_L0,ATTR_A, " %s. Brak pamieci dla opisu blankietu %d. <Ent>", R->tytul, nr_blank);
        nr_blank--;
        if (zn != 0) R->tytul[30] = zn;
        GET_char();
        return -1;
    }
    lwmall++;
    l_danych = R->max_l_danych_blank;
    Ok = (struct okno*)Malloc(l_danych * sizeof(struct okno));
    if (Ok == NULL)
    {
        int zn = 0, l = strlen(R->tytul);
        if (l > 30)
        {
            zn = R->tytul[30];
            R->tytul[30] = 0;
        }
        komunikat(MY_MAX, X_L0,ATTR_A, "%s. Brak pamieci dla blankietu %d: <Ent>",
                  R->tytul, nr_blank);
        nr_blank--;
        if (zn != 0) R->tytul[30] = zn;
        GET_char();
        return -1;
    }
    lwmall++;
    Akt_Kolor = kolor;
    ok = Ok;
    for (i = 0; i < l_danych; i++, ok++)
    {
        ok->format = NULL;
        ok->wpis = 0;
        ok->decyzja = -1;
        ok->Rep = NULL;
        ok->Rep_help = NULL;
        ok->index_helpu = NULL;
        ok->size_helpu = 0;
        ok->wartosc = NULL;
        for (l = 0; l < LHELP; l++) ok->help[l] = NULL;
    }
    nr_danej = 0;
    if (ramka == 1)
    {
        xp++;
        yp++;
        xm--;
        ym--;
    }
    X_maxBlank = xm;
    Y_maxBlank = ym;
    X_0 = xp;
    Y_0 = yp;
    /*fprintf(mystderr,"\notworz_blankiet(): X_maxBlank=%d Y_maxBlank=%d X_0=%d Y_0=%d", X_maxBlank,Y_maxBlank,X_0,Y_0); */
    liczba_danych = l_danych;
    BL[nr_blank]->czynny = czynny;
    BL[nr_blank]->Okbl = Ok;
    BL[nr_blank]->Uprawn = Uprawn;
    BL[nr_blank]->liczba_danych = R->max_l_danych_blank;
    BL[nr_blank]->ramka = ramka;
    BL[nr_blank]->anim_pid = R->anim_pid;
    BL[nr_blank]->Kolor = kolor;
    BL[nr_blank]->X_max = X_maxBlank;
    BL[nr_blank]->X_0 = X_0;
    BL[nr_blank]->Y_max = Y_maxBlank;
    BL[nr_blank]->Y_0 = Y_0;
    BL[nr_blank]->nr_raportu = nr_rap;
    BL[nr_blank]->reset = 0;
    BL[nr_blank]->y_shift = 0;
    BL[nr_blank]->y_scroll = -1;
    if (nr_rekordu < R->ob_pocz) nr_rekordu = R->ob_pocz;
    if (nr_rekordu > R->ob_konc) nr_rekordu = R->ob_konc;
    BL[nr_blank]->nr_rekordu = nr_rekordu;
    BL[nr_blank]->rek_danych_blank = nr_rekordu;
    BL[nr_blank]->nr_danej = dana_poczatkowa;
    Czy_Blankiet_otwarty = 1;
    blok_anim->ym = -1;
    /* fprintf(mystderr,"\n Po otworz_blankiet()"); */
    return 1;
}

void Kolory(signed char kolor, unsigned int* attr, unsigned int* tlo,
            unsigned int* at_dane, unsigned int* at_okno,
            unsigned int* at_wpis, unsigned int* at_inf, unsigned int* at_help)
{
    *attr = TERM_WHITE;
    *tlo = TERM_BLACK_BG;
    *at_inf = *attr | MTERM_HILIGHT | TERM_FLUSH;
    *at_wpis = TERM_WHITE | MTERM_HILIGHT | TERM_MAGENTA_BG | TERM_FLUSH;
    *at_dane = TERM_WHITE | MTERM_HILIGHT | TERM_GREEN_BG | TERM_FLUSH;
    /*  *at_okno=TERM_WHITE|MTERM_HILIGHT|TERM_GREEN_BG|TERM_FLUSH; */
    *at_help = TERM_CYAN | INVERSE | TERM_FLUSH;
    *at_okno = *at_help;
    if (kolor == 2)
    {
        *attr = TERM_WHITE;
        *tlo = TERM_BLUE_BG;
        /*    *at_okno=TERM_WHITE|MTERM_HILIGHT|TERM_YELLOW_BG|TERM_FLUSH; */
        *at_inf = TERM_WHITE | MTERM_HILIGHT | TERM_FLUSH | TERM_BLUE_BG;
    }
    if (kolor == 1)
    {
        *attr = TERM_BLUE;
        *tlo = TERM_WHITE_BG;
        *at_wpis = TERM_WHITE | MTERM_HILIGHT | TERM_MAGENTA_BG | TERM_FLUSH;
        /*    *at_okno=TERM_WHITE|MTERM_HILIGHT|TERM_GREEN_BG|TERM_FLUSH; */
        *at_inf = TERM_BLUE | TERM_FLUSH | TERM_WHITE_BG;
    }
}


/*#define WEWY_INCL */
/* ================### Tu zaczyna sie plik wewy5.c ================== */
#ifndef WEWY_INCL
#define _DOS_
#include <stdio.h>
#include <process.h>
#ifdef _DOS_
#include <alloc.h>
#include <dos.h>
#define Malloc farmalloc
#define Free farfree
#include "../blank/intdef.h"
#else
#include <malloc.h>
#define Malloc malloc
#define Free free
#include <sys/qnxterm.h>
#include "../blank/intdef.h"
#endif
#include <conio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#include "wewybl.h"
#define WEW_EXT extern
#include "WEWY1.H"
#endif
/* =================================================================== */
int dana_float_dec(int y, int x, char* monit, float* w_min,
                   float* w_max, float* wart, int size, char precyzja,
                   int ochrona, char kod_animacji, int kod_decyzji)
{
    int ret;
    ret = dana_float(y, x, monit, w_min, w_max, wart, size, precyzja, ochrona,
                     kod_animacji);
    (Ok + nr_danej - 1)->decyzja = kod_decyzji;
    return ret;
}

/* =================================================================== */
int okno_float(signed char wpis, int y, int x0, int size,
               unsigned int attr_d, unsigned int attr,
               unsigned int attrtx, float* wart, char precyzja,
               char* monit_pocz, float w_min, float w_max)
{
    int index = 0;
    return wpis_tabl_float(wpis, y, x0, attr_d, attr, attrtx, &index, (char*)wart,
                           0, 1, size, precyzja, monit_pocz, 0, 0, w_min, w_max,
                           NULL, NULL, 0., -1., -1.);
    /*  return wpis_float(wpis, y, x0, size, attr, attrtx, wart, precyzja,
                     monit_pocz, w_min, w_max, NULL, 0, -1., -1.);
    */
}


int okno_ogr_wzgl(signed char wpis, int y, int x0, int size,
                  unsigned int attr_d, unsigned int attr,
                  unsigned int attrtx, float* wart, char precyzja,
                  char* monit_pocz, float w_min, float w_max,
                  char* format, float baza, float abs_min, float abs_max)
{
    int index = 0;
    return wpis_tabl_float(wpis, y, x0, attr_d, attr, attrtx, &index, (char*)wart,
                           0, 1, size, precyzja, monit_pocz, 0, 0, w_min, w_max,
                           NULL, format, baza, abs_min, abs_max);

    /*  return wpis_float(wpis, y, x0, size, attr, attrtx, wart, precyzja,
                                     monit_pocz, w_min, w_max, format, baza, abs_min, abs_max);
    int wpis_tabl_float(signed char wpis, int y, int x0,
                      unsigned int attr_d, unsigned int attr,
                      unsigned int attrtx, int *index, char *tabl, int dim,
                      int L_kol, int size, char precyzja, char *monit_pocz, int ind_min,
                      int ind_max, float w_min, float w_max, char *format_tabl,
                      char *format_ogr, float baza, float abs_min, float abs_max)
    */
}

/* =================================================================== */
int okno_float_wzgl(signed char wpis, int y, int x0, int size,
                    unsigned int attr_d, unsigned int attr,
                    unsigned int attrtx, float* wart, char precyzja,
                    char* monit_pocz, float w_min, float w_max, float baza)
{
    int index = 0;
    return wpis_tabl_float(wpis, y, x0, attr_d, attr, attrtx, &index, (char*)wart,
                           0, 1, size, precyzja, monit_pocz, 0, 0, w_min, w_max,
                           NULL, NULL, baza, 0., 0.);
    /* {return wpis_float(wpis, y, x0, size, attr, attrtx, wart, precyzja,
                     monit_pocz, w_min, w_max, NULL, baza, -1., -1.);

    */
}

/* =================================================================== */
int okno_tabl_float(signed char wpis, int y, int x0, char* monit_pocz,
                    int ind_min, int ind_max, float w_min, float w_max,
                    unsigned int attr_d, unsigned int attr, unsigned int attrtx,
                    char* format_tabl, int* index, float tabl[],
                    int dim, int L_kol, int size, char prec)
{
    return wpis_tabl_float(wpis, y, x0, attr_d, attr, attrtx, index, (char*)tabl,
                           dim, L_kol, size, prec, monit_pocz, ind_min, ind_max, w_min,
                           w_max, format_tabl, NULL, 0.0, 0., 0.);
}

int okno_tabl_float_wzgl(signed char wpis, int y, int x0, char* monit_pocz,
                         int ind_min, int ind_max, float w_min, float w_max,
                         unsigned int attr_d, unsigned int attr,
                         unsigned int attrtx, char* format_tabl,
                         int* index, float tabl[], int dim, int size, char prec,
                         float baza)
{
    return wpis_tabl_float(wpis, y, x0, attr_d, attr, attrtx, index, (char*)tabl,
                           dim, 1, size, prec, monit_pocz, ind_min, ind_max, w_min, w_max,
                           format_tabl, NULL, baza, -1., -1.);
}

/* =================================================================== */
int okno_double(signed char wpis, int y, int x0, int size,
                unsigned int attr_d, unsigned int attr,
                unsigned int attrtx, double* wart, char precyzja,
                char* monit_pocz, double w_min, double w_max)
{
    double wartn;
    char *bufor, *format = NULL, text[80], monit[80], *form = NULL, ms, *z, *c;
    char zakres[80], *cc, *zz, *buffer, *taknie = "<t/n>? ";
    int ret, xmax, xp, ins, i, k, im = 0, x_konc, zn, xa, txl, iobl, dltn = 0;
    unsigned int attr_t;
    if (bez_potwierdzania == 0) dltn = strlen(taknie);
    wartn = *wart;
    z = szukaj(monit_pocz, '%');
    if (z != NULL)
    {
        c = szukaj(z + 1, '%');
        if (c != NULL)
        {
            c = szukaj(c, 'e');
            if (c == NULL) z = NULL;
            else c++;
        }
        else z = NULL;
    }
    if (z != NULL)
    {
        sprintf(monit, monit_pocz, w_min, w_max);
        for (cc = z, zz = &zakres[3]; cc != c; cc++, zz++) *zz = *cc;
        zakres[0] = '%';
        zakres[1] = 's';
        zakres[2] = '(';
        *zz = ')';
        zz++;
        *zz = '%';
        zz++;
        *zz = 's';
        zz++;
        *zz = EOS;
    }
    else
    {
        sprintf(monit, monit_pocz);
        zakres[0] = EOS;
    }
    if (monit[0] == '#' || monit[0] == '*' || monit[0] == '+') im = 1;
    if (monit[1] == '#' || monit[1] == '*' || monit[1] == '+') im = 2;
    ms = strlen(monit);
    text[0] = EOS;
    for (i = ms - 2; i >= im; i--)
    {
        if (monit[i + 1] == '?' && monit[i] == ' ')
        {
            for (k = i; k <= ms; k++) text[k - i] = monit[k];
            /*monit[i+1]=EOS;*/
            monit[i] = EOS;
            break;
        }
    }
    ms = strlen(monit) - im;
    xp = x0 + ms;
    if (size < 3 + precyzja) size = 3 + precyzja;
    xmax = xp + size - 1;
    for (txl = 0; txl < MXR_MAX; txl++) { if (text[txl] == EOS) break; }
    k = xmax + 4;
    if (wpis != 2) k = xmax + dltn;
    if (k > X_maxBlank)
    {
        if (y <= Y_maxBlank && y < MY_MAX) return 0;
        x0 += (X_maxBlank - k);
        if (x0 < X_L0 && ms >= -x0)
        {
            monit[ms + im - x0] = EOS;
            x0 = X_L0;
        }
        if (x0 < X_L0 && ms < -x0)
        {
            monit[im] = EOS;
            x0 = X_L0;
        }
        xmax = x0 + ms + size - 1;
    }
    ms = strlen(monit);
    format = (char*)Malloc((xmax - x0 + 20) * sizeof(char));
    if (format == NULL) return -1;
    lwmall++;
    form = &format[13];
    format[12] = EOS;
    bufor = &form[ms + 3 * sizeof(char)];
    bufor[0] = EOS;
    sprintf(format, " %d.%de", size, precyzja);
    format[0] = '%';
    iobl = 1;
    if (monit[0] != '#' && monit[1] != '#') iobl = 0;
    if (iobl == 0 || wpis == 2)
        sprintf(bufor, format, *wart);
    Okno(y, x0, y, xmax, attr);
    x_konc = xmax + txl;
    if (wpis != 1)
    {
        term_type(y, x0, &monit[im], 0, attrtx);
        term_type(y, m_wherex(), bufor, 0, attr_d);
        attr_t = attrtx;
        if (iobl == 0 && (wartn < w_min || wartn > w_max))
        {
            attr_t = ATTR_A;
            wpis = 0;
            if (txl > 1) text[1] = '!';
        }
        if (wpis == 0)
        {
            for (i = 0; i < txl; i++)
            {
                if (i + xmax + 1 < X_maxBlank) term_type(y, xmax + 1 + i, &text[i], 1, attr_t);
            }
        }
        else
        {
            for (i = 0; i < txl; i++)
            {
                if (i + xmax + 1 < X_maxBlank) term_type(y, xmax + 1 + i, " ", 1, attrtx);
            }
        }
        x_konc = m_wherex();
    }
    else
    {
        for (i = 0; i < txl; i++)
        {
            if (i + xmax + 1 < X_maxBlank) term_type(y, xmax + 1 + i, &text[i], 1, attrtx);
        }
        x_konc = m_wherex();
        sprintf(form, "%s e", monit);
        form[ms] = '%';
    WPIS:
        ins = INSERT;
        INSERT = 1;
        Okno(y, x0, y, xmax, attr);
        ret = bscanf(bufor, attrtx, form, &wartn);
        INSERT = ins;
        if (ret < 0)
        {
            Free(format);
            lwmall--;
            return -x_konc;
        }
        if (wartn < w_min || wartn > w_max)
        {
            sprintf(text, format, wartn);
            i = strlen(text) + 1;
            if (zakres[0] == EOS)
                sprintf(&text[i], " ZAKRES !! <ENT>-popraw ");
            else
                sprintf(&text[i], zakres, " poza ", w_min, w_max, " <ENT> ");
            i = strlen(text) + 1;
            if (x0 + i > X_maxBlank)
            {
                i = X_maxBlank - x0;
                text[i] = EOS;
            }
            xa = x_konc - i;
            if (xa < x0) xa = x0;
            k = term_save_image(y, xa, text, 0);
            buffer = (char*)Malloc(i * k + 1);
            if (buffer != NULL)
            {
                term_save_image(y, xa, buffer, i);
                lwmall++;
                term_type(y, xa, " ", 1,ATTR_A);
                term_type(y, xa + 1, text, 0,ATTR_A);
            }
            GET_char();
            if (buffer != NULL)
            {
                term_restore_image(y, xa, buffer, i);
                Free(buffer);
                lwmall--;
            }
            wartn = *wart;
            goto WPIS;
        }
        if (ret == 0 || wartn == *wart)
        {
            *wart = wartn;
            Free(format);
            lwmall--;
            return 0;
        }
        term_printf(y, xmax - size + 1, attr, format, wartn);
        if (bez_potwierdzania == 1)
        {
            zn = 't';
            goto TAK;
        }
        if (xmax + dltn > X_maxBlank)
        {
            i = X_maxBlank - xmax;
            taknie[i] = EOS;
        }
        k = term_save_image(y, xmax + 1, taknie, 0);
        do
        {
            buffer = (char*)Malloc(dltn * k + 1);
            if (buffer != NULL)
            {
                term_save_image(y, xmax + 1, buffer, dltn);
                lwmall++;
            }
            term_type(y, xmax + 1, taknie, 0, attrtx);
            i = m_wherex();
            if (i < MXR_MAX) i--;
            i--;
            term_cur(y, i);
            term_flush();
            zn = GET_char();
            zn = zn & 0x00ff;
            if (buffer != NULL)
            {
                term_restore_image(y, xmax + 1, buffer, dltn);
                Free(buffer);
                lwmall--;
            }
            if (zn == 'n') goto WPIS;
            if (zn == 't')
            TAK:
                {
                    *wart = wartn;
                    term_printf(y, xmax - size + 1, attr, format, wartn);
                }
        }
        while (zn != Esc && zn != 't');
    }
    Free(format);
    lwmall--;
    return x_konc;
}

/* =================================================================== */
int okno_tabl_grup(signed char wpis, int y, int x0, char* monit_pocz,
                   int ind_min, int ind_max, int w_min, int w_max,
                   unsigned int attr_d, unsigned int attr, unsigned int attrtx,
                   char* format_tabl, int* index, int* tabl, int dim,
                   int size, int skok, char** help)
{
    int ret;
    ret = wpis_tabl_int(wpis, y, x0, monit_pocz, ind_min, ind_max, w_min, w_max,
                        attr_d, attr, attrtx, 'G', format_tabl, index, (char*)tabl,
                        (char*)tabl, 0, dim, size, skok, NULL, NULL, help);
    return ret;
}

int okno_tabl_int(int wpis, int y, int x0, char* monit_pocz,
                  int ind_min, int ind_max, int w_min, int w_max,
                  unsigned int attr_d, unsigned int attr, unsigned int attrtx,
                  char* format_tabl, int* index, int* tabl, int dim,
                  int size, int l_poz_menu, char** menu)
{
    int ret;
    ret = wpis_tabl_int(wpis, y, x0, monit_pocz, ind_min, ind_max, w_min, w_max,
                        attr_d, attr, attrtx, 'i', format_tabl, index, (char*)tabl, (char*)tabl,
                        0, dim, size, l_poz_menu, NULL, NULL, menu);
    return ret;
}


int okno_grupy_posr(signed char wpis, int y, int x0, int size,
                    unsigned int attr_d, unsigned int attr,
                    unsigned int attrtx, int* wart, char* index_helpu,
                    int size_helpu, char* monit_pocz,
                    int w_min, int w_max, int skok, void* R, void* Rh, char** help)
{
    int ret;
    static int index = 0;
    ret = wpis_tabl_int(wpis, y, x0, monit_pocz, 0, 0, w_min, w_max,
                        attr_d, attr, attrtx, 'g', NULL, &index, (char*)wart,
                        index_helpu, size_helpu, 0, size, skok, R, Rh, help);
    return ret;
}


int okno_grupy(signed char wpis, int y, int x0, int size,
               unsigned int attr_d, unsigned int attr,
               unsigned int attrtx, int* wart, char* monit_pocz,
               int w_min, int w_max, int skok, void* R, char** help)
{
    int ret;
    static int index = 0;
    ret = wpis_tabl_int(wpis, y, x0, monit_pocz, 0, 0, w_min, w_max,
                        attr_d, attr, attrtx, 'g', NULL, &index, (char*)wart,
                        (char*)wart, 0, 0, size, skok, R, NULL, help);
    return ret;
}

int okno_int_menu(signed char wpis, int y, int x0, int size,
                  unsigned int attr_d, unsigned int attr,
                  unsigned int attrtx, int* wart, char* monit_pocz,
                  int w_min, int w_max, int l_poz_menu, char** menu)
{
    int ret;
    static int index = 0;
    ret = wpis_tabl_int(wpis, y, x0, monit_pocz, 0, 0, w_min, w_max,
                        attr_d, attr, attrtx, 'i', NULL, &index, (char*)wart,
                        (char*)wart, 0, 0, size, l_poz_menu, NULL, NULL, menu);
    return ret;
}

int okno_int(signed char wpis, int y, int x0, int size,
             unsigned int attr_d, unsigned int attr,
             unsigned int attrtx, int* wart, char* monit_pocz,
             int w_min, int w_max)
{
    static int index = 0;
    return wpis_tabl_int(wpis, y, x0, monit_pocz, 0, 0, w_min, w_max,
                         attr_d, attr, attrtx, 'i', NULL, &index, (char*)wart,
                         (char*)wart, 0, 0, size, 0, NULL, NULL, NULL);
    /*  return wpis_int(wpis, y, x0, size, attr, attrtx, 'i', wart, monit_pocz,
                   w_min, w_max, sizeof(int), NULL);
    */
}

int wpis_tabl_int(signed char wpis, int y, int x0, char* monit_pocz,
                  int ind_min, int ind_max, int w_min, int w_max,
                  unsigned int attr_d, unsigned int attr,
                  unsigned int attrtx, char typ,
                  char* format_tabl, int* index, char* tabl,
                  char* index_helpu, int size_helpu, int dim,
                  int size, int skok, void* R, void* Rh, char** help)
{
    int wartn, *wart, ind_size, dl_zakr, W_min, W_min_wpis, index_h, W_max;
    char *bufor, *format = NULL, text[80], *form = NULL, *c, grupa;
    char *buffer, *taknie = "<t/n>? ", hash = '*';
    char opis_danej[80], f_nazwy[40], *zle = " ?? ", *menu;
    char *f_ind = NULL, *fk_nazwy = NULL;
    char *fmon, *f_pocz, err = 0;
    int xmax, xp, xp_ind, ret, nr_zm, xp_naz, ins, i, k, x_konc, txl;
    int iobl, dltn = 0, dx_ind = 0, x_k_help, zn, lsmax;
    unsigned int attr_t;
    char blok = 0;
    if (wpis < -2)
    {
        blok = 1;
        wpis = -wpis;
    }
    W_min = w_min;
    W_min_wpis = w_min;
    grupa = 0;
    /* tak bylo do 21.08.98        if(typ=='g' || typ=='G') grupa=1; */
    /* ujemna tylko dla G          if(typ=='G') {W_min=-9999; typ='g';} */
    if (typ == 'g' || typ == 'G')
    {
        grupa = 1;
        W_min = -9999;
        typ = 'g';
        if (w_min < 0)
        {
            W_min_wpis = w_min + 1;
            w_min = 0;
        }
        /* dla tabl.grup wart. ujemna moze byc wpisana W_min<0*/
    }
    if (typ == 'I') typ = 'i';
    wart = (int*)tabl;
    fmon = formaty_tabl(f_nazwy, monit_pocz, &ind_min, &ind_max, index,
                        &ind_size, &hash, format_tabl, opis_danej, &dl_zakr, &f_ind,
                        &fk_nazwy, &dx_ind);
    if (fmon != NULL)
    {
        c = szukaj_formatow(fmon, 'd', &f_pocz);
        if (c == NULL)
            sprintf(&opis_danej[dl_zakr], fmon);
        else
            sprintf(&opis_danej[dl_zakr], fmon, w_min, w_max);
    }
    lsmax = 0;
    xmax = wydruk_monitu(wpis, opis_danej, dl_zakr, taknie, &dltn, text, &x0,
                         dx_ind, &lsmax, &size, y, &xp_naz, &xp, &txl, attrtx, w_min,
                         w_max, typ, skok, R, Rh, help);
    if (xmax == 0) return 0;
    format = (char*)Malloc((size + 15) * sizeof(char));
    if (format == NULL) return -1;
    lwmall++;
    if (size > 0)
    {
        sprintf(format, " %dd", size);
        format[0] = '%';
    }
    else format[0] = 0;
    form = &format[6];
    bufor = &form[6 * sizeof(char)];
    bufor[0] = EOS;
    iobl = 1;
    if (hash != '#') iobl = 0;
    wart = (int*)(tabl + (*index) * dim);
    wartn = *wart;
    if (grupa > 0)
    {
        if (wartn < W_min) wartn = W_min;
        if (wartn > w_max) wartn = w_max;
    }
    /*   if(help!=0 && skok>0)
        {if(wpis==0 && wartn>skok) {wartn=skok-1; *wart=wartn;}}
    */
    if (iobl == 0 || wpis == 2 && size > 0)
        sprintf(bufor, format, wartn);
    xp_ind = xp;
    if (ind_size > 0)
    {
        xp_ind = wydruk_indexu(y, xp_naz, attr, attrtx, f_nazwy, f_ind, *index, fk_nazwy);
        Okno(y, xp_ind, y, xmax, attr);
        ret = xp_ind;
    }
    if (abs(wpis) != 1 && wpis != -2)
    {
        if (size > 0)
        {
            term_type(y, xp, bufor, 0, attr_d);
            x_k_help = m_wherex();
        }
        else x_k_help = xp;
        x_konc = x_k_help;
        if (help != NULL)
        {
            if (grupa > 0)
            {
                index_h = wartn;
                W_max = w_max;
                err = 0;
                if (size_helpu > 0)
                {
                    long int skok_sum;
                    skok_sum = ustal_skok_adr(Rh, wartn, size_helpu, &err);
                    if (err == 0)
                        index_h = *(int*)((unsigned long int)index_helpu + skok_sum);
                    W_max = 9999;
                }
                if (err > 0)
                {
                    term_printf(y, xmax + 1, attrtx, "%d. Brak danych !! ", wartn);
                    x_k_help = m_wherex();
                }
                else
                {
                    x_k_help = objasn_grupy(y, xmax + 1, w_min, W_max, index_h, attr,
                                            attrtx, skok, dltn, R, help);
                }
            }
            else
            {
                menu = zle;
                if (wartn < skok && wartn >= 0) menu = help[wartn];
                term_type(y, xmax + 1, " ", 1, attrtx);
                ret = Term_Typef(y, m_wherex(), menu, lsmax, attr);
                x_k_help = m_wherex();
            }
            x_konc = x_k_help;
            if (x_k_help < 0)
            {
                for (i = x0; i < X_maxBlank; i++) term_type(y, x_konc, " ", 1, attrtx);
                return 0;
            }
        }
        /*     x_konc=m_wherex(); */
        attr_t = attrtx;
        if (iobl == 0 && (wartn < W_min || wartn > w_max))
        {
            attr_t = ATTR_A;
            wpis = 0;
            if (txl > 1) text[1] = '!';
        }
        if (wpis == 0)
        {
            for (i = 0; i < txl; i++)
            {
                if (i + x_konc <= X_maxBlank) term_type(y, x_konc + i, &text[i], 1, attr_t);
            }
            /*fprintf(mystderr, " po wpis==0 txl=%d",txl); */
        }
        else
        {
            for (i = 0; i < txl; i++)
            {
                if (i + x_konc <= X_maxBlank) term_type(y, x_konc + i, " ", 1, attrtx);
            }
            /*fprintf(mystderr, " po wpis!=0 txl=%d",txl); */
        }
        x_konc = m_wherex();
        /*fprintf(mystderr,"\ngrupa: wpis=%d y=%d/%d ind_size=%d help=%d",wpis,y,MY_MAX,ind_size,help); */
        /* ========================= dla tablicy ========================== */
        if (wpis > 2)
        {
            if (y < MY_MAX)
            {
                if (wpis == 3 && help != NULL)
                {
                    ret = okno_help(y + 1, xp - 1, x_k_help, attrtx, attr, typ, w_min, w_max,
                                    w_min, w_max, index_helpu, size_helpu, size, skok, R, Rh, help,
                                    0, NULL, 0, NULL, NULL, NULL);
                    if (blok == 1) ret = -1;
                    if (ret >= 0)
                    {
                        if ((ret + w_min) == wartn) x_konc = 0;
                        else *wart = ret + w_min;
                    }
                    else x_konc = -1;
                }
                if (wpis >= 4 && ind_size > 0)
                {
                    ret = okno_help(y + 1, xp_naz - 1, x_k_help, attrtx, attr, typ, ind_min,
                                    ind_max, w_min, w_max, index_helpu, size_helpu,
                                    size, skok, R, Rh, help, 0, tabl, dim, f_nazwy, f_ind, fk_nazwy);
                    if (ret >= 0)
                    {
                        if ((ret + ind_min) == (*index)) x_konc = 0;
                        else *index = ret + ind_min;
                    }
                    else x_konc = -1;
                }
            }
            wpis = 1;
        }
        Free(format);
        HELP = 0;
        lwmall--;
        /*fprintf(mystderr, " po Free(%d) x_konc=%d",format,x_konc); */
        return x_konc;
    } /* koniec dla wpis!=1 */
    /* ----------------------------------------------------------------- */
    else /* obsluga dla wpis=1 */
    {
        x_konc = xmax + 1;
        HELP = 0;
        if (help != NULL)
        {
            if (grupa > 0)
            {
                index_h = wartn;
                W_max = w_max;
                err = 0;
                if (size_helpu > 0)
                {
                    long int skok_sum;
                    skok_sum = ustal_skok_adr(Rh, wartn, size_helpu, &err);
                    if (err == 0)
                        index_h = *(int*)((unsigned long int)index_helpu + skok_sum);
                    W_max = 9999;
                }
                if (err > 0)
                {
                    term_printf(y, xmax + 1, attrtx, "%d. Brak danych !! ", wartn);
                    x_k_help = m_wherex();
                }
                else
                    x_k_help = objasn_grupy(y, xmax + 1, w_min, W_max, index_h, attr,
                                            attrtx, skok, dltn, R, help);
            }
            else
            {
                menu = zle;
                if (wartn < skok && wartn >= 0) menu = help[wartn];
                term_type(y, xmax + 1, " ", 1, attrtx);
                /*         for(i=0;i<skok;i++)
                                    {ls=strlen(help[i]); if(ls>lsmax) lsmax=ls;}
                */
                ret = Term_Typef(y, m_wherex(), menu, lsmax, attr);
            }
            HELP = 1;
            x_konc = m_wherex();
            if (x_konc + dltn > X_maxBlank) x_konc = X_maxBlank - dltn;
        }
        for (i = 0; i < txl; i++)
        {
            if (i + x_konc <= X_maxBlank) term_type(y, x_konc + i, &text[i], 1, attrtx);
        }
        sprintf(form, "%c d", hash);
        form[1] = '%';
        term_printf(y, xp, attr, format, wartn);
        /* -------------------- Obsluga wpisu indeksu ---------------------- */
        if (ind_size > 0 && wpis == -1)
        {
            i = *index;
            ret = wpis_indexu(y, xp_ind, ind_size, index, index_helpu, size_helpu,
                              ind_min, ind_max, tabl, dim, xp, xmax, f_ind, format, attrtx,
                              attr, skok, R, Rh, help, typ, dltn, w_min, w_max, lsmax);
            if (ret < 0)
            {
                *index = i;
                Free(format);
                HELP = 0;
                lwmall--;
                /*         term_printf(y,xp_ind,attr,i); term_printf(y,xp,attr,*wart); */
                return -1;
            }
            if (*index != i)
            {
                wart = (int*)(tabl + (*index) * dim);
                wartn = *wart;
                if (iobl == 0)
                    sprintf(bufor, format, wartn);
            }
            wpis = 1;
            if (help != NULL) HELP = 1;
            else HELP = 0;
        }
        /* ----------------------------------------------------------------- */
    WPIS:
        do
        {
            ins = INSERT;
            INSERT = 1;
            Okno(y, xp, y, xmax, attr);
            nr_zm = wartn;
            ret = bscanf(bufor, attrtx, form, &wartn);
            INSERT = ins;
            if (ret < 0)
            {
                Free(format);
                HELP = 0;
                lwmall--;
                /*         if(wartn<W_min) *wart=W_min;
                         if(wartn>w_max) *wart=w_max;   */
                return -x_konc;
            }
            if (wartn < W_min_wpis || wartn > w_max)
            {
                wartn = nr_zm;
                sprintf(bufor, format, wartn);
                if (HELP == 2) HELP = 1;
                goto WPIS;
            }
            if (help != NULL)
            {
                if (grupa > 0)
                {
                    index_h = wartn;
                    W_max = w_max;
                    err = 0;
                    if (size_helpu > 0)
                    {
                        long int skok_sum;
                        skok_sum = ustal_skok_adr(Rh, wartn, size_helpu, &err);
                        if (err == 0)
                            index_h = *(int*)((unsigned long int)index_helpu + skok_sum);
                        W_max = 9999;
                    }
                    if (err > 0)
                    {
                        term_printf(y, xmax + 1, attrtx, "%d. Brak danych !! ", wartn);
                        x_k_help = m_wherex();
                    }
                    else
                        x_k_help = objasn_grupy(y, xmax + 1, w_min, W_max, index_h, attr,
                                                attrtx, skok, dltn, R, help);
                }
                else
                {
                    menu = zle;
                    if (wartn < skok && wartn >= 0) menu = help[wartn];
                    term_type(y, xmax + 1, " ", 1, attrtx);
                    /*           for(i=0;i<skok;i++)
                                {ls=strlen(help[i]); if(ls>lsmax) lsmax=ls;}
                    */
                    ret = Term_Typef(y, m_wherex(), menu, lsmax, attr);
                }
                if (HELP == 2)
                {
                    HELP = 1;
                    continue;
                }
                else break;
            }
        }
        while (HELP != 0);
        /*     if(wartn<w_min || wartn>w_max)
                    {wartn=nr_zm; sprintf(bufor,format,wartn); goto WPIS;}
        */
        if (wartn == *wart)
        {
            *wart = wartn;
            Free(format);
            HELP = 0;
            lwmall--;
            return 0; /*x_konc;*/
        }
        term_printf(y, xmax - size + 1, attr, format, wartn);
        if (bez_potwierdzania == 1)
        {
            zn = 't';
            goto TAK;
        }
        k = term_save_image(y, x_konc, taknie, 0);
        do
        {
            buffer = (char*)Malloc(dltn * k + 1);
            if (buffer != NULL)
            {
                term_save_image(y, x_konc, buffer, dltn);
                lwmall++;
            }
            term_type(y, x_konc, taknie, 0, attrtx);
            i = m_wherex();
            if (i < MXR_MAX) i--;
            i--;
            term_cur(y, i);
            term_flush();
            zn = GET_char();
            zn = zn & 0x00ff;
            if (buffer != NULL)
            {
                term_restore_image(y, x_konc, buffer, dltn);
                Free(buffer);
                lwmall--;
            }
            if (zn == 'n') goto WPIS;
            if (zn == 't')
            TAK:
                *wart = wartn;
        }
        while (zn != Esc && zn != 't');
    }
    Free(format);
    HELP = 0;
    lwmall--;
    return x_konc;
}


int wpis_koment(int yo, int xo, unsigned int attrtx, char* text)
{
    int xmax, dl;
    dl = strlen(text);
    xmax = xo + dl;
    if (xmax > X_maxBlank)
    {
        if (yo <= Y_maxBlank) return 0;
        xo += (X_maxBlank - xmax);
        xmax = X_maxBlank;
        if (xo < X_L0)
        {
            xo = X_L0;
            dl = xmax - xo;
        }
    }
    Term_Typef(yo, xo, text, dl, attrtx);
    return xmax;
}

/************************************************************************/
int disp_dana(signed char wp, int y0, int x,
              unsigned int attr_D, unsigned int attr_o,
              unsigned int attrtx, struct okno* ok, int y_shift, char mod)
{
    int ret, y, *ind;
    unsigned int attr_d = attr_D;
    if (ok->ochrona > 5) attr_d = attr_o;
    wpisano_dana = 0;
    y = y0;
    /* if(y>=BL[nr_blank]->y_scroll)
            {y-=y_shift; if(y<BL[nr_blank]->y_scroll) return ???;}
    */
    ok->Wpis = wp;
    ok->mod = mod;
    switch (ok->typ)
    {
    case 'f':
        ret = wpis_tabl_float(wp, y, x, attr_d, attr_o, attrtx, &(ok->index),
                              (char*)(ok->wartosc), 0, 1, ok->size, ok->prec,
                              ok->format, 0, 0, *(ok->wmin_opcje), *(ok->wmax),
                              NULL, NULL, 0., 0., 0.);

        break;
    case 'w':
        ret = wpis_tabl_float(wp, y, x, attr_d, attr_o, attrtx, &(ok->index),
                              (char*)(ok->wartosc), 0, 1, ok->size, ok->prec,
                              ok->format, 0, 0, *(ok->wmin_opcje), *(ok->wmax),
                              NULL, NULL, *(float*)ok->help[1], 0., 0.);
        break;
    case 'F':
        if (ok->help[2] != NULL) { ind = (int*)ok->help[2]; } /* attr_d=attr_D;} */
        else ind = &(ok->index);
        ret = wpis_tabl_float(wp, y, x, attr_d, attr_o, attrtx, ind,
                              (char*)(ok->wartosc), ok->dim, ok->L_kol, ok->size,
                              ok->prec, ok->format, ok->ind_min, ok->ind_max,
                              *(ok->wmin_opcje), *(ok->wmax), ok->format_tabl,
                              NULL, 0.0, 0., 0.);
        break;
    case 'W':
        ret = wpis_tabl_float(wp, y, x, attr_d, attr_o, attrtx, &ok->index,
                              (char*)(ok->wartosc), ok->dim, 1, ok->size, ok->prec,
                              ok->format, ok->ind_min, ok->ind_max, *(ok->wmin_opcje),
                              *(ok->wmax), ok->format_tabl, NULL, *(float*)ok->help[1],
                              -1., -1.);
        break;
    case 'o':
        ret = wpis_tabl_float(wp, y, x, attr_d, attr_o, attrtx, &(ok->index),
                              (char*)(ok->wartosc), 0, 1, ok->size, ok->prec,
                              ok->format, 0, 0, *(ok->wmin_opcje), *(ok->wmax),
                              NULL, ok->help[0], *(float*)ok->help[1],
                              *(float*)ok->help[2], *(float*)ok->help[3]);
        break;
    case 'e':
        ret = okno_double(wp, y, x, ok->size, attr_d, attr_o, attrtx, (double*)ok->wartosc,
                          ok->prec, ok->format, *((double*)(ok->wmin_opcje)),
                          *((double*)(ok->wmax)));
        break;
    case 'i':
        /*int wpis_tabl_int(signed char wpis, int y, int x0, char *monit_pocz,
                                                int ind_min, int ind_max, int w_min, int w_max,
                                                unsigned int attr, unsigned int attrtx, char typ,
                                                char *format_tabl, int *index, char *tabl, int dim,
                                                int size, int skok, char **help); */
        ret = wpis_tabl_int(wp, y, x, ok->format, 0, 0,
                            *(int*)(ok->wmin_opcje), *(int*)(ok->wmax), attr_d, attr_o,
                            attrtx, 'i', NULL, &ok->index, (char*)(ok->wartosc),
                            (char*)(ok->wartosc), 0, 0,
                            ok->size, /*ok->size,*/ ok->prec, NULL, NULL, (char**)ok->help[0]);
        break;
    case 'I':
        ret = wpis_tabl_int(wp, y, x, ok->format, ok->ind_min, ok->ind_max,
                            *(int*)(ok->wmin_opcje), *(int*)(ok->wmax), attr_d, attr_o,
                            attrtx, 'I', ok->format_tabl, &ok->index,
                            (char*)(ok->wartosc), (char*)(ok->wartosc), 0,
                            ok->dim, ok->size, ok->prec, NULL, NULL, (char**)ok->help[0]);
        break;
    case 'z':
        ret = okno_tabl_znak(wp, y, x, attr_d, attr_o, attrtx, &(ok->index),
                             (char*)(ok->wartosc), 0, ok->format, 0, 0,
                             (char*)(ok->wmin_opcje), NULL, (char**)ok->wmax,
                             ok->prec);
        break;
    case 'Z':
        ret = okno_tabl_znak(wp, y, x, attr_d, attr_o, attrtx, &(ok->index),
                             (char*)(ok->wartosc), ok->dim, ok->format, ok->ind_min,
                             ok->ind_max, (char*)(ok->wmin_opcje),
                             ok->format_tabl, (char**)ok->wmax, ok->prec);
        break;
    case 's':
        ret = okno_text_menu(wp, y, x, ok->size, attr_d, attr_o, attrtx, (char*)ok->wartosc,
                             ok->format, ok->Rep, (char**)ok->help, ok->prec, ok->ind_min);
        break;
    case 'g':
        ret = wpis_tabl_int(wp, y, x, ok->format, 0, 0,
                            *(int*)(ok->wmin_opcje), *(int*)(ok->wmax), attr_d, attr_o,
                            attrtx, 'g', NULL, &ok->index, (char*)(ok->wartosc),
                            (char*)(ok->index_helpu), ok->size_helpu,
                            0, ok->size, ok->prec, ok->Rep, ok->Rep_help, ok->help);
        break;
    case 'G':
        ret = wpis_tabl_int(wp, y, x, ok->format, ok->ind_min, ok->ind_max,
                            *(int*)(ok->wmin_opcje), *(int*)(ok->wmax), attr_d, attr_o,
                            attrtx, 'G', ok->format_tabl, &ok->index, (char*)(ok->wartosc),
                            (char*)(ok->index_helpu), ok->size_helpu,
                            ok->dim, ok->size, ok->prec, ok->Rep, ok->Rep_help, ok->help);
        break;
    case 'k':
        ret = wpis_koment(y, x, attrtx, ok->format);
        break;
    default:
        komunikat(MY_MAX - 1, X_L0,ATTR_A, " Bledny typ okna danej: ok=%d form=%c%c%c%c.... wp=%d y=%d typ= (%c) <Ent>",
                  ok, ok->format[1], ok->format[2], ok->format[3],
                  ok->format[4], wp, y, ok->typ);
        GET_char();
        ret = -1;
    }
    return ret;
}

/* =================================================================== */
int dana_float(int y, int x, char* monit, float* w_min, float* w_max,
               float* wart, int size, char precyzja, int ochrona,
               char kod_animacji)
{
    return def_float(y, x, monit, 0, 0, w_min, w_max, NULL,
                     NULL, wart, 0, 1, size, precyzja, ochrona, kod_animacji, NULL,
                     NULL, NULL, NULL);
}

int dana_float_wzgl(int y, int x, char* monit, float* w_min,
                    float* w_max, float* wart, int size, char precyzja,
                    int ochrona, char kod_animacji, float* baza)
{
    return def_float(y, x, monit, 0, 0, w_min, w_max, NULL,
                     NULL, wart, 0, 1, size, precyzja, ochrona, kod_animacji, baza,
                     NULL, NULL, NULL);
}

int dana_float_wzgl_dec(int y, int x, char* monit, float* w_min,
                        float* w_max, float* wart, int size, char precyzja,
                        int ochrona, char kod_animacji, float* baza, int kod_decyzji)
{
    int ret;
    ret = def_float(y, x, monit, 0, 0, w_min, w_max, NULL,
                    NULL, wart, 0, 1, size, precyzja, ochrona, kod_animacji, baza,
                    NULL, NULL, NULL);
    (Ok + nr_danej - 1)->decyzja = kod_decyzji;
    return ret;
}

int tabl_float(int y, int x, char* monit, int ind_min, int ind_max,
               float* w_min, float* w_max, char* format_tabl,
               float* tablica, int dim, int L_kol, int size,
               int precyzja, int ochrona, char kod_animacji)
{
    int ret;
    ret = def_float(y, x, monit, ind_min, ind_max, w_min, w_max, format_tabl,
                    NULL, tablica, dim, L_kol, size, precyzja, ochrona, kod_animacji, NULL,
                    NULL, NULL, NULL);

    return ret;
}

int Tabl_Floatg(int y, int x, char* monit, int ind_min, int ind_max,
                float* w_min, float* w_max, char* format_tabl,
                int* index, float* tablica, int dim, int L_kol, int size,
                int precyzja, int ochrona, char kod_animacji)
{
    int ret;
    ret = def_float(y, x, monit, ind_min, ind_max, w_min, w_max, format_tabl,
                    index, tablica, dim, L_kol, size, precyzja, ochrona,
                    kod_animacji, NULL, NULL, NULL, NULL);

    return ret;
}

int tabl_float_dec(int y, int x, char* monit, int ind_min, int ind_max,
                   float* w_min, float* w_max, char* format_tabl,
                   float* tablica, int dim, int L_kol, int size,
                   int precyzja, int ochrona, char kod_animacji, int kod_decyzji)
{
    int ret;
    ret = def_float(y, x, monit, ind_min, ind_max, w_min, w_max, format_tabl,
                    NULL, tablica, dim, L_kol, size, precyzja, ochrona, kod_animacji, NULL,
                    NULL, NULL, NULL);
    (Ok + nr_danej - 1)->decyzja = kod_decyzji;
    return ret;
}


int tabl_float_wzgl(int y, int x, char* monit, int ind_min, int ind_max,
                    float* w_min, float* w_max, char* format_tabl,
                    float* tablica, int dim, int size, char precyzja,
                    int ochrona, char kod_animacji, float* baza)
{
    int ret;
    ret = def_float(y, x, monit, ind_min, ind_max, w_min, w_max, format_tabl,
                    NULL, tablica, dim, 1, size, precyzja, ochrona, kod_animacji, baza,
                    NULL, NULL, NULL);
    return ret;
}

int def_float(int y, int x, char* monit, int ind_min, int ind_max,
              float* w_min, float* w_max, char* format_tabl, int* index,
              float* tablica, int dim, int L_kol, int size, char precyzja,
              int ochrona, char kod_animacji, float* baza, char* format,
              float* abs_min, float* abs_max)
{
    struct okno* ok;
    if (Ok == NULL)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Brak adresu dla danej %d blankietu. <Ent> ",
                  nr_danej);
        GET_char();
        return -1;
    }
    if (nr_danej >= liczba_danych)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Za duzo danych w blankiecie: nr_danej=%d l_danych=%d. <Ent> ",
                  nr_danej, liczba_danych);
        GET_char();
        return -1;
    }
    ok = Ok + nr_danej;
    nr_danej++;
    ok->format = wpisz_format(ok->format, monit);
    if (ok->format == NULL) return -1;
    ok->help[0] = wpisz_format(ok->help[0], format);
    ok->ind_min = ind_min;
    ok->L_kol = L_kol;
    ok->decyzja = -1;
    ok->ind_max = ind_max;
    if (ind_min < ind_max && ind_min >= 0)
    {
        ok->typ = 'F';
        if (baza != NULL)
        {
            ok->typ = 'W';
            ok->help[1] = (char*)baza;
        }
        ok->index = 0;
        ok->format_tabl = format_tabl;
        ok->dim = dim;
        ok->help[2] = (char*)index;
    }
    else
    {
        ok->typ = 'f';
        if (format != NULL)
        {
            if (ok->help[0] == NULL) return -1;
            if (baza != NULL)
            {
                ok->typ = 'w';
                ok->help[1] = (char*)baza;
            }
            if (abs_min != NULL && abs_max != NULL)
            {
                ok->typ = 'o';
                ok->help[2] = (char*)abs_min;
                ok->help[3] = (char*)abs_max;
            }
            else ok->help[2] = NULL;
        }
        else
        {
            if (baza != NULL)
            {
                ok->typ = 'w';
                ok->help[1] = (char*)baza;
            }
        }
    }
    ok->wartosc = (float*)tablica;
    ok->kod_anim = kod_animacji;
    ok->size = size;
    ok->prec = precyzja;
    ok->wmin_opcje = (float*)w_min;
    ok->wmax = (float*)w_max;
    if (kod_animacji > 0) ok->proxy = BL[nr_blank]->anim_pid;
    if (kod_animacji == 0) ok->proxy = 0; /* grafika !!! */
    ok->yo = y;
    ok->xo = x;
    ok->obowiazk = 0;
    if (monit[0] == '#' || monit[1] == '#') ok->obowiazk = 1;
    ok->yplus = 0;
    if (monit[0] == '+' || monit[1] == '+') ok->yplus = 1;
    ok->blok_wpisu = 0;
    if (ochrona == -1) ok->blok_wpisu = 1;
    ok->ochrona = ochrona;
    if (ochrona < 0 || BL[nr_blank]->czynny == 0) ok->ochrona = Upr_max;
    return nr_danej;
}

int dana_ogr_wzgl_dec(int y, int x, char* monit, float* w_min, float* w_max,
                      float* wart, int size, char precyzja, int ochrona,
                      char kod_animacji, char* format, float* baza,
                      float* abs_min, float* abs_max, int kod_decyzji)
{
    int ret;
    ret = def_float(y, x, monit, 0, 0, w_min, w_max, NULL, NULL, wart, 0, 1, size, precyzja,
                    ochrona, kod_animacji, baza, format, abs_min, abs_max);
    (Ok + nr_danej - 1)->decyzja = kod_decyzji;
    return ret;
}

int dana_ogr_wzgl(int y, int x, char* monit, float* w_min, float* w_max,
                  float* wart, int size, char precyzja, int ochrona,
                  char kod_animacji, char* format, float* baza,
                  float* abs_min, float* abs_max)
{
    int ret;
    ret = def_float(y, x, monit, 0, 0, w_min, w_max, NULL, NULL, wart, 0, 1, size, precyzja,
                    ochrona, kod_animacji, baza, format, abs_min, abs_max);
    return ret;
}

/* ----------------------------------- pominiete ---------------------
 {struct okno *ok;
  int i, l;
   if(Ok==NULL)
		{komunikat(MY_MAX,X_L0,ATTR_A," Brak adresu dla danej %d blankietu. <Ent> ",
           nr_danej);
     GET_char(); return -1;
    }
   if(nr_danej>=liczba_danych)
		{komunikat(MY_MAX,X_L0,ATTR_A," Za duzo danych w blankiecie: nr_danej=%d l_danych=%d. <Ent> ",
           nr_danej,liczba_danych);
		 GET_char(); return -1;
    }
	 ok=Ok+nr_danej; nr_danej++;
   ok->format=wpisz_format(ok->format, monit); if(ok->format==NULL) return -1;
   ok->help[0]=wpisz_format(ok->help[0], format);
   ok->typ='f';
    if(format !=NULL)
     {if(ok->help[0]==NULL) return -1;
      ok->typ='o';
      ok->help[1]=(char *)baza;
      ok->help[2]=(char *)abs_min;
      ok->help[3]=(char *)abs_max;
     }
		else
     {if(baza!=NULL) {ok->typ='w'; ok->help[1]=(char *)baza;}}
		ok->wartosc=(float *)wart;
    ok->kod_anim=kod_animacji;
    ok->size=size;
    ok->prec=precyzja;
    ok->wmin_opcje=(float *)w_min;
    ok->wmax=(float *)w_max;
    if(kod_animacji>0) ok->proxy=BL[nr_blank]->anim_pid;
    ok->yo=y;
    ok->xo=x;
    ok->obowiazk=0;
    if(monit[0]=='#' || monit[1]=='#') ok->obowiazk=1;
		ok->yplus=0;
    if(monit[0]=='+' || monit[1]=='+') ok->yplus=1;
    ok->blok_wpisu=0; if(ochrona==-1) ok->blok_wpisu=1;
    ok->ochrona=ochrona;
    if(ochrona<0 || BL[nr_blank]->czynny==0) ok->ochrona=Upr_max;
   return nr_danej;
 }
 ------------------------------------------------------------------------- */
int wpis_tabl_float(signed char wpis, int y, int x0,
                    unsigned int attr_d, unsigned int attr, unsigned int attrtx,
                    int* index, char* tabl, int dim, int L_kol, int size,
                    char precyzja, char* monit_pocz, int ind_min, int ind_max,
                    float w_min, float w_max, char* format_tabl, char* format_ogr,
                    float baza, float abs_min, float abs_max)
{
    float *wart, wartn[30], wmin, wmax, w_abs;
    char *bufor, *format = NULL, text[80], *form = NULL, *c, *fh, typ;
    char zakres[80], *cc, *zz, *taknie = "<t/n>? ", znak;
    char hash = '*', f_nazwy[40], *f_ind = NULL, *fk_nazwy = NULL;
    char *fmon, *f_pocz, opis_danej[80];
    char help_ogr[80], *format_bazy, *format_tx, *format_wyn, *zakr_abs;
    int ret, xmax, xp, xp_ind, xp_naz, i, k, x_konc;
    int dlh_ogr, ll, l, j, n, poz_wart_abs = 0, txl, iobl = 0, dltn = 0;
    int dx_ind, ind_size, dl_zakr, xpp, nrkol, xmax_tab;
    unsigned int attr_t;
    typ = 'f';
    if (format_ogr == NULL && fabs(baza) > 1.e-4)
    {
        w_min = baza * w_min;
        w_max = baza * w_max;
    }
    /*   if(typ=='F' || typ=='W') {typ='f'; typf='d';} */
    fmon = formaty_tabl(f_nazwy, monit_pocz, &ind_min, &ind_max, index,
                        &ind_size, &hash, format_tabl, opis_danej, &dl_zakr,
                        &f_ind, &fk_nazwy, &dx_ind);
    /* ------------ koniec przeniesienia ------------------------------- */
    for (i = L_kol - 1; i >= 0; i--)
    {
        wart = (float*)((tabl + (ind_max + 1) * dim * i) + (*index) * dim);
        wartn[i] = *wart;
    }
    dlh_ogr = 0;
    help_ogr[0] = EOS;
    znak = EOS;
    if (format_ogr != NULL)
    {
        format = format_ogr;
        zakr_abs = &help_ogr[MXR_MAX];
        *zakr_abs = EOS;
        i = 0;
        l = 0;
        j = 0;
        for (k = 0; k < 2; k++)
        {
            for (fh = format; *fh != '%'; fh++, j++)
            {
                help_ogr[l] = format_ogr[j];
                l++;
            }
            if ((*fh) == EOS)
            {
                format = NULL;
                break;
            }
            help_ogr[l] = EOS;
            l++;
            typ = czy_format(fh, &i);
            if (typ == EOS) continue;
            if (typ != 'f')
            {
                komunikat(MY_MAX, 1,ATTR_A, " Bledny format %s w okno_float_wzgl(). <Ent> ",
                          format_ogr);
                GET_char();
                dlh_ogr = 0;
                break;
            }
            znak = fh[i];
            fh[i] = EOS;
            switch (k)
            {
            case 0: format_bazy = &help_ogr[l];
                ll = sprintf(zakres, format, baza);
                fh[i] = znak;
                break;
            case 1: ll += sprintf(zakres, format, baza*wartn[0]);
                format_wyn = &help_ogr[l];
                break;
            }
            fh[i] = znak;
            format = &fh[i];
            for (n = 0; n < i; n++)
            {
                help_ogr[l] = format_ogr[j];
                l++;
                j++;
            }
            help_ogr[l] = EOS;
            l++;
            if (k == 0) format_tx = &help_ogr[l];
        }
    }
    zakres[0] = EOS;
    if (fmon != NULL)
    {
        c = szukaj_formatow(fmon, 'f', &f_pocz);
        if (c == NULL)
            sprintf(&opis_danej[dl_zakr], fmon);
        else
        {
            sprintf(&opis_danej[dl_zakr], fmon, w_min, w_max);
            for (cc = f_pocz, zz = &zakres[3]; cc != c; cc++, zz++) *zz = *cc;
            zakres[0] = '%';
            zakres[1] = 's';
            zakres[2] = '(';
            *zz = ')';
            zz++;
            *zz = '%';
            zz++;
            *zz = 's';
            zz++;
            *zz = EOS;
        }
    }
    if (bez_potwierdzania == 0) dltn = strlen(taknie);
    if (format != NULL)
    {
        zakr_abs = &help_ogr[l];
        dlh_ogr = ll + sprintf(zakr_abs, format, abs_min, abs_max);
    }
    {
        int siztab;
        siztab = (size + 1) * L_kol - 1;
        xmax_tab = wydruk_monitu(wpis, opis_danej, dl_zakr, taknie, &dltn, text, &x0,
                                 dx_ind, &dlh_ogr, &siztab, y, &xp_naz, &xp, &txl,
                                 attrtx, w_min, w_max, typ, 0, NULL, NULL, NULL);
        size = (siztab + 1) / L_kol - 1;
        xmax = xmax_tab - (size + 1) * (L_kol - 1);
    }
    if (xmax_tab == 0) return 0;
    format = (char*)Malloc((size + 40) * sizeof(char));
    if (format == NULL) return -1;
    lwmall++;
    sprintf(format, " %d.%df", size, precyzja);
    format[0] = '%';
    form = &format[11];
    bufor = &form[10 * sizeof(char)];
    bufor[0] = EOS;
    iobl = 1;
    if (hash != '#') iobl = 0;
    if (iobl == 0 || wpis == 2)
        sprintf(bufor, format, *wart);
    Okno(y, x0, y, xmax_tab, attr);
    x_konc = xmax_tab + txl;
    x_konc = xmax_tab + 1;
    if (dlh_ogr > 0)
    {
        term_type(y, x_konc, help_ogr, 0, attrtx);
        term_printf(y, m_wherex(), attrtx, format_bazy, baza);
        term_type(y, m_wherex(), format_tx, 0, attrtx);
        poz_wart_abs = m_wherex();
        term_printf(y, poz_wart_abs, attr, format_wyn, baza * wartn[0]);
        term_type(y, m_wherex(), zakr_abs, 0, attrtx);
        x_konc = m_wherex();
    }
    xp_ind = xp;
    if (ind_size > 0)
    {
        xp_ind = wydruk_indexu(y, xp_naz, attr, attrtx, f_nazwy, f_ind, *index, fk_nazwy);
        Okno(y, xp_ind, y, xmax_tab, attr);
        ret = xp_ind;
    }
    if (abs(wpis) != 1)
    {
        for (nrkol = 0, xpp = xp; nrkol < L_kol; nrkol++, xpp += (size + 1))
        {
            if (iobl == 0 || wpis == 2)
            {
                sprintf(bufor, format, wartn[nrkol]);
                term_type(y, xpp, bufor, 0, attr_d);
            }
            else
            {
                Okno(y, xpp, y, xpp + size, attr_d);
                clr_Okno();
            }
            /*     x_konc=m_wherex(); */
            attr_t = attrtx;
            if (iobl == 0)
            {
                if ((wartn[nrkol] < w_min || wartn[nrkol] > w_max) ||
                    (format_ogr != NULL && (wartn[0] * baza < abs_min || wartn[0] * baza > abs_max)))
                {
                    attr_t = ATTR_A;
                    wpis = 0;
                    if (txl > 1) text[1] = '!';
                }
            }
            if (wpis == 0)
            {
                for (i = 0; i < txl; i++)
                {
                    if (i + x_konc <= X_maxBlank) term_type(y, x_konc + i, &text[i], 1, attr_t);
                }
            }
            else
            {
                for (i = 0; i < txl; i++)
                {
                    if (i + x_konc <= X_maxBlank) term_type(y, x_konc + i, " ", 1, attrtx);
                }
            }
            x_konc = m_wherex();
        } /* ================= koniec for(nrkol....) ============= */
        /* ========================= dla tablicy ========================== */
        if (wpis > 2)
        {
            if (y < MY_MAX)
            {
                if (wpis >= 4 && ind_size > 0)
                {
                    struct dane_wpisu* dw = NULL;
                    if (wpis == 4)
                    {
                        dw = (struct dane_wpisu*)Malloc(sizeof(struct dane_wpisu));
                        sprintf(form, "%c f", hash);
                        form[1] = '%';
                        dw->x0 = x0;
                        dw->xmax = xmax;
                        dw->x_konc = x_konc;
                        dw->dltn = dltn;
                        dw->poz_wart_abs = poz_wart_abs;
                        dw->dlh_ogr = dlh_ogr;
                        dw->format = format;
                        dw->format_bazy = format_bazy;
                        dw->format_tx = format_tx;
                        dw->form = form;
                        dw->format_wyn = format_wyn;
                        dw->help_ogr = help_ogr;
                        dw->format_ogr = format_ogr;
                        dw->text = text;
                        dw->zakres = zakres;
                        dw->zakr_abs = zakr_abs;
                        dw->bufor = bufor;
                        dw->taknie = taknie;
                        dw->iobl = iobl;
                        dw->wartn = wartn;
                        dw->baza = baza;
                        dw->w_min = w_min;
                        dw->w_max = w_max;
                        dw->abs_min = abs_min;
                        dw->abs_max = abs_max;
                    };
                    ret = okno_help(y + 1, xp_naz - 1, xmax_tab + 1, attrtx, attr, 'f', ind_min,
                                    ind_max, (int)w_min, (int)w_max, (char*)dw, L_kol, size,
                                    precyzja, NULL, NULL, NULL, 0, tabl, dim, f_nazwy, f_ind, fk_nazwy);
                    if (dw != NULL)
                        Free((char*)dw);
                    if (ret >= 0)
                    {
                        if ((ret + ind_min) == (*index)) x_konc = 0;
                        else *index = ret + ind_min;
                    }
                    else x_konc = -1;
                }
            }
            wpis = 1;
        }
        HELP = 0;
        if (format != NULL)
        {
            Free(format);
            lwmall--;
        }
        return x_konc;
    } /* koniec dla wpis!=1 */
    /* ----------------------------------------------------------------- */
    else /* obsluga dla wpis=1 */
    {
        HELP = 0; /*x_konc=xmax+1; */
        for (i = 0; i < txl; i++)
        {
            if (i + x_konc <= X_maxBlank) term_type(y, x_konc + i, &text[i], 1, attrtx);
        }
        sprintf(form, "%c f", hash);
        form[1] = '%';
        xpp = xp;
        for (i = 0; i < L_kol; i++, xpp += (size + 1)) term_printf(y, xpp, attr, format, wartn[i]);
        /* -------------------- Obsluga wpisu indeksu ---------------------- */
        if (ind_size > 0 && wpis <= -1)
        {
            i = *index;
            ret = wpis_indexu(y, xp_ind, ind_size, index, (char*)index, L_kol, ind_min,
                              ind_max, tabl, dim, xp, xmax_tab, f_ind, format, attrtx, attr,
                              0, NULL, NULL, NULL, 'f', dltn, (int)w_min, (int)w_max, dlh_ogr);
            if (ret < 0)
            {
                *index = i;
                Free(format);
                HELP = 0;
                lwmall--;
                return -1;
            }
            nrkol = 0;
            if (*index != i)
            {
                for (i = L_kol - 1; i >= 0; i--)
                {
                    wart = (float*)((tabl + (ind_max + 1) * dim * i) + (*index) * dim);
                    wartn[i] = *wart;
                }
                if (iobl == 0)
                    sprintf(bufor, format, wartn[nrkol]);
            }
            /*       if(wpis==-1) */
            {
                wpis = 1;
                HELP = 0;
            }
            /*       else {return 1;} */
        }
        /* ---------------------- koniec zmiany starego programu -------------- */
        x_konc = wpisanie_danych_fl(x0, xp, y, xmax, x_konc, 1, dltn, poz_wart_abs,
                                    dlh_ogr, &format, format_bazy, format_tx, form, format_wyn,
                                    help_ogr, format_ogr, text, zakres, zakr_abs, bufor, taknie,
                                    attr, attrtx, iobl, L_kol, size, dim, ind_max, *index, tabl,
                                    wart, wartn, baza, w_min, w_max, abs_min, abs_max);
    } /* ============= funkcja ta jest w pliku wewy1.c ============== */
    if (format != NULL)
    {
        Free(format);
        lwmall--;
    }
    return x_konc;
}

void begin_scroll(int dana_nr) { SCROLL = dana_nr; }
#endif // dla WEWY0_INCL
