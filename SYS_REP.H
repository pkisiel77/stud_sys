struct sys_rep
 {char *nazwa;
  int status;
  int l_rek_max;
  int nr_ag;
  int del_min;
  int del_sek;
  struct agenda Ag;
 };

int sys_main(void *A);
int sys_blankiet(int nr_rekordu, int ob_pocz,
                    int ob_konc, int x_lewy_gorny,
                    int y_lewy_gorny, int anim_pid,
                    char *D);
char *dane_sys(int ob_pocz, int ob_konc, int *rozmiar_ob);
void wpis_sys(int ob_pocz, int ob_konc,
               char *D, int rozmiar_ob, char zapis[], char *Kod_op);
int dec_sys(int decyzja, int kod_decyzji, int nr_dec,
                      int kod_raportu, int np, int *nr_rekordu);
