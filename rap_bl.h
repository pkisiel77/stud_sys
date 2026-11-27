#ifndef RAP_BL_H
#define RAP_BL_H

struct Service;

/* Petla obslugi raportu (blankietu) uslugi - rap_bl.c */
int raporty(signed char Kod_uslugi, int Nr_rekordu, int npzl,
            int kto, int r_pocz, int lrek, struct Service* S);

#endif /* RAP_BL_H */
