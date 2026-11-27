/* scheduler.c - planista RT: agenda zlecen (SysA), kolejka wykonania (SysQ)
 * i semafory danych raportow.
 *
 * Co sekunde check_agenda() zmniejsza delay rekordow agendy; gotowe trafiaja
 * do kolejki przez insert_to_queue(), a check_queue() wybiera z niej rekord,
 * ktorego main_modul() uruchamia check_services().
 */
#include "blank/moje.h"
#include "konfig.h"
#include "loc.h"
#include "randf.h"
#include "scheduler.h"
#include "core.h"


#define L_REP_MAX 5

struct agenda* SysA[AG_SIZE];
struct agenda* SysQ[Q_SIZE];
int AgendaMax = -1, NoServisMax = -1;
char select_mode = MAX_PRIOR;
time_t sek_akt;

static void** RepDataPtr[L_REP_MAX];
static int report_act[L_REP_MAX + 2];
static int ag_rep, q_rep;

static void mod_prior(struct agenda* Q);

/* Zeruje agende i kolejke oraz przydziela bufory danych raportow. */
void scheduler_init(void)
{
    int i, k;
    sek_akt = time(NULL);
    AgendaMax = -1;
    NoServisMax = -1;
    for (i = 1; i < AG_SIZE; i++)
    {
        SysA[i] = NULL;
    }
    for (i = 0; i < Q_SIZE; i++)
    {
        SysQ[i] = NULL;
    }
    ag_rep = L_REP_MAX;
    q_rep = ag_rep + 1;
    report_act[ag_rep] = 0;
    report_act[q_rep] = 0;
    for (i = 0; i < L_REP_MAX; i++)
    {
        report_act[i] = 0;
        RepDataPtr[i] = (void*)Malloc(AG_SIZE * sizeof(void*)); /* lwmall++; */
        for (k = 0; k < AG_SIZE; k++)
        {
            RepDataPtr[i][k] = NULL;
        }
    }
}

void chk_time(void)
{
    time_t sek1970, dt;
    sek1970 = time(NULL);
    if (sek_akt != sek1970) /* Zmiana sekundy */
    {
        dt = sek1970 - sek_akt;
        sek_akt = sek1970;
        check_agenda(dt);
    }
}

void check_services(void)
{
    struct agenda* A;
    struct Service* S;
    chk_time(); /* tam sprawdza sie agende po kazdej sekundzie */
    A = check_queue();
    if (A == NULL) return;
    S = (struct Service*)A->S;
    if (S->main_modul != NULL) S->main_modul(A);
}

void check_agenda(time_t dt)
{
    int i;
    struct agenda* A;
    for (i = 0; i <= AgendaMax; i++)
    {
        if (SysA[i] == NULL) continue;
        A = SysA[i];
        if (A->number_of_calls == 0)
        {
            free_service(A);
        }
        else
        {
            A->delay -= dt;
            if (A->delay > 0) continue;
            else
            {
                if (A->number_of_calls > 0) A->number_of_calls--;
                A->delay += A->Interval;
                if (A->delay < 0) A->delay = A->Interval;
                insert_to_queue(A);
            }
        }
    }
    return;
}

int insert_to_agenda(struct agenda* A)
{
    int i, poz = -1, x, y;
    if (A == NULL) return -1;
    if (A->S == NULL) return -1;
    if (A->mode != 'p' && A->mode != 's' && A->mode != 't') return -1;
    for (i = 1; i <= AgendaMax; i++)
    {
        if (SysA[i] == NULL)
        {
            poz = i;
            break;
        }
    }
    if (poz < 0)
    {
        poz = i;
        if (i >= AG_SIZE)
        {
            x = m_wherex();
            y = m_wherey();
            setcursor(nocursor);
            term_printf(MY_MAX, X_L0,ATTR_A, L_CORE_ERR_AGENDA_FULL,
                        ((struct Service*)(A->S))->name);
            term_cur(y, x);
            setcursor(cursor);
            czekaj_na_klawisz();
            return -1;
        }
        else { AgendaMax = poz; }
    }
    {
        struct agenda* An;
        int size, ag_size;
        size = (A->S)->str_size; /* +sizeof(struct agenda);  to jest juz w str_size */
        An = (struct agenda*)Malloc(size);
        if (An == NULL)
        {
            x = m_wherex();
            y = m_wherey();
            setcursor(nocursor);
            term_printf(MY_MAX, X_L0,ATTR_A, L_CORE_ERR_NO_MEM_ORDER, (A->S)->name);
            term_cur(y, x);
            setcursor(cursor);
            czekaj_na_klawisz();
            return -1;
        }
        ag_size = sizeof(struct agenda);
        lwmall++;
        if (A->mode == 'p') A->number_of_calls = -1;
        else { if (A->number_of_calls <= 0) A->number_of_calls = 1; }
        if (A->mode == 't') A->number_of_calls = 1;
        if (A->Interval < 1) A->Interval = 1;
        if (A->delay < 0) A->delay = 0;
        przepisz((char*)An, (char*)A, ag_size);
        if (size > ag_size)
        {
            An->data = (An + 1);
            przepisz((char*)An->data, (char*)A->data, size - ag_size);
        }
        { const char* sn = (An->S)->name; int snlen = sn ? (int)strlen(sn)+1 : 1;
          An->name = (char*)Malloc(snlen); if(An->name) memcpy(An->name, sn ? sn : "", snlen); }
        SysA[poz] = An;
    }
    if (poz > AgendaMax) { AgendaMax = poz; }
    return poz;
}

/* ----------------------------- deklaracja w sys_dekl.h ----------------
struct agenda
 {struct Service *S;    / * adres danych stalych uslugi (struct Service *)* /
	char *name;
	int Interval;       / * okres wywolan * /
	long int delay;       / * l.sekund do najblizszego wywolania * /
	char mode;            / * p stale; s seria; t - jednorazowe * /
	int number_of_calls;/ * liczba wykonan: -1 stale; 0 usuwamy; >0 cykliczne * /
	int prior;          / * priorytet * /
	int prior_plus;     / * przyrost priorytetu po 1 sek.* /
	int cur_prior;
	time_t last_time;
	int state;          / * stan wykonania: 0 - zakoncz; 1 - poczatek; 2, .. dalej * /
	void *data;           / * dane dynamiczne * /
 };  */
struct agenda* service_default(int (*decyzje)(int decyzja, int kod_decyzji, int nr_dec,
                                              int kod_uslugi, int np, int* nr_rekordu))
{
    struct agenda* A;
    int service_no;
    int size, ag_size, i, x, y, str_size;
    for (service_no = 1; service_no < L_SYS; service_no++)
    {
        if (Serv[service_no]->decyzje == decyzje) break;
    }
    if (service_no < 1 || service_no >= L_SYS) return NULL;
    size = Serv[service_no]->str_size; /* +sizeof(struct agenda);  to jest juz w str_size */
    if (Serv[service_no]->main_modul == NULL) return NULL;
    A = (struct agenda*)Malloc(size);
    lwmall++;
    if (A == NULL)
    {
        x = m_wherex();
        y = m_wherey();
        setcursor(nocursor);
        term_printf(MY_MAX, X_L0,ATTR_A, L_CORE_ERR_NO_MEM_ORDER, Serv[service_no]->name);
        term_cur(y, x);
        setcursor(cursor);
        czekaj_na_klawisz();
        return NULL;
    }
    A->S = Serv[service_no];
    A->data = NULL;
    ag_size = sizeof(struct agenda);
    str_size = (A->S)->str_size;
    if (str_size > ag_size)
    {
        A->data = (A + 1);
        for (i = 0; i < str_size - ag_size; i++) { *((char*)A->data + i) = 0; }
    }
    { const char* sn = (A->S)->name; int snlen = sn ? (int)strlen(sn)+1 : 1;
      A->name = (char*)Malloc(snlen); if(A->name) memcpy(A->name, sn ? sn : "", snlen); }
    A->state = 0;
    A->mode = 'n';
    A->number_of_calls = 1;
    A->prior = 1;
    A->prior_plus = 0;
    A->Interval = 60;
    A->cur_prior = A->prior;
    A->delay = 160;
    A->status = 0;
    A->rt.value   = 0.0f;
    A->rt.elapsed = 0.0f;
    A->rt.val_max = 0.0f;
    A->rt.val_min = 0.0f;
    A->rt.alarm   = RT_ALARM_OK;
    return A;
}

static void mod_prior(struct agenda* Q)
{
    (void)Q;
}

void free_agenda_mem(struct agenda* A)
{
    if (A == NULL) return;
    if (A->name != NULL)
    {
        const char* service_name = (A->S != NULL) ? A->S->name : NULL;
        if (A->name != service_name) Free(A->name);
    }
    Free(A);
    lwmall--;
}

void cancel_serv(struct agenda* A, int i)
{
    /* struct queue *Q; */
    if (i >= 0)
    {
        A = SysQ[i];
        if (A->mode == 't') free_service(A);
        SysQ[i] = NULL;
        return;
    }
    if (A->mode == 't')
    {
        free_agenda_mem(A); /* usuwamy servis */
    }
    return;
}

struct agenda* check_queue(void)
{
    int Nrq, lim = 0, max_prior = -1, sel = -1, nq = 0;
    int prior_lim[Q_SIZE], serv_ready[Q_SIZE];
    struct agenda* A;
    /*   struct queue *Q; */
    for (Nrq = 0, nq = -1; Nrq <= NoServisMax; Nrq++)
    {
        if (SysQ[Nrq] == NULL) continue;
        A = SysQ[Nrq]; /*A=Q->A; */
        if (A->state == 0)
        {
            cancel_serv(A, Nrq); /* usuwamy wykonany servis z kolejki */
            if (NoServisMax == Nrq) NoServisMax--;
            continue;
        }
        if (A->state < 0) continue;
        if (select_mode == NO_PRIOR)
        {
            sel = Nrq;
            break;
        }
        if (A->prior_plus > 0) mod_prior(A);
        if (select_mode == MAX_PRIOR)
        {
            if (A->cur_prior > max_prior)
            {
                sel = Nrq;
                max_prior = A->cur_prior;
            }
        }
        else if (select_mode == RAND_PRIOR)
        {
            nq++;
            if (A->cur_prior > 0) lim += A->cur_prior;
            prior_lim[nq] = lim;
            serv_ready[nq] = Nrq;
        }
    }
    if (sel < 0 && nq < 0) return NULL;
    if (sel < 0) /* to jest gdy RAND_PRIOR */
    {
        if (lim <= 0)
        {
            sel = serv_ready[0];
        }
        else
        {
            lim = (int)(lim * RND01());
            for (Nrq = 0; Nrq <= nq; Nrq++)
            {
                if (lim < prior_lim[Nrq])
                {
                    sel = serv_ready[Nrq];
                    break;
                }
            }
            if (sel < 0) sel = serv_ready[nq];
        }
    }
    A = SysQ[sel];
    A->state = -A->state; /* ----------- jest do wykonania -------------- */
    return A;
}

int insert_to_queue(struct agenda* A)
{
    int i, poz = -1, status;
    status = A->status;
    if (status != 1)
    {
        for (i = 0; i <= NoServisMax; i++)
        {
            if (SysQ[i] != A) continue;
            if (status > 1) { A->last_time = sek_akt; } /* usuwamy stare zlecenie */
            return -1;
        }
    }
    if (poz < 0) /* szukamy wolnego miejsca */
    {
        for (i = 0; i <= NoServisMax; i++)
        {
            if (SysQ[i] == NULL)
            {
                poz = i;
                break;
            }
            if (SysQ[i]->state == 0)
            {
                cancel_serv(SysQ[i], i);
                poz = i;
                break;
            }
        }
        if (poz < 0)
        {
            poz = i;
            if (poz >= Q_SIZE)
            {
                term_printf(MY_MAX, X_L0,ATTR_A, L_CORE_ERR_QUEUE_FULL,
                            ((struct Service*)(A->S))->name);
                czekaj_na_klawisz();
                return -1;
            }
            else { NoServisMax = poz; }
        }
    }
    A->state = 1;
    SysQ[poz] = A;
    (SysQ[poz])->last_time = sek_akt;
    SysQ[poz]->cur_prior = A->prior;
    return poz;
}

/* ==================================================================== */
struct agenda** getAgendaPtr(int* ag_size)
{
    *ag_size = AG_SIZE;
    report_act[ag_rep] = 1;
    return SysA;
}

struct agenda** getQueuePtr(int* q_size)
{
    *q_size = Q_SIZE;
    report_act[q_rep] = 1;
    return SysQ;
}

void** getRepDataPtr(int* d_size)
{
    int i;
    for (i = 0; i < L_REP_MAX; i++) { if (report_act[i] == 0) break; }
    if (i == L_REP_MAX)
    {
        *d_size = 0;
        return NULL;
    }
    *d_size = AG_SIZE;
    report_act[i] = 1;
    return RepDataPtr[i];
}

void freeAgendaPtr(void)
{
    report_act[ag_rep] = 0;
    return;
}

void freeQueuePtr(void)
{
    report_act[q_rep] = 0;
    return;
}

void freeRepDataPtr(void** DataPtr)
{
    int i, k;
    for (i = 0; i < L_REP_MAX; i++)
    {
        if (report_act[i] == 1)
        {
            if (DataPtr != NULL && DataPtr != RepDataPtr[i]) continue;
            report_act[i] = 0;
            for (k = 0; k < AG_SIZE; k++) RepDataPtr[i][k] = NULL;
            break;
        }
    }
    return;
}

void free_service(struct agenda* A)
{
    int i, k, nr_ag;
    for (nr_ag = 1; nr_ag < AG_SIZE; nr_ag++) { if (SysA[nr_ag] == A) break; }
    if (nr_ag == AG_SIZE) return;
    if (report_act[ag_rep] == 1) return;
    for (i = 0; i < L_REP_MAX; i++)
    {
        if (report_act[i] == 1)
        {
            for (k = 0; k < AG_SIZE; k++)
            {
                if (RepDataPtr[i][k] == A) return;
                if (A->data != NULL && RepDataPtr[i][k] == A->data) return;
            }
        }
    }
    /*	if(A->data!=NULL) Farfree(A->data);  */
    free_agenda_mem(A);
    SysA[nr_ag] = NULL;
    if (AgendaMax == nr_ag) AgendaMax--;
    return;
}

struct agenda* rekord_zlecenia_agendy(int* nr_rekordu, int nr_w_agendzie, int kod_uslugi)
{
    struct agenda *A = NULL, *last = NULL, **SA;
    int na, lzl, rap_no;
    SA = getAgendaPtr(&lzl);
    freeAgendaPtr();
    for (na = 1, rap_no = -1; na < lzl; na++)
    {
        if (SA[na] == NULL) continue;
        if ((SA[na]->S)->kod_uslugi == kod_uslugi)
        {
            rap_no++;
            last = SA[na];
            if (nr_w_agendzie < 0)
            {
                if (rap_no == (*nr_rekordu))
                {
                    A = SA[na];
                    break;
                }
            }
            else
            {
                if (na == nr_w_agendzie)
                {
                    A = SA[na];
                    *nr_rekordu = rap_no;
                    break;
                }
            }
        }
    }
    if (rap_no < 0)
    {
        (*nr_rekordu) = 0;
        return NULL;
    }
    if (rap_no < (*nr_rekordu)) (*nr_rekordu) = rap_no;
    /* brak dokladnego dopasowania: ostatni rekord uslugi (zgodnie z
       przycieciem *nr_rekordu powyzej) */
    if (A == NULL) A = last;
    return A;
}
