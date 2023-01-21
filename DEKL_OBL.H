#define ROUND(liczba)\
(int)(((float)liczba)+((liczba>0)?0.5:-0.5))
shint obl_main(void *A);
shint obl_blankiet(shint nr_rekordu, shint ob_pocz,
                    shint ob_konc, shint x_lewy_gorny,
                    shint y_lewy_gorny, shint anim_pid,
                    char *D);
char *dane_obl(shint ob_pocz, shint ob_konc, shint *rozmiar_ob);
void wpis_obl(shint ob_pocz, shint ob_konc,
               char *D, shint rozmiar_ob, char zapis[], char *Kod_op);
shint dec_obl(shint decyzja, shint kod_decyzji, shint nr_dec,
                      shint kod_raportu, shint np, shint *nr_rekordu);
#define L_OCEN 150
#define IL_WYN 5
#define IL_DAN 10
struct oblicz
 {
  shint nr_wzoru;
  shint nazwa[80];
  shint index_no;
  float dana_f[IL_DAN];
  float wynik_f[IL_WYN];
  char odczyt;
  char dzialanie;
  char rys_i_licz;
  char opcja;
  char funkcja;
  char komenda[60];
 };
