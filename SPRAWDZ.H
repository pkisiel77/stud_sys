struct sprawdz
 {char *nazwa;
	shint status;
	shint l_rek_max;
	shint nr_ag;
	shint del_min;
	shint del_sek;
	struct agenda Ag;
 };

shint sprawdz(void *A);
shint sprawdz_blankiet(shint nr_rekordu, shint ob_pocz,
										shint ob_konc, shint x_lewy_gorny,
										shint y_lewy_gorny, shint anim_pid,
										char *D);
char *dane_sprawdz(shint ob_pocz, shint ob_konc, shint *rozmiar_ob);
void wpis_sprawdz(shint ob_pocz, shint ob_konc,
							 char *D, shint rozmiar_ob, char zapis[], char *Kod_op);
shint dec_sprawdz(shint decyzja, shint kod_decyzji, shint nr_dec,
											shint kod_raportu, shint np, shint *nr_rekordu);
