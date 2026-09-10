/* #include <butil.h> */
#include "blank/moje.h"
#include "sys_rep.h"
#include "loc.h"
void chk_time(void);
extern struct Service *Service;
extern struct agenda *Agenda;
extern int X_time, X_tyt;
extern unsigned int attr_title;
extern struct agenda *SysA[AG_SIZE];
extern struct agenda *SysQ[Q_SIZE];
int sys_status_dashboard_active = 0;

static int agenda_is_queued(const struct agenda *A)
{
    int i;
    for (i = 0; i < Q_SIZE; i++) {
        if (SysQ[i] == A) return 1;
    }
    return 0;
}

int sys_status_snapshot(struct sys_status_row *rows, int capacity,
                        struct sys_status_summary *summary)
{
    int i, count = 0;

    if (summary != NULL) {
        summary->active = 0;
        summary->queued = 0;
        summary->alarms = 0;
        for (i = 0; i < Q_SIZE; i++) {
            if (SysQ[i] != NULL && SysQ[i]->number_of_calls != 0)
                summary->queued++;
        }
    }

    for (i = 0; i < AG_SIZE; i++) {
        struct agenda *A = SysA[i];
        struct sys_status_row *row;
        const char *name;

        if (A == NULL) continue;
        if (summary != NULL) {
            if (A->number_of_calls != 0) {
                summary->active++;
                if (A->rt.alarm != RT_ALARM_OK) summary->alarms++;
            }
        }
        if (rows == NULL || count >= capacity) {
            count++;
            continue;
        }

        row = &rows[count++];
        name = A->name;
        if ((name == NULL || name[0] == '\0') && A->S != NULL) name = A->S->name;
        snprintf(row->name, sizeof(row->name), "%s", name != NULL ? name : "-");
        row->agenda_index = i;
        row->mode = A->mode;
        row->interval = A->Interval;
        row->delay = A->delay;
        row->priority = A->prior;
        row->calls_left = A->number_of_calls;
        row->alarm = A->rt.alarm;
        row->value = A->rt.value;
        if (A->number_of_calls == 0) row->state = SYS_STATUS_DISABLED;
        else if (A->state < 0) row->state = SYS_STATUS_RUNNING;
        else if (agenda_is_queued(A)) row->state = SYS_STATUS_READY;
        else row->state = SYS_STATUS_WAITING;
    }
    return count;
}

static const char *status_state_name(int state)
{
    switch (state) {
    case SYS_STATUS_READY: return L_STATUS_READY;
    case SYS_STATUS_RUNNING: return L_STATUS_RUNNING;
    case SYS_STATUS_DISABLED: return L_STATUS_DISABLED;
    default: return L_STATUS_WAITING;
    }
}

static const char *status_alarm_name(int alarm)
{
    switch (alarm) {
    case RT_ALARM_RANGE: return L_STATUS_ALARM_RANGE;
    case RT_ALARM_TIMEOUT: return L_STATUS_ALARM_TIMEOUT;
    default: return L_STATUS_ALARM_OK;
    }
}

static const char *status_mode_name(char mode)
{
    switch (mode) {
    case 'p': return L_STATUS_MODE_PERMANENT;
    case 's': return L_STATUS_MODE_SERIAL;
    case 't': return L_STATUS_MODE_ONESHOT;
    default: return L_STATUS_MODE_UNKNOWN;
    }
}

static void render_status_dashboard(int selected)
{
    struct sys_status_row rows[AG_SIZE];
    struct sys_status_summary summary;
    const unsigned int base = TERM_WHITE | TERM_BLUE_BG;
    const unsigned int heading = TERM_WHITE | MTERM_HILIGHT | TERM_CYAN_BG;
    const unsigned int selected_attr = TERM_BLACK | MTERM_HILIGHT | TERM_WHITE_BG;
    const unsigned int alarm_attr = TERM_WHITE | MTERM_HILIGHT | TERM_RED_BG;
    char calls[24];
    int count, page_size, page_start, page_end, i, y;

    count = sys_status_snapshot(rows, AG_SIZE, &summary);
    if (selected < 0) selected = 0;
    if (selected >= count && count > 0) selected = count - 1;
    page_size = MY_MAX - 7;
    if (page_size < 1) page_size = 1;
    page_start = (selected / page_size) * page_size;
    page_end = page_start + page_size;
    if (page_end > count) page_end = count;

    term_fill(base);
    term_printf(0, 0, heading, "%-*.*s", MX_MAX, MX_MAX, L_STATUS_TITLE);
    term_printf(1, 1, base, L_STATUS_SUMMARY,
                summary.active, summary.queued, summary.alarms);
    term_printf(2, 1, base, L_STATUS_PAGE,
                count == 0 ? 0 : page_start / page_size + 1,
                count == 0 ? 0 : (count + page_size - 1) / page_size);
    term_type(3, 0, MX_MAX >= 76 ? L_STATUS_COLUMNS : L_STATUS_COLUMNS_COMPACT,
              0, heading);

    if (count == 0) term_type(5, 2, L_STATUS_EMPTY, 0, base);
    for (i = page_start, y = 4; i < page_end; i++, y++) {
        unsigned int row_attr = i == selected ? selected_attr : base;
        if (rows[i].state != SYS_STATUS_DISABLED && rows[i].alarm != RT_ALARM_OK)
            row_attr = alarm_attr;
        if (MX_MAX >= 76) {
            term_printf(y, 0, row_attr,
                        " %2d  %-22.22s %-8.8s %6.1f %6d %5d %9.2f %-7.7s",
                        rows[i].agenda_index, rows[i].name,
                        status_state_name(rows[i].state), rows[i].delay,
                        rows[i].interval, rows[i].priority, rows[i].value,
                        status_alarm_name(rows[i].alarm));
        } else {
            term_printf(y, 0, row_attr, " %2d  %-22.22s %-8.8s %6.1f %-7.7s",
                        rows[i].agenda_index, rows[i].name,
                        status_state_name(rows[i].state), rows[i].delay,
                        status_alarm_name(rows[i].alarm));
        }
    }

    if (count > 0) {
        if (rows[selected].calls_left < 0)
            snprintf(calls, sizeof(calls), "%s", L_STATUS_CALLS_PERMANENT);
        else
            snprintf(calls, sizeof(calls), "%d", rows[selected].calls_left);
        term_printf(MY_MAX - 2, 1, base, L_STATUS_DETAIL,
                    rows[selected].agenda_index, rows[selected].name,
                    status_mode_name(rows[selected].mode), calls);
    }
    term_printf(MY_MAX, 0, heading, "%-*.*s", MX_MAX, MX_MAX, L_STATUS_HELP);
    term_flush();
}

int sys_status_dashboard(void)
{
    struct sys_status_summary summary;
    int selected = 0, key, scan, count, page_size;
    int force_render = 1, last_width = -1, last_height = -1;
    time_t last_render = 0, now;

    sys_status_dashboard_active = 1;
    setcursor(nocursor);
    for (;;) {
        now = time(NULL);
        if (MX_MAX != last_width || MY_MAX != last_height) force_render = 1;
        if (force_render || now != last_render) {
            count = sys_status_snapshot(NULL, 0, &summary);
            if (selected >= count && count > 0) selected = count - 1;
            render_status_dashboard(selected);
            last_render = now;
            last_width = MX_MAX;
            last_height = MY_MAX;
            force_render = 0;
        }

        key = GET_char();
        if (key < 0) continue;
        if (key == 27) break;
        if (key == 'r' || key == 'R') {
            force_render = 1;
            continue;
        }
        if (key != 0) continue;

        scan = GET_char();
        count = sys_status_snapshot(NULL, 0, &summary);
        page_size = MY_MAX - 7;
        if (page_size < 1) page_size = 1;
        switch (scan) {
        case 72: if (selected > 0) selected--; break;
        case 80: if (selected + 1 < count) selected++; break;
        case 73: selected -= page_size; if (selected < 0) selected = 0; break;
        case 81:
            selected += page_size;
            if (selected >= count && count > 0) selected = count - 1;
            break;
        case 71: selected = 0; break;
        case 79: if (count > 0) selected = count - 1; break;
        default: continue;
        }
        force_render = 1;
    }
    sys_status_dashboard_active = 0;
    setcursor(cursor);
    return 0;
}

struct agenda **SA;
int sys_main(void *DA)
 {struct agenda *A;
	A=(struct agenda *)DA;
	A->state=0;
	return A->state;
 };
/* ----------------------------- w moje.h ---------------
#define DEC_TYP_US 2
#define DEC_NEW    3
 -------------------------------------------------------- */
#define DEC_USUN   11
static int nr_ag=0;
int sys_blankiet(int nr_rekordu, int ob_pocz, int ob_konc,
						int x_lewy_gorny, int y_lewy_gorny, int anim_pid, char *D)
/* ------------- D jest adresem zerowego rekordu bazy ----------------- */
 {struct agenda *A, **SA;
	int ls, nr_rek, size, ochr, ochrf=-3, raport, ret;
	static int z_min=0, z_max;
	int i, lsa;
	(void)ob_pocz;
	(void)ob_konc;
	(void)x_lewy_gorny;
	(void)y_lewy_gorny;
	(void)anim_pid;
	(void)D;
	SA=(struct agenda **)getAgendaPtr(&lsa);
	nr_rek=nr_rekordu;
	A=(struct agenda *)dane_raportowanego_rekordu(sys_blankiet,&nr_rek);
	nr_ag=nr_rek;
	if(A==NULL && nr_rek>=0)
	 {term_printf(MY_MAX,X_L0,ATTR_A,L_RAP_NO_RECORD_SYS,nr_rek);
		GET_char();  return -1;
	 }
	rekord_danych_do_naglowka(nr_ag);
	for(z_min=0;z_min<lsa;z_min++) {if(SA[z_min]!=NULL) break;}
	for(i=0,ls=0;i<lsa;i++) {if(SA[i]!=NULL) {ls++; z_max=i;}}
	if(ls==0)
	 {ret=dana_koment(-1, 26,L_RAP_NO_ORDERS_AGENDA);
		return ret;
	 }
	ustaw_rek_max_raportu(sys_blankiet, z_max);
/* ------------------------------------------------------- */
	ret=dana_koment(-1,16,L_RAP_TITLE_PERIODIC,ls);
	ret=dana_int_dec(-1,-1,L_RAP_ORDER_RANGE, &z_min,&z_max,
									 &nr_ag, size=2, ochr=2, raport=-1, DEC_NEW);
	if(A==NULL)
	 {ret=dana_koment(-1,10,L_RAP_NO_ORDER_AT_POS,nr_ag);}
	else
	 {const char *nazwa;
	  nazwa=(A->name!=NULL)?A->name:(((A->S)!=NULL && (A->S)->name!=NULL)?(A->S)->name:"");
	  ret=dana_koment(-1,-1,L_RAP_SERVICE_LINE,
										nazwa, A->mode, A->prior, A->Interval, A->delay);
	 {static char *typ_usl[3]={L_CORE_TYP_PERM, L_CORE_TYP_SERIAL, L_CORE_TYP_ONESHOT};
		ret=dana_decyzyjna(-1,-1,L_RAP_TYP_USL_NO_PREFIX, "p/s/t", typ_usl, 3,
											 &(A->mode), ochr=5, DEC_TYP_US);
	 }
	 {static int nmin=1, nmax=3000;
		ochr=-1;
		if(A->mode=='p')
		 {ret=dana_koment(-1, -1, L_RAP_PERMANENT_SHORT);}
		else
		 {ret=dana_int(-1,-1,L_RAP_NUM_CALLS, &nmin,&nmax,
										 &(A->number_of_calls), size=4, ochr, raport=-1);
		 }
	 }
	 {static int nmin=1, nmax=3000, del_min, del_sek;
		del_min=(long int)A->delay/60l; del_sek=((long int)A->delay)%60l;
		ret=dana_int(-1,-1,L_RAP_PERIOD_BR_QQ, &nmin,&nmax,
			&(A->Interval), size=4, ochr=5, raport=-1);
		ret=dana_koment(-1,-1,L_RAP_TIME_TO_NEXT,
			del_min, del_sek);
	 }
	 {static int nmin=1, nmax=3000, stmin=-999;
		ret=dana_int(-1,-1,L_RAP_PRIORITY, &nmin,&nmax,
			&(A->prior), size=4, ochr=5, raport=-1);
		ret=dana_int(-1,-1,L_RAP_STATE_PROGRESS, &stmin,&nmax,
			&(A->state), size=4, ochrf, raport=-1);
	 }
	}
	{static char *usun[3]={L_RAP_MENU_DATA_ENTRY,L_CORE_RUN_MAIN_MENU,L_RAP_MENU_DELETE}, dec='d';
	 ret=dana_decyzyjna(-1,-1,L_RAP_PROMPT_ACTION, "d/m/u", usun, 3,
										 &dec, ochr=0, DEC_USUN);
	}
/*    ret=dana_znak(-1,-1," Typ uslugi <%s>", "p/s/t", typ_usl, 3, &(A->mode), ochr=5);  */
	return ret;
 }

extern struct agenda *SysA[];

char *dane_sys(int ob_pocz, int ob_konc, int *rozmiar_ob)
 {int ns, x, y;
/*  struct Service *S; */
  for(ns=0; ns<AG_SIZE; ns++) {if(SysA[ns]!=NULL) break;}
	*rozmiar_ob=sizeof(struct agenda *);
	nr_ag=ns; if(ns==AG_SIZE) nr_ag=-1;
  x=m_wherex(); y=m_wherey();
  if(nr_ag>=0) term_printf(Y_G0,X_tyt,attr_title,"%s",(SysA[nr_ag]->S)->name);
  else term_printf(Y_G0,X_tyt,attr_title,"%s",Service->name);
  term_cur(y,x);
  return (char *)SysA;
 }

void wpis_sys(int ob_pocz, int ob_konc,
					 char *D, int rozmiar_ob, char zapis[], char *Kod_op)
 {freeAgendaPtr();}
/* -------------- def. w sys_dekl.h ----------------
struct agenda
 {struct Service *S;    / * adres danych stalych uslugi (struct Service *)* /
  int Interval;       / * okres wywolan * /
  int delay;          / * l.sekund do najblizszego wywolania * /
  char mode;            / * p stale; s seria; t - jednorazowe * /
  int number_of_calls; / * liczba wykonan: -1 stale; 0 usuwamy; >0 cykliczne * /
	int prior;         / * priorytet * /
	int prior_plus;    / * przyrost priorytetu po 1 sek.* /
	int state;         / * stan wykonania: 0 - zakoncz; 1 - poczatek; 2, .. dalej * /
	void *data;          / * dane dynamiczne * /
 } ----------------------------------------------------------------- */
int dec_sys(int decyzja, int kod_decyzji, int nr_dec,
											int kod_raportu, int np, int *nr_rekordu)
 {struct agenda *A;
/*  struct Service *S; */
	int p_min, p_max, ret;
	(void)czy_zdefiniowany(kod_raportu, &p_min, &p_max, &ret);
  A=(struct agenda *)ustal_adres_rek(kod_raportu,*nr_rekordu);
	switch(kod_decyzji)
	 {case DEC_USUN:
			switch(decyzja)
			 {case 'd': return kod_raportu;
				case 'u':
					A->number_of_calls=0;
					A->state=0;
					break;
				case 'm': return -1;
				default: break;
			 }
			break;
		case DEC_NEW:
			*nr_rekordu=nr_ag;
			break;
/*			SA=(struct agenda *)podaj_adres_rap(kod_raportu);
			for(ns=nr_ag+1; ns<AG_SIZE; ns++) {if(SA[ns]!=NULL) break;}
			if(ns==AG_SIZE)
			 {for(ns=0; ns<=nr_ag; ns++) {if(SA[ns]!=NULL) break;}
				nr_ag=ns;
			 }
			else nr_ag=ns;
			break; */
		case DEC_TYP_US:
			switch(decyzja)
			 {case 's':
				 if(A->number_of_calls<2) A->number_of_calls=2;
				 break;
				case 'p':
				 A->number_of_calls=-1;
				 break;
				case 't':
				 A->number_of_calls=1;
				 break;
				default: break;
			 }
			break;
		default: break;
	 }
	return kod_raportu;
 }
/* --------------------------------------------------------------- */
