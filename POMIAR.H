#define ROUND(liczba)\
(int)(((float)liczba)+((liczba>0)?0.5:-0.5))

shint pomiar_main(void *A);
shint pomiar_blankiet(shint nr_rekordu, shint ob_pocz,
                    shint ob_konc, shint x_lewy_gorny,
                    shint y_lewy_gorny, shint anim_pid,
                    char *D);
char *dane_pomiar(shint ob_pocz, shint ob_konc, shint *rozmiar_ob);
void wpis_pomiar(shint ob_pocz, shint ob_konc,
               char *D, shint rozmiar_ob, char zapis[], char *Kod_op);
shint dec_pomiar(shint decyzja, shint kod_decyzji, shint nr_dec,
                      shint kod_raportu, shint np, shint *nr_rekordu);

#define L_OCEN 150
#define IL_WYN 6
#define IL_DAN 10

// sta³e symboliczne dla blankietu POMIAR wa¿ne

// sta³e okreœlaj¹ce cechy pomiaru
#define POMIAR_DYNAMICZNY 1 // pomiar z portu, karty plus zapis na dysk
#define POMIAR_STATYCZNY  2 // odczyt danych z dysku

// tablica przechowuj¹ca dane pomiarowe przyjeto czas pomiaru 1 minutkê == 60s
#define TAB_POM_X 1000 // amplituda
#define TAB_POM_Y 2 // czas pomiaru
// zmienna okreœlaj¹ca wybór pomiaru
#define POMIAR   0
#define POMIAR_1 1
#define POMIAR_2 2

//

#define NAZWA_PLIKU 11

struct pomiar
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
  int beep;
  
  // nowe funkcje
  shint x,y;
  shint typ_pom;
  float tablica_pomiarowa[TAB_POM_X];
  shint usluga_pomiar;
  shint czas_pomiaru;

  char *zrodlo_danych;
  char *nazwa_pliku;
  char decyzja;
 };

