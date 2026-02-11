
/****************************************/
/* Definicja wszystkich sta�ych potrzebnych do narysowania wykresu */
#define KOL_TLA 7
#define KOL_WYK 1
#define DL_GROT 3       /* dlugosc grotu */
#define MAX_X 300       /* szerokosc wykresu */
#define MAX_Y 180       /* wysokosc wykresu */
#define WSP_X 20        /* wspolrzedne lewego gornego */
#define WSP_Y 20        /* rogu wykresu */
#define TAB_R 10        /* odleglosc obramowania od brzegu wykresu */
#define PODZ 2          /* dlugosc kreski podzialki */
#define PREC_W 0.1      /* precyzja wykresu (minimalnie 0.1) */
#define PI 3.141592654
/*****************************************/
/* opis ramki - nie modyfikowa� */
int KON_RAM_X = MAX_X - TAB_R;        /* konce pola wykresu */
int KON_RAM_Y = MAX_Y - TAB_R;
int OS_POCZ = 2 * TAB_R;              /* poczatek osi x */
int OS_X_KON = MAX_X - 2 * TAB_R ;    /* koniec osi x */
int OS_Y_KON = MAX_Y - 2 * TAB_R;     /* koniec osi y */
int X_HALF = MAX_X/2;                 /* srodek wykresu */
int Y_HALF = MAX_Y/2;
/*****************************************/
/* opis strza�ki - nie modyfikowa�*/
int GROT_X = MAX_X - 2 * TAB_R - DL_GROT; /* srodek grotu x */
int GROT_XG = MAX_Y/2 - DL_GROT;          /* gora grotu x */
int GROT_XD = MAX_Y/2 + DL_GROT;          /* dol grotu x */
int GROT_Y = 2 * TAB_R + DL_GROT;         /* srodek grotu y */
int GROT_YL = MAX_X/2 - DL_GROT;          /* lewa strona grotu y */
int GROT_YP = MAX_X/2 + DL_GROT;          /* prawa strona grotu y */
/*****************************************/
/* skala - nie modyfikowa� */                     
int X_SKAL = MAX_X - 2 * TAB_R;                   /* dlugosc skali wykresu x */
int Y_SKAL = MAX_Y - 2 * TAB_R;                   /* dlugosc skali wykresu y */
int SKOK_X = (MAX_X - 4 * TAB_R)/10;              /* krok wartosci x */
int SKOK_Y = (MAX_Y - 4 * TAB_R)/10;              /* krok wartosci y */
int ZAK_X = (5 * (MAX_X - 4 * TAB_R)/10) - TAB_R; /* zakres x */
int ABS = MAX_Y/2 - 2*TAB_R;                      /* zakres y */
/****************************************/
float zm_x;
