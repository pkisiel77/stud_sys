#define _MAIN_MODUL_
#define _WEWY_TERM_
// #define GRAF
// #define _NCURSES_  // Defined in Makefile with -D_NCURSES_
// miejsce dla opcjesys.h
#include "blank/moje.h"
// koniec miejsca dla opcjesys.h
// #include "helper/helper.cpp"
struct agenda* Agenda = NULL;
int X_time = 30, X_tyt = 0, Liczba_opcji;
// kolory pocz�tkowe
unsigned int attr = TERM_WHITE_BG | TERM_BLACK | TERM_FLUSH;
unsigned int at_wpis = TERM_WHITE | MTERM_HILIGHT | TERM_BLUE_BG | TERM_FLUSH;
unsigned int attr_time = TERM_WHITE | MTERM_HILIGHT | TERM_BLUE_BG | TERM_FLUSH;
unsigned int attr_title = TERM_WHITE | MTERM_HILIGHT | TERM_BLUE_BG | TERM_FLUSH;
extern unsigned int cursor, nocursor;
int podsNameLen = 0;
#define L_REP_MAX 5
struct agenda* SysA[AG_SIZE];
struct agenda* SysQ[Q_SIZE];
void** RepDataPtr[L_REP_MAX];
int report_act[L_REP_MAX + 2];
int ag_rep, q_rep;
#include "konfig.h"
#include "dek_budz.h"   /* budz_blankiet, budz_wczytaj_z_pliku */
#include "mqtt_pub.h"   /* mqtt_auto_start */
#include "randf.c"
/* ----------------------------------------------------------- */
void logo(int yg, int xg, unsigned int attr_logo, char* imie, char* nazwisko, signed char mode);
void naglowek(unsigned int attr_logo);
void term_clear(int co);
int open_sys(void);
/* struct queue
 {struct agenda *A;
	time_t last_time;
	int cur_prior;
 }; */
char select_mode = MAX_PRIOR;
int AgendaMax = -1, NoServisMax = -1;
void check_services(void);
void chk_time(void);
struct agenda* check_queue(void);
void check_agenda(time_t dt);
/* ------------------------------------------------------------ */
struct Service* Service;
unsigned short int NODE_NO = 1;
signed char _TYP_KONSOLI_ = -1;
int lmall;
extern int lwmall, lwmallp;
char Dec = 'p', typ_raportu = ' ';
time_t sek_akt;
int if_graf = 0, YZ_max_text, Yz_max_graf, Y_MAX_graf = 34;
int y_shift = 0;
/*extern unsigned _stklen=50000U; */
/* ---------------------------------------------------------------- */
int raporty(signed char Kod_uslugi, int Nr_rekordu, int npzl,
            int kto, int r_pocz, int lrek, struct Service* S);
/* ---------------------------------------------------------------- */
// dodatkowe funkcje dla logowania plus sta�e symboliczne
#define DL_NAZWY_UZYT 12
#define DL_NAZWY_HASLA 12
// ustawienie scie�ek do katalog�w pacjenta
#define PATH_LOG   "/tmp/stud_sys.log"
#define PATH_HASLA "/tmp/stud_sys_hasla.dat"
//
int ZapiszDoLog(char* sciezka, char* tryb);
int ZapiszLog(char* uzytkownik, char* haslo);
int SprawdzLog(char* uzytkownik, char* haslo);
/* ---------------------------------------------------------------- */

/*******************************************************************/

int main(int argc, char* argv[])
{
    int ret, Ret, l_poz_menu, y_logo = 3;
    char* graf_text[2] = {"Zmiana trybu na GRAFICZNY", "Zmiana trybu na TEKSTOWY "};
    /* ---------------------------------------------------------------- */

    // printf("%d",add(1, 2));

    InitConsole();
    term_fill(TERM_WHITE | TERM_BLACK_BG);
    Cls();
    /*
    { char *text[]={" Praca magisterska",
    " System RT w srodowisku Windows NT",
    " Modul RT ",
    " Wersja 1.0.0",
    " Data 29.10.2001",
    " Logowanie do systemu pomiarowego"};
    int xg=0,yg=1,sprawdz=0,krok=0;
    FILE *fp;
    term_type(yg,xg,text[0],strlen(text[0]),TERM_WHITE);
    term_type(yg++,xg,text[1],strlen(text[1]),TERM_WHITE);
    term_type(yg++,xg,text[2],strlen(text[2]),TERM_WHITE);
    term_type(yg++,xg,text[3],strlen(text[3]),TERM_WHITE);
    term_type(yg++,xg,text[4],strlen(text[4]),TERM_WHITE);
    term_type(yg+=2,xg,text[5],strlen(text[5]),TERM_WHITE);
    // start wpis�w do systemu
    ret=ZapiszDoLog(PATH_LOG,"START");
    // funkcje interaktywne
    yg+=2;
    do{
    Uzytkownik[0]=EOS;
    wczytaj_string(yg, xg, "# Uzytkownik: ",Uzytkownik,DL_NAZWY_UZYT, TERM_WHITE, TERM_BLACK_BG);
    if(strlen(Uzytkownik)>0) uzytkownik=Uzytkownik;
    Haslo[0]=EOS;
    wczytaj_string(yg+1, xg, "# Numer: ",Haslo, DL_NAZWY_HASLA, TERM_WHITE, TERM_BLACK);
    if(strlen(Haslo)>0) haslo=Haslo;
    //zapisanie logowanie do pliku log.dat
    ret=ZapiszLog(uzytkownik,haslo);
    // sprawdzanie wprowadzonych danych
    ret=SprawdzLog(uzytkownik,haslo);
    if(ret==1) sprawdz=1;
    else {MessageBox(NULL," Uwaga!!! brak poprawnosi logowania "," Log ",MB_ICONERROR);sprawdz=0;krok++;}
    if(krok>3)
    {MessageBox(NULL," Nie misiu tak sie bawic nie bedziemy "," Znasz czy nie znasz siebie ",MB_OK);
    ret=ZapiszDoLog(PATH_LOG,"BRAK HASLA");
    return 0;}
    }while(sprawdz!=1);
    }
    //
    #ifdef _CVC_
    // sprawdzenie czy system zosta� zamkni�ty prawid�owo plik tmp.dat
    { FILE *fp,*nfp;
    fp=fopen("c:\\proba\\pacjent\\pacjent\\tmp\\tmp.dat","r");
    if(fp==NULL) {nfp=fopen("c:\\proba\\pacjent\\pacjent\\tmp\\tmp.dat","a");
    fprintf(nfp,"start systemu");
    fclose(nfp);
    }
    else
    { MessageBox(NULL," System zosta� �le zmkni�ty "," B��d zamykania systemu ",MB_OK);
    system(del *.*);
    fclose(fp);
    }
    }
    #endif
    */
    Liczba_opcji = open_sys();
    /* Ignore long-lived allocations created during system startup when
       reporting per-session memory growth in the report engine. */
    lwmallp = lwmall;
    Ret = Liczba_opcji - 1;
    /*
        for(nd=1;nd<argc;nd++)
        {if(argv[nd][0]!='-') continue;
        switch(argv[nd][1])
        {case 'n': nazwisko=&argv[nd][2];  break;
        case 'f': imie=&argv[nd][2];      break;
        case 'm': mode=atoi(&argv[nd][2]);break;
        case 'o': Ret=atoi(&argv[nd][2]); break;
        default:  break;
        }
        }
        */
    term_fill(TERM_WHITE | TERM_BLUE_BG);
    //	logo(y_logo,x_logo,attr_logo, imie , nazwisko, mode);
    /* ------------------------------------------------------------- */
    YZ_max_text = MY_MAX;
    Yz_max_graf = YZ_max_text;
    l_poz_menu = Liczba_opcji - 1;
    MYR_MAX = MY_MAX - 1;
    Menu[L_SYS + 1] = graf_text[0];
    l_poz_menu = Liczba_opcji;
    Yz_max_graf = Y_MAX_graf;
    if (if_graf != 0) if_graf = otworz_graf_blank(0, 0, -1, -1, Yz_max_graf, TERM_WHITE | TERM_BLACK_BG);
    do
    {
        term_fill(TERM_WHITE | TERM_BLUE_BG);
        setcursor(nocursor);
        // logo(y_logo,x_logo,attr_logo, imie , nazwisko, mode);
        setcursor(nocursor);
        {
            int y_menu;
            y_menu = MY_MAX - l_poz_menu - 10;
            if (y_menu - (y_logo + 10) > 1) y_menu = y_logo + 11;
            Ret = okno_menu(Menu, l_poz_menu, Ret, attr, at_wpis, y_menu, 25, -1, " MENU GLOWNE ", 1);
        }
        if (Ret < 0)
        {
            break;
        }

        if (Menu[L_SYS + 1] != NULL)
        {
            if (Ret == L_SYS + 1)
            {
                if (if_graf != 0)
                {
                    if_graf = Endgraph(); /* ------ przechodzimy na tekstowy ---- */
                    MY_MAX = YZ_max_text;
                    MYR_MAX = MY_MAX - 1;
                    y_shift = 0;
                }
                else /* ------ przechodzimy na graficzny --- */
                {
                    MY_MAX = Yz_max_graf;
                    MYR_MAX = MY_MAX - 1;
                    //y_shift=12;
                    // okno w trybie graficznym spada o 12 jednostek
                    setcursor(nocursor);
                    if_graf = otworz_graf_blank(0, 0, -1, -1, Yz_max_graf, TERM_WHITE | TERM_BLACK_BG);
                }
                Menu[L_SYS + 1] = graf_text[if_graf];
                continue;
            }
        }

        Service = Serv[Ret];
        Agenda = NULL;
        {
            int Nr_rekordu = 0, npzl = 0, kto = 0, r_pocz = 0;
            ret = raporty(Serv[Ret]->kod_uslugi, Nr_rekordu, npzl, kto, r_pocz, Serv[Ret]->l_rek_max, Serv[Ret]);
        }
    }
    while (Ret != -1);
    zamknij_raporty();
    term_fill(TERM_WHITE | TERM_BLACK_BG);
    Endgraph();
    ZapiszDoLog(PATH_LOG, "KONIEC PRACY");
    (void)ret;
    /*	if(mystderr!=NULL) fclose(mystderr); */
    return 0;
}

/* -------------------------------------------------------------------- */
int open_sys(void)
{
    int liczba_opcji, i, k;
    struct agenda* A;
    liczba_opcji = set_services();
    otworz_raporty(0);
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
    /* ------------------- Ustawiamy zlecenie sprawdzania ---------- */
    {
        int rozmiar, zl = 0;
        AgendaMax = 0;
        rozmiar = Serv[zl]->str_size;
        A = (struct agenda*)Malloc(rozmiar);
        A->S = Serv[zl];
        A->data = (A + 1);
        A->mode = 'p';
        A->number_of_calls = -1;
        A->Interval = 1;
        A->prior = 200;
        A->prior_plus = 0;
        { const char* sn = (A->S)->name; int snlen = sn ? (int)strlen(sn)+1 : 1;
          A->name = (char*)Malloc(snlen); if(A->name) memcpy(A->name, sn ? sn : "", snlen); }
        A->state = 0;
        A->delay = 0;
        A->last_time = sek_akt;
        SysA[0] = A;
    }
    /* ------------------ Uruchamiamy dla testu 10 zlecen -------------
        {int rozmiar, zl, zlb=-1, zlo=-1, nr;
         for(i=1;i<L_SYS; i++)
            {if(Serv[i]->def_blankiet==budz_blankiet) zlb=i;
             //if(Serv[i]->def_blankiet==obl_blankiet) zlo=i;
            }
         for(i=1;i<10; i++)
         {if((i%2)==0) zl=zlb; //else {//zl=zlo;

             A=service_default(Serv[zl]->decyzje);
             if(A==NULL) continue;
         A->Interval=1000; A->delay=2000; A->number_of_calls=1000;
             if(zl!=zlb) {A->mode='p';}
             else
                {struct budzik *Bu;
                 int godz, min, sek;
                 A->mode='s';
                 Bu=(struct budzik *)(A->data);
                 snprintf(Bu->nazwa, sizeof(Bu->nazwa), " Budzik %d", i); if(i==1) Bu->nazwa[0]=0;
                 czas_zegarowy(&godz, &min, &sek);
                 Bu->min=min; Bu->godz=godz+i;
                 if(Bu->godz>=24) Bu->godz-=24;
                 Bu->co_ile=2; Bu->wyprzedz=15+i;
           ustaw_czas_budzika(A, Bu);
                }
             A->prior=100-i;
             nr=insert_to_agenda(A);
             Free(A); lwmall--;
    //		 SysA[i]=A; //(struct agenda *)Malloc(Serv[L_SYS]->str_size);
    //								 A=SysA[0]; A->S=Serv[L_SYS];
            }
         free_service(SysA[4]);
        }
        */
    wewy_abort_off();
    wpis_danych_bez_potwierdz();
def_haslo("st", "Student");
    haslo_nieaktywne();
    for (i = 0; i < liczba_opcji; i++)
    {
        int kod_uslugi, rek0, kolor = 0, ramka = 1, max_l_data_blank = 100;
        int rap_idx;
        kod_uslugi = Serv[i]->kod_uslugi;
        rek0 = 0;
        rap_idx = def_Report(kod_uslugi, -1, rek0, rek0 + Serv[i]->l_rek_max - 1, Serv[i]->str_size,
                             Serv[i]->name, kolor, ramka, Serv[i]->D, max_l_data_blank, Serv[i]->typ_bazy,
                             Serv[i]->def_blankiet, Serv[i]->dane_rap_bl, Serv[i]->wpis_rap_bl);
        if (rap_idx < 0)
        {
            komunikat(MY_MAX, X_L0, ATTR_A, " Blad def_Report dla uslugi %d (%s). <Ent> ",
                      kod_uslugi, Serv[i]->name);
            GET_char();
        }
    }
    budz_wczytaj_z_pliku();
    mqtt_auto_start();
    term_clear(TERM_CLS_SCR);
    sek_akt = time(NULL);
    return liczba_opcji;
}

/* -------------------------------------------------------------- */
void naglowek(unsigned int attr_logo)
{
    int i, len;
    term_printf(Y_G0, X_L0, attr_logo, " PODSYSTEM");
    X_tyt = m_wherex();
    for (i = 0, podsNameLen = 24; i < Liczba_opcji; i++)
    {
        len = strlen(Menu[i]);
        if (len > podsNameLen) podsNameLen = len;
    }
    if (podsNameLen > 24)
    {
        i = podsNameLen - 24;
        X_tyt -= i;
    }
    len = X_tyt + podsNameLen;
    for (i = X_tyt; i < len; i++) term_type(Y_G0, i, " ", 1, attr_logo);
    X_tyt++;
    X_time = m_wherex();
    chk_time();
}

void logo(int yg, int xg, unsigned int attr_logo, char* imie,
          char* nazwisko, signed char mode)
{
    char* text = "                                              ";
    int xtxt;
    naglowek(attr_logo);
    chk_time();
    if (yg <= Y_G0) { yg = Y_G0 + 1; }
    if (xg <= X_L0) { xg = X_L0 + 1; }
    Okno(yg, xg, yg + 9, xg + 3 + strlen(text) + 3, attr_logo);
    clr_Okno();
    xtxt = xg + 5;
    term_printf(yg + 2, xtxt, attr_logo, text);
    term_printf(yg + 8, xtxt, attr_logo, "      POMIAR SYGNALU EKG wersja 1.0");
    if (nazwisko != NULL || imie != NULL)
    {
        term_printf(yg + 6, xtxt, attr_logo, "      Uzytkownik: ");
        if (imie != NULL) term_printf(yg + 6, m_wherex(), attr_logo, "%s", imie);
        if (nazwisko != NULL) term_printf(yg + 6, m_wherex(), attr_logo, " %s", nazwisko);
        term_printf(yg + 6, m_wherex(), attr_logo, " (Mode=****)");
    }
}

/* ==================================================================== */
/*	--------------- struktura tm w time.h ---------------------------
struct	tm{int tm_sec;  int	tm_min;	 int tm_hour;	int	tm_mday;	int	tm_mon;
		 int tm_year; int	tm_wday; int tm_yday;	int tm_isdst;};
 ------------------------------------------------------------------- */
int czas_zegarowy(int* godz, int* min, int* sek)
{
    time_t sek1970;
    struct tm* Time;
    sek1970 = time(NULL);
    Time = localtime(&sek1970);
    *godz = Time->tm_hour;
    *sek = Time->tm_sec;
    *min = Time->tm_min;
    return (Time->tm_hour * 60l) + (*min);
}

// Kisiel
int data_systemowa(int* dzien, int* miesiac, int* rok)
{
    time_t sek1970;
    struct tm* Time;
    sek1970 = time(NULL);
    Time = localtime(&sek1970);
    *dzien = Time->tm_mday;
    *miesiac = Time->tm_mon;
    *rok = Time->tm_year + 1900;
    return 0;
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
            term_printf(MY_MAX, X_L0,ATTR_A, " Brak miejsca w AGENDZIE na %s <ENT> ",
                        ((struct Service*)(A->S))->name);
            term_cur(y, x);
            setcursor(cursor);
            GET_char();
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
            term_printf(MY_MAX, X_L0,ATTR_A, " Brak pamieci dla zlecenia %s <Ent> ", (A->S)->name);
            term_cur(y, x);
            setcursor(cursor);
            GET_char();
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
        term_printf(MY_MAX, X_L0,ATTR_A, " Brak pamieci dla zlecenia %s <Ent> ", Serv[service_no]->name);
        term_cur(y, x);
        setcursor(cursor);
        GET_char();
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

void mod_prior(struct agenda* Q)
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
        if (select_mode == NO_PRIOR) return A;
        if (A->prior_plus > 0) mod_prior(A);
        if (select_mode == MAX_PRIOR && A->cur_prior > max_prior)
        {
            sel = Nrq;
            max_prior = A->cur_prior;
        }
        else /* to jest gdy RAND_PRIOR */
        {
            nq++;
            lim += A->cur_prior;
            prior_lim[nq] = lim;
            serv_ready[nq] = Nrq;
        }
        break;
    }
    if (Nrq > NoServisMax) return NULL;
    if (sel < 0) /* to jest gdy RAND_PRIOR */
    {
        lim = lim * RND01();
        for (Nrq = 0; Nrq <= nq; Nrq++)
        {
            if (lim < prior_lim[Nrq])
            {
                sel = Nrq;
                break;
            }
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
                term_printf(MY_MAX, X_L0,ATTR_A, " Brak miejsca w KOLEJCE na %s <ENT> ",
                            ((struct Service*)(A->S))->name);
                GET_char();
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
    struct agenda *A, **SA;
    int na, lzl, rap_no;
    SA = getAgendaPtr(&lzl);
    freeAgendaPtr();
    for (na = 1, rap_no = -1; na < lzl; na++)
    {
        if (SA[na] == NULL) continue;
        if ((SA[na]->S)->kod_uslugi == kod_uslugi)
        {
            rap_no++;
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
    return A;
}

/* ---------- Funkcje uslugowe dla wszystkich zlecen ----------------- */

int pobierz_rekord_uslugi(int* nr_rekordu, int kod_uslugi, int ob_konc, struct agenda** As, struct agenda** Anew,
                          struct agenda*** SA, char* adres_rek0_uslugi, char* nazwa)
{
    struct agenda *A = NULL, *A0 = NULL, **Ab = NULL;
    int nr, i, nrR, lzl, nr_rek, ret = 0;
    A0 = (struct agenda*)adres_rek0_uslugi;
    nr_rek = *nr_rekordu;
    *SA = getAgendaPtr(&lzl);
    freeAgendaPtr();
    *As = NULL;
    if (A0 != *SA[0])
    {
        *nr_rekordu = -1;
        term_printf(MY_MAX, X_L0,ATTR_A, " Niespojnosc adsresow %s ", nazwa);
        GET_char();
        return 0;
    }
    Ab = (struct agenda**)getRepDataPtr(&nrR);
    for (i = 0, nr = 0; i < ob_konc; i++)
    {
        Ab[i] = NULL;
        A = (struct agenda*)ustal_adres_rek(kod_uslugi, i);
        if (A == NULL) continue;
        if ((A->S)->kod_uslugi == kod_uslugi)
        {
            Ab[nr] = A;
            nr++;
        }
    }
    if (nr == 0 && *Anew == NULL)
    {
        char *menu_wybor[2] = {"Tak - uruchom nowy", "Nie"};
        int wybor;
        dana_koment(-1, -1, "+ ");
        dana_koment(-1, -1, " Brak %s w agendzie ", nazwa);
        wybor = okno_menu(menu_wybor, 2, 0, attr, at_wpis, Y_G0+3, X_L0+1, -1, " Uruchomic nowe? ", 1);
        if (wybor != 0)
        {
            *nr_rekordu = -1;
            return 0;
        }
        Service->decyzje('t', DEC_NEW, 0, kod_uslugi, 0, nr_rekordu);
        if (*Anew == NULL)
        {
            *nr_rekordu = -1;
            return 0;
        }
    }
    if (*Anew != NULL) /* zadeklarow. w sys_dekl.h */
    {
        nr++;
        nr_rek = nr;
        Ab[nr_rek] = *Anew;
    }
    else { nr_rek = ((*nr_rekordu) % nr); }
    Ab[0] = Ab[nr_rek];
    for (i = 1; i < nrR; i++) Ab[i] = NULL;
    A = Ab[0]; /* tylko jeden !!!! jest tu raportowany wiec tylko Ab[0]!=NULL */
    *As = A; /* Aserv zadeklarow. w sys_dekl.h */
    *nr_rekordu = nr_rek;
    return ret;
}

int dane_agendy(struct agenda* A, struct agenda* Anew, int cykl_max)
{
    int ret, ochr, size, raport, l_opcji;
    char typ_form[32];
    static char* opcje = NULL;

    strcpy(typ_form, "* Typ uslugi <%s> ");
    if (Anew != NULL) /* zadeklarow. w sys_dekl.h */
    {
        l_opcji = 5;
        opcje = "d/n/l/m/u";

        if (Anew->mode != 'n')
        {
            l_opcji = 6;
            opcje = "d/n/l/m/u/w";
            typ_form[0] = '*';
        }
        else
        {
            typ_form[0] = '#';
        }

        ochr = 5;
    }
    else
    {
        l_opcji = 5;
        opcje = "d/n/l/m/u";
        ochr = -1;
    }
    {
        static char* status[] = {
            " nie wchodzi gdy poprzednie jest jeszcze w kolejce",
            " wchodzi zawsze do kolejki (wykonuje sie stare i nowe)",
            " wchodzi do kolejki i usuwa poprzednio wstawione "
        };
        static int nmin = 0, nmax = 2;
        ret = dana_int_menu(-1, -1, "+ Status zlecenia: ", &nmin, &nmax,
                            &(A->status), size = 1, ochr, raport = -1, 3, status);
    }
    {
        static char* typ_usl[] = {"p permanentna (stala)", "s seryjna", "t dorazna", "?  WPISZ TYP !!!!! "};
        ret = dana_decyzyjna(-1, -1, typ_form, "p/s/t", typ_usl, 4,
                             &(A->mode), ochr = 5, DEC_TYP_US);
    }
    {
        static int nmin = 1, nmax = 3000, nmin0 = 0;
        ret = dana_int(-1, -1, "+ Priorytet (%d-%d)  ?? ", &nmin, &nmax,
                       &(A->prior), size = 4, ochr = 5, raport = -1);
        ret = dana_int(-1, -1, "+ Przyrost prioryt.po 1sek. oczekiwania (%d-%d)  ?? ", &nmin0, &nmax,
                       &(A->prior_plus), size = 4, ochr = 5, raport = -1);
    }
    {
        static float dmin = 0, dmax = 1.e10;
        int prec, kod_an;
        if (A->mode != 't' && cykl_max != 0)
        {
            static int nmin = 1, nmax;
            nmax = cykl_max;
            ret = dana_int(-1, -1, "+ Okres cyku wywolan [sek] (%d-%d)  ?? ", &nmin, &nmax,
                           &(A->Interval), size = 4, ochr = 5, raport = -1);
        }
        ret = dana_float_dec(-1, -1, " Liczba sek.do najblizszego wywolania  ??",
                             &dmin, &dmax, &A->delay, size = 6, prec = 0,
                             ochr = 5, kod_an = 2, DEC_DELAY);
        if (A->number_of_calls < 0) ret = dana_koment(-1, -1, " Usluga stala ");
        else
        {
            static int min = 0, max = 32000;
            if (A->number_of_calls == 0)
            {
                ret = dana_koment(-1, -1, " USLUGA ANULOWANA !!!!! ");
            }
            else
            {
                if (Anew != NULL)
                {
                    ret = dana_int(-1, -1, " Liczba wywolan  ??", &min, &max, &A->number_of_calls,
                                   size = 5, ochr = 4, raport = -1);
                }
                else
                    ret = dana_int(-1, -1, " Pozostalo wywolan  ", &min, &max, &A->number_of_calls,
                                   size = 5, ochr = -1, raport = -1);
            }
        }
    }
    ret = dana_koment(-1, -1, "+ ");
    {
        static char *uruch[] = {
                        "d dane", "n nastepny", "l lista", "m menu glowne", "u USUNAC", "w WPIS do agendy"
                    }, dec = 'd';
        dec = 'd';
        ret = dana_decyzyjna(-1, -1, " Co dalej <%s>  ?? ", opcje, uruch, l_opcji,
                             &dec, ochr = 0, DEC_RUN);
    }
    if (A->mode != 'n' && Anew == NULL)
    {
        static char *uruch[2] = {"t tak", "n nie"}, dec = 'n';
        dec = 'n';
        wpis_danych_bez_potwierdz();
        ret = dana_decyzyjna(-1, -1, " Uruchomic nowe  <%s> ?? ", "t/n", uruch, 2,
                             &dec, ochr = 1, DEC_NEW);
        wpis_danych_z_potwierdz();
    }
    return ret;
}


int pokaz_listy_zlecen(struct agenda* Anew, int kod_uslugi, char* tytul)
{
    int ret, i, nr, yp, xp, ramka, lbm;
    struct agenda **Ab, *A;
    ret = 0;
    if (Anew != NULL)
    {
        Free(Anew);
        lwmall--;
        Anew = NULL;
    }
    /* ------- zebranie adresow uslug budzika w agendzie do pokazania listy -- */
    Ab = (struct agenda**)getRepDataPtr(&lbm); /* zalozenie semafora */
    for (i = 0, nr = 0; i < lbm; i++)
    {
        A = SysA[i];
        if (A == NULL) continue;
        /* term_printf(MY_MAX, X_L0+1,ATTR_A," S->kod=%d/%d ",(A->S)->kod_uslugi,kod_uslugi); GET_char(); */
        if ((A->S)->kod_uslugi == kod_uslugi)
        {
            Ab[nr] = A;
            nr++;
        }
    }
    if (nr == 0) term_printf(m_wherey(), X_L0 + 3, attr, " Brak budzikow w agendzie ");
    else
    {
        ret = okno_menu_rekordow((char**)Ab, nr - 1, ret, attr, at_wpis,
                                 yp = 4, xp = X_L0 + 9, -1, tytul, ramka = 1,
                                 " %19S typ=%c prio=%d krotn=%d razy",
                                 &Ab[0]->name, &(Ab[0]->mode),
                                 &(Ab[0]->prior), &(Ab[0]->number_of_calls));
    }
    freeRepDataPtr((void**)Ab); /* --- zwolnienie sefamora ---- */
    return ret;
}

void ustaw_typ_uslugi(struct agenda* A, int decyzja)
{
    switch (decyzja)
    {
    case 's':
        A->mode = 's';
        if (A->number_of_calls < 2) A->number_of_calls = 2;
        break;
    case 'p':
        A->mode = 'p';
        A->number_of_calls = -1;
        break;
    case 't':
        A->mode = 't';
        A->number_of_calls = 1;
        break;
    default: break;
    }
}

int decyzje_run(char decyzja, struct agenda** Aserv, struct agenda** Anew, int* nr_rekordu, int kod_uslugi, char* tytul)
{
    int ret = 0, i;
    switch (decyzja)
    {
    case 'n': (*nr_rekordu)++;
        break;
    case 'u': if ((*Anew) != NULL)
        {
            free_agenda_mem(*Anew);
            *Anew = NULL;
        }
        else (*Aserv)->number_of_calls = 0;
        /*
                                         { FILE *fp;
                                         fp=fopen("c:\\proba\\pacjent\\pacjent\\tmp\\info.dat","w");
                                         if(fp==NULL) {;}
                                         else
                                         { fprintf(fp,"0");
                                         fclose(fp);
                                         }
                                         }
                                         */
        break;
    case 'w': if (*Anew != NULL)
        {
            i = insert_to_agenda(*Anew);
            *Aserv = rekord_zlecenia_agendy(nr_rekordu, i, kod_uslugi); /*  Szukamy rekordu nowego zlecenia */
            free_agenda_mem(*Anew);
            *Anew = NULL;
            /*
                                                 { FILE *fp;
                                                 fp=fopen("c:\\proba\\pacjent\\pacjent\\tmp\\info.dat","w");
                                                 if(fp==NULL) {;}
                                                 else
                                                 { fprintf(fp,"1");
                                                 fclose(fp);
                                                 }
                                                 }
                                                 */
            break;
        }
    case 'm': return -1;
    case 'l': ret = pokaz_listy_zlecen(*Anew, kod_uslugi, tytul);
        if (ret >= 0) *nr_rekordu = ret;
        ret = 0;
    default: break;
    }
    return ret;
}

/*-------------------------------------------------------------*/
int ZapiszDoLog(char* sciezka, char* tryb)
{
    FILE* fp;
    fp = fopen(sciezka, "a");
    if (fp == NULL) { fprintf(stderr, "Blad otwarcia LOG: %s\n", sciezka); return 0; }
    else
    {
        // zapis data i time otwarcia systemu
        int godz, min, sek, dzien, miesiac, rok;
        czas_zegarowy(&godz, &min, &sek);
        data_systemowa(&dzien, &miesiac, &rok);
        fprintf(fp, "\n#LOG:[%.2d.%.2d.%.2d](czas) [%.2d.%.2d.%.4d](data)#%s", godz, min, sek, dzien, miesiac, rok,
                tryb);
        fclose(fp);
    }
    return 0;
}

int ZapiszLog(char* uzytkownik, char* haslo)
{
    FILE* fp;
    fp = fopen(PATH_LOG, "a");
    if (fp == NULL) { fprintf(stderr, "Blad otwarcia LOG: %s\n", PATH_LOG); return 0; }
    else
    {
        fprintf(fp, "\n%s %s", uzytkownik, haslo);
        fclose(fp);
    }
    return 0;
}

int SprawdzLog(char* uzytkownik, char* haslo)
{
    char Uzytkownik[DL_NAZWY_UZYT], Haslo[DL_NAZWY_HASLA];
    int ret;
    FILE* fp;
    fp = fopen(PATH_HASLA, "r");
    if (fp == NULL)
    {
        char* menu_blad[] = {"Blad otwarcia pliku"};
        okno_menu(menu_blad, 1, 0, attr, at_wpis, 10, 25, -1, " BLAD", 1);
        return -1;
    }
    else
    {
        do
        {
            ret = fscanf(fp, "%11s %11s", Uzytkownik, Haslo);
            if (ret == 2 && strcmp(uzytkownik, Uzytkownik) == 0 && strcmp(haslo, Haslo) == 0)
            {
                fclose(fp);
                return 1;
            }
        }
        while (ret != EOF);

        fclose(fp);
    }
    return 0;
}

/*-------------------------------------------------------------*/

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-but-set-variable"
#pragma clang diagnostic ignored "-Wself-assign"
#endif
#include "rap_bl.c"
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
