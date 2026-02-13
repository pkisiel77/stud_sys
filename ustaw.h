struct ustaw
{
	int x;
};

int ustaw_main(void *A);
int ustaw_blankiet(int nr_rekordu, int ob_pocz,
                    int ob_konc, int x_lewy_gorny,
                    int y_lewy_gorny, int anim_pid,
                    char *D);
char *dane_ustaw(int ob_pocz, int ob_konc, int *rozmiar_ob);
void wpis_ustaw(int ob_pocz, int ob_konc,
               char *D, int rozmiar_ob, char zapis[], char *Kod_op);
int dec_ustaw(int decyzja, int kod_decyzji, int nr_dec,
                      int kod_raportu, int np, int *nr_rekordu);
