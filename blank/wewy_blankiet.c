/* blank/wewy_blankiet.c - silnik formularzy:
   rap_blank(): petla blankietu, obsluga wpisu, otwieranie i animacja blankietu (dawniej wewy4.c).
   Wspolne naglowki i deklaracje: wewy_wsp.h; globalne zmienne silnika sa
   zdefiniowane w wewy.c. */
#include "wewy_wsp.h"

/* ===================== Tu zaczyna sie plik wewy4.c ============= */

/*   ================================================================ */
int rap_blank(char* tytul, signed char* kod_raportu, int* nr_rekordu)
{
    int no, y, x, yp, xp, ret, wp, l_oblig = 0, dana = 0, znak, d0, y_shift;
    struct okno* ok;
    signed char nr_raportu;
    int rozmiar_ob, l_danych, ypocz;
    char x_tyt = 0, y_tyt, *d_oryg = NULL, *d_rek = NULL, *zmiany;
    static int wpisano = 0;
    unsigned int attr, tlo, attr_ok, attr_wp;
    unsigned int text_a, attr_h, attr_d;
    if (nr_blank < 0 || BL[nr_blank] == NULL || Ok == NULL)
    {
        komunikat_ent(MY_MAX, X_L0,ATTR_A, " Blankiet %d nie otwarty. <Ent>", nr_blank);
        return -1;
    }
    liczba_danych = BL[nr_blank]->liczba_danych;
    l_danych = liczba_danych;
    if (l_danych == 0)
    {
        komunikat_ent(5, 1,ATTR_A, " Brak danych blankietu %d: <Ent>", nr_blank);
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
            komunikat_ent(MY_MAX, X_L0,ATTR_A, " no=%d ld=%d typ=%c x=%d y=%d Ymax=%d/%d X0=%d ok->x=%d ok->y=%d <Ent> ",
                          no, l_danych, ok->typ, x, y, Y_maxBlank, MY_MAX, X_L0, ok->xo, ok->yo);
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
            wyjscie(Ok, liczba_danych);
            return -CR;
        }
        dana = d0;
        BL[nr_blank]->nr_danej = d0;
    }
    /* ------- tu zrobimy kopie oryg.danych i udost.wskaznik do zapis[] --- */
    d_oryg = dane_oryginalne(*nr_rekordu, tytul, &d_rek, &rozmiar_ob, &zmiany);
    if (zmiany == NULL)
    {
        static char zmiany_dummy = 0;
        zmiany = &zmiany_dummy;
    }
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
        term_type(MY_MAX, X_L0,
                  "|<Tab>-powrot do danych|<F2> -ZAPIS|<q> -WYJSCIE|<Esc> -wyjscie Z ANULOW.ZMIAN |", 0, attr_h);
        do
        {
            znak = GET_char();
            znak = znak & 0x00ff;
            if (znak == Esc) /* porzucenie blankietu, do menu   */
            {
                if (wpisano >= ZAP_MIN)
                {
                    term_type(MY_MAX, X_L0,
                              "|  Chcesz zakonczyc raport BEZ WPISU !!! <t/n> ?? |", 0,ATTR_A);
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
                              "|Wpisz dane OBLIGATORYJNE (ze znakiem ?? ) -<Ent>; lub wyjdz bez zapisu -<q>|", 0,
                              ATTR_A);
                    znak = czekaj_na_klawisz();
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
            komunikat_ent(MY_MAX, 1,ATTR_A, "%s. Brak miejsca na dane. Bedzie bierny  !! <Ent> ",
                          tytul);
            if (zn != 0) tytul[30] = zn;
            *rozmiar = 0;
            *zmiany = NULL;
            return NULL;
        }
        R->d_oryg[nr_rekordu - R->ob_pocz] = d_or;
        lwmall++;
        {
            int copy_size = R->rozmiar_ob;
            struct agenda* Arek = (struct agenda*)(*d_rek);

            /* Bound the raw copy to the real record size to avoid overread when
               report metadata and in-memory record size diverge. */
            if (Arek != NULL && Arek->S != NULL && Arek->S->str_size > 0 &&
                Arek->S->str_size < copy_size)
            {
                copy_size = Arek->S->str_size;
            }

            {
                size_t alloc_size = 0;
#if defined(__APPLE__)
                alloc_size = malloc_size(*d_rek);
#elif defined(__linux__)
                alloc_size = malloc_usable_size(*d_rek);
#endif
                if (alloc_size > 0 && (size_t)copy_size > alloc_size)
                {
                    copy_size = (int)alloc_size;
                }
            }

            przepisz(d_or, *d_rek, copy_size);
            if (copy_size < R->rozmiar_ob)
            {
                memset(d_or + copy_size, 0, (size_t)(R->rozmiar_ob - copy_size));
            }
        }
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
    signed char kolor = -1, ramka = -1, nrdan = -1;
    char* Dane = NULL;
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
            czekaj_na_klawisz();
            return (char*)-1;
        }
        if (ret == 0)
        {
            return (char*)-1;
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
        komunikat_ent(MY_MAX, 1,ATTR_A, " Czy_Blankiet_otwarty=%d ?? <Ent> ",
                      Czy_Blankiet_otwarty);
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
    if (ret == BLANKIET_UI_EXIT)
    {
        return (char*)-1;
    }
    if (ret < 0)
    {
        komunikat_ent(MY_MAX, X_L0,ATTR_A, " Brak danych w blankiecie %d: nr_danej=%d nr_ob=%d . <Ent> ",
                      nr_blank, nr_danej, *nr_ob);
        return (char*)-1;
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
    (void)dana;
    (void)kod;
    (void)wartosc;
    (void)proxy;
};
/* =================================================================== */
int obsluga_wpisu(int dana, struct okno* Ok, int liczba, unsigned int attr_dat[],
                  unsigned int attr_o[], unsigned int attrtx[], unsigned int attr_h,
                  unsigned int attr_inf, int* loblig, signed char* raport,
                  int* wpisano)
{
    int znak, stara, nowa, ret, l_oblig, z_menu, x, yo, xo, ind, y_shift = 0;
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
        ret = disp_dana(wp, ok->yo + Y_0, ok->xo + X_0, attr_dat[(unsigned char)mod], attr_o[(unsigned char)mod], attrtx[(unsigned char)mod], ok, y_shift, mod);
        if (wp < -2) wp = -wp;
        if (ret == -1 && wp < 3) return ret;
        if (wp == 1 || wp == -1 || (wp == 4 && wpisano_dana > 0))
        {
            if (ret > 0 || z_menu > 0 || wpisano_dana > 0 || (ok->decyzja >= 0 && wp == 1 && ret >= 0))
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
            {
                term_printf(MY_MAX, X_L0, attr_h,
                            "|  Wpis danej: <Esc> - wyjscie|  <Ent> - akceptacja | <%c, %c> - inkrementacja   |",
                            24, 25);
                term_cur(MY_MAX, X_L0 + 1);
                if (ok->blok_wpisu == 1)
                {
                    wp = 0;
                    mod = 1;
                    goto Q;
                } /* ### dodane 22.01.98 */
                else
                {
                    term_type(MY_MAX, X_L0 + 1, L_WEWY_DATA_ENTRY, 0, attr_o[2]);
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
            switch (help)
            {
            case 7: /* help= 1+2+4- raport+menu+tablica */
                term_printf(MY_MAX, X_L0, attr_h,
                            "|<%c,%c,%c,%c>-dana|<F1>-raport|<F3>-menu|<F4>-tabl",
                            24, 25, 26, arrow);
                if (ok->blok_wpisu == 0) term_type(MY_MAX, m_wherex(), "|<ENT>-edycja |<Esc,Tab>-wyjscie |", 0, attr_h);
                else term_type(MY_MAX, m_wherex(), "ica  | <Esc,Tab>-wyjscie z rap.  |", 0, attr_h);
                break;
            case 6: /* help= 2+4- menu+tablica */
                term_printf(MY_MAX, X_L0, attr_h,
                            "|<%c,%c,%c,%c>-wybor danej|<F3>-menu |<F4>-tablica",
                            24, 25, 26, arrow);
                if (ok->blok_wpisu == 0) term_type(MY_MAX, m_wherex(), "|<ENT>-edycja |<Esc,Tab>-wyjscie |", 0, attr_h);
                else term_type(MY_MAX, m_wherex(), "  | <Esc,Tab>-wyjscie z raportu  |", 0, attr_h);

                break;
            case 5: /* help= 1+4- raport+tablica */
                term_printf(MY_MAX, X_L0, attr_h,
                            "|<%c,%c,%c,%c>-wybor danej|<F1>-raport|<F4>-tablica",
                            24, 25, 26, arrow);
                if (ok->blok_wpisu == 0) term_type(MY_MAX, m_wherex(), "|<ENT>-edycja |<Esc,Tab>-wyjscie |", 0, attr_h);
                else term_type(MY_MAX, m_wherex(), "  | <Esc,Tab>-wyjscie z raportu  |", 0, attr_h);

                break;
            case 4: /* help= 4- tablica */
                term_printf(MY_MAX, X_L0, attr_h,
                            "|<%c,%c,%c,%c,Home,End>-wybor danej |<F4>- tablica",
                            24, 25, 26, arrow);
                if (ok->blok_wpisu == 0) term_type(MY_MAX, m_wherex(), "|<ENT>-edycja |<Esc,Tab>-wyjscie |", 0, attr_h);
                else term_type(MY_MAX, m_wherex(), " | <Esc,Tab> - wyjscie z raportu |", 0, attr_h);
                break;
            case 14: /* help= 14- tablica i grafika */
                term_printf(MY_MAX, X_L0, attr_h,
                            "|<%c,%c,%c,%c,Home,End>-wybor|<F1>-wykr.|<F4>-tabl",
                            24, 25, 26, arrow);
                if (ok->blok_wpisu == 0) term_type(MY_MAX, m_wherex(), "|<ENT>-edycja |<Esc,Tab>-wyjscie |", 0, attr_h);
                else term_type(MY_MAX, m_wherex(), " | <Esc,Tab> - wyjscie z raportu |", 0, attr_h);
                break;
            case 3:
                term_printf(MY_MAX, X_L0, attr_h,
                            "|<%c,%c,%c,%c>-wybor danej|<F1>-raport |<F3>-menu ",
                            24, 25, 26, arrow);
                if (ok->blok_wpisu == 0) term_type(MY_MAX, m_wherex(), "|<ENT>-edycja |<Esc,Tab>-wyjscie |", 0, attr_h);
                else term_type(MY_MAX, m_wherex(), "| <Esc,Tab> - wyjscie z raportu  |", 0, attr_h);
                break;
            case 2:
                term_printf(MY_MAX, X_L0, attr_h,
                            "|<%c,%c,%c,%c,Home,End>-wybor danej |<F1,F3>-menu ",
                            24, 25, 26, arrow);
                if (ok->blok_wpisu == 0) term_type(MY_MAX, m_wherex(), "|<ENT>-edycja |<Esc,Tab>-wyjscie |", 0, attr_h);
                else term_type(MY_MAX, m_wherex(), " | <Esc,Tab> - wyjscie z raportu |", 0, attr_h);
                break;
            case 1:
                term_printf(MY_MAX, X_L0, attr_h,
                            "|<%c,%c,%c,%c,Home,End>-wybor danej |<F1>-raport ",
                            24, 25, 26, arrow);
                if (ok->blok_wpisu == 0) term_type(MY_MAX, m_wherex(), "|<ENT>-edycja |<Esc,Tab>-wyjscie |", 0, attr_h);
                else term_type(MY_MAX, m_wherex(), " | <Esc,Tab> - wyjscie z raportu |", 0, attr_h);
                break;
            case 0:
                term_printf(MY_MAX, X_L0, attr_h, /*TERM_CYAN|INVERSE|TERM_FLUSH,*/
                            "|<%c,%c,%c,%c, Home, End> - wybor danej ",
                            24, 25, 26, arrow);
                if (ok->blok_wpisu == 0)
                    term_type(MY_MAX, m_wherex(), "|<ENT> - edycja danej |<Esc,Tab> - wyjscie |",
                              0, attr_h);
                else term_type(MY_MAX, m_wherex(), "do prezent.| <Esc,Tab> - wyjscie z raportu |", 0, attr_h);
                break;
            case 16:
                term_printf(MY_MAX, X_L0, attr_h, /*TERM_CYAN|INVERSE|TERM_FLUSH,*/
                            "|<%c,%c,%c,%c,Home,End>-wybor|<F1>-wykr.",
                            24, 25, 26, arrow);
                if (ok->blok_wpisu == 0)
                    term_type(MY_MAX, m_wherex(), "|<ENT> - edycja danej |<Esc,Tab> - wyjscie |",
                              0, attr_h);
                else term_type(MY_MAX, m_wherex(), "do prezent.| <Esc,Tab> - wyjscie z raportu |", 0, attr_h);
            default: break;
            }
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
                    term_printf(MY_MAX, X_L0, attr_h, /*TERM_CYAN|INVERSE|TERM_FLUSH,*/
                                "|  Wpis danej: <Esc> - wyjscie|  <Ent> - akceptacja | <%c, %c> - inkrementacja   |",
                                24, 25);
                    term_type(MY_MAX, X_L0 + 1, L_WEWY_DATA_ENTRY, 0, attr_o[2]);
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
                        while ((((ok->ochrona > Uprawn) && (ok->blok_wpisu == 0)) || (ok->yo == yo)) && (nowa != stara));
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
                        while ((((ok->ochrona > Uprawn) && (ok->blok_wpisu == 0)) || (ok->yo == yo)) && (nowa != stara));
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
                            term_printf(MY_MAX, X_L0, attr_h,
                                        "|<%c,%c,%c,%c,Home,End>-wybor opcji  | <Ent> - akceptacja | <Esc> - wyjscie z menu  |",
                                        24, 25, 26, arrow);
                            term_printf(MY_MAX, X_L0 + 1, attr_o[2], "<%c,%c,%c,%c,Home,End> - wybor opcji",
                                        24, 25, 26, arrow);
                        }
                        if ((help & 0x04) == 4 && (znak == FONT4))
                        {
                            mod = 2;
                            wp = 4; /* if(ok->blok_wpisu==1) wp=-4; ### */
                            if (ok->blok_wpisu == 1) wp = 5; /* wpis w tabl.dodane 27.01.98 */
                            term_printf(MY_MAX, X_L0, attr_h,
                                        "|<%c,%c,%c,%c,Home,End> - wybierz do edycji | <Ent> - akceptacja | <Esc> - wyjscie |",
                                        24, 25, 26, arrow);
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
    int no, wp, i = 0, y_shift = 0, xkursor, ykursor, yd, mod;
    int X_akt0, XX_max, Y_akt0, YY_max;
    struct okno *ok;
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
        if (ok->typ == 's' || ok->typ == 'S') continue; /* bez aktualizacji bo tablica */
        if (ok->typ == 'z' || ok->typ == 'Z') continue; /* bez aktualizacji bo tablica */
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
    term_cur(ykursor, xkursor); /*m_gotoxy(xkursor,ykursor); */
}

int kod_rap_aktual_blank(void)
{
    if (nr_blank < 0) return -1;
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
        L_WEWY_PERM_0,
        L_WEWY_PERM_1,
        L_WEWY_PERM_2,
        L_WEWY_PERM_3,
        L_WEWY_PERM_4,
        L_WEWY_PERM_5
    };
    char znak, zakres[] = "0-4";
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
        komunikat_ent(MY_MAX, X_L0, ATTR_A, "Nie zdefiniowany raport %d. <ENT> ", kod_raportu);
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
                                    L_WEWY_ASK_PERM_CODE, zakres, kod_upr, 6);
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
        komunikat_ent(MY_MAX, X_L0,ATTR_A, " Przekroczony limit blankietow (%d). <Ent>",LBLANK);
        nr_blank--;
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
        komunikat_ent(MY_MAX, X_L0,ATTR_A, " %s. Brak pamieci dla opisu blankietu %d. <Ent>", R->tytul, nr_blank);
        nr_blank--;
        if (zn != 0) R->tytul[30] = zn;
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
        komunikat_ent(MY_MAX, X_L0,ATTR_A, "%s. Brak pamieci dla blankietu %d: <Ent>",
                      R->tytul, nr_blank);
        nr_blank--;
        if (zn != 0) R->tytul[30] = zn;
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

