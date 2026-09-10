#ifndef STUD_SYS_SYS_REP_H
#define STUD_SYS_SYS_REP_H

struct agenda;  /* Forward declaration */

enum sys_status_state {
    SYS_STATUS_WAITING = 0,
    SYS_STATUS_READY,
    SYS_STATUS_RUNNING,
    SYS_STATUS_DISABLED
};

struct sys_status_row {
    int agenda_index;
    char name[32];
    char mode;
    int interval;
    float delay;
    int priority;
    int calls_left;
    int state;
    int alarm;
    float value;
};

struct sys_status_summary {
    int active;
    int queued;
    int alarms;
};

extern int sys_status_dashboard_active;

struct sys_rep
 {char *nazwa;
  int status;
  int l_rek_max;
  int nr_ag;
  int del_min;
  int del_sek;
  struct agenda *Ag;  /* Changed to pointer */
 };

int sys_main(void *A);
int sys_blankiet(int nr_rekordu, int ob_pocz,
                    int ob_konc, int x_lewy_gorny,
                    int y_lewy_gorny, int anim_pid,
                    char *D);
char *dane_sys(int ob_pocz, int ob_konc, int *rozmiar_ob);
void wpis_sys(int ob_pocz, int ob_konc,
               char *D, int rozmiar_ob, char zapis[], char *Kod_op);
int dec_sys(int decyzja, int kod_decyzji, int nr_dec,
                      int kod_raportu, int np, int *nr_rekordu);
int sys_status_snapshot(struct sys_status_row *rows, int capacity,
                        struct sys_status_summary *summary);
int sys_status_dashboard(void);

#endif
