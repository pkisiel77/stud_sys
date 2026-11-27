/* blank/wewy_liczby.c - silnik formularzy:
   pola liczbowe: okna float/double/int, tablice liczb, disp_dana (dawniej wewy5.c).
   Wspolne naglowki i deklaracje: wewy_wsp.h; globalne zmienne silnika sa
   zdefiniowane w wewy.c. */
#include "wewy_wsp.h"

/* ================### Tu zaczyna sie plik wewy5.c ================== */
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
        snprintf(monit, sizeof(monit), monit_pocz, w_min, w_max);
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
        snprintf(monit, sizeof(monit), "%s", monit_pocz);
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
    snprintf(format, xmax - x0 + 20, " %d.%de", size, precyzja);
    format[0] = '%';
    iobl = 1;
    if (monit[0] != '#' && monit[1] != '#') iobl = 0;
    if (iobl == 0 || wpis == 2)
        snprintf(bufor, 10 * sizeof(char), format, *wart);
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
        snprintf(form, (xmax - x0 + 20) - 13, "%s e", monit);
        form[(unsigned char)ms] = '%';
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
            snprintf(text, 80, format, wartn);
            i = strlen(text) + 1;
            if (zakres[0] == EOS)
                snprintf(&text[i], 80 - i, L_WEWY_OUT_OF_RANGE);
            else
                snprintf(&text[i], 80 - i, zakres, " poza ", w_min, w_max, " <ENT> ");
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
            czekaj_na_klawisz();
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
            zn = czekaj_na_klawisz();
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
            snprintf(&opis_danej[dl_zakr], sizeof(opis_danej) - dl_zakr, "%s", fmon);
        else
            snprintf(&opis_danej[dl_zakr], sizeof(opis_danej) - dl_zakr, fmon, w_min, w_max);
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
        snprintf(format, size + 15, " %dd", size);
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
    if ((iobl == 0) || ((wpis == 2) && (size > 0)))
        snprintf(bufor, size + 3, format, wartn);
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
        snprintf(form, 6, "%c%%d", hash);
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
                    snprintf(bufor, size + 3, format, wartn);
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
                snprintf(bufor, size + 3, format, wartn);
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
            zn = czekaj_na_klawisz();
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
    (void)y_shift;
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
        czekaj_na_klawisz();
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
        czekaj_na_klawisz();
        return -1;
    }
    if (nr_danej >= liczba_danych)
    {
        komunikat(MY_MAX, X_L0,ATTR_A, " Za duzo danych w blankiecie: nr_danej=%d l_danych=%d. <Ent> ",
                  nr_danej, liczba_danych);
        czekaj_na_klawisz();
        return -1;
    }
    ok = Ok + nr_danej;
    nr_danej++;
    ok->format = wpisz_format(ok->format, monit);
    if (ok->format == NULL) return -1;
    if (format != NULL) ok->help[0] = wpisz_format(ok->help[0], format);
    else if (ok->help[0] != NULL) { Free(ok->help[0]); lwmall--; ok->help[0] = NULL; }
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

int wpis_tabl_float(signed char wpis, int y, int x0,
                    unsigned int attr_d, unsigned int attr, unsigned int attrtx,
                    int* index, char* tabl, int dim, int L_kol, int size,
                    char precyzja, char* monit_pocz, int ind_min, int ind_max,
                    float w_min, float w_max, char* format_tabl, char* format_ogr,
                    float baza, float abs_min, float abs_max)
{
    float *wart = NULL, wartn[30];
    char *bufor, *format = NULL, text[80], *form = NULL, *c, *fh, typ;
    char zakres[80], *cc, *zz, *taknie = "<t/n>? ", znak;
    char hash = '*', f_nazwy[40], *f_ind = NULL, *fk_nazwy = NULL;
    char *fmon, *f_pocz, opis_danej[80];
    char help_ogr[80], *format_bazy = NULL, *format_tx = NULL, *format_wyn = NULL, *zakr_abs = NULL;
    int ret, xmax, xp, xp_ind, xp_naz, i, k, x_konc;
    int dlh_ogr, ll = 0, l = 0, j, n, poz_wart_abs = 0, txl, iobl = 0, dltn = 0;
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
                czekaj_na_klawisz();
                dlh_ogr = 0;
                break;
            }
            znak = fh[i];
            fh[i] = EOS;
            switch (k)
            {
            case 0: format_bazy = &help_ogr[l];
                ll = snprintf(zakres, sizeof(zakres), format, baza);
                fh[i] = znak;
                break;
            case 1: ll += snprintf(zakres, sizeof(zakres), format, baza * wartn[0]);
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
            snprintf(&opis_danej[dl_zakr], sizeof(opis_danej) - dl_zakr, "%s", fmon);
        else
        {
            snprintf(&opis_danej[dl_zakr], sizeof(opis_danej) - dl_zakr, fmon, w_min, w_max);
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
        dlh_ogr = ll + snprintf(zakr_abs, sizeof(help_ogr) - l, format, abs_min, abs_max);
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
    snprintf(format, size + 40, " %d.%df", size, precyzja);
    format[0] = '%';
    form = &format[11];
    bufor = &form[10 * sizeof(char)];
    bufor[0] = EOS;
    iobl = 1;
    if (hash != '#') iobl = 0;
    if (iobl == 0 || wpis == 2)
        snprintf(bufor, 10 * sizeof(char), format, *wart);
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
                snprintf(bufor, 10 * sizeof(char), format, wartn[nrkol]);
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
                        snprintf(form, size + 29, "%c%%f", hash);
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
        snprintf(form, size + 29, "%c%%f", hash);
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
                    snprintf(bufor, 10 * sizeof(char), format, wartn[nrkol]);
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
