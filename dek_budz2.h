
int budz2_main(void *A);
int budz2_blankiet(int nr_rekordu, int ob_pocz,
                    int ob_konc, int x_lewy_gorny,
                    int y_lewy_gorny, int anim_pid,
                    char *D);
char *dane_budz2(int ob_pocz, int ob_konc, int *rozmiar_ob);
void wpis_budz2(int ob_pocz, int ob_konc,
               char *D, int rozmiar_ob, char zapis[], char *Kod_op);
int dec_budz2(int decyzja, int kod_decyzji, int nr_dec,
                      int kod_raportu, int np, int *nr_rekordu);
struct budzik2
 {char nazwa[78];
  int godz;
  int min;
  int poprz_godz_budz;
  int poprz_min_budz;
  char mod_time;
  int sek;
  int co_ile;
  int wyprzedz;
 };

void ustaw_czas_budzika2(struct agenda *A, struct budzik2 *B);