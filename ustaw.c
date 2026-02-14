#include "../blank/moje.h"
#include "ustaw.h"
void chk_time(void);
extern struct Service *Service;
extern struct agenda *Agenda;
extern unsigned int attr_title;
extern unsigned int attr,at_wpis;
extern unsigned int cursor, nocursor;
extern int X_time, X_tyt, lwmall;
//int l_algor=1;
static struct agenda **SA;
/* -------------------- kody decyzji w moje.h ------------
#define DEC_NEW    1
#define DEC_RUN    2
#define DEC_TYP_US 3
 --------------------------------------------------------- */
#define DEC_FORM   10
#define DEC_COMP   11
#define LFORM 4

int dane_agendy(struct agenda *A, struct agenda *An, int cykl_max);
int pobierz_rekord_uslugi(int *nr_rekordu, int kod_uslugi, int ob_konc,
									struct agenda **As, struct agenda **An,
									struct agenda ***SA, char *adres_rek0_uslugi, char *nazwa);
int pokaz_listy_zlecen(struct agenda *Anew, int kod_uslugi, char *tytul);
int decyzje_run(char decyzja, struct agenda **Aserv, struct agenda **Anew,
                  int *nr_rekordu, int kod_uslugi, char *tytul);
void ustaw_typ_uslugi(struct agenda *A, int decyzja);
extern int lwmall;
/* ======================================================== */
extern unsigned int attryb;
int ustaw_main(void *DA)
 {
/*	
	struct agenda *A;
	struct oblicz *Ob;
	A=(struct agenda *)DA;
	Ob=(struct oblicz *)(A->data);
	if(strlen(Ob->komenda)>0)
	 {int yp, xp, ret;
		char text[200];
		yp=MY_MAX; xp=X_L0+1;
		snprintf(text,sizeof(text)," Wykonac komende: %s <t/Esc> ?????? ",Ob->komenda);
		ret=monit_textowy(yp, xp, ATTR_A, text);
		if(ret=='t') system(Ob->komenda);
	 }
	A->state=0;
	return A->state;
*/
	return 0;
 };

int ustaw_blankiet(int nr_rekordu, int ob_pocz,
										int ob_konc, int x_lewy_gorny,
										int y_lewy_gorny, int kod_uslugi,
										char *adres_rek0_uslugi)
 {
	/*
	int ret, ochr, ochrf=-1, size, nr_rek, raport;
	int cykl_max=6000;
	struct oblicz *Ob;
	struct agenda *A=NULL;
    sledzenie(); 
	nr_rek=nr_rekordu;
	ret=pobierz_rekord_uslugi(&nr_rek,kod_uslugi, ob_konc, &A, &Anew, &SA,
														adres_rek0_uslugi, "zlecen obliczeniowych");
  Aserv=A;
	if(A==NULL)
	 {if(nr_rek<0) return ret;
		ret=dana_koment(-1, -1, " Brak zlecenia oblicz. na pozycji %d w agendzie ",nr_rek);
		return ret;
	 }
	Ob=(struct oblicz *)(A->data);
	rekord_danych_do_naglowka(nr_rek);
	if(Anew==NULL)
	 {ret=dana_koment(-1,13,"+ Modyfikacja danych OBLICZEN juz zgloszonych ");}
	else
	 {ret=dana_koment(-1,15,"+ Wpis danych NOWEGO ZLECENIA OBLICZENIOWEGO ");}
	{static int nmin=0, nmax=LFORM-1;
	 if(Anew==NULL) ochr=-1; else ochr=4;
		ret=dana_int_menu_dec(-1,-1,"+ Formula nr (%d-%d)  ?? ", &nmin, &nmax,
								 &(Ob->nr_wzoru), size=1, ochr, raport=-1, LFORM, MenuObl,DEC_FORM);
	 switch(Ob->nr_wzoru)
		{case 0: break;
		 case 1: break;
		 case 2: break;
		 case 3:
			 {static char *MenuComp[3]={"copy oblicz.c a:\\zbior.c", "Visual C", "WATCOM C"};
        ret=dana_text_dec(-1,-1,"+ Wpisz komende:  ?? ", Ob->komenda, size=40,
													 MenuComp, 3, ochr=1, DEC_COMP);
			 }
       break;
		 default: break;
		}
	}
	return dane_agendy(A, Anew, cykl_max);
	*/
	return 0;
 }

char *dane_ustaw(int ob_pocz, int ob_konc, int *rozmiar_ob)
 {
	/*
	static struct Service *S;
	static struct agenda AS, *A;
  struct oblicz OBL, *Ob;
	int x,y;
	Ob=&OBL; A=&AS; S=Service;
	A=(struct agenda *)Malloc(sizeof(struct agenda)+l_algor*sizeof(struct oblicz));
  A->S=S; A->data=(A+1);
  x=m_wherex(); y=m_wherey(); setcursor(nocursor);
	term_printf(Y_G0,X_tyt,attr_title,"%s",S->name);
	term_cur(y,x); setcursor(cursor);
	return (char *)A;
	*/
	return "A";

 }

void wpis_ustaw(int ob_pocz, int ob_konc, char *D, int rozmiar_ob, char zapis[], char *Kod_op) {;}
int dec_ustaw(int decyzja, int kod_decyzji, int nr_dec, int kod_uslugi, int np, int *nr_rekordu)
 {
	/*
	struct agenda *A0, *A;
	struct oblicz *Ob;
	int sek, min, godz, minB, dmin, minp, min_sum;
	int ag_no, p_min, p_max, ret, nast=0;
/*	 A0=(struct agenda *)czy_zdefiniowany(kod_uslugi, &p_min, &p_max, &ret);
	 A=(struct agenda *)ustal_adres_rek(kod_uslugi,*nr_rekordu);
	 Ob=(struct oblicz *)(A->data);  */
/*
	 if(kod_decyzji==DEC_NEW && decyzja=='t')
		{if(Anew==NULL)
			{Anew=service_default(dec_obl);
			 A=Anew; Ob=(struct oblicz *)(A->data); Ob->komenda[0]=EOS;
			}
		 if(Anew==NULL) return kod_uslugi;
		 A=Anew; Ob=(struct oblicz *)(A->data);
		 czas_zegarowy(&godz, &min, &sek); 	/* oddaje minute od godz.0.0 (stud_sys.c) */
/*		 Aserv=Anew;
		 return kod_uslugi;
		}
	 else
		{if(Anew!=NULL) A=Anew;
		 else A=Aserv; /* rekord zlecenia agendy */
/*		 if(A==NULL) return kod_uslugi;
		}
	 Ob=(struct oblicz *)(A->data);
	 if(kod_decyzji==DEC_COMP)
		{/*if(strlen(Ob->komenda)>0)
			{term_printf(MY_MAX, X_L0,ATTR_A,"Wykonanie komendy %s <t/Esc>",Ob->komenda);
			 if(GET_char()=='t') system(Ob->komenda);
			}*/
/*		 return kod_uslugi;
		}
	 if(kod_decyzji==DEC_RUN)
		{ret=decyzje_run(decyzja, &A, &Anew, nr_rekordu,
										 kod_uslugi, " Lista zlecen obliczeniowych ");
		 if(ret==-1) return -1;
		}
	 if(kod_decyzji==DEC_TYP_US) ustaw_typ_uslugi(A, decyzja);
	 return kod_uslugi;
	 */
	return 0;
 }
