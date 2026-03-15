#include "blank/moje.h"
#include "dek_budz.h"
void chk_time(void);
extern struct Service *Service;
/* extern struct agenda *Agenda; */
extern unsigned int attr_title;
extern unsigned int cursor, nocursor;


extern int X_time, X_tyt;
extern unsigned int attr,at_wpis;
extern time_t sek_akt;
#define S_MIN 60l
/* ------------------ kody decyzji sa w moje.h -------------
#define DEC_NEW    1
#define DEC_RUN    2
#define DEC_TYP_US 3
#define DEC_TIME   4
#define DEC_DELAY  5
 ----------------------------------------------------------- */
#define DEC_MOD_TIME 16
extern int AgendaMax, lwmall;
extern struct agenda *SysA[];
static struct agenda *Anew = NULL, *Aserv = NULL; /* stan lokalny uslugi budzik */
int dane_agendy(struct agenda *A, struct agenda *An, int cykl_max);
int pobierz_rekord_uslugi(int *nr_rekordu, int kod_uslugi, int ob_konc,
									struct agenda **As, struct agenda **An,
									struct agenda ***SA, char *adres_rek0_uslugi, char *nazwa);
int decyzje_run(char decyzja, struct agenda **Aserv, struct agenda **Anew,
                  int *nr_rekordu, int kod_uslugi, char *tytul);
int pokaz_listy_zlecen(struct agenda *Anew, struct agenda ***SA,
												 int kod_uslugi, char *tytul);
void ustaw_typ_uslugi(struct agenda *A, int decyzja);
static void budz_format_pozostalo(float delay, char *buf, size_t buflen);
/* ============ Persystencja budzikow ================================ */

struct budz_rekord_pliku {
    char nazwa[78];
    int  godz;
    int  min;
    int  co_ile;
    int  wyprzedz;
    char mode;
    char _pad[3];
};

static const char *budz_sciezka(void)
{
    static char path[256];
    const char *home = getenv("HOME");
    if (home) snprintf(path, sizeof(path), "%s/.stud_sys_budz.dat", home);
    else       snprintf(path, sizeof(path), "./budz.dat");
    return path;
}

void budz_zapisz_do_pliku(void)
{
    FILE *fp;
    int i, count = 0;
    struct agenda *A;
    struct budzik *B;
    struct budz_rekord_pliku rec;

    for (i = 1; i <= AgendaMax; i++) {
        A = SysA[i];
        if (A == NULL || A->number_of_calls == 0) continue;
        if ((A->S)->decyzje == dec_budz) count++;
    }

    fp = fopen(budz_sciezka(), "wb");
    if (fp == NULL) return;

    fwrite(&count, sizeof(int), 1, fp);

    for (i = 1; i <= AgendaMax; i++) {
        A = SysA[i];
        if (A == NULL || A->number_of_calls == 0) continue;
        if ((A->S)->decyzje != dec_budz) continue;
        B = (struct budzik *)(A->data);
        memset(&rec, 0, sizeof(rec));
        strncpy(rec.nazwa, B->nazwa, sizeof(rec.nazwa) - 1);
        rec.godz     = B->godz;
        rec.min      = B->min;
        rec.co_ile   = B->co_ile;
        rec.wyprzedz = B->wyprzedz;
        rec.mode     = A->mode;
        fwrite(&rec, sizeof(rec), 1, fp);
    }
    fclose(fp);
}

void budz_wczytaj_z_pliku(void)
{
    FILE *fp;
    int count, i;
    struct agenda *A;
    struct budzik *B;
    struct budz_rekord_pliku rec;

    fp = fopen(budz_sciezka(), "rb");
    if (fp == NULL) return;

    if (fread(&count, sizeof(int), 1, fp) != 1 || count < 0 || count > 100) {
        fclose(fp); return;
    }

    for (i = 0; i < count; i++) {
        if (fread(&rec, sizeof(rec), 1, fp) != 1) break;
        A = service_default(dec_budz);
        if (A == NULL) break;
        B = (struct budzik *)(A->data);
        memset(B, 0, sizeof(struct budzik));
        strncpy(B->nazwa, rec.nazwa, sizeof(B->nazwa) - 1);
        B->godz     = rec.godz;
        B->min      = rec.min;
        B->co_ile   = (rec.co_ile  > 0) ? rec.co_ile  : 2;
        B->wyprzedz = (rec.wyprzedz > 0) ? rec.wyprzedz : 15;
        A->mode = (rec.mode == 's' || rec.mode == 'p' || rec.mode == 't')
                  ? rec.mode : 's';
        ustaw_czas_budzika(A, B);
        insert_to_agenda(A);
        free_agenda_mem(A);
    }
    fclose(fp);
}

/* ================================================================ */
int budz_main(void *DA)
 {struct agenda *A;
	struct budzik *B;
	int ret;
	char tytul[75], *M[3],text[75],text1[75];
	A=(struct agenda *)DA;
	B=(struct budzik *)(A->data);
	setalarm(); /* for(i=0;i<10;i++) printf("\a"); */
	snprintf(tytul,sizeof(tytul)," BUDZIK: <Ent> - wylaczenie sygnalu; <Esc> - usuniecie monitu ");
	snprintf(text,sizeof(text),"     Zgloszono na godz.%02d.%02d",B->godz,B->min);
	snprintf(text1,sizeof(text1),"     Najblizszy monit za %dmin.%02dsek.", (int)A->Interval/60,(int)A->Interval%60);
	{
	 setcursor(nocursor);
	 M[0]=B->nazwa;
	 M[1]=text; M[2]=text1;
	 do
		{ret=okno_menu(M,3,0,attr,at_wpis, Y_G0+3,X_L0+1,-1,tytul,1);
		 clearalarm(); nosound();
		}while(ret>=0);
	 setcursor(cursor);
//	 restore_var(buf_var);
//	 Free(buf_var);
	}
	{int sek, min, minB, dmin, godz, min_doby;
	 long int ss;
	 min_doby=czas_zegarowy(&godz, &min, &sek); 	/* oddaje minute od godz.0.0 (stud_sys.c) */
	 minB=B->godz*60+B->min;
	 dmin=minB-min_doby; if(dmin<0) dmin+=1440;
	 if(dmin<B->co_ile)
		{B->co_ile=dmin; ss=B->co_ile*60l; if(ss>32000) ss=32000;
		 A->Interval=(int)ss;
		}
	}
	A->state=0;
	return A->state;
 }

/* --------------------------------- dek_budz.h ----------
struct budzik
 {char nazwa[78];
	int godz;
	int min;
	int sek;
	int co_ile;
	int wyprzedz;
 }; ------------------------------------------------------ */
/* ---------- Pomocnicze: wczytaj liczbe w wierszu statusu ---------- */
static int budz_czytaj_int(const char *prompt, int min_val, int max_val, int current)
{
    char buf[12];
    int len, ch, val;
    snprintf(buf, sizeof(buf), "%d", current);
    len = (int)strlen(buf);
    setcursor(cursor);
    for (;;) {
        term_printf(MY_MAX, X_L0, ATTR_A,
                    " %s [%d-%d]: %-8s  (Enter=zatwierdz  Esc=anuluj)", prompt, min_val, max_val, buf);
        ch = getch();
        if (ch == '\n' || ch == '\r' || ch == KEY_ENTER) break;
        if (ch == 27) { len = -1; break; }
        if ((ch == KEY_BACKSPACE || ch == 127 || ch == 8) && len > 0) {
            buf[--len] = 0;
        } else if (ch >= '0' && ch <= '9' && len < (int)(sizeof(buf) - 2)) {
            buf[len++] = ch; buf[len] = 0;
        }
    }
    term_printf(MY_MAX, X_L0, ATTR_A, "%-79s", " ");
    setcursor(nocursor);
    if (len <= 0) return current;
    val = atoi(buf);
    if (val < min_val) val = min_val;
    if (val > max_val) val = max_val;
    return val;
}

/* ---------- Pomocnicze: wczytaj tekst w wierszu statusu ----------- */
static void budz_czytaj_text(char *buf, int max_len, const char *prompt)
{
    char tmp[128];
    int len, ch;
    strncpy(tmp, buf, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = 0;
    len = (int)strlen(tmp);
    setcursor(cursor);
    for (;;) {
        term_printf(MY_MAX, X_L0, ATTR_A,
                    " %s: %-44s  (Enter=zatwierdz  Esc=anuluj)", prompt, tmp);
        ch = getch();
        if (ch == '\n' || ch == '\r' || ch == KEY_ENTER) break;
        if (ch == 27) { len = -1; break; }
        if ((ch == KEY_BACKSPACE || ch == 127 || ch == 8) && len > 0) {
            tmp[--len] = 0;
        } else if (ch >= 32 && ch < 256 && len < max_len - 1) {
            tmp[len++] = ch; tmp[len] = 0;
        }
    }
    term_printf(MY_MAX, X_L0, ATTR_A, "%-79s", " ");
    setcursor(nocursor);
    if (len > 0) {
        strncpy(buf, tmp, max_len - 1);
        buf[max_len - 1] = 0;
    }
}

/* ====================== BLANKIET BUDZIKA — LISTA I ZARZADZANIE ===== */

/* Formularz edycji jednego budzika.
   Zwraca indeks wybranej pozycji (>= 3 = akcja, < 0 = Escape). */
static int budz_formularz(struct agenda *A, struct budzik *B,
                           char *tytul, char **menu, int nmenu)
{
    char e0[80], e1[80], e2[80];
    int ew = 0, w;
    for (;;) {
        snprintf(e0, sizeof(e0), "  Sprawa  : %.52s", B->nazwa);
        snprintf(e1, sizeof(e1), "  Godzina : %02d:%02d", B->godz, B->min);
        snprintf(e2, sizeof(e2), "  Co ile  : %d min  (wyprzedz: %d min)",
                 B->co_ile, B->wyprzedz);
        menu[0] = e0; menu[1] = e1; menu[2] = e2;
        w = okno_menu(menu, nmenu, ew, attr, at_wpis,
                      Y_G0 + 2, X_L0 + 2, -1, tytul, 1);
        ew = (w >= 0) ? w : 0;
        switch (w) {
        case 0:
            budz_czytaj_text(B->nazwa, sizeof(B->nazwa), "Nazwa budzika");
            break;
        case 1:
            B->godz     = budz_czytaj_int("Godzina", 0, 23, B->godz);
            B->min      = budz_czytaj_int("Minuta",  0, 59, B->min);
            B->mod_time = 1;
            break;
        case 2:
            B->co_ile   = budz_czytaj_int("Co ile minut",       1, 1440, B->co_ile);
            B->wyprzedz = budz_czytaj_int("Wyprzedzenie [min]", 1, 1440, B->wyprzedz);
            break;
        default:
            return w; /* akcja lub Escape */
        }
    }
}

int budz_blankiet(int nr_rekordu, int ob_pocz, int ob_konc,
                  int x_lewy_gorny, int y_lewy_gorny, int kod_uslugi,
                  char *adres_rek0_uslugi)
{
    struct agenda *lista[AG_SIZE];
    struct agenda *A;
    struct budzik *B;
    char mstr[AG_SIZE + 3][80];
    char *menu_l[AG_SIZE + 3];
    static int lw = 0;
    int n, i, w;
    (void)nr_rekordu; (void)ob_pocz; (void)ob_konc;
    (void)x_lewy_gorny; (void)y_lewy_gorny; (void)adres_rek0_uslugi;

    for (;;) {
        /* --- Zbierz aktywne budziki z agendy --- */
        n = 0;
        for (i = 1; i <= AgendaMax && n < AG_SIZE; i++) {
            char pozostal[20];
            A = SysA[i];
            if (A == NULL || A->number_of_calls == 0) continue;
            if ((A->S)->decyzje != dec_budz) continue;
            lista[n] = A;
            B = (struct budzik *)(A->data);
            budz_format_pozostalo(A->delay, pozostal, sizeof(pozostal));
            snprintf(mstr[n], 80, "  %02d:%02d  [%8s]  %-33s",
                     B->godz, B->min, pozostal, B->nazwa);
            menu_l[n] = mstr[n];
            n++;
        }
        snprintf(mstr[n],     80, "  +++ Dodaj nowy budzik +++");
        snprintf(mstr[n + 1], 80, "  Wyjdz");
        menu_l[n]     = mstr[n];
        menu_l[n + 1] = mstr[n + 1];
        if (lw > n + 1) lw = 0;

        w = okno_menu(menu_l, n + 2, lw, attr, at_wpis,
                      Y_G0 + 1, X_L0, -1, " LISTA BUDZIKOW ", 1);
        lw = (w >= 0) ? w : 0;

        if (w < 0 || w == n + 1) return BLANKIET_UI_EXIT;

        if (w == n) {
            /* ---------- Nowy budzik ---------- */
            int godz, min, sek;
            char *mn[5] = {NULL, NULL, NULL,
                           "  >>> Wpisz do agendy <<<",
                           "  Anuluj"};
            A = service_default(dec_budz);
            if (A == NULL) continue;
            A->mode = 's';
            B = (struct budzik *)(A->data);
            czas_zegarowy(&godz, &min, &sek);
            B->godz = (godz + 1) % 24; B->min = min;
            B->co_ile = 2; B->wyprzedz = 15;

            w = budz_formularz(A, B, " NOWY BUDZIK ", mn, 5);
            if (w == 3) {
                ustaw_czas_budzika(A, B);
                insert_to_agenda(A);
                free_agenda_mem(A);
                budz_zapisz_do_pliku();
                lw = 0;
            } else {
                free_agenda_mem(A);
            }

        } else {
            /* ---------- Edytuj istniejacy ---------- */
            char *me[6] = {NULL, NULL, NULL,
                           "  >>> Zatwierdz zmiany <<<",
                           "  >>> Usun budzik <<<",
                           "  Wyjdz (bez zmian)"};
            A = lista[w];
            B = (struct budzik *)(A->data);

            w = budz_formularz(A, B, " BUDZIK ", me, 6);
            if (w == 3) {
                ustaw_czas_budzika(A, B);
                budz_zapisz_do_pliku();
            } else if (w == 4) {
                A->number_of_calls = 0;
                budz_zapisz_do_pliku();
                if (lw >= n - 1) lw = 0;
            }
        }
    }
}

static void budz_format_pozostalo(float delay, char *buf, size_t buflen)
{
    long total;
    long hh, mm, ss;
    if (buf == NULL || buflen == 0) return;
    if (delay < 0.0f) delay = 0.0f;
    total = (long)(delay + 0.5f);
    hh = total / 3600L;
    mm = (total % 3600L) / 60L;
    ss = total % 60L;
    if (hh > 0)
        snprintf(buf, buflen, "%02ld:%02ld:%02ld", hh, mm, ss);
    else
        snprintf(buf, buflen, "%02ld:%02ld", mm, ss);
}

extern time_t sek_akt;
char *dane_budz(int ob_pocz, int ob_konc, int *rozmiar_ob)
 {static struct Service *S;
	static struct agenda *A;
	struct budzik *Bu;
	int x,y, i;
	int min, sek, godz;
	S=Service;
	*rozmiar_ob=sizeof(struct agenda)+sizeof(struct budzik);
	A=(struct agenda *)Malloc(*rozmiar_ob);
	A->S=S; A->data=(A+1);
	Bu=(struct budzik *)(A->data);
	A->mode='s'; A->prior=1;
	for(i=0;i<10;i++) Bu->nazwa[i]=0; Bu->nazwa[i]=0;
	czas_zegarowy(&godz, &min, &sek);  /* oddaje minute od godz.0.0 (stud_sys.c) */
	Bu->min=min; Bu->godz=godz+1;
	if(Bu->godz>=24) Bu->godz-=24;
	Bu->co_ile=2; Bu->wyprzedz=15;
	x=m_wherex(); y=m_wherey(); setcursor(nocursor);
	term_printf(Y_G0,X_tyt,attr_title,"%s",S->name);
	term_cur(y,x); setcursor(cursor);
	return (char *)A;
 }

void wpis_budz(int ob_pocz, int ob_konc,
					 char *D, int rozmiar_ob, char zapis[], char *Kod_op)
 {struct agenda *A;
	 A=(struct agenda *)D;
	 (void)A;
 }

void ustaw_czas_budzika(struct agenda *A, struct budzik *B);
void ustaw_delay_budzika(struct agenda *A, struct budzik *B);

void ustaw_czas_budzika(struct agenda *A, struct budzik *B)
{
	int sek, min, godz, minB, dmin, minp, min_doby;
	if (B->godz >= 24) B->godz -= 24;
	if (B->wyprzedz <= 0) A->mode = 't';
	if (A->mode == 't')
	{
		A->number_of_calls = 1; /*B->wyprzedz=0;*/ B->co_ile = B->wyprzedz;
	}
	else
	{
		if (B->co_ile < 1) B->co_ile = 1;
		if (A->mode == 'p')  A->number_of_calls = -1; /* jest to usluga permanentna */
		else
		{
			A->number_of_calls = B->wyprzedz / B->co_ile + 1;
		}
		if (B->co_ile > B->wyprzedz) B->co_ile = B->wyprzedz;
	}
	{long int ss;
	ss = B->co_ile * 60l; if (ss > 32000) ss = 32000;
	A->Interval = (int)ss;
	}
	min_doby = czas_zegarowy(&godz, &min, &sek); 	/* oddaje minute od godz.0.0 (stud_sys.c) */
	minB = B->godz * 60 + B->min;
	minp = minB - B->wyprzedz;
	if (minp < 0)   /* poczatek alarmu we wczesn.dobie */
	{
		minp += 1440;
		dmin = minp - min_doby;  /* l.minut do pierwszego alarmu */
		if (dmin < 0) { dmin = 0; } /* pocz.alarmu w aktualnej dobie nie pozniej niz min_doby */
	}
	else        /* poczatek alarmu w tej dobie co budzik */
	{
		dmin = minp - min_doby;  /* l.minut do pierwszego alarmu */
		if (min_doby > minB) /* budzik na nastepna dobe */
		{
			if (dmin < 0) { dmin += 1440; }
		} /* pocz.alarmu w nastepnej dobie */
		else             /* budzik w aktualnej dobie i 1szy alarm tez (minp>0)*/
		{
			if (dmin < 0) { dmin = 0; }
		} /* pocz.alarmu w aktualnej dobie nie pozniej niz min_doby */
	}
	if (dmin <= 0) A->delay = 0;
	else { A->delay = (float)(dmin*S_MIN) - sek; }
	B->mod_time = 0;
	B->poprz_godz_budz = B->godz;  B->poprz_min_budz = B->min;
}

void ustaw_delay_budzika(struct agenda *A, struct budzik *B)
{
	int sek, min, godz, minB, dmin, minp, min_doby, za_duzo;
	long int ss;
	min_doby = czas_zegarowy(&godz, &min, &sek);  /* aktualna minuta od godz.0.0 */
	minB = B->godz * 60 + B->min;
	ss = (long int)(A->delay + 0.49) + (long int)sek;
	dmin = (int)(ss / S_MIN);
	if ((ss%S_MIN) != 0) dmin++; /* zostaly sek. w minucie - dodac cala minute */
	minp = dmin + min_doby;
	za_duzo = 0;
	if (minp >= 1440)   /* 1szy monit w nast.dobie */
	{
		minp -= 1440;
		if (minB >= min_doby)  /* budzenie w aktualnej dobie (za duzy delay)*/
		{
			za_duzo = 1; minp = minB; dmin = minB - min_doby;
		}
		else  /* budzenie tez w nastepnej dobie */
		{
			if (minp > minB) { dmin -= (minp - minB); za_duzo = 1; }
		}
	}
	else     /* minp<1440: 1szy monit w aktualnej dobie */
	{
		if (minp >= minB)
		{
			if (minB >= min_doby)  /* budzenie w aktualnej dobie (za duzy delay)*/
			{
				za_duzo = 1; minp = minB; dmin = minB - min_doby;
			}
		}
	}  /* Jesli minB<min_doby (budz.w nast.dobie) to minp>minB, bo tu minp>min_doby !!! */
	if (za_duzo > 0)  /* (za duzy delay)*/
	{
		A->delay = (float)(dmin*S_MIN) - sek; if (A->delay < 0) A->delay = 0;
		if (A->mode != 'p')
		{
			A->mode = 't'; A->Interval = 0; B->co_ile = 0; B->wyprzedz = 0;
			A->number_of_calls = 1;
		}
		return;
	}
	B->wyprzedz = minB - minp; if (B->wyprzedz < 0) B->wyprzedz += 1440;
	{long int ss;
	ss = B->co_ile * 60l; if (ss>32000) ss = 32000;
	A->Interval = (int)ss;
	}
	if (A->mode == 't') A->number_of_calls = 1;
	else { A->number_of_calls = B->wyprzedz / B->co_ile + 1; }
}

int dec_budz(int decyzja, int kod_decyzji, int nr_dec, int kod_uslugi, int np, int *nr_rekordu)
{
	struct agenda *A;
	struct budzik *B;
	int sek, min, godz;
	int ret;
	if (kod_decyzji == DEC_NEW && decyzja == 't')
	{
		if (Anew == NULL) Anew = service_default(dec_budz);
		if (Anew == NULL) return kod_uslugi;
		A = Anew; B = (struct budzik *)(A->data);
		/*		 for(i=0;i<10;i++) B->nazwa[i]=0; B->nazwa[i]=0; */
		czas_zegarowy(&godz, &min, &sek); 	/* oddaje minute od godz.0.0 (stud_sys.c) */
		B->min = min; B->godz = godz + 1;
		B->co_ile = 2; B->wyprzedz = 15;
		ustaw_czas_budzika(A, B);
		Aserv = Anew;
		return kod_uslugi;
	}
	else
	{
		if (Anew != NULL) A = Anew;
		else A = Aserv; /* rekord zlecenia agendy */
		if (A == NULL) return kod_uslugi;
		B = (struct budzik *)(A->data);
	}
	if (kod_decyzji == DEC_TIME)
	{
		if (decyzja == 'n' && B->mod_time > 0)
		{
			B->godz = B->poprz_godz_budz; B->min = B->poprz_min_budz;
		}
		else ustaw_czas_budzika(A, B); /* Zatw.czasu lub zmiana wyprz.lub okresu */
		B->mod_time = 0;
	}
	if (kod_decyzji == DEC_MOD_TIME) B->mod_time = 1;
	if (kod_decyzji == DEC_DELAY)	{ ustaw_delay_budzika(A, B); }
	if (kod_decyzji == DEC_RUN)
	{
		ret = decyzje_run(decyzja, &A, &Anew, nr_rekordu,
			kod_uslugi, " Lista budzikow ");
		if (ret == -1) return -1;
	}
	if (kod_decyzji == DEC_TYP_US)
	{
		ustaw_typ_uslugi(A, decyzja); ustaw_czas_budzika(A, B);
	}
	return kod_uslugi;
}
