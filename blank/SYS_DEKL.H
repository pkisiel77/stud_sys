#ifndef _SYS_DEKL_H_
#define _SYS_DEKL_H_
#define AG_SIZE 50
#define Q_SIZE  80
/* -------------------------------------------------------------- */
#include "moje.h"
struct Service
 {char *name;           /* nazwa uslugi */
	shint l_rek_max;      /* max.liczba danych w blankiecie */
	shint kod_uslugi;     /* unikalny kod uslugi i raportu  */
	shint str_size;       /* rozmiar struktury danych wlasnych */
	char typ_bazy;        /* BAZA_SPOJNA lub BAZA_WEKTOR lub BAZA_LISTA - patrz wewybl.h */
	void *D;              /* adres bazy danych (NULL - bedzie alkokowana) */
	shint (*main_modul)(void *A); /* nazwa modulu glownego (NULL - nie jest zleceniem RT) */
	shint (*def_blankiet)(shint nr_rekordu, shint ob_pocz,
												shint ob_konc, shint x_lewy_gorny,
												shint y_lewy_gorny, shint kod_uslugi,
												char *adr_rek0);
	char *(*dane_rap_bl)(shint ob_pocz, shint ob_konc, shint *rozmiar_ob);
	void (*wpis_rap_bl)(shint ob_pocz, shint ob_konc,
											char *D, shint rozmiar_ob, char zapis[], char *Kod_op);
	shint (*decyzje)(shint decyzja, shint kod_decyzji, shint nr_dec,
											shint kod_uslugi, shint np, shint *nr_rekordu);
 };

struct agenda
 {struct Service *S;    /* adres danych stalych uslugi (struct Service *)*/
	char *name;           /* nazwa uslugi */
	shint Interval;       /* okres wywolan */
	float delay;          /* l.sekund do najblizszego wywolania */
	char mode;            /* p stale; s seria; t - jednorazowe */
	shint number_of_calls; /* liczba wykonan: -1 stale; 0 usuwamy; >0 cykliczne */
	shint prior;         /* priorytet */
	shint prior_plus;    /* przyrost priorytetu po 1 sek.*/
	shint cur_prior;
	time_t last_time;
	shint state;         /* stan wykonania: 0 - zakoncz; 1 - poczatek; 2, .. dalej */
	shint status;         /* 0 nie wchodzi gdy stare w kol; 1 wchodzi zawsze; 2 usuwa stare */
	void *data;          /* dalsze (specyficzne) dane dane uslugi */
	// proteza
	float dana;
	float czas;
	float wart_max;
	float wart_min;
	shint alarm;
 };


struct _dane_pacjenta_
{
	shint  pac_nr_ew;
	shint  pac_imie;
	shint  pac_nazwisko;
};



shint insert_to_queue(struct agenda *A);
void cancel_serv(struct agenda *A, shint i);
shint insert_to_agenda(struct agenda *A);
struct agenda **getAgendaPtr(shint *ag_size);
struct agenda **getQueuePtr(shint *q_size);
void **getRepDataPtr(shint *d_size);
void freeAgendaPtr(void);
void freeQueuePtr(void);
void freeRepDataPtr(void **DataPtr);
void free_service(struct agenda *A);
struct agenda *service_default(shint (*decyzje)(shint decyzja, shint kod_decyzji,
															 shint nr_dec, shint kod_uslugi, shint np, shint *nr_rekordu));
shint czas_zegarowy(shint *godz, shint *min, shint *sek);
shint data_systemowa(shint *dzien, shint *miesiac, shint *rok);
struct agenda *rekord_zlecenia_agendy(shint *nr_rekordu, shint nr_w_agendzie, shint kod_uslugi);
static struct agenda *Anew=NULL, *Aserv=NULL;
#define NO_PRIOR    0
#define MAX_PRIOR   1
#define RAND_PRIOR  2
#endif