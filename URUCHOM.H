#define L_OCEN 150

struct uruchom
 {int imie_no;
  int nazw_no;
  int nr_nazw;
  int index_no;
  int dzien_u;
  int mies_u;
  int rok_u;
  int rok_stud;
  int l_ocen;
  float oceny[L_OCEN];
  float ocena_sr;

  int nr_ewd;
  int czas_pom;
  int wiek;
  int ox,oy;
 };
// int uruchom_main(void *A);
int uruchom_blankiet(int nr_rekordu, int ob_pocz,
                    int ob_konc, int x_lewy_gorny,
                    int y_lewy_gorny, int anim_pid,
                    char *D);
char *dane_uruchom(int ob_pocz, int ob_konc, int *rozmiar_ob);
void wpis_uruchom(int ob_pocz, int ob_konc,
							 char *D, int rozmiar_ob, char zapis[], char *Kod_op);
int dec_uruchom(int decyzja, int kod_decyzji, int nr_dec,
											int kod_raportu, int np, int *nr_rekordu);

