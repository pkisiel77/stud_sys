#include "blank/moje.h"
#include "dek_budz.h"
#include "loc.h"
#include "scheduler.h"
#include "zlecenia.h"
#include "core.h"
#include "ui_wpis.h"
/* extern struct agenda *Agenda; */


#define S_MIN 60l
/* ------------------ kody decyzji sa w moje.h -------------
#define DEC_NEW    1
#define DEC_RUN    2
#define DEC_TYP_US 3
#define DEC_TIME   4
#define DEC_DELAY  5
 ----------------------------------------------------------- */
#define DEC_MOD_TIME 16
static struct agenda *Anew = NULL, *Aserv = NULL; /* stan lokalny uslugi budzik */
static void budz_format_pozostalo(float delay, char *buf, size_t buflen);
static float budz_pozostalo_teraz(struct agenda *A);
static int budz_menu_live(struct agenda *lista[], int nbudz, int opcja0, char *tytul);
static void budz_odswiez_ekran(int wymus);
static struct agenda *budz_kopia_robocza(struct agenda *A);
static void budz_przepisz_zmiany(struct agenda *dst, struct agenda *src);
/* ============ Widok: lista budzikow i formularz ====================
   Lista (z odliczaniem) i otwarty formularz rysuje razem
   budz_odswiez_ekran(), wywolywana co sekunde z kazdej petli czekajacej
   na klawisz (lista, formularz, pole wpisu). */

static struct {
    struct agenda **lista;
    int nbudz, count, sel;   /* count == 0: lista nie jest wyswietlana */
    char *tytul;
    int yp, xp, ym, xm;
} BW;

static struct {
    struct budzik *B;
    char **menu;             /* NULL: formularz zamkniety */
    char pola[3][80];        /* teksty pol 0-2, wpisywane do menu[0..2] */
    int nmenu, sel;
    char *tytul;
    int yp, xp;
    struct ui_tlo tlo;       /* ekran pod formularzem */
} FW;


static void budz_wiersz_listy(int i, char *buf, size_t n)
{
    if (i < BW.nbudz) {
        char pozostal[20];
        struct budzik *B = (struct budzik *)(BW.lista[i]->data);
        budz_format_pozostalo(budz_pozostalo_teraz(BW.lista[i]), pozostal, sizeof(pozostal));
        /* nazwa celowo ucinana, by linia zmiescila sie w 79 kolumnach */
        snprintf(buf, n, "  %02d:%02d  [%8s]  %-33.40s", B->godz, B->min, pozostal, B->nazwa);
    } else if (i == BW.nbudz) {
        snprintf(buf, n, "%s", L_BUDZ_MENU_ADD);
    } else {
        snprintf(buf, n, "%s", L_BUDZ_MENU_EXIT);
    }
}

static void budz_rysuj_liste(void)
{
    char rows[AG_SIZE + 2][80];
    char *w[AG_SIZE + 3];
    int i;
    for (i = 0; i < BW.count; i++) {
        budz_wiersz_listy(i, rows[i], sizeof(rows[i]));
        w[i] = rows[i];
    }
    w[BW.count] = " <Esc>"; /* podpowiedz w ostatnim wierszu, jak w menu i formularzach */
    ui_rysuj_okno(BW.yp, BW.xp, BW.ym, BW.xm, BW.tytul, w, BW.count + 1, BW.sel, 1);
}

static void budz_rysuj_formularz(void)
{
    char *w[8];
    int i, n, len, maxlen, xm, ym;
    snprintf(FW.pola[0], sizeof(FW.pola[0]), L_BUDZ_FIELD_NAME, FW.B->nazwa);
    snprintf(FW.pola[1], sizeof(FW.pola[1]), L_BUDZ_FIELD_TIME, FW.B->godz, FW.B->min);
    snprintf(FW.pola[2], sizeof(FW.pola[2]), L_BUDZ_FIELD_EVERY, FW.B->co_ile, FW.B->wyprzedz);
    n = FW.nmenu < 7 ? FW.nmenu : 7;
    maxlen = 5; /* "<Esc>" */
    if (FW.tytul != NULL && (int)strlen(FW.tytul) > maxlen) maxlen = (int)strlen(FW.tytul);
    for (i = 0; i < n; i++) {
        w[i] = (i < 3) ? FW.pola[i] : FW.menu[i];
        len = (int)strlen(w[i]);
        if (len > maxlen) maxlen = len;
    }
    w[n] = "<Esc>";
    xm = FW.xp + maxlen + 3;
    if (xm > MXR_MAX) xm = MXR_MAX;
    ym = FW.yp + n + 2;
    if (ym > MYR_MAX) ym = MYR_MAX;
    ui_rysuj_okno(FW.yp, FW.xp, ym, xm, FW.tytul, w, n + 1, FW.sel, 2);
}

/* Rysuje liste i otwarty formularz: zawsze gdy wymus != 0, poza tym
   raz na sekunde (odliczanie na liscie). */
static void budz_odswiez_ekran(int wymus)
{
    static time_t ostatnio = 0;
    time_t teraz = time(NULL);
    if (!wymus && teraz == ostatnio) return;
    ostatnio = teraz;
    if (FW.menu != NULL) ui_przywroc_tlo(&FW.tlo); /* najpierw tlo: formularz mogl sie zwezic */
    if (BW.count > 0) budz_rysuj_liste();
    if (FW.menu != NULL) budz_rysuj_formularz();
    term_flush();
}


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
        snprintf(rec.nazwa, sizeof(rec.nazwa), "%s", B->nazwa);
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
        snprintf(B->nazwa, sizeof(B->nazwa), "%s", rec.nazwa);
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
	snprintf(tytul,sizeof(tytul),L_BUDZ_MAIN_TITLE);
	snprintf(text,sizeof(text),L_BUDZ_REPORTED,B->godz,B->min);
	snprintf(text1,sizeof(text1),L_BUDZ_NEXT_PING, (int)A->Interval/60,(int)A->Interval%60);
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


/* ====================== BLANKIET BUDZIKA — LISTA I ZARZADZANIE ===== */

/* Formularz edycji jednego budzika.
   Zwraca indeks wybranej pozycji (>= 3 = akcja, < 0 = Escape). */
/* Formularz edycji jednego budzika (pola 0-2, dalej akcje z menu[]).
   Zwraca indeks wybranej akcji (>= 3) albo -1 dla Esc.
   Formularz rysuje budz_odswiez_ekran(), wiec zostaje widoczny takze
   podczas wpisywania wartosci, a lista pod nim dalej odlicza. */
static int budz_formularz(struct agenda *A, struct budzik *B,
                          char *tytul, char **menu, int nmenu)
{
    (void)A;
    int w, ch;

    FW.B = B; FW.menu = menu; FW.nmenu = nmenu; FW.sel = 0; FW.tytul = tytul;
    /* pod lista, zeby bylo widac jej odliczanie; gdy sie nie miesci -
       na liscie, jak dawniej */
    FW.yp = Y_G0 + 2; FW.xp = X_L0 + 2;
    if (BW.count > 0 && BW.ym + 1 + nmenu + 2 <= MYR_MAX) FW.yp = BW.ym + 1;
    /* tlo pod formularzem: pelne wiersze, bo szerokosc formularza
       zmienia sie razem z dlugoscia wpisanej Sprawy */
    ui_zapamietaj_tlo(&FW.tlo, FW.yp, nmenu + 3);

    w = UI_NIC;
    while (w == UI_NIC) {
        ch = ui_czekaj_klawisz(budz_odswiez_ekran);
        w = ui_klawisz_menu(ch, &FW.sel, nmenu, budz_odswiez_ekran);
        switch (w) {
        case 0:
            ui_czytaj_text(B->nazwa, sizeof(B->nazwa), L_BUDZ_ASK_NAME, budz_odswiez_ekran);
            w = UI_NIC;
            break;
        case 1:
            B->godz     = ui_czytaj_int(L_BUDZ_ASK_HOUR, 0, 23, B->godz, budz_odswiez_ekran);
            B->min      = ui_czytaj_int(L_BUDZ_ASK_MIN,  0, 59, B->min, budz_odswiez_ekran);
            B->mod_time = 1;
            w = UI_NIC;
            break;
        case 2:
            B->co_ile   = ui_czytaj_int(L_BUDZ_ASK_EVERY, 1, 1440, B->co_ile, budz_odswiez_ekran);
            B->wyprzedz = ui_czytaj_int(L_BUDZ_ASK_LEAD,  1, 1440, B->wyprzedz, budz_odswiez_ekran);
            w = UI_NIC;
            break;
        default:
            break; /* akcja albo Esc (-1) albo nawigacja (UI_NIC) */
        }
    }

    ui_przywroc_tlo(&FW.tlo);
    ui_zwolnij_tlo(&FW.tlo);
    FW.menu = NULL;
    term_flush();
    return w;
}

int budz_blankiet(int nr_rekordu, int ob_pocz, int ob_konc,
                  int x_lewy_gorny, int y_lewy_gorny, int kod_uslugi,
                  char *adres_rek0_uslugi)
{
    (void)kod_uslugi;
    struct agenda *lista[AG_SIZE];
    struct agenda *A;
    struct agenda *Awrk;
    struct budzik *B;
    static int lw = 0;
    int n, i, w;
    (void)nr_rekordu; (void)ob_pocz; (void)ob_konc;
    (void)x_lewy_gorny; (void)y_lewy_gorny; (void)adres_rek0_uslugi;

    for (;;) {
        /* --- Zbierz aktywne budziki z agendy --- */
        n = 0;
        for (i = 1; i <= AgendaMax && n < AG_SIZE; i++) {
            A = SysA[i];
            if (A == NULL || A->number_of_calls == 0) continue;
            if ((A->S)->decyzje != dec_budz) continue;
            lista[n] = A;
            n++;
        }
        if (lw > n + 1) lw = 0;

        w = budz_menu_live(lista, n, lw, L_BUDZ_LIST_TITLE);
        lw = (w >= 0) ? w : 0;

        if (w < 0 || w == n + 1) { BW.count = 0; return BLANKIET_UI_EXIT; }

        if (w == n) {
            /* ---------- Nowy budzik ---------- */
            int godz, min, sek;
            char *mn[5] = {NULL, NULL, NULL,
                           L_BUDZ_MENU_SAVE_NEW,
                           L_BUDZ_MENU_CANCEL};
            A = service_default(dec_budz);
            if (A == NULL) continue;
            A->mode = 's';
            B = (struct budzik *)(A->data);
            czas_zegarowy(&godz, &min, &sek);
            B->godz = (godz + 1) % 24; B->min = min;
            B->co_ile = 2; B->wyprzedz = 15;

            w = budz_formularz(A, B, L_BUDZ_NEW_TITLE, mn, 5);
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
                           L_BUDZ_MENU_APPLY,
                           L_BUDZ_MENU_DELETE,
                           L_BUDZ_MENU_EXIT_NC};
            A = lista[w];
            Awrk = budz_kopia_robocza(A);
            if (Awrk == NULL) continue;
            B = (struct budzik *)(Awrk->data);

            w = budz_formularz(Awrk, B, L_BUDZ_EDIT_TITLE, me, 6);
            if (w == 3) {
                ustaw_czas_budzika(Awrk, B);
                budz_przepisz_zmiany(A, Awrk);
                budz_zapisz_do_pliku();
            } else if (w == 4) {
                A->number_of_calls = 0;
                budz_zapisz_do_pliku();
                if (lw >= n - 1) lw = 0;
            }
            Free(Awrk);
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
    if (hh > 99) hh = 99; /* budzik dziala w obrebie doby; pole ma 2 cyfry */
    if (hh > 0)
        snprintf(buf, buflen, "%02ld:%02ld:%02ld", hh, mm, ss);
    else
        snprintf(buf, buflen, "%02ld:%02ld", mm, ss);
}

static float budz_pozostalo_teraz(struct agenda *A)
{
    time_t now;
    float left;
    if (A == NULL) return 0.0f;
    now = time(NULL);
    left = A->delay - (float)(now - sek_akt);
    if (left < 0.0f) left = 0.0f;
    return left;
}

static int budz_menu_live(struct agenda *lista[], int nbudz, int opcja0, char *tytul)
{
    char wiersz[80];
    int i, len, maxlen, ch, w;

    BW.lista = lista; BW.nbudz = nbudz; BW.count = nbudz + 2; BW.tytul = tytul;
    BW.sel = (opcja0 >= 0 && opcja0 < BW.count) ? opcja0 : 0;
    maxlen = 5; /* "<Esc>" */
    if (tytul != NULL && (int)strlen(tytul) > maxlen) maxlen = (int)strlen(tytul);
    for (i = 0; i < BW.count; i++) {
        budz_wiersz_listy(i, wiersz, sizeof(wiersz));
        len = (int)strlen(wiersz);
        if (len > maxlen) maxlen = len;
    }
    BW.yp = Y_G0 + 1; BW.xp = X_L0;
    BW.xm = BW.xp + maxlen + 3;
    if (BW.xm > MXR_MAX) BW.xm = MXR_MAX;
    BW.ym = BW.yp + BW.count + 2;
    if (BW.ym > MYR_MAX) BW.ym = MYR_MAX;

    for (;;) {
        ch = ui_czekaj_klawisz(budz_odswiez_ekran);
        w = ui_klawisz_menu(ch, &BW.sel, BW.count, budz_odswiez_ekran);
        if (w != UI_NIC) return w;
    }
}

static struct agenda *budz_kopia_robocza(struct agenda *A)
{
    int size;
    struct agenda *Acopy;
    if (A == NULL || A->S == NULL || A->S->str_size < (int)sizeof(struct agenda))
        return NULL;
    size = A->S->str_size;
    Acopy = (struct agenda*)Malloc(size);
    if (Acopy == NULL) return NULL;
    memcpy(Acopy, A, size);
    Acopy->data = (char*)(Acopy + 1);
    memcpy(Acopy->data, A->data, (size_t)(size - (int)sizeof(struct agenda)));
    return Acopy;
}

static void budz_przepisz_zmiany(struct agenda *dst, struct agenda *src)
{
    char *data_dst;
    struct Service *service_dst;
    char *name_dst;
    if (dst == NULL || src == NULL) return;
    data_dst = dst->data;
    service_dst = dst->S;
    name_dst = dst->name;
    memcpy(dst, src, sizeof(struct agenda));
    dst->data = data_dst;
    dst->S = service_dst;
    dst->name = name_dst;
    memcpy(dst->data, src->data, sizeof(struct budzik));
}

char *dane_budz(int ob_pocz, int ob_konc, int *rozmiar_ob)
 {static struct Service *S;
	(void)ob_pocz; (void)ob_konc;
	static struct agenda *A;
	struct budzik *Bu;
	int x,y;
	int min, sek, godz;
	S=Service;
	*rozmiar_ob=sizeof(struct agenda)+sizeof(struct budzik);
	A=(struct agenda *)Malloc(*rozmiar_ob);
	A->S=S; A->data=(A+1);
	Bu=(struct budzik *)(A->data);
	memset(Bu, 0, sizeof(struct budzik));
	A->mode='s'; A->prior=1;
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
	(void)ob_pocz; (void)ob_konc; (void)rozmiar_ob; (void)zapis; (void)Kod_op;
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
	(void)nr_dec; (void)np;
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
			kod_uslugi, L_BUDZ_AGENDA_TITLE);
		if (ret == -1) return -1;
	}
	if (kod_decyzji == DEC_TYP_US)
	{
		ustaw_typ_uslugi(A, decyzja); ustaw_czas_budzika(A, B);
	}
	return kod_uslugi;
}
