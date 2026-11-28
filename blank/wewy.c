/* blank/wewy.c - silnik formularzy: klawiatura i podzial na slowa,
   okno_menu, okna komunikatow, przelaczniki trybu, wczytaj_*().
   Pozostale czesci silnika: wewy_pola.c, wewy_raporty.c, wewy_dane.c,
   wewy_blankiet.c, wewy_liczby.c. Ten plik definiuje globalne zmienne
   silnika (deklaracje extern: wewy1.h i opcjesys/scr_size.h). */
#include "wewy_wsp.h"

extern HWND handle_window_device;
char real_time = 0;  /* Definition for NCURSES */

static time_t lticks_min, Ltickm = 0.2 * CLK_TCK, kbtickm = 0.02 * CLK_TCK; /* klawisz co 0.2sek */

/* ---------- Globalne zmienne silnika (deklaracje extern: wewy1.h) ---------- */
struct Blok_anim BA;
shint mouse_free=0;
char blokada_potwierdzania=0;
char ABORT=1, BLAD=1, blokada_zapisu=0, bez_potwierdzania=0;
shint SCROLL=-1;
signed char Akt_Kolor=0, Klucz=-1, arrow=27;
char haslo[LHASEL][11];
char kod_o[LHASEL][21];
unsigned int attr_o[3], attrtx[3], attr_dat[3], attr_inf;
struct Blok_anim *blok_anim=&BA; /*NULL; */
/* Parametry wymiarowe blankietu */
shint X_0=0, Y_0=0, X_maxBlank, Y_maxBlank;
int dana_poczatkowa=0;
/* ----------------------------------------------------------------- */
unsigned int attryb=TERM_WHITE|TERM_BLUE_BG|TERM_FLUSH /*, opcje*/;
shint INSERT=-1, HELP=0, ZAPIS=0, BEZ_HASLA=0, SLEDZIC=0;
struct okno *Ok=NULL;
shint nr_danej=0, liczba_danych=0;
shint lwmall=0, lwmallp=0;

/* Alokacja/zwolnienie z licznikiem lwmall (liczba blokow niezwolnionych). */
void* Malloc(size_t rozmiar)
{
    void* p = malloc(rozmiar);
    if (p != NULL) lwmall++;
    return p;
}

void Free(void* p)
{
    if (p == NULL) return;
    free(p);
    lwmall--;
}
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
/* ---------- Wymiary ekranu (deklaracje extern: opcjesys/scr_size.h) -------- */
int MYR_MAX=24;
int MY_MAX=25;
int MX_MAX=80;
int MXR_MAX=79;
int X_L0=0;
int Y_G0=0;
int BUF_com_size = 0, nr_komunik = -1;
void* BUF_com = NULL;
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
char separator[L_SEP] = {' ', ',', '!', ':', ';', '?', EOS, '.'};

int (*odbior_komunik)(void* b, pid_t a) = NULL;
/* ---------------------- struktura dla obslugi grafiki ------------ */
#define _TYLKO_RYS_
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
        czekaj_na_klawisz(); /* dev_mode(0,opcje,_DEV_MODES);*/
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
    while (co != (unsigned char)monit[0] && co != (unsigned char)monit[2]);
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
postac stosu - znak :# - jesli wystepuje - dane musza byc wpisane
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
    /* Collect variadic args via va_list (ARM64-safe replacement for &stos+1) */
    char *_va_arr[16];
    int _va_n = 0;
    {
        va_list _ap;
        const char *_p;
        va_start(_ap, stos);
        for (_p = stos; *_p && _va_n < 16; _p++)
            if (*_p == '%' && *(_p+1) != '%') _va_n++;
        for (int _i = 0; _i < _va_n; _i++)
            _va_arr[_i] = va_arg(_ap, char*);
        va_end(_ap);
    }
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
    adres_danej = _va_arr;
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
                                L_WEWY_BAD_DATA, sl, f);
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
                    komunikat(MY_MAX, X_L0, attryb, L_WEWY_BAD_DATA, sl, f);
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
        case 'g': ff = 2; /* fall through */
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
                    term_printf(y_akt, x_akt0, attryb, L_WEWY_BAD_DATA, sl, f);
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
    /* Collect variadic args via va_list (ARM64-safe replacement for &stos+1) */
    char *_va_arr[16];
    int _va_n = 0;
    {
        va_list _ap;
        const char *_p;
        va_start(_ap, stos);
        for (_p = stos; *_p && _va_n < 16; _p++)
            if (*_p == '%' && *(_p+1) != '%') _va_n++;
        for (int _i = 0; _i < _va_n; _i++)
            _va_arr[_i] = va_arg(_ap, char*);
        va_end(_ap);
    }
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
    adres_danej = _va_arr;
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
                              L_WEWY_BAD_DATA, sl, f);
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
                    komunikat(MY_MAX, X_L0, attryb, L_WEWY_BAD_DATA, sl, f);
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
        case 'g': ff = 2; /* fall through */
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
                    term_printf(y_akt, x_akt0, attryb, L_WEWY_BAD_DATA, sl, f);
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

/*------------------------------------------------------------------------*/
int czytaj_we(char* monit, char* text, int lz0, int liczba_znakow)
{
    int lz, nEOS = 0, lzp = 0, x, x_pocz, start = 0, ccx;
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
                term_cur(y_akt, x_akt - 1);
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

            if (c == BBS || c == LEFT)
            {
                if (lz == 0) continue;
                lz--;
                m_gotoxy(x_pocz + lz, y_cursor); /* term_cur(y_akt,x_akt+lz); */
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
    int lz, nEOS = 0, lzp = 0, x, x_pocz, start = 0;
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
                m_gotoxy(x_akt - 1, y_akt); /*  term_cur(y_akt,x_akt); */
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
                m_gotoxy(x_pocz + lz, y_akt); term_cur(y_akt, x_akt);
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
    term_printf(y_akt, x_akt, attryb, "+---------------------------------------+");
    yplus();
    x_akt = x_akt0;
    term_printf(y_akt, x_akt, attryb, "|   ABORT programu !!! ?? TAK - 't'     |");
    yplus();
    x_akt = x_akt0;
    term_printf(y_akt, x_akt, attryb, "+---------------------------------------+");
    yplus();
    x_akt = x_akt0;
    if (czekaj_na_klawisz() == 't') exit(0);
    term_printf(y_akt, x_akt, attryb, L_WEWY_ENTER_REPLY);
}

/*------------------------------------------------------------------------*/
void bip(char mod)
{
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
/* NCURSES implementations - most functions are in term_ncurses.c */
void zamknij_grafiki(int a)
{
    (void)a;
};

int czy_mozna_nowe_wykresy(void*(*akcja_grafik)(int typ_kom, int kto,
                                                char* pdane, char* Text, void*(*Dane_Wykr)(void* Rd), void* R))
{
    (void)akcja_grafik;
    return 0;
}

int zamknij_jeden_wykres(void* R, pid_t pid)
{
    (void)R; (void)pid;
    return 0;
}

/*====================================================================== */
int Term_Typef(int y, int x, char* text, int lenth, unsigned int attr)
{
    int l, i;
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
    int xp, xm, yp, ym, Ym, y_kur = 0, x_kur = 0, n, ik, x, ip_old;
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
            czekaj_na_klawisz();
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
        czekaj_na_klawisz();
        return -1;
    }
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
            }
            Free(buf);
            czekaj_na_klawisz();
            return -1;
        }
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
                case NLCR: ret = CR/*Get_Char()*/; /* fall through */
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
    }
    Free(buf);
    blok_anim->ym = -1;
    return (k_inv);
}

/* ===================================================================== */
void maluj_ramke(int yp, int xp, int ym, int xm, unsigned int attr)
{
    if (xp < X_L0) xp = X_L0;
    if (yp < Y_G0) yp = Y_G0;
    if (xm > MXR_MAX) xm = MXR_MAX;
    if (ym > MYR_MAX) ym = MYR_MAX;
    ramka_graficzna(yp, xp, ym, xm, attr);
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
    /*fprintf(mystderr," X_maxBlank=%d/%d ymax=%d y_maxOkna=%d Y_maxBlankOK=%d",X_maxBlank,x_maxOkna,ymax,y_maxOkna,Y_maxBlank);*/
}

/* ---------------------------------------------------------------------- */
void clr_Okno(void)
{
    int x, y;
    {
        for (y = y_akt0; y <= y_maxOkna; y++)
        {
            for (x = x_akt0; x <= x_maxOkna; x++) term_type(y, x, " ", 1, attryb);
        }
        m_gotoxy(x_akt0, y_akt0);
    }
}

/* ---------------------------------------------------------------------- */
static int vkomunikat(int y, int x, unsigned int attr, char* format, va_list arg)
{
    int l, i = 0, max_len;
    char bufor[120];
    vsnprintf(bufor, sizeof(bufor), format, arg);
    l = strlen(bufor);
    max_len = MX_MAX - x;
    if (max_len < 0) max_len = 0;
    if (max_len > (int)sizeof(bufor) - 1) max_len = (int)sizeof(bufor) - 1;
    if (l > max_len)
    {
        bufor[max_len] = EOS;
        l = max_len;
    }
    for (i = x + l; i < MX_MAX; i++) term_type(y, i, " ", 1, attr);
    term_type(y, x, bufor, 0, attr);
    term_color(TERM_WHITE | TERM_BLACK_BG);
    term_type(y, MXR_MAX, " ", 1,TERM_WHITE | TERM_BLACK_BG);
    return m_wherex();
}

int komunikat(int y, int x, unsigned int attr, char* format, ...)
{
    int ret;
    va_list arg;
    va_start(arg, format);
    ret = vkomunikat(y, x, attr, format, arg);
    va_end(arg);
    return ret;
}

/* Komunikat jak komunikat() i czekanie na klawisz ("... <Ent>").
   Zwraca nacisniety klawisz (0 dla klawisza specjalnego). */
int komunikat_ent(int y, int x, unsigned int attr, char* format, ...)
{
    va_list arg;
    va_start(arg, format);
    vkomunikat(y, x, attr, format, arg);
    va_end(arg);
    return czekaj_na_klawisz();
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
                komunikat_ent(MY_MAX, 1,ATTR_A, " Bledny format %s w okno_tabl_..(). <Ent> ",
                              format);
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
                komunikat_ent(5, 1,ATTR_A, " Bledny format %s. <Ent>", form);
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
                komunikat_ent(5, 1,ATTR_A, " Bledny format %s. <Ent>", form);
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
                komunikat_ent(5, 1,ATTR_A, " Bledny format %s. <Ent>", form);
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
    lhasel = 0;
    ZAPIS = 0;
    BEZ_HASLA = 0;
    EDIT = 0;
    SLEDZIC = 0;
    blokada_zapisu = ZAPIS_DOWOLNY;
    if (Klucz != -1)
    {
        komunikat_ent(MY_MAX, X_L0,ATTR_A, L_WEWY_PREV_REPORTS_OPEN);
        return -1;
    }
    for (i = 0; i < LHASEL; i++)
    {
        haslo[i][0] = EOS;
        kod_o[i][0] = EOS;
    }
    if (lwmall > 0)
    {
        komunikat_ent(MY_MAX, X_L0,ATTR_A, L_WEWY_PREV_ALLOCS_LEFT,
                      lwmall);
    }
    lwmallp = lwmall;
    if (nr_blank >= 0 || Czy_Blankiet_otwarty != 0)
    {
        komunikat(MY_MAX, 1,ATTR_A,
                  L_WEWY_PREV_BLANKS_OPEN,
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
            komunikat(MY_MAX, 1,ATTR_A, L_WEWY_PREV_REPORT_OPEN,
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
    return i;
}

/* ------------------------------------------------------------------- */
signed char zamknij_blankiet(int* nr_rekordu)
{
    int i, k;
    signed char nr_raportu = -1;
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
                    if (ok->format != NULL)
                    {
                        Free(ok->format);
                    }
                }
                Free(Ok);
            }
            Ok = NULL;
            return -1;
        }
        i = 0;
        if (BL[nr_blank] == NULL)
        {
            komunikat_ent(5, 1,ATTR_A, " Zla specyfikacja blankietu %d: <Ent>", nr_blank);
            nr_blank--;
            i = 1;
            k = 1;
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
            }
        }
        Free(Ok);
        czekaj_na_klawisz();
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
            }
        }
        Free(Ok);
        Ok = NULL;
        if (*nr_rekordu != BL[nr_blank]->nr_rekordu)
        {
            komunikat_ent(MY_MAX, X_L0,ATTR_A,
                          " Niezgodnosc rekordu dekl.(%d) z zapisem (%d) w zamykanym blankiecie %d. <ENT> ",
                          *nr_rekordu, BL[nr_blank]->nr_rekordu, nr_blank);
        }
        if (nr_blank == 0)
        {
        }
        Free(BL[nr_blank]);
        BL[nr_blank] = NULL;
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
            komunikat_ent(5, 1,ATTR_A, " Zla specyfikacja blankietu %d (nr_raportu=%d): <Ent>",
                          nr_blank, nr_raportu);
            if (nr_blank >= 0) nr_blank--;
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
	 {snprintf(buf, sizeof(buf), format, liczba);\
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
	if(liczba<w_min) liczba=w_min;\
	if(liczba>w_max) liczba=w_max;\
	term_printf(y,x0,attryb,format,liczba);\
	attryb=attr_old;	term_color(attryb);
/* --------------------- KONIEC MACRA ----------------------------- */
char ustaw_buf_wpisu(int y, int x, char* monit, int* size,
                     unsigned int attr_monit, unsigned int attr_wpis)
{
    int dl, dlug;
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
    int x0, ret;
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
    snprintf(format, sizeof(format), "%%%dd", size);
    macro_wczyt("%d");
    return liczba;
}

/* ---------------------------------------------------------------- */
float wczytaj_float(int y, int x, char* monit, int size, int prec,
                    float w_def, float w_min, float w_max,
                    unsigned int attr_monit, unsigned int attr_wpis)
{
    int x0, ret;
    char buf[WLMAX], format[32], oblig = 0;
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
    snprintf(format, sizeof(format), "%%%d.%df", size, prec);
    macro_wczyt("%f")
    return liczba;
}

/* ---------------------------------------------------------------- */
char wczytaj_char(int y, int x, char* monit, unsigned char w_def,
                  unsigned int attr_monit, unsigned int attr_wpis)
{
    unsigned int attr_old;
    char format[5], buf[2];
    unsigned char liczba, w_min = 0, w_max = 'z';
    int oblig = 0, size = 1, x0, ret;
    if (!isalnum(w_def)) { w_def = w_max; }
    attr_old = attryb;
    liczba = w_def;
    if (monit[0] == '#') oblig = 1;
    x0 = ustaw_buf_wpisu(y, x, monit, &size, attr_monit, attr_wpis);
    y = m_wherey();
    snprintf(format, sizeof(format), "%%c");
    macro_wczyt("%c");
    return liczba;
}

char wczytaj_string(int y, int x, char* monit, char* text, int size,
                    unsigned int attr_monit, unsigned int attr_wpis)
{
    int x0, ret, strl, i;
    char buf[WLMAX], oblig = 0;
    unsigned int attr_old;
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
        snprintf(buf, sizeof(buf), "%s", text);
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
        for (i = 0; i <= (int)strlen(buf); i++) text[i] = buf[i];
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
        term_save_image(yp, xp, buf, tx_size);
    }
    term_type(yp, xp, text, 0, attr);
    ret = czekaj_na_klawisz();
    if (buf != NULL)
    {
        term_restore_image(yp, xp, buf, tx_size);
        Free(buf);
    }
    text[tx_size] = znak;
    attryb = old_attr;
    setcursor(cursor);
    m_gotoxy(xkursora, ykursora);
    return ret;
}
