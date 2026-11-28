/* Testy jednostkowe uslugi Budzik.
   Plik wlacza budz.c, zeby sprawdzic takze funkcje static; ten test jest
   linkowany ze wszystkimi modulami oprocz budz.c (CMakeLists.txt). */
#include "budz.c"
#include "konfig.h"
#include "test_util.h"

#include <math.h>
#include <unistd.h>

/* ---------------- budz_format_pozostalo ---------------- */
static void test_format_pozostalo(void)
{
    char buf[20];
    budz_format_pozostalo(0.0f, buf, sizeof(buf));
    CHECK_STR(buf, "00:00");
    budz_format_pozostalo(59.6f, buf, sizeof(buf));    /* zaokraglenie w gore */
    CHECK_STR(buf, "01:00");
    budz_format_pozostalo(125.0f, buf, sizeof(buf));
    CHECK_STR(buf, "02:05");
    budz_format_pozostalo(3725.0f, buf, sizeof(buf));  /* od godziny: hh:mm:ss */
    CHECK_STR(buf, "01:02:05");
    budz_format_pozostalo(-5.0f, buf, sizeof(buf));    /* ujemne = 0 */
    CHECK_STR(buf, "00:00");
    budz_format_pozostalo(1.0e6f, buf, sizeof(buf));   /* godziny obciete do 99 */
    CHECK(strncmp(buf, "99:", 3) == 0);
    budz_format_pozostalo(3725.0f, buf, 4);            /* maly bufor: bez przepelnienia */
    CHECK_STR(buf, "01:");
}

/* ---------------- budz_pozostalo_teraz ---------------- */
static void test_pozostalo_teraz(void)
{
    struct agenda A;
    float left;
    memset(&A, 0, sizeof(A));
    sek_akt = time(NULL) - 30;
    A.delay = 100.0f;
    left = budz_pozostalo_teraz(&A);
    CHECK(left >= 69.0f && left <= 70.0f);
    A.delay = 10.0f;                       /* termin minal: 0, nie wartosc ujemna */
    CHECK(budz_pozostalo_teraz(&A) == 0.0f);
    CHECK(budz_pozostalo_teraz(NULL) == 0.0f);
}

/* ---------------- ustaw_czas_budzika ----------------
   Wynik zalezy od biezacej minuty doby, wiec godzine budzika liczymy
   wzgledem "teraz". Gdy w trakcie wywolania zmieni sie minuta, powtarzamy. */
struct wynik_czasu {
    struct agenda A;
    struct budzik B;
    int sek;               /* sekunda minuty w chwili wywolania */
};

static struct wynik_czasu ustaw(char mode, int dmin_od_teraz, int co_ile, int wyprzedz)
{
    struct wynik_czasu w;
    int godz, min, sek, m1, m2, cel;
    do {
        memset(&w, 0, sizeof(w));
        m1 = czas_zegarowy(&godz, &min, &sek);
        cel = ((m1 + dmin_od_teraz) % 1440 + 1440) % 1440;
        w.A.mode = mode;
        w.B.godz = cel / 60;
        w.B.min = cel % 60;
        w.B.co_ile = co_ile;
        w.B.wyprzedz = wyprzedz;
        ustaw_czas_budzika(&w.A, &w.B);
        w.sek = sek;
        m2 = czas_zegarowy(&godz, &min, &sek);
    } while (m1 != m2 || sek < w.sek);
    return w;
}

static int delay_okolo(float delay, long oczekiwane_sek)
{
    return fabsf(delay - (float)oczekiwane_sek) <= 1.0f;
}

static void test_ustaw_czas_przed_oknem(void)
{
    /* budzik za 10 min, monity od 5 min przed: pierwszy za 5 min */
    struct wynik_czasu w = ustaw('s', 10, 1, 5);
    CHECK(delay_okolo(w.A.delay, 5 * 60L - w.sek));
    CHECK(w.A.Interval == 60);
    CHECK(w.A.number_of_calls == 6);   /* wyprzedz/co_ile + 1 */
    CHECK(w.B.mod_time == 0);
    CHECK(w.B.poprz_godz_budz == w.B.godz && w.B.poprz_min_budz == w.B.min);
}

static void test_ustaw_czas_w_oknie(void)
{
    /* budzik za 2 min, wyprzedzenie 5 min: okno juz trwa, alarm od razu */
    struct wynik_czasu w = ustaw('s', 2, 1, 5);
    CHECK(w.A.delay == 0.0f);
}

static void test_ustaw_czas_nastepna_doba(void)
{
    /* godzina minela 10 min temu: budzik na jutro */
    struct wynik_czasu w = ustaw('s', -10, 1, 5);
    CHECK(delay_okolo(w.A.delay, (1440L - 15L) * 60L - w.sek));
}

static void test_ustaw_czas_tryby(void)
{
    struct wynik_czasu w;

    w = ustaw('s', 30, 2, 0);          /* bez wyprzedzenia: jednorazowy */
    CHECK(w.A.mode == 't');
    CHECK(w.A.number_of_calls == 1);

    w = ustaw('p', 30, 2, 10);         /* permanentny */
    CHECK(w.A.number_of_calls == -1);
    CHECK(w.A.Interval == 120);

    w = ustaw('s', 30, 20, 5);         /* co_ile > wyprzedz: przyciete */
    CHECK(w.B.co_ile == 5);
    CHECK(w.A.Interval == 300);

    w = ustaw('s', 30, 0, 5);          /* co_ile < 1: co najmniej 1 min */
    CHECK(w.B.co_ile == 1);
}

static void test_ustaw_czas_godzina_25(void)
{
    struct agenda A;
    struct budzik B;
    memset(&A, 0, sizeof(A));
    memset(&B, 0, sizeof(B));
    A.mode = 's';
    B.godz = 25;
    B.co_ile = 1;
    B.wyprzedz = 1;
    ustaw_czas_budzika(&A, &B);
    CHECK(B.godz == 1);
}

/* ---------------- zapis/odczyt ~/.stud_sys_budz.dat ---------------- */
static void zapisz_rekord(FILE *fp, const char *nazwa, int godz, int min,
                          int co_ile, int wyprzedz, char mode)
{
    struct budz_rekord_pliku rec;
    memset(&rec, 0, sizeof(rec));
    snprintf(rec.nazwa, sizeof(rec.nazwa), "%s", nazwa);
    rec.godz = godz;
    rec.min = min;
    rec.co_ile = co_ile;
    rec.wyprzedz = wyprzedz;
    rec.mode = mode;
    fwrite(&rec, sizeof(rec), 1, fp);
}

static int policz_budziki(void)
{
    int i, n = 0;
    for (i = 1; i <= AgendaMax; i++)
        if (SysA[i] != NULL && (SysA[i]->S)->decyzje == dec_budz) n++;
    return n;
}

static struct budzik *budzik_nr(int k)
{
    int i;
    for (i = 1; i <= AgendaMax; i++)
        if (SysA[i] != NULL && (SysA[i]->S)->decyzje == dec_budz && k-- == 0)
            return (struct budzik *)SysA[i]->data;
    return NULL;
}

static long rozmiar_pliku(const char *path, char *buf, long max)
{
    long n;
    FILE *fp = fopen(path, "rb");
    if (fp == NULL) return -1;
    n = (long)fread(buf, 1, (size_t)max, fp);
    fclose(fp);
    return n;
}

static void test_plik_odczyt_i_zapis(void)
{
    FILE *fp;
    int count = 2;
    char przed[1024], po[1024];
    long n_przed, n_po;
    struct budzik *B;

    scheduler_init();
    fp = fopen(budz_sciezka(), "wb");
    CHECK(fp != NULL);
    if (fp == NULL) return;
    fwrite(&count, sizeof(int), 1, fp);
    zapisz_rekord(fp, "Demo", 7, 30, 1, 1, 's');
    zapisz_rekord(fp, "Kawa", 15, 5, 2, 4, 'p');
    fclose(fp);

    budz_wczytaj_z_pliku();
    CHECK(policz_budziki() == 2);
    B = budzik_nr(0);
    CHECK(B != NULL && strcmp(B->nazwa, "Demo") == 0 && B->godz == 7 && B->min == 30);
    B = budzik_nr(1);
    CHECK(B != NULL && strcmp(B->nazwa, "Kawa") == 0 && B->co_ile == 2 && B->wyprzedz == 4);

    /* zapis tych samych budzikow daje identyczny plik */
    n_przed = rozmiar_pliku(budz_sciezka(), przed, sizeof(przed));
    budz_zapisz_do_pliku();
    n_po = rozmiar_pliku(budz_sciezka(), po, sizeof(po));
    CHECK(n_przed > 0 && n_przed == n_po);
    CHECK(n_przed == n_po && memcmp(przed, po, (size_t)n_po) == 0);
}

static void test_plik_bledny(void)
{
    FILE *fp;
    int count = -1;

    scheduler_init();
    fp = fopen(budz_sciezka(), "wb");
    CHECK(fp != NULL);
    if (fp == NULL) return;
    fwrite(&count, sizeof(int), 1, fp);   /* uszkodzony licznik */
    fclose(fp);
    budz_wczytaj_z_pliku();
    CHECK(policz_budziki() == 0);

    remove(budz_sciezka());                /* brak pliku */
    budz_wczytaj_z_pliku();
    CHECK(policz_budziki() == 0);
}

static void test_plik_domyslne_okresy(void)
{
    FILE *fp;
    int count = 1;
    struct budzik *B;

    scheduler_init();
    fp = fopen(budz_sciezka(), "wb");
    CHECK(fp != NULL);
    if (fp == NULL) return;
    fwrite(&count, sizeof(int), 1, fp);
    zapisz_rekord(fp, "Zero", 12, 0, 0, 0, 'x');  /* 0 i zly tryb: wartosci domyslne */
    fclose(fp);
    budz_wczytaj_z_pliku();
    B = budzik_nr(0);
    CHECK(B != NULL && B->co_ile == 2 && B->wyprzedz == 15);
}

int main(void)
{
    test_ustaw_home();
    set_services();

    test_format_pozostalo();
    test_pozostalo_teraz();
    test_ustaw_czas_przed_oknem();
    test_ustaw_czas_w_oknie();
    test_ustaw_czas_nastepna_doba();
    test_ustaw_czas_tryby();
    test_ustaw_czas_godzina_25();
    test_plik_odczyt_i_zapis();
    test_plik_bledny();
    test_plik_domyslne_okresy();

    remove(budz_sciezka());
    rmdir(test_home);
    return test_wynik("budzik");
}
