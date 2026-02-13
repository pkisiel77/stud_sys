/* #include <butil.h> */
#include "blank/moje.h"
#include "sys_rep.h"
void chk_time(void);
extern struct Service *Service;
extern struct agenda *Agenda;
extern int X_time, X_tyt;
extern unsigned int attr_title;
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
 {struct agenda *A, *A0, **SA;
	int ls, nr_rek, size, ochr, ochrf=-3, raport, ret;
	static int z_min=0, z_max;
	int i, lsa;
	SA=(struct agenda **)getAgendaPtr(&lsa);
	A0=(struct agenda *)D;
	nr_rek=nr_rekordu;
	A=(struct agenda *)dane_raportowanego_rekordu(sys_blankiet,&nr_rek);
	nr_ag=nr_rek;
	if(A0==NULL || A0!=SA[0])
	 {term_printf(MY_MAX,X_L0,ATTR_A," Niezgodnosc adresow w RAP_SYS(SA0=%d A0=%d). <Ent> ",
																		 SA[0],A0);
		GET_char();  return -1;
	 }
	rekord_danych_do_naglowka(nr_ag);
	for(z_min=0;z_min<lsa;z_min++) {if(SA[z_min]!=NULL) break;}
	for(i=0,ls=0;i<lsa;i++) {if(SA[i]!=NULL) {ls++; z_max=i;}}
	if(ls==0)
	 {ret=dana_koment(-1, 26," Brak zlecen w agendzie ");
		return ret;
	 }
	ustaw_rek_max_raportu(sys_blankiet, z_max);
/* ------------------------------------------------------- */
	ret=dana_koment(-1,16,"+ Raport zlecen okresowych w agendzie (jest razem %d)",ls);
	if(A==NULL)
	 {ret=dana_koment(-1,10,"+ Brak zlecenia na poz.%d w agendzie",nr_ag);}
	else
	 {ret=dana_rekord_str_dec(-1,-1, "+ Zlec. (%d-%d) ", &z_min, &z_max, &nr_ag,
													 size=2, ochr=2, Service->kod_uslugi,
													 raport=(A->S)->kod_uslugi, DEC_NEW,
													 " %19S typ=%c prior=%d Cykl=%d Opozn=%5.0f",
													 &A0->name,&(A0->mode), &(A0->prior),
													 &(A0->Interval),&(A0->delay));
	 {static char *typ_usl[3]={"p permanentna (stala)", "s seryjna", "t dorazna"};
		ret=dana_decyzyjna(-1,-1," Typ uslugi <%s>", "p/s/t", typ_usl, 3,
											 &(A->mode), ochr=5, DEC_TYP_US);
	 }
	 {static int nmin=1, nmax=3000;
		ochr=-1;
		if(A->mode=='p')
		 {ret=dana_koment(-1, -1, "  Zlecenie stale !! ");}
		else
		 {ret=dana_int(-1,-1," Liczba wywolan  ", &nmin,&nmax,
										 &(A->number_of_calls), size=4, ochr, raport=-1);
		 }
	 }
	 {static int nmin=1, nmax=3000, del_min, del_sek;
		del_min=(long int)A->delay/60l; del_sek=((long int)A->delay)%60l;
		ret=dana_int(-1,-1," Okres wywolan (%d-%d)  ?? ", &nmin,&nmax,
			&(A->Interval), size=4, ochr=5, raport=-1);
		ret=dana_koment(-1,-1," Czas do nastepnego wywolania %dmin %dsek",
			del_min, del_sek);
	 }
	 {static int nmin=1, nmax=3000, stmin=-999;
		ret=dana_int(-1,-1," Priorytet (%d-%d) ?? ", &nmin,&nmax,
			&(A->prior), size=4, ochr=5, raport=-1);
		ret=dana_int(-1,-1," Stan realizacji ", &stmin,&nmax,
			&(A->state), size=4, ochrf, raport=-1);
	 }
	}
	{static char *usun[3]={"d wpis danych","m menu glowne","u usuwamy"}, dec='d';
	 ret=dana_decyzyjna(-1,-1," Co robimy <%s>  ?? ", "d/m/u", usun, 3,
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
  term_printf(Y_G0,X_tyt,attr_title,"%s",(SysA[nr_ag]->S)->name);
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
