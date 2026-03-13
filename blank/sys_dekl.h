#ifndef _SYS_DEKL_H_
#define _SYS_DEKL_H_
#define AG_SIZE 50
#define Q_SIZE  80
/* -------------------------------------------------------------- */
#include "moje.h"

struct Service
{
    char* name; /* nazwa uslugi */
    int l_rek_max; /* max.liczba danych w blankiecie */
    int kod_uslugi; /* unikalny kod uslugi i raportu  */
    int str_size; /* rozmiar struktury danych wlasnych */
    char typ_bazy; /* BAZA_SPOJNA lub BAZA_WEKTOR lub BAZA_LISTA - patrz wewybl.h */
    void* D; /* adres bazy danych (NULL - bedzie alkokowana) */
    int (*main_modul)(void* A); /* nazwa modulu glownego (NULL - nie jest zleceniem RT) */
    int (*def_blankiet)(int nr_rekordu, int ob_pocz,
                        int ob_konc, int x_lewy_gorny,
                        int y_lewy_gorny, int kod_uslugi,
                        char* adr_rek0);
    char*(*dane_rap_bl)(int ob_pocz, int ob_konc, int* rozmiar_ob);
    void (*wpis_rap_bl)(int ob_pocz, int ob_konc,
                        char* D, int rozmiar_ob, char zapis[], char* Kod_op);
    int (*decyzje)(int decyzja, int kod_decyzji, int nr_dec,
                   int kod_uslugi, int np, int* nr_rekordu);
};

/* Stany alarmu dla rt_data.alarm */
#define RT_ALARM_OK      0  /* brak alarmu               */
#define RT_ALARM_TIMEOUT 1  /* przekroczony czas pomiaru */
#define RT_ALARM_RANGE   2  /* wartosc poza zakresem     */

/* Dane pomiarowe przekazywane przez kazdy modul RT.
 * Kazdy main_modul() zapisuje tutaj wynik swojego pomiaru;
 * system (sprawdz, MQTT) czyta te pola bez znajomosci
 * wewnetrznej struktury uslugi.                         */
struct rt_data
{
    float value;    /* biezaca wartosc pomiaru              */
    float elapsed;  /* czas trwania / liczba probek [s]     */
    float val_max;  /* maksymalna zaobserwowana wartosc     */
    float val_min;  /* minimalna zaobserwowana wartosc      */
    int   alarm;    /* stan alarmu: RT_ALARM_*              */
};

struct agenda
{
    struct Service* S; /* adres danych stalych uslugi (struct Service *)*/
    char* name; /* nazwa uslugi */
    int Interval; /* okres wywolan */
    float delay; /* l.sekund do najblizszego wywolania */
    char mode; /* p stale; s seria; t - jednorazowe */
    int number_of_calls; /* liczba wykonan: -1 stale; 0 usuwamy; >0 cykliczne */
    int prior; /* priorytet */
    int prior_plus; /* przyrost priorytetu po 1 sek.*/
    int cur_prior;
    time_t last_time;
    int state; /* stan wykonania: 0 - zakoncz; 1 - poczatek; 2, .. dalej */
    int status; /* 0 nie wchodzi gdy stare w kol; 1 wchodzi zawsze; 2 usuwa stare */
    void* data; /* dalsze (specyficzne) dane dane uslugi */
    struct rt_data rt; /* dane pomiarowe RT */
};


struct _dane_pacjenta_
{
    int pac_nr_ew;
    int pac_imie;
    int pac_nazwisko;
};


int insert_to_queue(struct agenda* A);
void cancel_serv(struct agenda* A, int i);
int insert_to_agenda(struct agenda* A);
struct agenda** getAgendaPtr(int* ag_size);
struct agenda** getQueuePtr(int* q_size);
void** getRepDataPtr(int* d_size);
void freeAgendaPtr(void);
void freeQueuePtr(void);
void freeRepDataPtr(void** DataPtr);
void free_agenda_mem(struct agenda* A);
void free_service(struct agenda* A);
struct agenda* service_default(int (*decyzje)(int decyzja, int kod_decyzji,
                                              int nr_dec, int kod_uslugi, int np, int* nr_rekordu));
int czas_zegarowy(int* godz, int* min, int* sek);
int data_systemowa(int* dzien, int* miesiac, int* rok);
struct agenda* rekord_zlecenia_agendy(int* nr_rekordu, int nr_w_agendzie, int kod_uslugi);
/* Anew i Aserv sa stanem lokalnym kazdego modulu uslugi - deklaruj w swoim .c */
#define NO_PRIOR    0
#define MAX_PRIOR   1
#define RAND_PRIOR  2

/* Blankiet z wlasnym UI moze zakonczyc sie lokalnie bez zamykania
   calego systemu raportow. Silnik blankietu tlumaczy ten kod na
   standardowe wyjscie z raportu. */
#define BLANKIET_UI_EXIT (-2)
#endif
