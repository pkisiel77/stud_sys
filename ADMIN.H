#define A_IMIE     80
#define A_NAZWISKO 80
#define DEC_GEN_KOD   31
#define DEC_DODAJ     2

/* zmienne globalne dla admin.c */

/**/
struct admin
 {
	char Imie[A_IMIE];
    char Nazwisko[A_NAZWISKO];
	shint Mode;
 };

shint admin_main(void *A);
shint admin_blankiet(shint nr_rekordu, shint ob_pocz,
                    shint ob_konc, shint x_lewy_gorny,
                    shint y_lewy_gorny, shint anim_pid,
                    char *D);
char *dane_admin(shint ob_pocz, shint ob_konc, shint *rozmiar_ob);
void wpis_admin(shint ob_pocz, shint ob_konc,
               char *D, shint rozmiar_ob, char zapis[], char *Kod_op);
shint dec_admin(shint decyzja, shint kod_decyzji, shint nr_dec,
                      shint kod_raportu, shint np, shint *nr_rekordu);

