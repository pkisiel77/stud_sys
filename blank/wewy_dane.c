/* blank/wewy_dane.c - silnik formularzy:
   API pol formularza: dana_*, tabl_*, grupa_*, dana_koment, blankiet() (dawniej wewy3.c).
   Wspolne naglowki i deklaracje: wewy_wsp.h; globalne zmienne silnika sa
   zdefiniowane w wewy.c. */
#include "wewy_wsp.h"

/* ========================= Tu zaczyna sie plik wewy3.c =========== */
void dane_helpu(char* hformat, char* help[], va_list* ap);

int sprawdz_nr_danej(void)
{
    if (Ok == NULL)
    {
        komunikat_ent(MY_MAX, X_L0,ATTR_A, " Brak adresu dla danej %d blankietu. <Ent> ",
                      nr_danej);
        return -1;
    }
    if (nr_danej >= liczba_danych)
    {
        komunikat_ent(MY_MAX, X_L0,ATTR_A, " Za duzo danych w blankiecie: nr_danej=%d l_danych=%d. <Ent> ",
                      nr_danej, liczba_danych);
        return -1;
    }
    return 0;
}
/* ========================================================== */
int okno_menu_rekordow(char* Menu[], int w_max, int opcja0,
                       unsigned int attr, unsigned int attr_o,
                       int Yp, int Xp, int Xm, char* tytul, char ramka,
                       char* hformat, ...)
{
    struct reports Rd, *R;
    char *help[LHELP];
    va_list _ap;
    int size, /* wydruk nr'u rekordu gdy >0 */dim = 0, xk, tx_size, i;
    va_start(_ap, hformat);
    R = &Rd;
    R->D = (char*)Menu;
    R->typ_bazy = BAZA_WEKTOR;
    R->tytul = tytul;
    R->kod_raportu = -1;
    R->ob_pocz = opcja0;
    R->ob_konc = w_max;
    help[0] = hformat;
    dane_helpu(hformat, help, &_ap);
    va_end(_ap);
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
    if (sprawdz_nr_danej() < 0) return -1;
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
        komunikat_ent(MY_MAX, X_L0,ATTR_A, " Za duzo danych w blankiecie: nr_danej=%d l_danych=%d. <Ent> ",
                      nr_danej, liczba_danych);
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
    int _r;
    va_list _ap;
    va_start(_ap, hformat);
    (Ok + nr_danej)->decyzja = kod_dec;
    (Ok + nr_danej)->Rep = NULL;
    (Ok + nr_danej)->Rep_help = NULL;
    _r = v_tabl_grup(y, x, monit, ind_min, ind_max, w_min, w_max,
                     format_tabl, tablica, dim, size, ochrona, skok, raport,
                     hformat, &_ap);
    va_end(_ap);
    return _r;
}

int tabl_grup(int y, int x, char* monit, int ind_min,
              int ind_max, int* w_min, int* w_max, char* format_tabl,
              int* tablica, int dim, int size, int ochrona,
              int skok, signed char raport, char* hformat, ...)
{
    int _r;
    va_list _ap;
    va_start(_ap, hformat);
    (Ok + nr_danej)->Rep = NULL;
    (Ok + nr_danej)->Rep_help = NULL;
    _r = v_tabl_grup(y, x, monit, ind_min, ind_max, w_min, w_max,
                     format_tabl, tablica, dim, size, ochrona, skok, raport,
                     hformat, &_ap);
    va_end(_ap);
    return _r;
}

int tabl_grup_posredn(int y, int x, char* monit, int ind_min,
                      int ind_max, int* w_min, int* w_max, char* format_tabl,
                      int* tablica, int dim, char* index_helpu,
                      int size_helpu, int size, int ochrona,
                      int skok, signed char raport, char* hformat, ...)
{
    int _r;
    va_list _ap;
    va_start(_ap, hformat);
    (Ok + nr_danej)->index_helpu = (int*)index_helpu;
    (Ok + nr_danej)->size_helpu = size_helpu;
    (Ok + nr_danej)->decyzja = -1;
    (Ok + nr_danej)->Rep = NULL;
    (Ok + nr_danej)->Rep_help = NULL;
    (Ok + nr_danej)->proxy = -1; /* to wskaznik raportu strukt.helpu */
    _r = v_tabl_grup(y, x, monit, ind_min, ind_max, w_min, w_max,
                     format_tabl, tablica, dim, size, ochrona, skok, raport,
                     hformat, &_ap);
    va_end(_ap);
    return _r;
}

int grupa_rap_posredn_danych(int y, int x, char* monit, int* w_min,
                             int* w_max, int* wart, char* index_helpu,
                             int size_helpu, int size, int ochrona,
                             int skok, signed char raport, char* hformat, ...)
{
    int _r;
    va_list _ap;
    va_start(_ap, hformat);
    (Ok + nr_danej)->index_helpu = (int*)index_helpu;
    (Ok + nr_danej)->size_helpu = size_helpu;
    (Ok + nr_danej)->decyzja = -1;
    (Ok + nr_danej)->Rep = NULL;
    (Ok + nr_danej)->Rep_help = NULL;
    (Ok + nr_danej)->proxy = 0; /* to wskaznik raportu strukt.helpu */
    _r = v_tabl_grup(y, x, monit, 0, 0, w_min, w_max, NULL, wart, 0, size,
                     ochrona, skok, raport, hformat, &_ap);
    va_end(_ap);
    return _r;
}

int grupa_rap_posredn_danych_dec(int y, int x, char* monit, int* w_min,
                                 int* w_max, int* wart, char* index_helpu,
                                 int size_helpu, int size, int ochrona, int skok,
                                 signed char raport, int kod_dec, char* hformat, ...)
{
    int _r;
    va_list _ap;
    va_start(_ap, hformat);
    (Ok + nr_danej)->index_helpu = (int*)index_helpu;
    (Ok + nr_danej)->size_helpu = size_helpu;
    (Ok + nr_danej)->decyzja = kod_dec;
    (Ok + nr_danej)->Rep = NULL;
    (Ok + nr_danej)->Rep_help = NULL;
    (Ok + nr_danej)->proxy = 0; /* to wskaznik raportu strukt.helpu */
    _r = v_tabl_grup(y, x, monit, 0, 0, w_min, w_max, NULL, wart, 0, size,
                     ochrona, skok, raport, hformat, &_ap);
    va_end(_ap);
    return _r;
}


int dana_rekord_str(int y, int x0, char* monit, int* w_min,
                    int* w_max, int* wart, int size, int ochrona,
                    int kod_raportu, signed char raport, char* hformat, ...)
{
    int _r;
    va_list _ap;
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
    va_start(_ap, hformat);
    (Ok + nr_danej)->index_helpu = wart;
    (Ok + nr_danej)->size_helpu = 0;
    /*  (Ok+nr_danej)->decyzja=-1; */
    _r = v_tabl_grup(y, x0, monit, 0, 0, w_min, w_max, NULL, wart, 0, size,
                     ochrona, R->rozmiar_ob, raport, hformat, &_ap);
    va_end(_ap);
    return _r;
}

int grupa_danych(int y, int x, char* monit, int* w_min,
                 int* w_max, int* wart, int size, int ochrona,
                 int skok, signed char raport, char* hformat, ...)
{
    int _r;
    va_list _ap;
    va_start(_ap, hformat);
    (Ok + nr_danej)->index_helpu = wart;
    (Ok + nr_danej)->size_helpu = 0;
    (Ok + nr_danej)->Rep = NULL;
    (Ok + nr_danej)->Rep_help = NULL;
    /*  (Ok+nr_danej)->decyzja=-1; */
    _r = v_tabl_grup(y, x, monit, 0, 0, w_min, w_max, NULL, wart, 0, size,
                     ochrona, skok, raport, hformat, &_ap);
    va_end(_ap);
    return _r;
}

int dana_rekord_posredn_str(int y, int x, char* monit, int* w_min,
                            int* w_max, int* wart, char* index_helpu,
                            int kod_raportu_helpu, int size, int ochrona,
                            int kod_raportu, signed char raport, char* hformat, ...)
{
    int _r;
    va_list _ap;
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
    va_start(_ap, hformat);
    (Ok + nr_danej)->index_helpu = (int*)index_helpu;
    (Ok + nr_danej)->size_helpu = R->rozmiar_ob;
    (Ok + nr_danej)->decyzja = -1;
    _r = v_tabl_grup(y, x, monit, 0, 0, w_min, w_max, NULL, wart, 0, size,
                     ochrona, skok, raport, hformat, &_ap);
    va_end(_ap);
    return _r;
}

int grupa_posredn_danych(int y, int x, char* monit, int* w_min,
                         int* w_max, int* wart, char* index_helpu,
                         int size_helpu, int size, int ochrona,
                         int skok, signed char raport, char* hformat, ...)
{
    int _r;
    va_list _ap;
    va_start(_ap, hformat);
    (Ok + nr_danej)->index_helpu = (int*)index_helpu;
    (Ok + nr_danej)->size_helpu = size_helpu;
    (Ok + nr_danej)->decyzja = -1;
    (Ok + nr_danej)->Rep = NULL;
    (Ok + nr_danej)->Rep_help = NULL;
    _r = v_tabl_grup(y, x, monit, 0, 0, w_min, w_max, NULL, wart, 0, size,
                     ochrona, skok, raport, hformat, &_ap);
    va_end(_ap);
    return _r;
}

int dana_rekord_str_dec(int y, int x, char* monit, int* w_min, int* w_max,
                        int* wart, int size, int ochrona, int kod_raportu,
                        signed char raport, char kod_dec, char* hformat, ...)
{
    int _r;
    va_list _ap;
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
    va_start(_ap, hformat);
    (Ok + nr_danej)->index_helpu = wart;
    (Ok + nr_danej)->size_helpu = 0;
    (Ok + nr_danej)->decyzja = kod_dec;
    _r = v_tabl_grup(y, x, monit, 0, 0, w_min, w_max, NULL, wart, 0, size,
                     ochrona, R->rozmiar_ob, raport, hformat, &_ap);
    va_end(_ap);
    return _r;
}


int grupa_danych_dec(int y, int x, char* monit, int* w_min, int* w_max,
                     int* wart, int size, int ochrona, int skok,
                     signed char raport, char kod_dec, char* hformat, ...)
{
    int _r;
    va_list _ap;
    (Ok + nr_danej)->index_helpu = wart;
    (Ok + nr_danej)->size_helpu = 0;
    (Ok + nr_danej)->decyzja = kod_dec;
    (Ok + nr_danej)->Rep = NULL;
    (Ok + nr_danej)->Rep_help = NULL;
    _r = v_tabl_grup(y, x, monit, 0, 0, w_min, w_max, NULL, wart, 0, size,
                     ochrona, skok, raport, hformat, &_ap);
    va_end(_ap);
    return _r;
}

int dana_rekord_posredn_str_dec(int y, int x, char* monit, int* w_min,
                                int* w_max, int* wart, char* index_helpu,
                                int kod_raportu_helpu, int size, int ochrona,
                                int kod_raportu, signed char raport, char kod_dec,
                                char* hformat, ...)
{
    int _r;
    va_list _ap;
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
    va_start(_ap, hformat);
    (Ok + nr_danej)->index_helpu = (int*)index_helpu;
    (Ok + nr_danej)->size_helpu = R->rozmiar_ob;
    (Ok + nr_danej)->decyzja = kod_dec;
    _r = v_tabl_grup(y, x, monit, 0, 0, w_min, w_max, NULL, wart, 0, size,
                     ochrona, skok, raport, hformat, &_ap);
    va_end(_ap);
    return _r;
}

int grupa_posredn_danych_dec(int y, int x, char* monit, int* w_min,
                             int* w_max, int* wart, char* index_helpu,
                             int size_helpu, int size, int ochrona,
                             int skok, signed char raport, int kod_dec,
                             char* hformat, ...)
{
    int _r;
    va_list _ap;
    va_start(_ap, hformat);
    (Ok + nr_danej)->index_helpu = (int*)index_helpu;
    (Ok + nr_danej)->size_helpu = size_helpu;
    (Ok + nr_danej)->decyzja = kod_dec;
    (Ok + nr_danej)->Rep = NULL;
    (Ok + nr_danej)->Rep_help = NULL;
    _r = v_tabl_grup(y, x, monit, 0, 0, w_min, w_max, NULL, wart, 0, size,
                     ochrona, skok, raport, hformat, &_ap);
    va_end(_ap);
    return _r;
}

/* ----------------------------------------------------------------- */
int v_tabl_grup(int y, int x, char* monit, int ind_min,
                int ind_max, int* w_min, int* w_max, char* format_tabl,
                int* tablica, int dim, int size, int ochrona,
                int skok, signed char raport, char* hformat, va_list* ap)
{
    struct okno* ok;
    if (sprawdz_nr_danej() < 0) return -1;
    ok = Ok + nr_danej;
    nr_danej++;
    ok->format = wpisz_format(ok->format, monit);
    if (ok->format == NULL) return -1;
    ok->help[0] = wpisz_format(ok->help[0], hformat);
    if (ok->help[0] == NULL) return -1;
    dane_helpu(hformat, ok->help, ap);
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
void dane_helpu(char* hformat, char* help[], va_list* ap)
{
    char *f = NULL, *form = NULL, typ, *zn;
    int lzm, k, lz, dlz = 0;
    char *_va_peek_cur = NULL;
    int _va_peeked = 0;
    lzm = 0;
    form = hformat;
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
            help[lzm] = va_arg(*ap, char*);
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
                komunikat_ent(MY_MAX, X_L0,ATTR_A, " Blad formatu %s w grupie (zn=%c, k=%d). <Ent>", form, *zn, k);
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
                    _va_peek_cur = va_arg(*ap, char*); /* biezacy arg */
                    {
                        char* _va_peek_nxt = va_arg(*ap, char*); /* nastepny arg */
                        if (lz >= LHELP || _va_peek_nxt == NULL)
                        {
                            komunikat_ent(MY_MAX, X_L0,ATTR_A, " Brak liczby danych dla spec. %s w grupie (zn=%c k=%d). <Ent>",
                                          form, *zn, k);
                            break;
                        }
                        help[lz] = _va_peek_nxt;
                    }
                    _va_peeked = 1;
                    zn += 2;
                    k = 6;
                }
                else
                {
                    komunikat_ent(MY_MAX, X_L0,ATTR_A, " Brak liczby danych dla spec. %s w grupie - zn=%c k=%d. <Ent>",
                                  form, *zn, k);
                    /*fprintf(mystderr,"\n zle dane_form=%s",zn); */
                    break;
                }
            }
            lzm++;
            if (lzm < LHELP)
            {
                if (_va_peeked)
                {
                    help[lzm] = _va_peek_cur; /* arg pre-read w [%a%l] */
                    _va_peeked = 0;
                }
                else
                {
                    help[lzm] = va_arg(*ap, char*);
                }
                lzm += dlz;
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
                    komunikat_ent(MY_MAX, X_L0,ATTR_A,
                                  " Brak klamry ] lub liczby stringow w formacie %s grupy (k=%d). <Ent>", form, k);
                    break;
                }
                if (typ != 's' && typ != 'S')
                {
                    komunikat_ent(MY_MAX, X_L0,ATTR_A, " Blad (s) formatu %s w grupie. <Ent>", form);
                    break;
                }
                lzm++;
                if (lzm == LHELP)
                {
                    komunikat_ent(MY_MAX, X_L0,ATTR_A, " Za duzo danych w grupie. <Ent>");
                    break;
                }
                help[lzm] = va_arg(*ap, char*);
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
    if (sprawdz_nr_danej() < 0) return -1;
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
    if (sprawdz_nr_danej() < 0) return -1;
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
    if (sprawdz_nr_danej() < 0) return -1;
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
                         long int skok, void* R, char* hformat, va_list* ap);

int dana_text_menu_rek(int y, int x, char* monit, char* string, int size,
                       int l_opcji, int ochrona,
                       int kod_raportu, char* hformat, ...)
{
    int _r;
    va_list _ap;
    struct reports* R = NULL;
    int i, skok;
    va_start(_ap, hformat);
    for (i = 0; i < LRAP_BL; i++)
    {
        R = RAP[i];
        if (R == NULL) { va_end(_ap); return nr_danej; }
        if (R->kod_raportu == kod_raportu) break;
    }
    if (i == LRAP_BL) { va_end(_ap); return nr_danej; }
    skok = R->rozmiar_ob;
    if (R->typ_bazy == BAZA_SPOJNA) R = NULL;
    _r = wpis_danej_text_menu(y, x, monit, string, size, l_opcji, ochrona, skok, R, hformat, &_ap);
    va_end(_ap);
    return _r;
}

int dana_text_menu(int y, int x, char* monit, char* string, int size,
                   int l_opcji, int ochrona,
                   long int skok, char* hformat, ...)
{
    int _r;
    va_list _ap;
    va_start(_ap, hformat);
    _r = wpis_danej_text_menu(y, x, monit, string, size, l_opcji, ochrona, skok, NULL, hformat, &_ap);
    va_end(_ap);
    return _r;
}

int wpis_danej_text_menu(int y, int x, char* monit, char* string, int size,
                         int l_opcji, int ochrona,
                         long int skok, void* R, char* hformat, va_list* ap)
{
    struct okno* ok;
    if (sprawdz_nr_danej() < 0) return -1;
    ok = Ok + nr_danej;
    nr_danej++;
    ok->Rep_help = R;
    ok->format = wpisz_format(ok->format, monit);
    if (ok->format == NULL) return -1;
    ok->help[0] = wpisz_format(ok->help[0], hformat);
    if (ok->help[0] == NULL) return -1;
    dane_helpu(hformat, ok->help, ap);
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
    vsnprintf(bufor, sizeof(bufor), &format[i], arg);
    va_end(arg);
    l = strlen(bufor) + 1;
    text = (char*)Malloc(l * sizeof(char));
    if (text == NULL)
    {
        komunikat_ent(MY_MAX, X_L0,ATTR_A, " Brak pamieci na komentarz: %s. <Ent> ",
                      bufor);
        return -1;
    }
    przepisz(text, bufor, l);
    if (sprawdz_nr_danej() < 0) return -1;
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
static int blankiet_ret_no_save(int ret)
{
    return (ret == Esc || ret == -CR || ret == UNDO || ret == TAB);
}

static int blankiet_ret_read_only(int ret)
{
    return (ret == -CR);
}

static int blankiet_ret_error(int ret)
{
    return (ret == -1);
}

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
        if (D == (char*)-1) return -Esc;
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
            if (blankiet_ret_no_save(Ret)) *zapis = 0; /* juz zapisano */
            if (Ret == UNDO) return UNDO;
            if (nr_blank < 0) break;
            if ((*kod_raportu) < 0)
            {
                Ret = -CR;
                break;
            }
            if (blankiet_ret_error(Ret))
            {
                /* Wyjscie z bledami */
                /* przepisz(d+(*nr_ob)*rozmiar_ob, Dr, rozmiar_ob); */
                *kod_raportu = zamknij_blankiet(nr_ob);
                break;
            }
            {
                char format_inny_rek[32];
                if (ob_pocz == ob_konc)
                    snprintf(format_inny_rek, sizeof(format_inny_rek), "| Chcesz WYJSC z RAPORTU ??? ");
                else
                    snprintf(format_inny_rek, sizeof(format_inny_rek), "| <%c,%c,%c,%c,Home,End>-inny rek", 24, 25, 26, arrow);

                if (*zapis == 1)
                {
                    term_printf(MY_MAX, 0, TERM_CYAN | INVERSE | TERM_FLUSH,
                                "|<Tab>-powrot do edycji | <%c,%c,%c,%c,Home,End>-inny rek|<F2>-ZAPIS |<Esc>-wyjscie |",
                                24, 25, 26, arrow);
                    term_printf(MY_MAX, 24, ATTR_A, "%s%s", format_inny_rek, "|<F2>-ZAPIS|");
                }
                else
                {
                    term_printf(MY_MAX, 0, TERM_CYAN | INVERSE | TERM_FLUSH,
                                "| <Tab> - powrot do edycji raportu | <%c,%c,%c,%c,Home,End>-inny rek|<Esc>-wyjscie |",
                                24, 25, 26, arrow);
                    term_printf(MY_MAX, X_L0 + 35, ATTR_A, "%s%s", format_inny_rek, "|");
                }
      }
                if (blankiet_ret_read_only(Ret))
                    komun_text(MY_MAX, 0, "|PREZENTACJA DANYCH bez prawa zmian", 0, TERM_CYAN | INVERSE | TERM_FLUSH);
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
        case TAB: if (blankiet_ret_read_only(Ret)) continue;
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
                if (blankiet_ret_read_only(Ret)) continue;
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
