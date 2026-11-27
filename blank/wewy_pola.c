/* blank/wewy_pola.c - silnik formularzy:
   pola znakowe i grupowe, indeksy tablic, formaty, okno pomocy, wpis liczb (dawniej wewy1.c).
   Wspolne naglowki i deklaracje: wewy_wsp.h; globalne zmienne silnika sa
   zdefiniowane w wewy.c. */
#include "wewy_wsp.h"

/* ================### Tu zaczyna sie plik wewy1.c ================== */
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
    (void)R; (void)Rh;
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
    snprintf(bufor, sizeof(bufor), f_ind, wartn);
    snprintf(form, sizeof(form), "%%d");
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
            snprintf(bufor, sizeof(bufor), f_ind, wartn);
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
    int len;
    char* buf;
    if (format != NULL) { Free(format); lwmall--; }
    len = strlen(monit) + 1;
    buf = (char*)Malloc(len);
    if (buf == NULL)
    {
        komunikat(5, 1, ATTR_A, " Brak pamieci dla formatu danej %d: <Ent>", nr_danej);
        nr_danej--;
        GET_char();
        return NULL;
    }
    memcpy(buf, monit, len);
    lwmall++;
    return buf;
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
            *dl_zakr = snprintf(opis_danej, 80, fmon, *ind_min, *ind_max);
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
        snprintf(*f_ind, 6, " %dd", *ind_size);
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
    int xp, xpf = 0, xm, yp, ym, yd, Ym, y_kur = 0, x_kur = 0, n, ik, x, ind, ip_old, ind_size;
    char znak, **buf, *koniec = "<Esc>", form_dat[24], *form_d, *menu, *zle, jest;
    int dl_tyt = 0, opcja0 = 0;
    float w, *wart = NULL;
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
        snprintf(&form_d[1], sizeof(form_dat) - 1, "%dd", size);
        if (typ == FLOAT)
            snprintf(&form_d[1], sizeof(form_dat) - 1, "%d.%df", size, skok);
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
                case NLCR: ret = CR/*Get_Char()*/; /* fall through */
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
                                snprintf(dw->bufor, 10 * sizeof(char), dw->format, *wart);
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
    int xmax, xp, i, k, x_konc, zn, znw, lopt, iopt;
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
        snprintf(&opis_danej[dl_zakr], sizeof(opis_danej) - dl_zakr, fmon, opcje);
    else
        snprintf(&opis_danej[dl_zakr], sizeof(opis_danej) - dl_zakr, "[%s] ", opcje);
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
    char znk, znp, znf, nznak = EOS;
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
    long int skok = 0, shift;
    int dnrz = 0, nrz;
    char *format, *dana, *form, *fm, znak, text[80], *fdanej, kod, **menu;
    char *brak[1] = {" !? "}, *zn, f_bledu[10], wzor;
    int lenth = 80;
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
            Term_Typef(y, x, L_WEWY_MISSING_LONG, 0, attr);
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
            snprintf(text, 80, fdanej, *(int*)(dana));
            break;
        case 'f':
            snprintf(text, 80, fdanej, *(float*)(dana));
            break;
        case 'e':
            snprintf(text, 80, fdanej, *(double*)(dana));
            break;
        case 'c':
            snprintf(text, 80, fdanej, *(dana));
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
            /* Use local buffer to avoid modifying possibly read-only string */
            if (menu != NULL)
            {
                lenth += 2;
                {
                    char s_buf[82];
                    const char *s_src = &(*(char**)dana)[2];
                    int s_len = i > 2 ? i - 2 : 0;
                    int copy_len = (s_len < lenth - 2) ? s_len : (lenth - 2);
                    if (copy_len < 0) copy_len = 0;
                    if (copy_len > 80) copy_len = 80;
                    memcpy(s_buf, s_src, copy_len);
                    s_buf[copy_len] = EOS;
                    snprintf(text, 80, fdanej, s_buf);
                }
                /* fprintf(mystderr," text=%s###",text); */
            }
            else
            {
                char s_buf[82];
                int copy_len = (i < lenth) ? i : lenth;
                if (copy_len > 80) copy_len = 80;
                memcpy(s_buf, *(char**)dana, copy_len);
                s_buf[copy_len] = EOS;
                snprintf(text, 80, fdanej, s_buf);
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
                snprintf(text, 80, fdanej, (char*)dana);
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
    (void)format_ogr;
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
                    snprintf(text, 80, *format, wartn[nrkol]);
                    i = strlen(text);
                    if (ll == 1)
                    {
                        i += snprintf(&text[i], 80 - i, "%s", help_ogr);
                        i += snprintf(&text[i], 80 - i, format_bazy, baza);
                        i += snprintf(&text[i], 80 - i, "%s", format_tx);
                        i += snprintf(&text[i], 80 - i, format_wyn, baza * wartn[nrkol]);
                    }
                    if (zakr[ll][0] == EOS)
                        snprintf(&text[i], 80 - i, L_WEWY_OUT_OF_RANGE);
                    else
                    {
                        if (ll == 0)
                            snprintf(&text[i], 80 - i, zakres, " poza ", wmin, wmax, " <ENT> ");
                        if (ll == 1)
                            snprintf(&text[i], 80 - i, L_WEWY_OUT_OF_FMT, zakr_abs);
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
                    snprintf(bufor, 10 * sizeof(char), *format, wartn[nrkol]);
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
