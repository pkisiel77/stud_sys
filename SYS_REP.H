struct sys_rep
 {char *nazwa;
  shint status;
  shint l_rek_max;
  shint nr_ag;
  shint del_min;
  shint del_sek;
  struct agenda Ag;
 };

shint sys_main(void *A);
shint sys_blankiet(shint nr_rekordu, shint ob_pocz,
                    shint ob_konc, shint x_lewy_gorny,
                    shint y_lewy_gorny, shint anim_pid,
                    char *D);
char *dane_sys(shint ob_pocz, shint ob_konc, shint *rozmiar_ob);
void wpis_sys(shint ob_pocz, shint ob_konc,
               char *D, shint rozmiar_ob, char zapis[], char *Kod_op);
shint dec_sys(shint decyzja, shint kod_decyzji, shint nr_dec,
                      shint kod_raportu, shint np, shint *nr_rekordu);
