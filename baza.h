#define DL_IMIE      25  // d�ugo�c imienia
#define DL_NAZW      40  // d�ugo�� nazwiska
#define DL_NR_EW     11  // d�ugo�� numeru ewidencyjnego
#define DL_NAZW_MIEJ 25  // d�ugo�� nazwy miejscowo�ci
#define DL_NAZW_WOJ  25  // d�ugo�� nazwy wojew�dztwa
                         // ...pacjenta
#define IL_PRZEDM 6
struct kurs
 {int nazwa_przedm;  /*numer wsk na pole w tablicy stringow z nazwami przedmiotow*/
 float ocena_egz;
 float ocena_cwi;
 float ocena_lab;
 };

struct pac_data_ur
{
	int dzien;
	int miesiac;
	int rok;
};

struct student
 {int imie_no;
  int nazw_no;
  int index_no;
  int dzien_u;
  int mies_u;
  int rok_u;
  int rok_stud;
  int miasto_no;
  int l_przedm_stu;
  struct kurs *przedmioty[IL_PRZEDM];
 };

int baza_main(void *A);
int baza_blankiet(int nr_rekordu, int ob_pocz,
                    int ob_konc, int x_lewy_gorny,
                    int y_lewy_gorny, int anim_pid,
                    char *D);
char *dane_baza(int ob_pocz, int ob_konc, int *rozmiar_ob);
void wpis_baza(int ob_pocz, int ob_konc,
               char *D, int rozmiar_ob, char zapis[], char *Kod_op);
int dec_baza(int decyzja, int kod_decyzji, int nr_dec,
                      int kod_raportu, int np, int *nr_rekordu);
