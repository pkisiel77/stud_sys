/* ================== Plik konfigurujacy sys_dekl.h ==================== * /
struct Service          / * deklaracja w sys_dekl.h * /
 {char *name;           / * nazwa uslugi * /
	int l_rek_max;      / * max.liczba danych w blankiecie * /
	int kod_uslugi;     / * unikalny kod uslugi i raportu  * /
	int str_size;       / * rozmiar struktury danych wlasnych * /
	int status;         / * 0 nie wchodzi gdy stare w kol; 1 wchodzi zawsze; 2 usuwa stare * /
	char typ_bazy;        / * BAZA_SPOJNA lub BAZA_WEKTOR lub BAZA_LISTA - patrz wewybl.h * /
	void *D;              / * adres bazy danych (NULL - bedzie alkokowana) * /
	int (*main_modul)(void *A); / * nazwa modulu glownego (NULL - nie jest zleceniem RT) * /
	int (*def_blankiet)(int nr_rekordu, int ob_pocz,
												int ob_konc, int x_lewy_gorny,
												int y_lewy_gorny, int kod_uslugi,
												char *adr_rek0);
	char *(*dane_rap_bl)(int ob_pocz, int ob_konc, int *rozmiar_ob);
	void (*wpis_rap_bl)(int ob_pocz, int ob_konc,
											char *D, int rozmiar_ob, char zapis[], char *Kod_op);
	int (*decyzje)(int decyzja, int kod_decyzji, int nr_dec,
											int kod_uslugi, int np, int *nr_rekordu);
 };
 -------------------------------------------------------- */
#include "konfig.h"
#include "loc.h"
#include "sprawdz.h"   /* plik zrodlowy sprawdz.c */
#include "dek_budz.h"  /* plik zrodlowy budz.c */
#include "sys_rep.h"   /* plik zrodlowy sys_rap.c */
#include "admin.h"     /* plik zrodlowy admin.c */
#include "mqtt_pub.h"   /* plik zrodlowy mqtt_pub.c */ // MQTT publisher
#include "sensor_sim.h" /* plik zrodlowy sensor_sim.c */ // symulowany czujnik RT
#include <string.h>

/* --------------------------------------------------------------- */
char* Menu[L_SYS + 3];
int (*Fun_sys[L_SYS + 1])();
int (*dialog[L_SYS + 1])();
struct Service* Serv[L_SYS + 1];
/* -------------------------------------------------------------- */
int set_services(void)
{
    struct Service* S;
    int Nr_sys = 0;
    /* --------------- Podstawiamy dane uslugi z pliku sprawdz.c --------- */
    Menu[Nr_sys] = L_KONFIG_SRV_SPRAWDZ;
    S = (struct Service*)Malloc(sizeof(struct Service));
    if (S == NULL) goto ERR_MEM;
    memset(S, 0, sizeof(struct Service));
    Serv[Nr_sys] = S;
    S->l_rek_max = 1; /* tylko jeden rekord danych */
    S->kod_uslugi = Nr_sys + 1;
    S->D = SysA;
    S->typ_bazy = BAZA_WEKTOR;
    S->str_size = sizeof(struct agenda) + sizeof(struct sprawdz);
    S->name = Menu[Nr_sys];
    S->main_modul = sprawdz; /* Jest zleceniem RT */
    S->def_blankiet = sprawdz_blankiet; /* fkcja definiujaca blankiet */
    S->dane_rap_bl = dane_sprawdz; /* fkcja wczytujaca baze (gdy S->D=NULL) */
    S->wpis_rap_bl = wpis_sprawdz; /* fkcja zapisujaca baze na dysk */
    S->decyzje = dec_sprawdz; /* fkcja obslugujaca decyzje z blankietu */
    /* --------------- Podstawiamy dane uslugi z pliku budz.c --------- */
    Nr_sys++;
    if (Nr_sys >= L_SYS) goto ERR_SERV;
    Menu[Nr_sys] = L_KONFIG_SRV_BUDZIK;
    S = (struct Service*)Malloc(sizeof(struct Service));
    if (S == NULL) goto ERR_MEM;
    memset(S, 0, sizeof(struct Service));
    Serv[Nr_sys] = S;
    S->kod_uslugi = Nr_sys + 1;
    S->l_rek_max = AG_SIZE;
    S->str_size = sizeof(struct budzik);
    S->typ_bazy = BAZA_WEKTOR;
    S->D = SysA;
    S->name = Menu[Nr_sys];
    S->main_modul = budz_main; /* Jest zleceniem RT */
    S->def_blankiet = budz_blankiet;
    S->dane_rap_bl = dane_budz;
    S->wpis_rap_bl = wpis_budz;
    S->decyzje = dec_budz;
    /* --------------- Podstawiamy dane dalszych uslug z pliku admin.c ---- */
    Nr_sys++;
    if (Nr_sys >= L_SYS) goto ERR_SERV;
    Menu[Nr_sys] = L_KONFIG_SRV_ADMIN;
    S = (struct Service*)Malloc(sizeof(struct Service));
    if (S == NULL) goto ERR_MEM;
    memset(S, 0, sizeof(struct Service));
    Serv[Nr_sys] = S;
    S->kod_uslugi = Nr_sys + 1;
    S->l_rek_max = 1;
    S->D = NULL;
    S->typ_bazy = BAZA_SPOJNA;
    S->str_size = sizeof(struct admin);
    S->name = Menu[Nr_sys];
    S->main_modul = NULL;
    S->def_blankiet = admin_blankiet;
    S->dane_rap_bl = dane_admin;
    S->wpis_rap_bl = wpis_admin;
    S->decyzje = dec_admin;
    /* --------------- Podstawiamy dane uslugi MQTT publisher ------------- */
    Nr_sys++;
    if (Nr_sys >= L_SYS) goto ERR_SERV;
    Menu[Nr_sys] = L_KONFIG_SRV_MQTT;
    S = (struct Service*)Malloc(sizeof(struct Service));
    if (S == NULL) goto ERR_MEM;
    memset(S, 0, sizeof(struct Service));
    Serv[Nr_sys] = S;
    S->kod_uslugi  = Nr_sys + 1;
    S->l_rek_max   = AG_SIZE;
    S->str_size    = sizeof(struct mqtt_cfg);
    S->typ_bazy    = BAZA_WEKTOR;
    S->D           = SysA;
    S->name        = Menu[Nr_sys];
    S->main_modul  = mqtt_main;
    S->def_blankiet= mqtt_blankiet;
    S->dane_rap_bl = dane_mqtt;
    S->wpis_rap_bl = wpis_mqtt;
    S->decyzje     = dec_mqtt;
    /* --------------- Symulowany czujnik RT (sensor_sim.c) --------------- */
    Nr_sys++;
    if (Nr_sys >= L_SYS) goto ERR_SERV;
    Menu[Nr_sys] = L_KONFIG_SRV_SENSOR;
    S = (struct Service*)Malloc(sizeof(struct Service));
    if (S == NULL) goto ERR_MEM;
    memset(S, 0, sizeof(struct Service));
    Serv[Nr_sys] = S;
    S->kod_uslugi  = Nr_sys + 1;
    S->l_rek_max   = AG_SIZE;
    S->str_size    = sizeof(struct sensor_sim);
    S->typ_bazy    = BAZA_WEKTOR;
    S->D           = SysA;
    S->name        = Menu[Nr_sys];
    S->main_modul  = sensor_sim_main;
    S->def_blankiet= sensor_sim_blankiet;
    S->dane_rap_bl = dane_sensor_sim;
    S->wpis_rap_bl = wpis_sensor_sim;
    S->decyzje     = dec_sensor_sim;
    /* --------------------------------------------------------------- */
    if (Nr_sys < L_SYS - 1)
    {
        fprintf(stderr, "\nZa malo zlecen wg L_SYS=%d Nr_sys=%d !!!. Popraw L_SYS w konfig.c ",
                L_SYS, Nr_sys);
        getch();
        exit(1);
    }
    /* --------------------------------------------------------------- */
    Menu[L_SYS] = L_KONFIG_SRV_STAN;
    S = (struct Service*)Malloc(sizeof(struct Service));
    if (S == NULL) goto ERR_MEM;
    memset(S, 0, sizeof(struct Service));
    Serv[L_SYS] = S;
    S->kod_uslugi = L_SYS + 1;
    S->D = SysA;
    S->typ_bazy = BAZA_WEKTOR;
    S->l_rek_max = AG_SIZE;
    S->str_size = sizeof(struct agenda);
    S->name = Menu[L_SYS];
    S->main_modul = NULL; /* sys_main; */
    S->def_blankiet = sys_blankiet;
    S->dane_rap_bl = dane_sys;
    S->wpis_rap_bl = wpis_sys;
    S->decyzje = dec_sys;
    /* ============================================================== */
    for (int i = 1; i < L_SYS; i++)
    {
        S = Serv[i];
        if (S->main_modul != NULL)
        {
            S->D = SysA;
            S->typ_bazy = BAZA_WEKTOR;
            S->str_size += sizeof(struct agenda);
            S->l_rek_max = AG_SIZE;
        }
    }
    Menu[L_SYS + 1] = NULL;
    Menu[L_SYS + 2] = NULL;
    return L_SYS + 1;
ERR_MEM:
    {
        fprintf(stderr, "\nBrak pamieci dla servisu %d", Nr_sys);
        getch();
        exit(1);
    }
ERR_SERV:
    {
        fprintf(stderr, "\nBrak miejsca na podsystem: L_SYS=%d Nr_sys=%d !!!. Popraw L_SYS w konfig.c ",
                L_SYS, Nr_sys);
        getch();
        exit(1);
    }
    return 0;
}
