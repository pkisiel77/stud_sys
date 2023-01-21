/* 
   Plik: pacjnet .h
   Data: 29.10.2001
   ...na podstawie STUD_BAZ.C
*/

#define L_KASY_CHORYCH 13

// struktura g³owna blankietu
struct pacjnet
 {
  shint pac_id;
  shint pac_imie;
  shint pac_nazwisko;
  shint nr_ewidencyjny;

  shint ur_dzien;// dzieñ urodzin
  shint ur_miesiac;// miesi¹c urodzin
  shint ur_rok;// rok urodzin

  shint ur_miasto;// miasto urodzin
  shint wojewodztwo;

  shint nr_kasy_chorych[L_KASY_CHORYCH];
 };
//

// funkcje blankietu
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
//

