
/****************************************/
/* Definicja wszystkich staˆych potrzebnych do narysowania wykresu */
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
/* opis ramki - nie modyfikowa† */
int KON_RAM_XX = MAX_X - TAB_R;        /* konce pola wykresu */
int KON_RAM_YY = MAX_Y - TAB_R;
int OS_POCZZ = 2 * TAB_R;              /* poczatek osi x */
int OS_X_KONN = MAX_X - 2 * TAB_R ;    /* koniec osi x */
int OS_Y_KONN = MAX_Y - 2 * TAB_R;     /* koniec osi y */
int X_HALFF = MAX_X/2;                 /* srodek wykresu */
int Y_HALFF = MAX_Y/2;
/*****************************************/
/* opis strzaˆki - nie modyfikowa†*/
int GROT_XX = MAX_X - 2 * TAB_R - DL_GROT; /* srodek grotu x */
int GROT_XGG = MAX_Y/2 - DL_GROT;          /* gora grotu x */
int GROT_XDD = MAX_Y/2 + DL_GROT;          /* dol grotu x */
int GROT_YY = 2 * TAB_R + DL_GROT;         /* srodek grotu y */
int GROT_YLL = MAX_X/2 - DL_GROT;          /* lewa strona grotu y */
int GROT_YPP = MAX_X/2 + DL_GROT;          /* prawa strona grotu y */
/*****************************************/
/* skala - nie modyfikowa† */                     
int X_SKALL = MAX_X - 2 * TAB_R;                   /* dlugosc skali wykresu x */
int Y_SKALL = MAX_Y - 2 * TAB_R;                   /* dlugosc skali wykresu y */
int SKOK_XX = (MAX_X - 4 * TAB_R)/10;              /* krok wartosci x */
int SKOK_YY = (MAX_Y - 4 * TAB_R)/10;              /* krok wartosci y */
int ZAK_XX = (5 * (MAX_X - 4 * TAB_R)/10) - TAB_R; /* zakres x */
int ABSS = MAX_Y/2 - 2*TAB_R;                      /* zakres y */
/****************************************/
float zm_x;
