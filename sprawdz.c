/* #include <butil.h> */
#include "blank/moje.h"
#include "sprawdz.h"
#include "loc.h"
void chk_time(void);
extern struct Service* Service;
/* extern struct agenda *Agenda, *SysA[]; */
extern int X_time, X_tyt;
extern time_t sek_akt;
extern unsigned int attr_title, attryb, attr_time;
extern unsigned int cursor, nocursor;

char* dni[] = {L_RAP_DAY_SUNDAY, L_RAP_DAY_MONDAY, L_RAP_DAY_TUESDAY, L_RAP_DAY_WEDNESDAY,
               L_RAP_DAY_THURSDAY, L_RAP_DAY_FRIDAY, L_RAP_DAY_SATURDAY};
extern struct agenda* SysA[];

int sprawdz(void* DA)
{
    struct agenda* A;
    unsigned int old_attr;
    struct tm* Time;
    time_t tsek1970;
    int i, xkursora, ykursora;
    A = (struct agenda*)DA;
    A->state = 0;
    tsek1970 = time(NULL);
    Time = localtime(&tsek1970);
    xkursora = m_wherex();
    ykursora = m_wherey();
    setcursor(nocursor);
    old_attr = attryb;
    term_printf(Y_G0, X_time, attr_time, L_RAP_DATE_PREFIX,
                Time->tm_mday, Time->tm_mon + 1, (Time->tm_year % 100));
    term_printf(Y_G0, m_wherex(), attr_time, L_RAP_TIME_FMT,
                Time->tm_hour, Time->tm_min, Time->tm_sec, dni[Time->tm_wday]);
    /*	term_printf(Y_G0,X_time,attr_time," Dnia %02d.%02d.%02d  Godz.%02d:%02d.%02d (%s)",
                                Time->tm_mday,Time->tm_mon+1,(Time->tm_year%100),
                                Time->tm_hour,Time->tm_min,Time->tm_sec,dni[Time->tm_wday]); */
    for (i = m_wherex(); i < MXR_MAX; i++) term_type(0, i, " ", 1, attr_time);
    attryb = old_attr;
    setcursor(cursor);
    m_gotoxy(xkursora, ykursora);
    /* ----------------- animacja blankietow ------------------------ */
    {
        void** Ag;
        struct agenda** SA;
        int i, lzl, lza, kod_uslugi_blank;
        kod_uslugi_blank = kod_rap_aktual_blank(); /* kod_uslugi_blank=-1; */
        if (kod_uslugi_blank >= 0)
        {
            SA = getAgendaPtr(&lzl);
            Ag = (void**)Malloc(7 * lzl * sizeof(void*));
            if (Ag != NULL)
            {
                for (i = 0, lza = 0; i < lzl; i++)
                {
                    if (SA[i] == NULL) continue;
                    Ag[lza] = &SA[i]->number_of_calls;
                    lza++;
                    Ag[lza] = &SA[i]->delay;
                    lza++;
                    // moje...to ja
                    Ag[lza] = &SA[i]->rt.value;
                    lza++;
                    Ag[lza] = &SA[i]->rt.elapsed;
                    lza++;
                    Ag[lza] = &SA[i]->rt.val_max;
                    lza++;
                    Ag[lza] = &SA[i]->rt.val_min;
                    lza++;
                    Ag[lza] = &SA[i]->rt.alarm;
                    lza++;
                    //
                }
                animuj_blank(lza, Ag);
                free(Ag);
            }
            freeAgendaPtr();
        }
    }
    attryb = old_attr;
    setcursor(cursor);
    m_gotoxy(xkursora, ykursora);
    return A->state;
};

#define DEC_DALEJ  11
/* -------------------- dalsze stale w moje.h -------------------
#define DEC_TYP_US 2
#define DEC_NEW    3
	--------------------------------------------------------------- */
static int nr_ag = 0;

int sprawdz_blankiet(int nr_rekordu, int ob_pocz, int ob_konc,
                     int x_lewy_gorny, int y_lewy_gorny, int kod_uslugi, char* DaneUslugi)
/* ------------- D jest adresem zerowego rekordu bazy ----------------- */
{
    struct agenda *A;
    int ls, nr_rek, size, ochr, ochrf = -3, raport, ret;
    static char nazwa_buf[80];
    static char* nazwa_w = nazwa_buf;
    (void)DaneUslugi;
    nr_rek = nr_rekordu;
    A = (struct agenda*)dane_raportowanego_rekordu(sprawdz_blankiet, &nr_rek);
    if (A == NULL)
    {
        term_printf(MY_MAX, X_L0,ATTR_A, L_RAP_ERR_ADDR_SPRAWDZ, A);
        GET_char();
        return -1;
    }
    rekord_danych_do_naglowka(nr_ag);
    snprintf(nazwa_buf, sizeof(nazwa_buf), "%s", (A->name != NULL) ? A->name : "");
    ls = 0;
    {
        int i;
        for (i = 0; i < AG_SIZE; i++)
        {
            if (SysA[i] != NULL) ls++;
        }
    }
    ret = dana_koment(-1, 16, L_RAP_TITLE_BASIC, ls);

    if (A == NULL)
    {
        ret = dana_koment(-1, 10, L_RAP_NO_ORDER_AT_POS, nr_ag);
        return ret;
    }
    /* ------------------------------------------------------- */
    {
        static int z_min = 0, z_max = 0;
        ret = dana_rekord_str_dec(-1, -1, L_RAP_MAIN_ORDER, &z_min, &z_max, (int*)A,
                                  size = 0, ochr = -1, Service->kod_uslugi,
                                  raport = (A->S)->kod_uslugi, DEC_NEW,
                                  L_RAP_LIST_ROW_PRIOR,
                                  &nazwa_w, &(A->mode),
                                  &(A->prior), &(A->Interval));
    }
    {
        static char* typ_usl[3] = {L_CORE_TYP_PERM, L_CORE_TYP_SERIAL, L_CORE_TYP_ONESHOT};
        ret = dana_znak(-1, -1, L_RAP_TYP_USL_NO_PREFIX, "p/s/t", typ_usl, 3,
                        &(A->mode), ochr = -1);
    }
    {
        if (A->mode == 'p') ochr = -1;
        else ochr = 2;
        ret = dana_koment(-1, -1, L_RAP_PERMANENT_NOREM);
    }
    {
        static int nmin = 1, nmax = 3000, del_min, del_sek;
        del_min = (long int)A->delay / 60l;
        del_sek = ((long int)A->delay) % 60l;
        ret = dana_int(-1, -1, L_RAP_PERIOD_BR, &nmin, &nmax,
                       &(A->Interval), size = 4, ochr = 5, raport = -1);
        ret = dana_koment(-1, -1, L_RAP_TIME_TO_NEXT,
                          del_min, del_sek);
    }
    {
        static int nmin = 1, nmax = 3000, stmin = -999;
        ret = dana_int(-1, -1, L_RAP_PRIORITY, &nmin, &nmax,
                       &(A->prior), size = 4, ochr = 5, raport = -1);
        ret = dana_int(-1, -1, L_RAP_STATE_PROGRESS, &stmin, &nmax,
                       &(A->state), size = 4, ochrf, raport = -1);
    }

    if (ls == 0)
    {
        ret = dana_koment(-1, -1, "+ ");
    }
    {
        static char *usun[3] = {L_RAP_MENU_DATA_ENTRY, L_CORE_RUN_MAIN_MENU, L_RAP_MENU_ALG_DATA}, dec = 'm';
        ret = dana_decyzyjna(-1, -1, L_RAP_PROMPT_ACTION, "d/m/a", usun, 3,
                             &dec, ochr = 0, DEC_DALEJ);
    }


    /*    ret=dana_znak(-1,-1," Typ uslugi <%s>", "p/s/t", typ_usl, 3, &(A->mode), ochr=5);  */
    return ret;
}


char* dane_sprawdz(int ob_pocz, int ob_konc, int* rozmiar_ob)
{
    int ls;
    struct agenda** SA = getAgendaPtr(&ls);
    *rozmiar_ob = sizeof(struct agenda*);
    return (char*)SA;
}

void wpis_sprawdz(int ob_pocz, int ob_konc,
                  char* D, int rozmiar_ob, char zapis[], char* Kod_op)
{
    (void)D; (void)rozmiar_ob; (void)zapis; (void)Kod_op;
    (void)ob_pocz; (void)ob_konc;
}

/* -------------- def. w sys_dekl.h ----------------
struct agenda
 {struct Service *S;    / * adres danych stalych uslugi (struct Service *)* /
	int Interval;       / * okres wywolan * /
	int delay;          / * l.sekund do najblizszego wywolania * /
	char mode;            / * p stale; s seria; t - jednorazowe * /
	int number_of_calls; / * liczba wykonan: -1 stale; 0 usuwamy; >0 cykliczne * /
	int prior;         / * priorytet * /
	int prior_plus;    / * przyrost priorytetu po 1 sek.* /
	int state;         / * stan wykonania: 0 - zakoncz; 1 - poczatek; 2, .. dalej * /
	void *data;          / * dane dynamiczne * /
 } ----------------------------------------------------------------- */
int dec_sprawdz(int decyzja, int kod_decyzji, int nr_dec,
                int kod_raportu, int np, int* nr_rekordu)
{
    int p_min, p_max, ret;
    (void)czy_zdefiniowany(kod_raportu, &p_min, &p_max, &ret);
    (void)ustal_adres_rek(kod_raportu, *nr_rekordu);
    switch (kod_decyzji)
    {
    case DEC_DALEJ:
        if (decyzja == 'm') return -1;
        if (decyzja == 'a') return -1;
        break;
    case DEC_NEW:
        break;
    case DEC_TYP_US:
        break;
    default: break;
    }
    return kod_raportu;
}

/* --------------------------------------------------------------- */
