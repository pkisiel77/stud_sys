
#define DL_IMIE      25  // d³ugoœc imienia
#define DL_NAZW      40  // d³ugoœæ nazwiska
#define DL_NR_EW     11  // d³ugoœæ numeru ewidencyjnego
#define DL_NAZW_MIEJ 25  // d³ugoœæ nazwy miejscowoœci
#define DL_NAZW_WOJ  25  // d³ugoœæ nazwy województwa
                         // ...pacjenta

#define PATH_FIX "c:\\pacjent\\dane\\fix.dat"   // sciezka do fix'a
#define PATH_BAZA "c:\\pacjent\\dane\\baza.dat" // sciezka do bazy


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

/*
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
*/

/*

*/

struct student
{
//	shint  pac_id;
	shint  pac_imie;
	shint  pac_nazwisko;
	shint  pac_nr_ew;
    shint  pac_ur_dzien;
    shint  pac_ur_miesiac;
    shint  pac_ur_rok;
	shint  pac_nr_kasy;
	shint  pac_miejsce_zam;
//	shint  pac_woj;
	shint l_przedm_stu;
	struct kurs *przedmioty[IL_PRZEDM];
};

shint pacjent_main(void *A);
shint pacjent_blankiet(shint nr_rekordu, shint ob_pocz,
                    shint ob_konc, shint x_lewy_gorny,
                    shint y_lewy_gorny, shint anim_pid,
                    char *D);
char *dane_pacjent(shint ob_pocz, shint ob_konc, shint *rozmiar_ob);
void wpis_pacjent(shint ob_pocz, shint ob_konc,
               char *D, shint rozmiar_ob, char zapis[], char *Kod_op);
shint dec_pacjent(shint decyzja, shint kod_decyzji, shint nr_dec,
                      shint kod_raportu, shint np, shint *nr_rekordu);

