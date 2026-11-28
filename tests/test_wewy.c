/* Testy jednostkowe funkcji tekstowych silnika formularzy (blank/wewy*.c).
   Utrwalaja obecne zachowanie przed refaktorem silnika - takze dziwactwa,
   oznaczone komentarzem. */
#include "blank/moje.h"
#include "wewy1.h"          /* Ok, nr_danej, liczba_danych */

int sprawdz_nr_danej(void);  /* wewy_wsp.h - wewnetrzne dla silnika */
#include "test_util.h"

static void test_szukaj(void)
{
    char t[] = "abc";
    CHECK(szukaj(t, 'b') == t + 1);
    CHECK(szukaj(t, 'a') == t);
    CHECK(szukaj(t, 'x') == NULL);
}

static void test_podzial_na_slowa(void)
{
    char *s[10];
    char t1[] = "Ala ma, kota";
    char t2[] = "x=1.5";
    char t3[] = "1.5 2";
    char t4[] = "ABC Def";
    char t5[] = "a b c";
    char t6[] = "a b ";
    char t7[] = "  ;  slowo";

    CHECK(podzial_na_slowa(t1, s, 10, 0, 0) == 3);
    CHECK_STR(s[0], "Ala");
    CHECK_STR(s[1], "ma");
    CHECK_STR(s[2], "kota");

    CHECK(podzial_na_slowa(t2, s, 10, 0, 0) == 2);   /* '.' jest separatorem */
    CHECK_STR(s[0], "x=1");
    CHECK_STR(s[1], "5");

    CHECK(podzial_na_slowa(t3, s, 10, 1, 0) == 2);   /* f=1: kropka nie dzieli */
    CHECK_STR(s[0], "1.5");
    CHECK_STR(s[1], "2");

    CHECK(podzial_na_slowa(t4, s, 10, 0, 1) == 2);   /* male=1: male litery */
    CHECK_STR(s[0], "abc");
    CHECK_STR(s[1], "def");

    CHECK(podzial_na_slowa(t5, s, 2, 0, 0) == 2);    /* limit slow */
    CHECK_STR(s[0], "a");
    CHECK_STR(s[1], "b");

    /* dziwactwo: separator na koncu daje dodatkowe puste slowo */
    CHECK(podzial_na_slowa(t6, s, 10, 0, 0) == 3);
    CHECK_STR(s[2], "");

    CHECK(podzial_na_slowa(t7, s, 10, 0, 0) == 1);   /* separatory na poczatku */
    CHECK_STR(s[0], "slowo");
}

static void test_znaki_liczby(void)
{
    char *z;
    /* kropka: 0 - tylko calkowite, 1 - z kropka, 2 - z kropka i wykladnikiem */
    z = "123";    CHECK(znaki_liczby(z, 0) == 1);
    z = "-12";    CHECK(znaki_liczby(z, 0) == 1);
    z = "+7";     CHECK(znaki_liczby(z, 0) == 1);
    z = "1.5";    CHECK(znaki_liczby(z, 0) == 0);
    z = "1.5";    CHECK(znaki_liczby(z, 1) == 1);
    z = "1.2.3";  CHECK(znaki_liczby(z, 1) == 0);
    z = "1e5";    CHECK(znaki_liczby(z, 1) == 0);
    z = "1e5";    CHECK(znaki_liczby(z, 2) == 1);
    z = "1.5e-3"; CHECK(znaki_liczby(z, 2) == 1);
    z = "1e";     CHECK(znaki_liczby(z, 2) == 0);
    z = "12a";    CHECK(znaki_liczby(z, 1) == 0);
    z = "";       CHECK(znaki_liczby(z, 1) == 0);
    z = "-";      CHECK(znaki_liczby(z, 1) == 0);
}

static void test_sprawdz_znak(void)
{
    char sukces;
    int iopt = -1;
    char cyfry[] = "0-9";
    char litery[] = "a-c,x-z";
    char tn[] = "t,n,";

    CHECK(sprawdz_znak('5', cyfry, 3, &sukces, &iopt) == '5');
    CHECK(sukces == 1 && iopt == 0);
    CHECK(sprawdz_znak('0', cyfry, 3, &sukces, &iopt) == '0' && iopt == 0);
    CHECK(sprawdz_znak('9', cyfry, 3, &sukces, &iopt) == '9' && iopt == 2);
    CHECK(sprawdz_znak('a', cyfry, 3, &sukces, &iopt) == EOS);
    CHECK(sukces == 0);

    CHECK(sprawdz_znak('b', litery, 7, &sukces, &iopt) == 'b' && iopt == 0);
    CHECK(sprawdz_znak('y', litery, 7, &sukces, &iopt) == 'y' && iopt == 4);
    CHECK(sprawdz_znak('z', litery, 7, &sukces, &iopt) == 'z' && iopt == 6);
    CHECK(sprawdz_znak('m', litery, 7, &sukces, &iopt) == EOS && sukces == 0);

    CHECK(sprawdz_znak('n', tn, 4, &sukces, &iopt) == 'n' && iopt == 2);
    CHECK(sprawdz_znak('x', tn, 4, &sukces, &iopt) == EOS);
}

static void test_sprawdz_nr_danej(void)
{
    struct okno pola[2];
    /* sciezka bledu wypisuje komunikat i czeka na <Ent>: tylko sciezka OK */
    Ok = pola;
    liczba_danych = 2;
    nr_danej = 0;
    CHECK(sprawdz_nr_danej() == 0);
    nr_danej = 1;
    CHECK(sprawdz_nr_danej() == 0);
    Ok = NULL;
    liczba_danych = 0;
    nr_danej = 0;
}

int main(void)
{
    test_szukaj();
    test_podzial_na_slowa();
    test_znaki_liczby();
    test_sprawdz_znak();
    test_sprawdz_nr_danej();
    return test_wynik("wewy");
}
