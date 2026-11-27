/* zlecenia.c - wspolne formularze i decyzje dla modulow uslug RT
 * (budzik, czujnik symulowany): wybor rekordu zlecenia, edycja pol agendy,
 * lista zlecen i obsluga decyzji z blankietu.
 */
#include "blank/moje.h"
#include "loc.h"
#include "scheduler.h"
#include "zlecenia.h"
#include "core.h"


int pobierz_rekord_uslugi(int* nr_rekordu, int kod_uslugi, int ob_konc, struct agenda** As, struct agenda** Anew,
                          struct agenda*** SA, char* adres_rek0_uslugi, char* nazwa)
{
    struct agenda *A = NULL, *A0 = NULL, **Ab = NULL;
    int nr, i, nrR, lzl, nr_rek, ret = 0;
    A0 = (struct agenda*)adres_rek0_uslugi;
    nr_rek = *nr_rekordu;
    *SA = getAgendaPtr(&lzl);
    freeAgendaPtr();
    *As = NULL;
    if (A0 != *SA[0])
    {
        *nr_rekordu = -1;
        term_printf(MY_MAX, X_L0,ATTR_A, L_CORE_ERR_ADDR_MISMATCH, nazwa);
        czekaj_na_klawisz();
        return 0;
    }
    Ab = (struct agenda**)getRepDataPtr(&nrR);
    for (i = 0, nr = 0; i < ob_konc; i++)
    {
        Ab[i] = NULL;
        A = (struct agenda*)ustal_adres_rek(kod_uslugi, i);
        if (A == NULL) continue;
        if ((A->S)->kod_uslugi == kod_uslugi)
        {
            Ab[nr] = A;
            nr++;
        }
    }
    if (nr == 0 && *Anew == NULL)
    {
        char *menu_wybor[2] = {L_CORE_MENU_LAUNCH_NEW, L_CORE_MENU_NO};
        int wybor;
        dana_koment(-1, -1, "+ ");
        dana_koment(-1, -1, L_CORE_INFO_NOT_IN_AGENDA, nazwa);
        wybor = okno_menu(menu_wybor, 2, 0, attr, at_wpis, Y_G0+3, X_L0+1, -1, L_CORE_TITLE_LAUNCH_NEW, 1);
        if (wybor != 0)
        {
            *nr_rekordu = -1;
            return 0;
        }
        Service->decyzje('t', DEC_NEW, 0, kod_uslugi, 0, nr_rekordu);
        if (*Anew == NULL)
        {
            *nr_rekordu = -1;
            return 0;
        }
    }
    if (*Anew != NULL) /* zadeklarow. w sys_dekl.h */
    {
        nr++;
        nr_rek = nr;
        Ab[nr_rek] = *Anew;
    }
    else { nr_rek = ((*nr_rekordu) % nr); }
    Ab[0] = Ab[nr_rek];
    for (i = 1; i < nrR; i++) Ab[i] = NULL;
    A = Ab[0]; /* tylko jeden !!!! jest tu raportowany wiec tylko Ab[0]!=NULL */
    *As = A; /* Aserv zadeklarow. w sys_dekl.h */
    *nr_rekordu = nr_rek;
    return ret;
}

int dane_agendy(struct agenda* A, struct agenda* Anew, int cykl_max)
{
    int ret, ochr, size, raport, l_opcji;
    char typ_form[32];
    static char* opcje = NULL;

    strcpy(typ_form, L_CORE_FORM_SERVICE_TYPE);
    if (Anew != NULL) /* zadeklarow. w sys_dekl.h */
    {
        l_opcji = 5;
        opcje = "d/n/l/m/u";

        if (Anew->mode != 'n')
        {
            l_opcji = 6;
            opcje = "d/n/l/m/u/w";
            typ_form[0] = '*';
        }
        else
        {
            typ_form[0] = '#';
        }

        ochr = 5;
    }
    else
    {
        l_opcji = 5;
        opcje = "d/n/l/m/u";
        ochr = -1;
    }
    {
        static char* status[] = {
            L_CORE_STATUS_SKIP,
            L_CORE_STATUS_ALWAYS,
            L_CORE_STATUS_REPLACE
        };
        static int nmin = 0, nmax = 2;
        ret = dana_int_menu(-1, -1, L_CORE_FORM_ORDER_STATUS, &nmin, &nmax,
                            &(A->status), size = 1, ochr, raport = -1, 3, status);
    }
    {
        static char* typ_usl[] = {L_CORE_TYP_PERM, L_CORE_TYP_SERIAL, L_CORE_TYP_ONESHOT, L_CORE_TYP_PROMPT};
        ret = dana_decyzyjna(-1, -1, typ_form, "p/s/t", typ_usl, 4,
                             &(A->mode), ochr = 5, DEC_TYP_US);
    }
    {
        static int nmin = 1, nmax = 3000, nmin0 = 0;
        ret = dana_int(-1, -1, L_CORE_FORM_PRIORITY, &nmin, &nmax,
                       &(A->prior), size = 4, ochr = 5, raport = -1);
        ret = dana_int(-1, -1, L_CORE_FORM_PRIO_GROWTH, &nmin0, &nmax,
                       &(A->prior_plus), size = 4, ochr = 5, raport = -1);
    }
    {
        static float dmin = 0, dmax = 1.e10;
        int prec, kod_an;
        if (A->mode != 't' && cykl_max != 0)
        {
            static int nmin = 1, nmax;
            nmax = cykl_max;
            ret = dana_int(-1, -1, L_CORE_FORM_CYCLE_PERIOD, &nmin, &nmax,
                           &(A->Interval), size = 4, ochr = 5, raport = -1);
        }
        ret = dana_float_dec(-1, -1, L_CORE_FORM_NEXT_CALL,
                             &dmin, &dmax, &A->delay, size = 6, prec = 0,
                             ochr = 5, kod_an = 2, DEC_DELAY);
        if (A->number_of_calls < 0) ret = dana_koment(-1, -1, L_CORE_INFO_PERMANENT);
        else
        {
            static int min = 0, max = 32000;
            if (A->number_of_calls == 0)
            {
                ret = dana_koment(-1, -1, L_CORE_INFO_CANCELLED);
            }
            else
            {
                if (Anew != NULL)
                {
                    ret = dana_int(-1, -1, L_CORE_FORM_NUM_CALLS, &min, &max, &A->number_of_calls,
                                   size = 5, ochr = 4, raport = -1);
                }
                else
                    ret = dana_int(-1, -1, L_CORE_FORM_CALLS_LEFT, &min, &max, &A->number_of_calls,
                                   size = 5, ochr = -1, raport = -1);
            }
        }
    }
    ret = dana_koment(-1, -1, "+ ");
    {
        static char *uruch[] = {
                        L_CORE_RUN_DATA, L_CORE_RUN_NEXT, L_CORE_RUN_LIST,
                        L_CORE_RUN_MAIN_MENU, L_CORE_RUN_DELETE, L_CORE_RUN_ENTER
                    }, dec = 'd';
        dec = 'd';
        ret = dana_decyzyjna(-1, -1, L_CORE_FORM_WHAT_NEXT, opcje, uruch, l_opcji,
                             &dec, ochr = 0, DEC_RUN);
    }
    if (A->mode != 'n' && Anew == NULL)
    {
        static char *uruch[2] = {L_CORE_YES_SHORT, L_CORE_NO_SHORT}, dec = 'n';
        dec = 'n';
        wpis_danych_bez_potwierdz();
        ret = dana_decyzyjna(-1, -1, L_CORE_FORM_LAUNCH_NEW, "t/n", uruch, 2,
                             &dec, ochr = 1, DEC_NEW);
        wpis_danych_z_potwierdz();
    }
    return ret;
}


int pokaz_listy_zlecen(struct agenda* Anew, int kod_uslugi, char* tytul)
{
    int ret, i, nr, yp, xp, ramka, lbm;
    struct agenda **Ab, *A;
    ret = 0;
    if (Anew != NULL)
    {
        Free(Anew);
        lwmall--;
        Anew = NULL;
    }
    /* ------- zebranie adresow uslug budzika w agendzie do pokazania listy -- */
    Ab = (struct agenda**)getRepDataPtr(&lbm); /* zalozenie semafora */
    for (i = 0, nr = 0; i < lbm; i++)
    {
        A = SysA[i];
        if (A == NULL) continue;
        /* term_printf(MY_MAX, X_L0+1,ATTR_A," S->kod=%d/%d ",(A->S)->kod_uslugi,kod_uslugi); GET_char(); */
        if ((A->S)->kod_uslugi == kod_uslugi)
        {
            Ab[nr] = A;
            nr++;
        }
    }
    if (nr == 0) term_printf(m_wherey(), X_L0 + 3, attr, L_CORE_INFO_NO_ALARMS);
    else
    {
        ret = okno_menu_rekordow((char**)Ab, nr - 1, ret, attr, at_wpis,
                                 yp = 4, xp = X_L0 + 9, -1, tytul, ramka = 1,
                                 L_CORE_LIST_ROW,
                                 &Ab[0]->name, &(Ab[0]->mode),
                                 &(Ab[0]->prior), &(Ab[0]->number_of_calls));
    }
    freeRepDataPtr((void**)Ab); /* --- zwolnienie sefamora ---- */
    return ret;
}

void ustaw_typ_uslugi(struct agenda* A, int decyzja)
{
    switch (decyzja)
    {
    case 's':
        A->mode = 's';
        if (A->number_of_calls < 2) A->number_of_calls = 2;
        break;
    case 'p':
        A->mode = 'p';
        A->number_of_calls = -1;
        break;
    case 't':
        A->mode = 't';
        A->number_of_calls = 1;
        break;
    default: break;
    }
}

int decyzje_run(char decyzja, struct agenda** Aserv, struct agenda** Anew, int* nr_rekordu, int kod_uslugi, char* tytul)
{
    int ret = 0, i;
    switch (decyzja)
    {
    case 'n': (*nr_rekordu)++;
        break;
    case 'u': if ((*Anew) != NULL)
        {
            free_agenda_mem(*Anew);
            *Anew = NULL;
        }
        else (*Aserv)->number_of_calls = 0;
        break;
    case 'w': if (*Anew != NULL)
        {
            i = insert_to_agenda(*Anew);
            *Aserv = rekord_zlecenia_agendy(nr_rekordu, i, kod_uslugi); /*  Szukamy rekordu nowego zlecenia */
            free_agenda_mem(*Anew);
            *Anew = NULL;
        }
        break;
    case 'm': return -1;
    case 'l': ret = pokaz_listy_zlecen(*Anew, kod_uslugi, tytul);
        if (ret >= 0) *nr_rekordu = ret;
        ret = 0;
    default: break;
    }
    return ret;
}
