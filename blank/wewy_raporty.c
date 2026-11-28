/* blank/wewy_raporty.c - silnik formularzy:
   pola tekstowe, hasla i uprawnienia, definicje i pamiec raportow, zapis raportu (dawniej wewy2.c).
   Wspolne naglowki i deklaracje: wewy_wsp.h; globalne zmienne silnika sa
   zdefiniowane w wewy.c. */
#include "wewy_wsp.h"

/* ================### Tu zaczyna sie plik wewy2.c ================== */
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
    char *bufor, *format = NULL, text[80], *form = NULL;
    char textn[80], *buffer, *taknie = "<t/n>? ";
    int xmax, xp, ins, i, k, im = 0, x_konc, zn, txl, dltn = 0, dlstr, ms;
    char blok = 0;
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
    snprintf(format, xmax - x0 + 14, " -%ds", size);
    format[0] = '%';
    textn[0] = EOS;
    if (((monit[0] != '#') && (monit[1] != '#')) || wpis == 2)
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
                char *buf = NULL;
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
                w_min = 0;
                size_helpu = 0;
                sizh = 0;
                ret = okno_help(y + 1, xmax - size, xmax_h, attrtx, attr, 'g', w_min, w_max,
                                w_min, w_max, NULL /*&index_helpu*/, size_helpu,
                                sizh, skok, R, NULL, help, 0, NULL, 0, NULL, NULL, NULL); /* tu powinno byc R */
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
        snprintf(form, (xmax - x0 + 14) - 8, "%s s", monit);
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
        komunikat_ent(MY_MAX, X_L0,ATTR_A, L_WEWY_NO_REPORTS_OPEN);
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
        przepisz(haslo[(unsigned char)lhasel], hasl, l + 1);
        l = strlen(kod);
        if (l > 20)
        {
            kod[20] = EOS;
            l = 20;
        }
        przepisz(kod_o[(unsigned char)lhasel], kod, l + 1);
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
        komun_text(MY_MAX, X_L0, L_WEWY_ONLY_MAIN_TERM, 0,
                   ATTR_A);
        czekaj_na_klawisz();
        return 0;
    }
    if (nr_oper == 0) poziom_upr = 1;
    if (poziom_upr < Uprawn_max) poziom_upr = Uprawn_max;
    if (Rp != NULL && potrzebne_uprawn >= ZAP_MIN) /* zamykamy raporty */
    {
        Okno(MY_MAX, X_L0, MY_MAX, MXR_MAX, attrtx);
        clr_Okno(); /*term_flush(); */
        komun_text(MY_MAX, X_L0, tytul, 0,TERM_WHITE | INVERSE | TERM_BLACK_BG);
        form = L_WEWY_SAVE_OR_CANCEL;
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
            form = L_WEWY_ENTER_PASSWORD;
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
                           L_WEWY_BAD_PASSWORD_1, 0,ATTR_A);
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
                       L_WEWY_LOW_PERMS_1, 0,ATTR_A);
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
    char buf[10], znak, *form, sukces;
    int i, l, j, ret, lh, poziom_upr;
    unsigned int attrtx = TERM_WHITE | MTERM_HILIGHT | TERM_GREEN_BG;
    unsigned int attr_o = TERM_WHITE | TERM_MAGENTA_BG;
    poziom_upr = 0;
    if ((potrzebne_uprawn % 2) == 1 && nr_oper != 0)
    {
        komun_text(MY_MAX, X_L0, L_WEWY_NEED_MAIN_TERM, 0,ATTR_A);
        czekaj_na_klawisz();
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
            form = L_WEWY_ENTER_PASSWORD;
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
                           L_WEWY_BAD_PASSWORD_2, 0,ATTR_A);
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
                       L_WEWY_LOW_PERMS_2, 0,ATTR_A);
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
        komunikat_ent(MY_MAX, X_L0,ATTR_A, L_WEWY_NO_REPORTS_OPEN);
        return -1;
    }
    if (typ_bazy != BAZA_SPOJNA)
    {
        ob_konc = ob_konc - ob_pocz;
        ob_pocz = 0;
    }
    if (ob_konc < ob_pocz)
    {
        komunikat_ent(MY_MAX, 1,ATTR_A, "%s. Brak obiektow raportu !! <Ent> ", tytul);
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
        komunikat_ent(MY_MAX, 1,ATTR_A, "%s. Brak miejsca na dane !! <Ent> ",
                      tytul);
        if (zn != 0) tytul[40] = zn;
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
                komunikat_ent(MY_MAX, X_L0,ATTR_A, "%s. Brak pamieci na notatnik. Bedzie tylko bierny. <ENT> ",
                              tytul);
                if (zn != 0) tytul[30] = zn;
                R->uprawnienia = -1;
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
                    czekaj_na_klawisz();
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
    int i = 0, j, lrek;
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
    char opcja = 1;
    if (blokada_zapisu == ZAPIS_WSZYSTKICH_RAZEM) opcja = 0;
    return zamkniecie_raportow(opcja);
}

int zamkniecie_raportow(char kontr_spojnosci)
{
    int i = 0, j, lrek, ret = 0, Ret, poprz_dec = 0, wymag_uprawn = -1, zmiany;
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
    if (lwmall > lwmallp)
    {
        komunikat_ent(MY_MAX, X_L0,ATTR_A, L_WEWY_NEW_ALLOCS_SESS,
                      lwmallp, lwmall);
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
            komunikat(MY_MAX, X_L0,ATTR_A, L_WEWY_NO_REPORT, kod_raportu);
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
                char **baza;
                int k;
                baza = (char**)(R->D);
                k = rek_no;
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
    komunikat(MY_MAX, X_L0,ATTR_A, L_WEWY_NO_REPORT_DATA, kod_raportu);
    return NULL;
ERR_BRAK_RAP:
    komunikat(MY_MAX, X_L0,ATTR_A, L_WEWY_NO_REPORT, kod_raportu);
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
            komunikat_ent(MY_MAX, X_L0,ATTR_A, "%s. Brak pamieci na notatnik. Bedzie tylko bierny. <ENT> ",
                          R->tytul);
            if (zn != 0) R->tytul[30] = zn;
            R->uprawnienia = 0;
            R->d_oryg = NULL;
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
                czekaj_na_klawisz();
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
            czekaj_na_klawisz();
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
        czekaj_na_klawisz();
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
        komunikat_ent(MY_MAX, X_L0,ATTR_A, " Otworz blankiet dla rekordu %d. <ENT> ", *nr_rekordu);
        *nr_rekordu = -1;
        return NULL;
    }
    R = RAP[BL[nr_blank]->nr_raportu];
    if (R == NULL)
    {
        komunikat_ent(MY_MAX, X_L0,ATTR_A, " Otworz raport %d dla rekordu %d. <ENT> ",
                      BL[nr_blank]->nr_raportu, *nr_rekordu);
        *nr_rekordu = -1;
        return NULL;
    }
    if (R->def_blankiet != def_blank)
    {
        komunikat_ent(MY_MAX, X_L0,ATTR_A, " Niezgodnosc funkcji generujacej %s. <ENT> ", R->tytul);
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
    int i;
    struct reports* R = NULL;
    for (i = 0; i < LRAP_BL; i++)
    {
        if (RAP[i] == NULL) continue;
        if (RAP[i]->kod_raportu != kod_raportu) continue;
        R = RAP[i];
        R->tytul = tytul;
    }
}

void wpisz_rek_konc_raportu(int kod_raportu, int rek_pocz, int rek_konc)
{
    int i;
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
    char *buf, *adr_buf, znak = 0;
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
        komunikat_ent(MY_MAX, X_L0,ATTR_A, " Brak raportu %d do alokacji bufora %s(%d) i=%d <Ent>    ",
                      kod_raportu, txt, size, i);
        if (znak != 0) txt[TXM] = znak;
        return NULL;
    }
    if (size <= 0)
    {
        if (strlen(txt) > 0)
        {
            komunikat_ent(MY_MAX, X_L0,ATTR_A, " Niewlasciwy rozmiar=%d bufora %s dla raportu %d  <Ent>    ",
                          size, txt, kod_raportu);
        }
        if (znak != 0) txt[TXM] = znak;
        return NULL;
    }
    ds = sizeof(struct lista);
    buf = (char*)Malloc(size + ds);
    if (buf == NULL)
    {
        komunikat_ent(MY_MAX, X_L0,ATTR_A, " Brak pamieci na %s(%d) dla raportu %d (tot.siz=%d) <Ent> ",
                      txt, size, kod_raportu, total_size);
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
        komunikat_ent(MY_MAX, X_L0,ATTR_A, " Brak raportu %d do %s %s <Ent>    ",
                      kod_raportu, co, txt);
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
    (void)rekord;
    int liczba;
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
    int ds;
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
    (void)rekord;
    int i, liczba;
    char znak = 0, *bufn, *dalej;
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
        komunikat_ent(MY_MAX, X_L0,ATTR_A, " Brak wskazanego adresu na liscie: %s(%d) dla rap.%d (tot.siz=%d) <Ent> ",
                      txt, size, kod_raportu, total_size);
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
        komunikat_ent(MY_MAX, X_L0,ATTR_A, " Brak pamieci na realok.%s(%d) dla rap.%d (tot.siz=%d) <Ent> ",
                      txt, size, kod_raportu, total_size);
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
        komunikat_ent(MY_MAX, 0,ATTR_A, " Brak raportu %d do zapisu !!! Nacisnij <Ent> ", kod_raportu);
        return 0;
    }
    return zapis_raportu(R, poprz_dec, wymag_uprawn);
}

/* ------------------------------------------------------------------- */
int zapis_raportu(struct reports* Rp, int poprz_dec, int wymag_uprawn)
{
    signed char nr_raportu = -1, i;
    char zmiany = 0, *kod_operatora, *text = L_WEWY_ALL_REPORTS;
    struct reports* R;
    int lrek, ret = 0;
    if (Uprawn < 1 && wymag_uprawn > 0)
    {
        komunikat_ent(MY_MAX, 0,ATTR_A, " Za niskie uprawnienia (Uprawn=%d) <Ent> ", Uprawn);
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
                komunikat_ent(MY_MAX, 0,ATTR_A, " Brak blankietu. Zastosuj fkcje: Zapisz_raport(kod_raportu,%d,%d); <Ent> ",
                              poprz_dec, wymag_uprawn);
                return 0;
            }
        }
        nr_raportu = BL[nr_blank]->nr_raportu;
        if (nr_raportu < 0)
        {
            komunikat_ent(MY_MAX, X_L0,ATTR_A, " Bledny numer raportu %d w blank.%d. <Ent> ",
                          nr_raportu, nr_blank);
            if (poprz_dec == UNDO) return -1;
            return 0;
        }
        R = RAP[nr_raportu];
        if (R == NULL)
        {
            komunikat_ent(MY_MAX, X_L0,ATTR_A, " Brak raportu %d w blank.%d. <Ent> ",
                          nr_raportu, nr_blank);
            if (poprz_dec == UNDO) return -1;
            return 0;
        }
    }
    if (R->zapis == NULL)
    {
        komunikat_ent(MY_MAX, X_L0,ATTR_A, " Brak rejestracji zmian w raporcie %d, blank.%d. <Ent> ",
                      nr_raportu, nr_blank);
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
    if (nr_blank < 0) return;
    ok = Ok;
    for (i = 0; i < liczba_danych; i++, ok++)
    {
        if (ok->format != NULL)
        {
            Free(ok->format);
            lwmall--;
        }
        ok->format = NULL;
        ok->wpis = 0;
        /* help[0] is allocated by wpisz_format for types g/G/s/f/F/W/w/o */
        switch (ok->typ)
        {
        case 'g': case 'G': case 's':
        case 'f': case 'F': case 'W': case 'w': case 'o':
            if (ok->help[0] != NULL) { Free(ok->help[0]); lwmall--; }
            break;
        default:
            break;
        }
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
