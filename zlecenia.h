#ifndef _ZLECENIA_H_
#define _ZLECENIA_H_
/* Wspolne formularze i decyzje modulow uslug RT (zlecenia.c). */
#include "blank/sys_dekl.h"

int pobierz_rekord_uslugi(int* nr_rekordu, int kod_uslugi, int ob_konc, struct agenda** As, struct agenda** Anew,
                          struct agenda*** SA, char* adres_rek0_uslugi, char* nazwa);
int dane_agendy(struct agenda* A, struct agenda* Anew, int cykl_max);
int pokaz_listy_zlecen(struct agenda* Anew, int kod_uslugi, char* tytul);
void ustaw_typ_uslugi(struct agenda* A, int decyzja);
int decyzje_run(char decyzja, struct agenda** Aserv, struct agenda** Anew, int* nr_rekordu, int kod_uslugi,
                char* tytul);
#endif
