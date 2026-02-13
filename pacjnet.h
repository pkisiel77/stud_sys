/* 
   Plik: pacjnet .h
   Data: 29.10.2001
   ...na podstawie STUD_BAZ.C
*/

#define L_KASY_CHORYCH 13

// struktura g�owna blankietu
struct pacjnet
{
    int pac_id;
    int pac_imie;
    int pac_nazwisko;
    int nr_ewidencyjny;

    int ur_dzien; // dzie� urodzin
    int ur_miesiac; // miesi�c urodzin
    int ur_rok; // rok urodzin

    int ur_miasto; // miasto urodzin
    int wojewodztwo;

    int nr_kasy_chorych[L_KASY_CHORYCH];
};

//

// funkcje blankietu
int pacjent_main(void* A);
int pacjent_blankiet(int nr_rekordu, int ob_pocz,
                     int ob_konc, int x_lewy_gorny,
                     int y_lewy_gorny, int anim_pid,
                     char* D);
char* dane_pacjent(int ob_pocz, int ob_konc, int* rozmiar_ob);
void wpis_pacjent(int ob_pocz, int ob_konc,
                  char* D, int rozmiar_ob, char zapis[], char* Kod_op);
int dec_pacjent(int decyzja, int kod_decyzji, int nr_dec,
                int kod_raportu, int np, int* nr_rekordu);
//
