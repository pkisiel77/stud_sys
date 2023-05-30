/* #include <butil.h> */
#include "blank/moje.h"
#include "sprawdz.h"
void chk_time(void);
extern struct Service *Service;
/* extern struct agenda *Agenda, *SysA[]; */
extern int X_time, X_tyt;
extern time_t sek_akt;
extern unsigned int attr_title, attryb, attr_time;
extern unsigned int cursor, nocursor;

char *dni[]={"Niedziela","Poniedzialek", "Wtorek","Sroda","Czwartek","Piatek","Sobota"};
extern struct agenda *SysA[];
int sprawdz(void *DA)
 {struct agenda *A;
	struct sprawdz *Sr;
	unsigned int old_attr;
	struct tm *Time;
	time_t tsek1970;
	int i, xkursora,ykursora;
	A=(struct agenda *)DA;
	Sr=(struct sprawdz *)(A->data);
	A->state=0;   tsek1970=time(NULL);
	Time=localtime(&tsek1970);
	xkursora=m_wherex(); ykursora=m_wherey();
	setcursor(nocursor);  old_attr=attryb;
	term_printf(Y_G0,X_time,attr_time," Dnia %02d.%02d.%02d ",
							Time->tm_mday,Time->tm_mon+1,(Time->tm_year%100));
	term_printf(Y_G0,m_wherex(),attr_time,"Godz.%02d:%02d.%02d (%s)",
							Time->tm_hour,Time->tm_min,Time->tm_sec,dni[Time->tm_wday]);
/*	term_printf(Y_G0,X_time,attr_time," Dnia %02d.%02d.%02d  Godz.%02d:%02d.%02d (%s)",
							Time->tm_mday,Time->tm_mon+1,(Time->tm_year%100),
							Time->tm_hour,Time->tm_min,Time->tm_sec,dni[Time->tm_wday]); */
	for(i=m_wherex();i<MXR_MAX;i++) term_type(0,i," ",1,attr_time);
	attryb=old_attr; setcursor(cursor); m_gotoxy(xkursora,ykursora);
/* ----------------- animacja blankietow ------------------------ */
	{void **Ag;
	 struct agenda **SA;
	 int i, lzl, lza, kod_uslugi_blank;
	 kod_uslugi_blank=kod_rap_aktual_blank(); /* kod_uslugi_blank=-1; */
	 if(kod_uslugi_blank>=0)
		{SA=getAgendaPtr(&lzl); freeAgendaPtr();
		 Ag=(void **)Malloc(2*lzl*sizeof(void *));
		 if(Ag!=NULL)
			{for(i=0, lza=0; i<lzl;i++)
				{if(SA[i]==NULL) continue;
				 Ag[lza]=&SA[i]->number_of_calls; lza++;
				 Ag[lza]=&SA[i]->delay; lza++;
				 // moje...to ja 
				 Ag[lza]=&SA[i]->dana; lza++;
				 Ag[lza]=&SA[i]->czas; lza++;
				 Ag[lza]=&SA[i]->wart_max; lza++;
                 Ag[lza]=&SA[i]->wart_min; lza++;
				 Ag[lza]=&SA[i]->alarm;lza++;
				 //
				}
			 animuj_blank(lza,Ag);
			 free(Ag);
			}
		}
	}
	attryb=old_attr; setcursor(cursor); m_gotoxy(xkursora,ykursora);
	return A->state;
 };

#define DEC_DALEJ  11
/* -------------------- dalsze stale w moje.h -------------------
#define DEC_TYP_US 2
#define DEC_NEW    3
	--------------------------------------------------------------- */
static int nr_ag=0;
int sprawdz_blankiet(int nr_rekordu, int ob_pocz, int ob_konc,
						int x_lewy_gorny, int y_lewy_gorny, int kod_uslugi, char *DaneUslugi)
/* ------------- D jest adresem zerowego rekordu bazy ----------------- */
 {static struct Service *S;
	struct agenda *A, *A0, *nA, **SA;
	int ls, nr_rek, size, ochr, ochrf=-3, raport, ret;
	A0=(struct agenda *)DaneUslugi;
	nr_rek=nr_rekordu;
	A=(struct agenda *)dane_raportowanego_rekordu(sprawdz_blankiet,&nr_rek);
	if(A==NULL)
	 {term_printf(MY_MAX,X_L0,ATTR_A," Blad adresu w Sprawdz (A=%p). <Ent> ",A);
		GET_char();  return -1;
	 }
	rekord_danych_do_naglowka(nr_ag);
	ls=0;
	ret=dana_koment(-1,16,"+ Raport zlecen podstawowych SPRAWDZ (jest razem %d)",ls);

	if(A==NULL)
	 {ret=dana_koment(-1,10,"+ Brak zlecenia na poz.%d w agendzie",nr_ag);
		return ret;
	 }
/* ------------------------------------------------------- */
	 {static int z_min=0, z_max=0, nrzl=0;

		ret=dana_rekord_str_dec(-1,-1, "+ Zlecenie glowne:", &z_min, &z_max, &nrzl,
													 size=0, ochr=-1, Service->kod_uslugi,
													 raport=(A->S)->kod_uslugi, DEC_NEW,
													 " %19S typ=%c prior=%d okres=%d",
													 &A0->name,&(A0->mode),
													 &(A0->prior), &(A0->Interval));
	 }
	 {static char *typ_usl[3]={"p permanentna (stala)", "s seryjna", "t dorazna"};
		ret=dana_znak(-1,-1," Typ uslugi <%s> ", "p/s/t", typ_usl, 3,
											 &(A->mode), ochr=-1);
	 }
	 {static int nmin=1, nmax=3000;
		if(A->mode=='p') {ochr=-1; nmin=0; nmax=0;} else ochr=2;
		ret=dana_koment(-1,-1,"+   Zlecenie stale nieusuwlne !!!! ");
	 }
	 {static int nmin=1, nmax=3000, del_min, del_sek;
		del_min=(long int)A->delay/60l; del_sek=((long int)A->delay)%60l;
		ret=dana_int(-1,-1," Okres wywolan [sek](%d-%d) ?? ", &nmin,&nmax,
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

	if(ls==0)
	 {ret=dana_koment(-1,-1,"+ ");}
	 {static char *usun[3]={"d wpis danych","m menu glowne","a dane algorytmow"}, dec='d';
		ret=dana_decyzyjna(-1,-1," Co robimy <%s>  ?? ", "d/m/a", usun, 2,
										 &dec, ochr=5, DEC_DALEJ);
	 }
	 
	 
/*    ret=dana_znak(-1,-1," Typ uslugi <%s>", "p/s/t", typ_usl, 3, &(A->mode), ochr=5);  */
	return ret;
 }



char *dane_sprawdz(int ob_pocz, int ob_konc, int *rozmiar_ob)
 {int ls;
	return (char *)getAgendaPtr(&ls);
 }

void wpis_sprawdz(int ob_pocz, int ob_konc,
					 char *D, int rozmiar_ob, char zapis[], char *Kod_op)
 {;}
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
int dec_sprawdz(int decyzja, int kod_decyzji, int nr_dec,
											int kod_raportu, int np, int *nr_rekordu)
 {struct agenda *A0, *A, **SA;
/*  struct Service *S; */
	int ns, p_min, p_max, ret;
	A0=(struct agenda *)czy_zdefiniowany(kod_raportu, &p_min, &p_max, &ret);
	A=(struct agenda *)ustal_adres_rek(kod_raportu,*nr_rekordu);
	switch(kod_decyzji)
	 {case DEC_DALEJ:
			if(decyzja=='m') return -1;
			break;
		case DEC_NEW:
			break;
		case DEC_TYP_US:
			break;
		default: break;
	 }
	return kod_raportu;
 }
/* --------------------------------------------------------------- */