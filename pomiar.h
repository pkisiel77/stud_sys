#define ROUND(liczba)\
(int)(((float)liczba)+((liczba>0)?0.5:-0.5))

int pomiar_main(void* A);
int pomiar_blankiet(int nr_rekordu, int ob_pocz,
                    int ob_konc, int x_lewy_gorny,
                    int y_lewy_gorny, int anim_pid,
                    char* D);
char* dane_pomiar(int ob_pocz, int ob_konc, int* rozmiar_ob);
void wpis_pomiar(int ob_pocz, int ob_konc,
                 char* D, int rozmiar_ob, char zapis[], char* Kod_op);
int dec_pomiar(int decyzja, int kod_decyzji, int nr_dec,
               int kod_raportu, int np, int* nr_rekordu);

#define L_OCEN 150
#define IL_WYN 6
#define IL_DAN 10

// sta�e symboliczne dla blankietu POMIAR wa�ne

// sta�e okre�laj�ce cechy pomiaru
#define POMIAR_DYNAMICZNY 1 // pomiar z portu, karty plus zapis na dysk
#define POMIAR_STATYCZNY  2 // odczyt danych z dysku

// tablica przechowuj�ca dane pomiarowe przyjeto czas pomiaru 1 minutk� == 60s
#define TAB_POM_X 1000 // amplituda
#define TAB_POM_Y 2 // czas pomiaru
// zmienna okre�laj�ca wyb�r pomiaru
#define POMIAR   0
#define POMIAR_1 1
#define POMIAR_2 2

//

#define NAZWA_PLIKU 11

struct pomiar
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
    int beep;

    // nowe funkcje
    int x, y;
    int typ_pom;
    float tablica_pomiarowa[TAB_POM_X];
    int usluga_pomiar;
    int czas_pomiaru;

    char* zrodlo_danych;
    char* nazwa_pliku;
    char decyzja;
};
