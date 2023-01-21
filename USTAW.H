struct ustaw
{
	int x;
};

shint ustaw_main(void *A);
shint ustaw_blankiet(shint nr_rekordu, shint ob_pocz,
                    shint ob_konc, shint x_lewy_gorny,
                    shint y_lewy_gorny, shint anim_pid,
                    char *D);
char *dane_ustaw(shint ob_pocz, shint ob_konc, shint *rozmiar_ob);
void wpis_ustaw(shint ob_pocz, shint ob_konc,
               char *D, shint rozmiar_ob, char zapis[], char *Kod_op);
shint dec_ustaw(shint decyzja, shint kod_decyzji, shint nr_dec,
                      shint kod_raportu, shint np, shint *nr_rekordu);
