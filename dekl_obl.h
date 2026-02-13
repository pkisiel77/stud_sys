#define ROUND(liczba)\
(int)(((float)liczba)+((liczba>0)?0.5:-0.5))
int obl_main(void *A);
int obl_blankiet(int nr_rekordu, int ob_pocz,
                    int ob_konc, int x_lewy_gorny,
                    int y_lewy_gorny, int anim_pid,
                    char *D);
char *dane_obl(int ob_pocz, int ob_konc, int *rozmiar_ob);
void wpis_obl(int ob_pocz, int ob_konc,
               char *D, int rozmiar_ob, char zapis[], char *Kod_op);
int dec_obl(int decyzja, int kod_decyzji, int nr_dec,
                      int kod_raportu, int np, int *nr_rekordu);
#define L_OCEN 150
#define IL_WYN 5
#define IL_DAN 10
struct oblicz
 {
  int nr_wzoru;
  int nazwa[80];
  int index_no;
  float dana_f[IL_DAN];
  float wynik_f[IL_WYN];
  char odczyt;
  char dzialanie;
  char rys_i_licz;
  char opcja;
  char funkcja;
  char komenda[60];
 };
