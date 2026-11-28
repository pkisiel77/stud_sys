/* Program pomocniczy testu ui_tn.py: pytanie potwierdz_tn() w terminalu.
   Wiersz 2 to "tlo"; trzy pytania (pelne, obciete do 3 znakow, pelne),
   wyniki w wierszach 4-6, potem KONIEC i wyjscie po dowolnym klawiszu. */
#include "blank/moje.h"
#include "konfig.h"
#include "scheduler.h"

#include <stdio.h>

#define TLO "0123456789ABCDEFGHIJ"
#define ATTR (TERM_WHITE | TERM_BLACK_BG)

static char wynik(int zn)
{
    return zn == Esc ? 'E' : (char)zn;
}

int main(void)
{
    char buf[8];
    int zn;

    set_services();
    scheduler_init();
    InitConsole();
    term_type(2, 0, TLO, 0, ATTR);
    term_type(1, 0, "PYTANIE1", 0, ATTR);
    zn = potwierdz_tn(2, 4, "<t/n>? ", 7, 7, ATTR);
    snprintf(buf, sizeof(buf), "R1=%c", wynik(zn));
    term_type(4, 0, buf, 0, ATTR);

    term_type(1, 0, "PYTANIE2", 0, ATTR);
    zn = potwierdz_tn(2, 4, "<t/n>? ", 7, 3, ATTR);
    snprintf(buf, sizeof(buf), "R2=%c", wynik(zn));
    term_type(5, 0, buf, 0, ATTR);

    term_type(1, 0, "PYTANIE3", 0, ATTR);
    zn = potwierdz_tn(2, 4, "<t/n>? ", 7, 7, ATTR);
    snprintf(buf, sizeof(buf), "R3=%c", wynik(zn));
    term_type(6, 0, buf, 0, ATTR);

    term_type(1, 0, "KONIEC  ", 0, ATTR);
    term_flush();
    czekaj_na_klawisz();
    CloseConsole();
    return 0;
}
