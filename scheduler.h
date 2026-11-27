#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_
/* Planista RT (scheduler.c). Funkcje dla modulow uslug (insert_to_agenda,
   service_default, getAgendaPtr, ...) sa zadeklarowane w blank/sys_dekl.h;
   tutaj jest stan planisty i jego petla. */
#include <time.h>
#include "blank/sys_dekl.h"

extern struct agenda* SysA[AG_SIZE]; /* agenda zlecen; SysA[0] - zlecenie systemowe */
extern struct agenda* SysQ[Q_SIZE];  /* kolejka zlecen gotowych do wykonania */
extern int AgendaMax;                /* najwyzszy zajety indeks SysA */
extern int NoServisMax;              /* najwyzszy zajety indeks SysQ */
extern char select_mode;             /* NO_PRIOR / MAX_PRIOR / RAND_PRIOR */
extern time_t sek_akt;               /* czas ostatniego tykniecia planisty */

void scheduler_init(void);
void chk_time(void);
void check_services(void);
void check_agenda(time_t dt);
struct agenda* check_queue(void);
#endif
