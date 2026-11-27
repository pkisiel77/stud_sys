#ifndef _CORE_H_
#define _CORE_H_
/* Globalny stan aplikacji wspolny dla modulow uslug.
   Definicje sa w core.c (poza lwmall/lwmallp - blank/wewy.c). */
#include "blank/sys_dekl.h"

extern struct Service* Service;  /* usluga wybrana w menu glownym */
extern struct agenda* Agenda;    /* rekord agendy biezacej uslugi (NULL - brak) */
extern int Liczba_opcji;         /* liczba uslug zwrocona przez set_services() */
extern int X_time, X_tyt;        /* kolumny zegara i tytulu na pasku statusu */
extern int y_shift;              /* przesuniecie okna blankietu w pionie */
extern unsigned short int NODE_NO; /* numer wezla (MQTT, client_id) */

/* Atrybuty kolorow interfejsu */
extern unsigned int attr;        /* tlo menu i formularzy */
extern unsigned int at_wpis;     /* pole wpisu / podswietlenie */
extern unsigned int attr_time;   /* zegar */
extern unsigned int attr_title;  /* tytul okna */

/* Licznik alokacji rekordow agendy (raport pamieci); blank/wewy.c */
extern int lwmall, lwmallp;
#endif
