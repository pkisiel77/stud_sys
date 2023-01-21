#define L_OCEN 150

struct uruchom
 {shint imie_no;
  shint nazw_no;
  shint nr_nazw;
  shint index_no;
  shint dzien_u;
  shint mies_u;
  shint rok_u;
  shint rok_stud;
  shint l_ocen;
  float oceny[L_OCEN];
  float ocena_sr;

  shint nr_ewd;
  shint czas_pom;
  shint wiek;
  shint ox,oy;
 };
// shint uruchom_main(void *A);
shint uruchom_blankiet(shint nr_rekordu, shint ob_pocz,
                    shint ob_konc, shint x_lewy_gorny,
                    shint y_lewy_gorny, shint anim_pid,
                    char *D);
char *dane_uruchom(shint ob_pocz, shint ob_konc, shint *rozmiar_ob);
void wpis_uruchom(shint ob_pocz, shint ob_konc,
							 char *D, shint rozmiar_ob, char zapis[], char *Kod_op);
shint dec_uruchom(shint decyzja, shint kod_decyzji, shint nr_dec,
											shint kod_raportu, shint np, shint *nr_rekordu);

