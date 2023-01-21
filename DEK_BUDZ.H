
shint budz_main(void *A);
shint budz_blankiet(shint nr_rekordu, shint ob_pocz,
                    shint ob_konc, shint x_lewy_gorny,
                    shint y_lewy_gorny, shint anim_pid,
                    char *D);
char *dane_budz(shint ob_pocz, shint ob_konc, shint *rozmiar_ob);
void wpis_budz(shint ob_pocz, shint ob_konc,
               char *D, shint rozmiar_ob, char zapis[], char *Kod_op);
shint dec_budz(shint decyzja, shint kod_decyzji, shint nr_dec,
                      shint kod_raportu, shint np, shint *nr_rekordu);
struct budzik
 {char nazwa[78];
  shint godz;
  shint min;
  shint poprz_godz_budz;
  shint poprz_min_budz;
  char mod_time;
  shint sek;
  shint co_ile;
  shint wyprzedz;
 };

void ustaw_czas_budzika(struct agenda *A, struct budzik *B);