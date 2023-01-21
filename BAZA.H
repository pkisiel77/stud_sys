#define DL_IMIE      25  // d³ugoœc imienia
#define DL_NAZW      40  // d³ugoœæ nazwiska
#define DL_NR_EW     11  // d³ugoœæ numeru ewidencyjnego
#define DL_NAZW_MIEJ 25  // d³ugoœæ nazwy miejscowoœci
#define DL_NAZW_WOJ  25  // d³ugoœæ nazwy województwa
                         // ...pacjenta
#define IL_PRZEDM 6
struct kurs
 {shint nazwa_przedm;  /*numer wsk na pole w tablicy stringow z nazwami przedmiotow*/
 float ocena_egz;
 float ocena_cwi;
 float ocena_lab;
 };

struct pac_data_ur
{
	shint dzien;
	shint miesiac;
	shint rok;
};

struct student
 {shint imie_no;
  shint nazw_no;
  shint index_no;
  shint dzien_u;
  shint mies_u;
  shint rok_u;
  shint rok_stud;
  shint miasto_no;
  shint l_przedm_stu;
  struct kurs *przedmioty[IL_PRZEDM];
 };

shint baza_main(void *A);
shint baza_blankiet(shint nr_rekordu, shint ob_pocz,
                    shint ob_konc, shint x_lewy_gorny,
                    shint y_lewy_gorny, shint anim_pid,
                    char *D);
char *dane_baza(shint ob_pocz, shint ob_konc, shint *rozmiar_ob);
void wpis_baza(shint ob_pocz, shint ob_konc,
               char *D, shint rozmiar_ob, char zapis[], char *Kod_op);
shint dec_baza(shint decyzja, shint kod_decyzji, shint nr_dec,
                      shint kod_raportu, shint np, shint *nr_rekordu);
