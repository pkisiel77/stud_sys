struct sprawdz
 {char *nazwa;
	int status;
	int l_rek_max;
	int nr_ag;
	int del_min;
	int del_sek;
	struct agenda Ag;
 };

int sprawdz(void *A);
int sprawdz_blankiet(int nr_rekordu, int ob_pocz,
										int ob_konc, int x_lewy_gorny,
										int y_lewy_gorny, int anim_pid,
										char *D);
char *dane_sprawdz(int ob_pocz, int ob_konc, int *rozmiar_ob);
void wpis_sprawdz(int ob_pocz, int ob_konc,
							 char *D, int rozmiar_ob, char zapis[], char *Kod_op);
int dec_sprawdz(int decyzja, int kod_decyzji, int nr_dec,
											int kod_raportu, int np, int *nr_rekordu);
