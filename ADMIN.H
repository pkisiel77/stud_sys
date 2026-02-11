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
	int Mode;
 };

int admin_main(void *A);
int admin_blankiet(int nr_rekordu, int ob_pocz,
                    int ob_konc, int x_lewy_gorny,
                    int y_lewy_gorny, int anim_pid,
                    char *D);
char *dane_admin(int ob_pocz, int ob_konc, int *rozmiar_ob);
void wpis_admin(int ob_pocz, int ob_konc,
               char *D, int rozmiar_ob, char zapis[], char *Kod_op);
int dec_admin(int decyzja, int kod_decyzji, int nr_dec,
                      int kod_raportu, int np, int *nr_rekordu);

