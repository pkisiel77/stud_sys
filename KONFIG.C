/* ================== Plik konfigurujacy sys_dekl.h ==================== * /
struct Service          / * deklaracja w sys_dekl.h * /
 {char *name;           / * nazwa uslugi * /
	shint l_rek_max;      / * max.liczba danych w blankiecie * /
	shint kod_uslugi;     / * unikalny kod uslugi i raportu  * /
	shint str_size;       / * rozmiar struktury danych wlasnych * /
	shint status;         / * 0 nie wchodzi gdy stare w kol; 1 wchodzi zawsze; 2 usuwa stare * /
	char typ_bazy;        / * BAZA_SPOJNA lub BAZA_WEKTOR lub BAZA_LISTA - patrz wewybl.h * /
	void *D;              / * adres bazy danych (NULL - bedzie alkokowana) * /
	shint (*main_modul)(void *A); / * nazwa modulu glownego (NULL - nie jest zleceniem RT) * /
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
 -------------------------------------------------------- */
/* #include "...."
	 ........ itd headery kolejnych podsystemow */
#include "sprawdz.h"   /* plik zrodlowy sprawdz.c */
//#include "baza_stu.h"  /* plik zrodlowy stud_baz.c */
//#include "dekl_obl.h"  /* plik zrodlowy oblicz.c */
#include "dek_budz.h"  /* plik zrodlowy budz.c */
#include "sys_rep.h"   /* plik zrodlowy sys_rap.c */
#include "admin.h"     /* plik zrodlowy admin.c */
#include "pomiar.h"    /* plik zrodlowy pomiar.c */ // pomiar ekg
#include "uruchom.h"
// #include "pacjent.h"
// #include "baza.h"     /* plik zrodlowy baza.c */   // dodaj,usun,przegladaj pacjentów
// #include "ustaw.h"    /* plik zrodlowy ustaw.c */  // ustawienia pomiaru

#define L_SYS 5
/* --------------------------------------------------------------- */
#ifdef _DEF_SYS_
shint set_services(void);
char *Menu[L_SYS+3];
shint (*Fun_sys[L_SYS+1])();
shint (*dialog[L_SYS+1])();
struct Service *Serv[L_SYS+1];
/* -------------------------------------------------------------- */
shint set_services(void)
 {shint Nr_sys, i;
	struct Service *S;
	Nr_sys=0;
/* --------------- Podstawiamy dane uslugi z pliku sprawdz.c --------- */
	Menu[Nr_sys]=" Sprawdz.otoczenia";
	S=(struct Service *)Malloc(sizeof(struct Service));
	if(S==NULL) goto ERR_MEM;
	Serv[Nr_sys]=S;
	S->l_rek_max=1;          /* tylko jeden rekord danych */
	S->kod_uslugi=Nr_sys+1;
	S->D=SysA; S->typ_bazy=BAZA_WEKTOR;
	S->str_size=sizeof(struct agenda)+sizeof(struct sprawdz);
	S->name=Menu[Nr_sys];
	S->main_modul=sprawdz;     /* Jest zleceniem RT */
	S->def_blankiet=sprawdz_blankiet;/* fkcja definiujaca blankiet */
	S->dane_rap_bl=dane_sprawdz;     /* fkcja wczytujaca baze (gdy S->D=NULL) */
	S->wpis_rap_bl=wpis_sprawdz;   	 /* fkcja zapisujaca baze na dysk */
	S->decyzje=dec_sprawdz;        	 /* fkcja obslugujaca decyzje z blankietu */
/* --------------- Podstawiamy dane uslugi z stud_baz.c --------- 
	Nr_sys++; if(Nr_sys>=L_SYS) goto ERR_SERV;
	Menu[Nr_sys]=" Pacjent";
	S=(struct Service *)Malloc(sizeof(struct Service));
	if(S==NULL) goto ERR_MEM;
	Serv[Nr_sys]=S;
	S->l_rek_max=20; S->kod_uslugi=Nr_sys+1;
	S->D=NULL;
	S->str_size=sizeof(struct pacjent);
	S->name=Menu[Nr_sys];
	S->main_modul=NULL;     
	S->typ_bazy=BAZA_SPOJNA;
	S->def_blankiet=pacjent_blankiet;
	S->dane_rap_bl=dane_pacjent;
	S->wpis_rap_bl=wpis_pacjent;
	S->decyzje=dec_pacjent;
/* --------------- Podstawiamy dane uslugi z stud_baz.c --------- */
	Nr_sys++; if(Nr_sys>=L_SYS) goto ERR_SERV;
	Menu[Nr_sys]=" Uruchom";
	S=(struct Service *)Malloc(sizeof(struct Service));
	if(S==NULL) goto ERR_MEM;
	Serv[Nr_sys]=S;
	S->l_rek_max=20; S->kod_uslugi=Nr_sys+1;
	S->D=NULL;
	S->str_size=sizeof(struct uruchom);
	S->name=Menu[Nr_sys];
	S->main_modul=NULL;     
	S->typ_bazy=BAZA_SPOJNA;
	S->def_blankiet=uruchom_blankiet;
	S->dane_rap_bl=dane_uruchom;
	S->wpis_rap_bl=wpis_uruchom;
	S->decyzje=dec_uruchom;
/* --------------- Podstawiamy dane uslugi z pliku oblicz.c --------- 
	Nr_sys++; if(Nr_sys>=L_SYS) goto ERR_SERV;
	Menu[Nr_sys]=" Obliczenia ";
	S=(struct Service *)Malloc(sizeof(struct Service));
	if(S==NULL) goto ERR_MEM;
	Serv[Nr_sys]=S;
	S->kod_uslugi=Nr_sys+1;
	S->l_rek_max=AG_SIZE;
	S->str_size=sizeof(struct oblicz);
	S->typ_bazy=BAZA_WEKTOR;
	S->D=SysA; /*NULL; 
	S->name=Menu[Nr_sys];
	S->main_modul=obl_main;  // Jest zleceniem RT 
	S->def_blankiet=obl_blankiet;
	S->dane_rap_bl=dane_obl;
	S->wpis_rap_bl=wpis_obl;
	S->decyzje=dec_obl;
/* --------------- Podstawiamy dane uslugi z pliku budz.c --------- */
	Nr_sys++; if(Nr_sys>=L_SYS) goto ERR_SERV;
	Menu[Nr_sys]=" Budzik";
	S=(struct Service *)Malloc(sizeof(struct Service));
	if(S==NULL) goto ERR_MEM;
	Serv[Nr_sys]=S;
	S->kod_uslugi=Nr_sys+1;
	S->l_rek_max=AG_SIZE;
	S->str_size=sizeof(struct budzik);
	S->typ_bazy=BAZA_WEKTOR;
	S->D=SysA;
	S->name=Menu[Nr_sys];
	S->main_modul=budz_main;  /* Jest zleceniem RT */
	S->def_blankiet=budz_blankiet;
	S->dane_rap_bl=dane_budz;
	S->wpis_rap_bl=wpis_budz;
	S->decyzje=dec_budz;
/* --------------- Podstawiamy dane dalszych uslug z pliku admin.c ---- */
	Nr_sys++; if(Nr_sys>=L_SYS) goto ERR_SERV;
	Menu[Nr_sys]=" O systemie...";
	S=(struct Service *)Malloc(sizeof(struct Service));
	if(S==NULL) goto ERR_MEM;
	Serv[Nr_sys]=S;
	S->kod_uslugi=Nr_sys+1;
	S->str_size=sizeof(struct admin);
	S->name=Menu[Nr_sys];
	S->def_blankiet=admin_blankiet;
	S->dane_rap_bl=dane_admin;
	S->wpis_rap_bl=wpis_admin;
	S->decyzje=dec_admin;
/* --------------- Podstawiamy dane dalszych uslug z pliku RT pomiar.c ---- */
    Nr_sys++; if(Nr_sys>=L_SYS) goto ERR_SERV;
	Menu[Nr_sys]=" Pomiar EKG";
	S=(struct Service *)Malloc(sizeof(struct Service));
	if(S==NULL) goto ERR_MEM;
	Serv[Nr_sys]=S;
	S->kod_uslugi=Nr_sys+1;
	S->l_rek_max=AG_SIZE;
	S->str_size=sizeof(struct pomiar);
	S->typ_bazy=BAZA_WEKTOR;
	S->D=SysA;
	S->name=Menu[Nr_sys];
	S->main_modul=pomiar_main;  /* Jest zleceniem RT */
	S->def_blankiet=pomiar_blankiet;
	S->dane_rap_bl=dane_pomiar;
	S->wpis_rap_bl=wpis_pomiar;
	S->decyzje=dec_pomiar;
/* --------------- Podstawiamy dane dalszych uslug z pliku pacjent.c ---- 
	Nr_sys++; if(Nr_sys>=L_SYS) goto ERR_SERV;
	Menu[Nr_sys]=" Pacjent";
	S=(struct Service *)Malloc(sizeof(struct Service));
	if(S==NULL) goto ERR_MEM;
	Serv[Nr_sys]=S;
	S->kod_uslugi=Nr_sys+1;
	S->str_size=sizeof(struct pacjent);
	S->name=Menu[Nr_sys];
	S->typ_bazy=BAZA_SPOJNA;
	S->def_blankiet=pacjent_blankiet;
	S->dane_rap_bl=dane_pacjent;
	S->wpis_rap_bl=wpis_pacjent;
	S->decyzje=dec_pacjent;
/* --------------- Podstawiamy dane dalszych uslug z pliku ustaw.c ---- 
	Nr_sys++; if(Nr_sys>=L_SYS) goto ERR_SERV;
	Menu[Nr_sys]=" Ustawienia pomiaru";
	S=(struct Service *)Malloc(sizeof(struct Service));
	if(S==NULL) goto ERR_MEM;
	Serv[Nr_sys]=S;
	S->kod_uslugi=Nr_sys+1;
	S->str_size=sizeof(struct ustaw);
	S->name=Menu[Nr_sys];
	S->main_modul=ustaw_main;
	S->def_blankiet=ustaw_blankiet;
	S->dane_rap_bl=dane_ustaw;
	S->wpis_rap_bl=wpis_ustaw;
	S->decyzje=dec_ustaw;
/* --------------- Podstawiamy dane dalszych uslug z plkow ___.c ---- * /
	Nr_sys++; if(Nr_sys>=L_SYS) goto ERR_SERV;
	Menu[Nr_sys]=" .....";
	S=(struct Service *)Malloc(sizeof(struct Service));
	if(S==NULL) goto ERR_MEM;
	Serv[Nr_sys]=S;
	S->kod_uslugi=Nr_sys+1;
	S->str_size=sizeof(struct ...);
	S->name=Menu[Nr_sys];
	S->main_modul=..._main;
	S->def_blankiet=..._blankiet;
	S->dane_rap_bl=dane_...;
	S->wpis_rap_bl=wpis_...;
	S->decyzje=dec_...;
/ * ----------------------------------------------------------------- * /
	.................
	itd
/ * --------------------------------------------------------------- */
	if(Nr_sys<L_SYS-1)
	 {fprintf(stderr,"\nZa malo zlecen wg L_SYS=%d Nr_sys=%d !!!. Popraw L_SYS w konfig.c ",
											L_SYS,Nr_sys);
		getch(); exit(1);
	 }
/* --------------------------------------------------------------- */
	Menu[L_SYS]=" -- Stan systemu --";
	S=(struct Service *)Malloc(sizeof(struct Service));
	if(S==NULL) goto ERR_MEM;
	Serv[L_SYS]=S;
	S->kod_uslugi=L_SYS+1;
	S->D=SysA;
	S->typ_bazy=BAZA_WEKTOR;
	S->l_rek_max=AG_SIZE;
	S->str_size=sizeof(struct agenda);
	S->name=Menu[L_SYS];
	S->main_modul=NULL; /* sys_main; */
	S->def_blankiet=sys_blankiet;
	S->dane_rap_bl=dane_sys;
	S->wpis_rap_bl=wpis_sys;
	S->decyzje=dec_sys;
/* ============================================================== */
	for(i=1;i<L_SYS;i++)
	 {S=Serv[i];
		if(S->main_modul!=NULL)
		 {S->D=SysA; S->typ_bazy=BAZA_WEKTOR;
			S->str_size+=sizeof(struct agenda);
			S->l_rek_max=AG_SIZE;
		 }
	 }
	Menu[L_SYS+1]=NULL;
	Menu[L_SYS+2]=NULL;
	return L_SYS+1;
ERR_MEM:
	 {fprintf(stderr,"\nBrak pamieci dla servisu %d",Nr_sys); getch(); exit(1);}
ERR_SERV:
	 {fprintf(stderr,"\nBrak miejsca na podsystem: L_SYS=%d Nr_sys=%d !!!. Popraw L_SYS w konfig.c ",
											L_SYS,Nr_sys);
		getch(); exit(1);
	 }
	return 0;
 }
#endif