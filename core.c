#define _MAIN_MODUL_
#define _WEWY_TERM_
// #define _NCURSES_  // Defined in Makefile with -D_NCURSES_
#include "blank/moje.h"
#include "core.h"
struct agenda* Agenda = NULL;
int X_time = 30, X_tyt = 0, Liczba_opcji;
// kolory poczatkowe
unsigned int attr = TERM_WHITE_BG | TERM_BLACK | TERM_FLUSH;
unsigned int at_wpis = TERM_WHITE | MTERM_HILIGHT | TERM_BLUE_BG | TERM_FLUSH;
unsigned int attr_time = TERM_WHITE | MTERM_HILIGHT | TERM_BLUE_BG | TERM_FLUSH;
unsigned int attr_title = TERM_WHITE | MTERM_HILIGHT | TERM_BLUE_BG | TERM_FLUSH;
#include "konfig.h"
#include "dek_budz.h"   /* budz_blankiet, budz_wczytaj_z_pliku */
#include "mqtt_pub.h"   /* mqtt_auto_start */
#include "sensor_sim.h" /* demonstracyjny monitor IoT */
#include "sys_rep.h"    /* czytelny dashboard stanu systemu */
#include "loc.h"
#include "rap_bl.h"
#include "scheduler.h"
/* ----------------------------------------------------------- */
static int open_sys(void);
/* ------------------------------------------------------------ */
struct Service* Service;
unsigned short int NODE_NO = 1;
signed char _TYP_KONSOLI_ = -1;
static int if_graf = 0, YZ_max_text, Yz_max_graf, Y_MAX_graf = 34;
int y_shift = 0;
/* ---------------------------------------------------------------- */
/* ---------------------------------------------------------------- */
#define PATH_LOG   "/tmp/stud_sys.log"

static int ZapiszDoLog(char* sciezka, char* tryb);
static int build_presentation_menu(char* menu_view[], int menu_map[], char* graph_text);
/* ---------------------------------------------------------------- */

/*******************************************************************/

int main(int argc, char* argv[])
{
    (void)argc; (void)argv;
    int ret, Ret, l_poz_menu, y_logo = 3;
    int poz_menu = 0; /* pozycja kursora w menu (Ret to numer uslugi) */
    char* graf_text[2] = {L_CORE_MODE_GRAPH, L_CORE_MODE_TEXT};
    char* menu_view[L_SYS + 3];
    int menu_map[L_SYS + 3];
    int visible_options;
    /* ---------------------------------------------------------------- */


    InitConsole();
    term_fill(TERM_WHITE | TERM_BLACK_BG);
    Cls();
    Liczba_opcji = open_sys();
    /* Ignore long-lived allocations created during system startup when
       reporting per-session memory growth in the report engine. */
    lwmallp = lwmall;
    Ret = Liczba_opcji - 1;
    term_fill(TERM_WHITE | TERM_BLUE_BG);
    /* ------------------------------------------------------------- */
    YZ_max_text = MY_MAX;
    Yz_max_graf = YZ_max_text;
    visible_options = build_presentation_menu(menu_view, menu_map, graf_text[0]);
    Ret = 0;
    l_poz_menu = visible_options - 1;
    MYR_MAX = MY_MAX - 1;
    l_poz_menu = visible_options;
    Yz_max_graf = Y_MAX_graf;
    if (if_graf != 0) if_graf = otworz_graf_blank(0, 0, -1, -1, Yz_max_graf, TERM_WHITE | TERM_BLACK_BG);
    do
    {
        term_fill(TERM_WHITE | TERM_BLUE_BG);
        setcursor(nocursor);
        {
            int y_menu;
            y_menu = MY_MAX - l_poz_menu - 10;
            if (y_menu - (y_logo + 10) > 1) y_menu = y_logo + 11;
            poz_menu = okno_menu(menu_view, l_poz_menu, poz_menu, attr, at_wpis, y_menu, 25, -1, L_CORE_MAIN_MENU_TITLE, 1);
        }
        Ret = poz_menu;
        if (Ret < 0)
        {
            break;
        }

        if (menu_map[Ret] < 0)
        {
            if (menu_view[Ret] != NULL)
            {
                if (if_graf != 0)
                {
                    if_graf = Endgraph(); /* ------ przechodzimy na tekstowy ---- */
                    MY_MAX = YZ_max_text;
                    MYR_MAX = MY_MAX - 1;
                    y_shift = 0;
                }
                else /* ------ przechodzimy na graficzny --- */
                {
                    MY_MAX = Yz_max_graf;
                    MYR_MAX = MY_MAX - 1;
                    //y_shift=12;
                    // okno w trybie graficznym spada o 12 jednostek
                    setcursor(nocursor);
                    if_graf = otworz_graf_blank(0, 0, -1, -1, Yz_max_graf, TERM_WHITE | TERM_BLACK_BG);
                }
                menu_view[Ret] = graf_text[if_graf];
                continue;
            }
        }

        Ret = menu_map[Ret];
        if (Ret == L_SYS - 1)
        {
            sensor_sim_start_demo();
            sys_status_dashboard();
            Ret = 0;
            continue;
        }
        if (Ret == L_SYS)
        {
            sys_status_dashboard();
            Ret = 0;
            continue;
        }
        Service = Serv[Ret];
        Agenda = NULL;
        {
            int Nr_rekordu = 0, npzl = 0, kto = 0, r_pocz = 0;
            ret = raporty(Serv[Ret]->kod_uslugi, Nr_rekordu, npzl, kto, r_pocz, Serv[Ret]->l_rek_max, Serv[Ret]);
        }
    }
    while (Ret != -1);
    zamknij_raporty();
    term_fill(TERM_WHITE | TERM_BLACK_BG);
    Endgraph();
    CloseConsole();
    ZapiszDoLog(PATH_LOG, "KONIEC PRACY");
    (void)ret;
    return 0;
}

static int build_presentation_menu(char* menu_view[], int menu_map[], char* graph_text)
{
    int idx = 0;
    menu_view[idx] = Menu[1];
    menu_map[idx] = 1;
    idx++;
    menu_view[idx] = L_CORE_MENU_IOT_DEMO;
    menu_map[idx] = L_SYS - 1;
    idx++;
    menu_view[idx] = Menu[L_SYS];
    menu_map[idx] = L_SYS;
    idx++;
    (void)graph_text;
    menu_view[idx] = NULL;
    return idx;
}

/* -------------------------------------------------------------------- */
static int open_sys(void)
{
    int liczba_opcji, i;
    struct agenda* A;
    liczba_opcji = set_services();
    otworz_raporty(0);
    scheduler_init();
    /* ------------------- Ustawiamy zlecenie sprawdzania ---------- */
    {
        int rozmiar, zl = 0;
        AgendaMax = 0;
        rozmiar = Serv[zl]->str_size;
        A = (struct agenda*)Malloc(rozmiar);
        A->S = Serv[zl];
        A->data = (A + 1);
        A->mode = 'p';
        A->number_of_calls = -1;
        A->Interval = 1;
        A->prior = 200;
        A->prior_plus = 0;
        { const char* sn = (A->S)->name; int snlen = sn ? (int)strlen(sn)+1 : 1;
          A->name = (char*)Malloc(snlen); if(A->name) memcpy(A->name, sn ? sn : "", snlen); }
        A->state = 0;
        A->delay = 0;
        A->last_time = sek_akt;
        SysA[0] = A;
    }
    wewy_abort_off();
    wpis_danych_bez_potwierdz();
def_haslo("st", "Student");
    haslo_nieaktywne();
    for (i = 0; i < liczba_opcji; i++)
    {
        int kod_uslugi, rek0, kolor = 0, ramka = 1, max_l_data_blank = 100;
        int rap_idx;
        kod_uslugi = Serv[i]->kod_uslugi;
        rek0 = 0;
        rap_idx = def_Report(kod_uslugi, -1, rek0, rek0 + Serv[i]->l_rek_max - 1, Serv[i]->str_size,
                             Serv[i]->name, kolor, ramka, Serv[i]->D, max_l_data_blank, Serv[i]->typ_bazy,
                             Serv[i]->def_blankiet, Serv[i]->dane_rap_bl, Serv[i]->wpis_rap_bl);
        if (rap_idx < 0)
        {
            komunikat(MY_MAX, X_L0, ATTR_A, L_CORE_ERR_DEF_REPORT,
                      kod_uslugi, Serv[i]->name);
            czekaj_na_klawisz();
        }
    }
    budz_wczytaj_z_pliku();
    mqtt_auto_start();
    term_clear(TERM_CLS_SCR);
    sek_akt = time(NULL);
    return liczba_opcji;
}

/* ==================================================================== */
int czas_zegarowy(int* godz, int* min, int* sek)
{
    time_t sek1970;
    struct tm* Time;
    sek1970 = time(NULL);
    Time = localtime(&sek1970);
    *godz = Time->tm_hour;
    *sek = Time->tm_sec;
    *min = Time->tm_min;
    return (Time->tm_hour * 60l) + (*min);
}

int data_systemowa(int* dzien, int* miesiac, int* rok)
{
    time_t sek1970;
    struct tm* Time;
    sek1970 = time(NULL);
    Time = localtime(&sek1970);
    *dzien = Time->tm_mday;
    *miesiac = Time->tm_mon + 1; /* tm_mon liczone od 0 */
    *rok = Time->tm_year + 1900;
    return 0;
}


/*-------------------------------------------------------------*/
static int ZapiszDoLog(char* sciezka, char* tryb)
{
    FILE* fp;
    fp = fopen(sciezka, "a");
    if (fp == NULL) { fprintf(stderr, "Blad otwarcia LOG: %s\n", sciezka); return 0; }
    else
    {
        // zapis data i time otwarcia systemu
        int godz, min, sek, dzien, miesiac, rok;
        czas_zegarowy(&godz, &min, &sek);
        data_systemowa(&dzien, &miesiac, &rok);
        fprintf(fp, "\n#LOG:[%.2d.%.2d.%.2d](czas) [%.2d.%.2d.%.4d](data)#%s", godz, min, sek, dzien, miesiac, rok,
                tryb);
        fclose(fp);
    }
    return 0;
}

/*-------------------------------------------------------------*/
