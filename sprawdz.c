/* #include <butil.h> */
#include "blank/moje.h"
#include "sprawdz.h"
#include <stdarg.h>
#include <stdio.h>
void chk_time(void);
extern struct Service* Service;
/* extern struct agenda *Agenda, *SysA[]; */
extern int X_time, X_tyt;
extern time_t sek_akt;
extern unsigned int attr_title, attryb, attr_time;
extern unsigned int cursor, nocursor;

char* dni[] = {"Niedziela", "Poniedzialek", "Wtorek", "Sroda", "Czwartek", "Piatek", "Sobota"};
extern struct agenda* SysA[];

static void sprawdz_log(const char* fmt, ...)
{
    FILE* fp;
    va_list ap;
    fp = fopen("/tmp/stud_sys_ctrl.log", "a");
    if (fp == NULL) return;
    fprintf(fp, "[SPRAWDZ] ");
    va_start(ap, fmt);
    vfprintf(fp, fmt, ap);
    va_end(ap);
    fprintf(fp, "\n");
    fclose(fp);
}

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
    term_printf(Y_G0, X_time, attr_time, " Dnia %02d.%02d.%02d ",
                Time->tm_mday, Time->tm_mon + 1, (Time->tm_year % 100));
    term_printf(Y_G0, m_wherex(), attr_time, "Godz.%02d:%02d.%02d (%s)",
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
            freeAgendaPtr();
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
                    Ag[lza] = &SA[i]->dana;
                    lza++;
                    Ag[lza] = &SA[i]->czas;
                    lza++;
                    Ag[lza] = &SA[i]->wart_max;
                    lza++;
                    Ag[lza] = &SA[i]->wart_min;
                    lza++;
                    Ag[lza] = &SA[i]->alarm;
                    lza++;
                    //
                }
                animuj_blank(lza, Ag);
                free(Ag);
            }
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
    sprawdz_log("enter blankiet nr_rekordu=%d kod_uslugi=%d ob=[%d,%d)", nr_rekordu, kod_uslugi, ob_pocz, ob_konc);
    A = (struct agenda*)dane_raportowanego_rekordu(sprawdz_blankiet, &nr_rek);
    if (A == NULL)
    {
        sprawdz_log("dane_raportowanego_rekordu returned NULL");
        term_printf(MY_MAX, X_L0,ATTR_A, " Blad adresu w Sprawdz (A=%p). <Ent> ", A);
        GET_char();
        return -1;
    }
    sprawdz_log("record ok A=%p name=%p mode=%c prior=%d interval=%d", (void*)A, (void*)A->name, A->mode, A->prior, A->Interval);
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
    ret = dana_koment(-1, 16, "+ Raport zlecen podstawowych SPRAWDZ (jest razem %d)", ls);

    if (A == NULL)
    {
        ret = dana_koment(-1, 10, "+ Brak zlecenia na poz.%d w agendzie", nr_ag);
        return ret;
    }
    /* ------------------------------------------------------- */
    {
        static int z_min = 0, z_max = 0, nrzl = 0;
        sprawdz_log("before dana_rekord_str_dec name='%s' mode=%c prior=%d interval=%d", nazwa_buf, A->mode, A->prior, A->Interval);
        ret = dana_rekord_str_dec(-1, -1, "+ Zlecenie glowne:", &z_min, &z_max, (int*)A,
                                  size = 0, ochr = -1, Service->kod_uslugi,
                                  raport = (A->S)->kod_uslugi, DEC_NEW,
                                  " %19S typ=%c prior=%d okres=%d",
                                  &nazwa_w, &(A->mode),
                                  &(A->prior), &(A->Interval));
        sprawdz_log("after dana_rekord_str_dec ret=%d nrzl=%d", ret, nrzl);
    }
    {
        static char* typ_usl[3] = {"p permanentna (stala)", "s seryjna", "t dorazna"};
        sprawdz_log("before dana_znak typ_usl");
        ret = dana_znak(-1, -1, " Typ uslugi <%s> ", "p/s/t", typ_usl, 3,
                        &(A->mode), ochr = -1);
        sprawdz_log("after dana_znak ret=%d mode=%c", ret, A->mode);
    }
    {
        if (A->mode == 'p') ochr = -1;
        else ochr = 2;
        sprawdz_log("before dana_koment stale info");
        ret = dana_koment(-1, -1, "+   Zlecenie stale nieusuwlne !!!! ");
        sprawdz_log("after dana_koment ret=%d", ret);
    }
    {
        static int nmin = 1, nmax = 3000, del_min, del_sek;
        del_min = (long int)A->delay / 60l;
        del_sek = ((long int)A->delay) % 60l;
        sprawdz_log("before dana_int interval interval=%d", A->Interval);
        ret = dana_int(-1, -1, " Okres wywolan [sek](%d-%d) ?? ", &nmin, &nmax,
                       &(A->Interval), size = 4, ochr = 5, raport = -1);
        sprawdz_log("after dana_int interval ret=%d interval=%d", ret, A->Interval);
        sprawdz_log("before dana_koment delay");
        ret = dana_koment(-1, -1, " Czas do nastepnego wywolania %dmin %dsek",
                          del_min, del_sek);
        sprawdz_log("after dana_koment delay ret=%d", ret);
    }
    {
        static int nmin = 1, nmax = 3000, stmin = -999;
        sprawdz_log("before dana_int prior prior=%d", A->prior);
        ret = dana_int(-1, -1, " Priorytet (%d-%d) ?? ", &nmin, &nmax,
                       &(A->prior), size = 4, ochr = 5, raport = -1);
        sprawdz_log("after dana_int prior ret=%d prior=%d", ret, A->prior);
        sprawdz_log("before dana_int state state=%d", A->state);
        ret = dana_int(-1, -1, " Stan realizacji ", &stmin, &nmax,
                       &(A->state), size = 4, ochrf, raport = -1);
        sprawdz_log("after dana_int state ret=%d state=%d", ret, A->state);
    }

    if (ls == 0)
    {
        sprawdz_log("before dana_koment plus");
        ret = dana_koment(-1, -1, "+ ");
        sprawdz_log("after dana_koment plus ret=%d", ret);
    }
    {
        static char *usun[3] = {"d wpis danych", "m menu glowne", "a dane algorytmow"}, dec = 'm';
        sprawdz_log("before dana_decyzyjna dec");
        ret = dana_decyzyjna(-1, -1, " Co robimy <%s>  ?? ", "d/m/a", usun, 3,
                             &dec, ochr = 0, DEC_DALEJ);
        sprawdz_log("after dana_decyzyjna ret=%d dec=%c", ret, dec);
    }


    /*    ret=dana_znak(-1,-1," Typ uslugi <%s>", "p/s/t", typ_usl, 3, &(A->mode), ochr=5);  */
    return ret;
}


char* dane_sprawdz(int ob_pocz, int ob_konc, int* rozmiar_ob)
{
    int ls;
    return (char*)getAgendaPtr(&ls);
}

void wpis_sprawdz(int ob_pocz, int ob_konc,
                  char* D, int rozmiar_ob, char zapis[], char* Kod_op)
{
    ;
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
    sprawdz_log("dec_sprawdz wejscie decyzja=%c kod_decyzji=%d nr_dec=%d kod_raportu=%d nr_rekordu=%d",
                decyzja, kod_decyzji, nr_dec, kod_raportu, *nr_rekordu);
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
