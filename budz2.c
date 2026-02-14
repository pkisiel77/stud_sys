#include "blank/moje.h"
#include "dek_budz2.h"
void chk_time(void);
extern struct Service *Service;
/* extern struct agenda *Agenda; */
extern unsigned int attr_title;
extern unsigned int cursor, nocursor;


extern int X_time, X_tyt;
extern unsigned int attr,at_wpis;
extern time_t sek_akt;
#define S_MIN 60l
/* ------------------ kody decyzji sa w moje.h -------------
#define DEC_NEW    1
#define DEC_RUN    2
#define DEC_TYP_US 3
#define DEC_TIME   4
#define DEC_DELAY  5
 ----------------------------------------------------------- */
#define DEC_MOD_TIME 16
static struct agenda **SA;
extern int AgendaMax, lwmall;
int dane_agendy(struct agenda *A, struct agenda *An, int cykl_max);
int pobierz_rekord_uslugi(int *nr_rekordu, int kod_uslugi, int ob_konc,
									struct agenda **As, struct agenda **An,
									struct agenda ***SA, char *adres_rek0_uslugi, char *nazwa);
int decyzje_run(char decyzja, struct agenda **Aserv, struct agenda **Anew,
                  int *nr_rekordu, int kod_uslugi, char *tytul);
int pokaz_listy_zlecen(struct agenda *Anew, struct agenda ***SA,
												 int kod_uslugi, char *tytul);
void ustaw_typ_uslugi(struct agenda *A, int decyzja);
/* ================================================================ */
int budz2_main(void *DA)
 {struct agenda *A;
	struct budzik2 *B;
	int ret;
	char tytul[75], *M[3],text[75],text1[75];
	A=(struct agenda *)DA;
	B=(struct budzik2 *)(A->data);
	setalarm(); /* for(i=0;i<10;i++) printf("\a"); */
	snprintf(tytul,sizeof(tytul)," BUDZIK 2: <Ent> - wylaczenie sygnalu; <Esc> - usuniecie monitu ");
	snprintf(text,sizeof(text),"     Zgloszono na godz.%02d.%02d",B->godz,B->min);
	snprintf(text1,sizeof(text1),"     Najblizszy monit za %dmin.%02dsek.", (int)A->Interval/60,(int)A->Interval%60);
	{
	 setcursor(nocursor);
	 M[0]=B->nazwa;
	 M[1]=text; M[2]=text1;
	 do
		{ret=okno_menu(M,3,0,attr,at_wpis, Y_G0+3,X_L0+1,-1,tytul,1);
		 clearalarm(); nosound();
		}while(ret>=0);
	 setcursor(cursor);
//	 restore_var(buf_var);
//	 Free(buf_var);
	}
	{int sek, min, minB, dmin, godz, min_doby;
	 long int ss;
	 min_doby=czas_zegarowy(&godz, &min, &sek); 	/* oddaje minute od godz.0.0 (stud_sys.c) */
	 minB=B->godz*60+B->min;
	 dmin=minB-min_doby; if(dmin<0) dmin+=1440;
	 if(dmin<B->co_ile)
		{B->co_ile=dmin; ss=B->co_ile*60l; if(ss>32000) ss=32000;
		 A->Interval=(int)ss;
		}
	}
	A->state=0;
	return A->state;
 }

/* --------------------------------- dek_budz2.h ----------
struct budzik2
 {char nazwa[78];
	int godz;
	int min;
	int sek;
	int co_ile;
	int wyprzedz;
 }; ------------------------------------------------------ */
int budz2_blankiet(int nr_rekordu, int ob_pocz, int ob_konc,
											int x_lewy_gorny, int y_lewy_gorny, int kod_uslugi,
											char *adres_rek0_uslugi)
 {int ret, ochr, size, raport, nr_rek;
	int i, ma_rekord=0;
	struct budzik2 *B;
	struct agenda *A;
/*sledzenie(); */
	if(Anew==NULL)
	 {for(i=ob_pocz;i<ob_konc;i++)
		 {A=(struct agenda *)ustal_adres_rek(kod_uslugi, i);
			if(A==NULL) continue;
			if((A->S)->kod_uslugi==kod_uslugi) {ma_rekord=1; break;}
		 }
		if(ma_rekord==0)
		 {int nr0=0;
			dec_budz2('t', DEC_NEW, 0, kod_uslugi, 0, &nr0);
		 }
	 }
  nr_rek=nr_rekordu;
  ret=pobierz_rekord_uslugi(&nr_rek,kod_uslugi, ob_konc, &A, &Anew, &SA,
                            adres_rek0_uslugi, "budzikow 2");
  Aserv=A;
	if(A==NULL)
	 {if(nr_rek<0) return ret;
    ret=dana_koment(-1, -1, " Brak budzika 2 na pozycji %d w agendzie ",nr_rek);
		return ret;
	 }
	B=(struct budzik2 *)(A->data);
	rekord_danych_do_naglowka(nr_rek);
	if(Anew==NULL)
	 {ret=dana_koment(-1,17,"+ Modyfikacja danych BUDZIKA 2 juz zgloszonego ");}
	else
	 {ret=dana_koment(-1,23,"+ Wpis danych NOWEGO BUDZIKA 2 ");}
	ret=dana_text(-1, -1," Sprawa ", B->nazwa, size=60, NULL, 0,ochr=1);
	ret=dana_koment(-1,-1," Czas budzenia: ");
	{static int g_min=0, g_max=23, m_min=0, m_max=59, ile_min=1, ile_max=1440;
	 ret=dana_int_dec(-1, -1,"godz.", &g_min, &g_max, &B->godz, size=2, ochr=1, raport=-1,DEC_MOD_TIME);
	 ret=dana_int_dec(-1, -1," min.", &m_min, &m_max, &B->min, size=2, ochr=1, raport=-1,DEC_MOD_TIME);
	 if(B->mod_time>0 && Anew==NULL)
		{static char dec, *zatw[3]={"t tak","n anulow.zmiany","?  CZEKAM !!! "};
		 dec='?';
		 ret=dana_decyzyjna(-1,-1, " Zatwierdz czas <%s>  ?? ", "t/n/?", zatw, 3,
											 &dec, ochr=5, DEC_TIME);
		}
	 else {ret=dana_koment(-1, -1, "+ ");}
	 ret=dana_int_dec(-1, -1,"+ Podaj WYPRZEDZENIE alarmu [min] ", &ile_min, &ile_max, &B->wyprzedz,
										size=4, ochr=1, raport=-1,DEC_TIME);
	 if(A->mode=='t')
		{int godz, min, sek, min_doby;
		 long int Sek;
		 min_doby=czas_zegarowy(&godz, &min, &sek);
		 Sek=min_doby*S_MIN+sek+(long int)A->delay;
		 godz=(int)(Sek/3600l); min=(int)(Sek%3600l); min=min/60; sek=min%60;
		 if(godz>=24) godz-=24;
		 ret=dana_koment(-1, -1, "+ Wywolanie jednorazowe o godz.%02d.%02d,%02d",godz,min,sek);
		}
	 else
		{ret=dana_int_dec(-1, -1,"+ CO ILE MINUT monit (%d-%d) ?? ", &ile_min, &B->wyprzedz, &B->co_ile,
										size=4, ochr=1, raport=-1,DEC_TIME);
		}
	}
	return dane_agendy(A, Anew, 0);
 }

extern time_t sek_akt;
char *dane_budz2(int ob_pocz, int ob_konc, int *rozmiar_ob)
 {static struct Service *S;
		static struct agenda *A;
	struct budzik2 *Bu;
	int x,y, i;
	int min, sek, godz;
	S=Service;
	*rozmiar_ob=sizeof(struct agenda)+sizeof(struct budzik2);
	A=(struct agenda *)Malloc(*rozmiar_ob);
	A->S=S; A->data=(A+1);
	Bu=(struct budzik2 *)(A->data);
	A->mode='s'; A->prior=1;
	for(i=0;i<10;i++) Bu->nazwa[i]=0; Bu->nazwa[i]=0;
	czas_zegarowy(&godz, &min, &sek);  /* oddaje minute od godz.0.0 (stud_sys.c) */
	Bu->min=min; Bu->godz=godz+1;
	if(Bu->godz>=24) Bu->godz-=24;
	Bu->co_ile=2; Bu->wyprzedz=15;
	x=m_wherex(); y=m_wherey(); setcursor(nocursor);
	term_printf(Y_G0,X_tyt,attr_title,"%s",S->name);
	term_cur(y,x); setcursor(cursor);
	return (char *)A;
 }

void wpis_budz2(int ob_pocz, int ob_konc,
					 char *D, int rozmiar_ob, char zapis[], char *Kod_op)
 {struct agenda *A;
	 A=(struct agenda *)D;
	 (void)A;
 }

void ustaw_czas_budzika2(struct agenda *A, struct budzik2 *B);
void ustaw_delay_budzika2(struct agenda *A, struct budzik2 *B);

void ustaw_czas_budzika2(struct agenda *A, struct budzik2 *B)
{
	int sek, min, godz, minB, dmin, minp, min_doby;
	if (B->godz >= 24) B->godz -= 24;
	if (B->wyprzedz <= 0) A->mode = 't';
	if (A->mode == 't')
	{
		A->number_of_calls = 1; /*B->wyprzedz=0;*/ B->co_ile = B->wyprzedz;
	}
	else
	{
		if (B->co_ile < 1) B->co_ile = 1;
		if (A->mode == 'p')  A->number_of_calls = -1; /* jest to usluga permanentna */
		else
		{
			A->number_of_calls = B->wyprzedz / B->co_ile + 1;
		}
		if (B->co_ile > B->wyprzedz) B->co_ile = B->wyprzedz;
	}
	{long int ss;
	ss = B->co_ile * 60l; if (ss > 32000) ss = 32000;
	A->Interval = (int)ss;
	}
	min_doby = czas_zegarowy(&godz, &min, &sek); 	/* oddaje minute od godz.0.0 (stud_sys.c) */
	minB = B->godz * 60 + B->min;
	minp = minB - B->wyprzedz;
	if (minp < 0)   /* poczatek alarmu we wczesn.dobie */
	{
		minp += 1440;
		dmin = minp - min_doby;  /* l.minut do pierwszego alarmu */
		if (dmin < 0) { dmin = 0; } /* pocz.alarmu w aktualnej dobie nie pozniej niz min_doby */
	}
	else        /* poczatek alarmu w tej dobie co budzik */
	{
		dmin = minp - min_doby;  /* l.minut do pierwszego alarmu */
		if (min_doby > minB) /* budzik na nastepna dobe */
		{
			if (dmin < 0) { dmin += 1440; }
		} /* pocz.alarmu w nastepnej dobie */
		else             /* budzik w aktualnej dobie i 1szy alarm tez (minp>0)*/
		{
			if (dmin < 0) { dmin = 0; }
		} /* pocz.alarmu w aktualnej dobie nie pozniej niz min_doby */
	}
	if (dmin <= 0) A->delay = 0;
	else { A->delay = (float)(dmin*S_MIN) - sek; }
	B->mod_time = 0;
	B->poprz_godz_budz = B->godz;  B->poprz_min_budz = B->min;
}

void ustaw_delay_budzika2(struct agenda *A, struct budzik2 *B)
{
	int sek, min, godz, minB, dmin, minp, min_doby, za_duzo;
	long int ss;
	min_doby = czas_zegarowy(&godz, &min, &sek);  /* aktualna minuta od godz.0.0 */
	minB = B->godz * 60 + B->min;
	ss = (long int)(A->delay + 0.49) + (long int)sek;
	dmin = (int)(ss / S_MIN);
	if ((ss%S_MIN) != 0) dmin++; /* zostaly sek. w minucie - dodac cala minute */
	minp = dmin + min_doby;
	za_duzo = 0;
	if (minp >= 1440)   /* 1szy monit w nast.dobie */
	{
		minp -= 1440;
		if (minB >= min_doby)  /* budzenie w aktualnej dobie (za duzy delay)*/
		{
			za_duzo = 1; minp = minB; dmin = minB - min_doby;
		}
		else  /* budzenie tez w nastepnej dobie */
		{
			if (minp > minB) { dmin -= (minp - minB); za_duzo = 1; }
		}
	}
	else     /* minp<1440: 1szy monit w aktualnej dobie */
	{
		if (minp >= minB)
		{
			if (minB >= min_doby)  /* budzenie w aktualnej dobie (za duzy delay)*/
			{
				za_duzo = 1; minp = minB; dmin = minB - min_doby;
			}
		}
	}  /* Jesli minB<min_doby (budz.w nast.dobie) to minp>minB, bo tu minp>min_doby !!! */
	if (za_duzo > 0)  /* (za duzy delay)*/
	{
		A->delay = (float)(dmin*S_MIN) - sek; if (A->delay < 0) A->delay = 0;
		if (A->mode != 'p')
		{
			A->mode = 't'; A->Interval = 0; B->co_ile = 0; B->wyprzedz = 0;
			A->number_of_calls = 1;
		}
		return;
	}
	B->wyprzedz = minB - minp; if (B->wyprzedz < 0) B->wyprzedz += 1440;
	{long int ss;
	ss = B->co_ile * 60l; if (ss>32000) ss = 32000;
	A->Interval = (int)ss;
	}
	if (A->mode == 't') A->number_of_calls = 1;
	else { A->number_of_calls = B->wyprzedz / B->co_ile + 1; }
}

int dec_budz2(int decyzja, int kod_decyzji, int nr_dec, int kod_uslugi, int np, int *nr_rekordu)
{
	struct agenda *A;
	struct budzik2 *B;
	int sek, min, godz;
	int ret;
	if (kod_decyzji == DEC_NEW)
	{
		if (decyzja == 'n' || decyzja == 'N') return kod_uslugi;
		if (Anew == NULL) Anew = service_default(dec_budz2);
		if (Anew == NULL) return kod_uslugi;
		A = Anew; B = (struct budzik2 *)(A->data);
		/*		 for(i=0;i<10;i++) B->nazwa[i]=0; B->nazwa[i]=0; */
		czas_zegarowy(&godz, &min, &sek); 	/* oddaje minute od godz.0.0 (stud_sys.c) */
		B->min = min; B->godz = godz + 1;
		B->co_ile = 2; B->wyprzedz = 15;
		ustaw_czas_budzika2(A, B);
		Aserv = Anew;
		return kod_uslugi;
	}
	else
	{
		if (Anew != NULL) A = Anew;
		else A = Aserv; /* rekord zlecenia agendy */
		if (A == NULL) return kod_uslugi;
		B = (struct budzik2 *)(A->data);
	}
	if (kod_decyzji == DEC_TIME)
	{
		if (decyzja == 'n' && B->mod_time > 0)
		{
			B->godz = B->poprz_godz_budz; B->min = B->poprz_min_budz;
		}
		else ustaw_czas_budzika2(A, B); /* Zatw.czasu lub zmiana wyprz.lub okresu */
		B->mod_time = 0;
	}
	if (kod_decyzji == DEC_MOD_TIME) B->mod_time = 1;
	if (kod_decyzji == DEC_DELAY)	{ ustaw_delay_budzika2(A, B); }
	if (kod_decyzji == DEC_RUN)
	{
		ret = decyzje_run(decyzja, &A, &Anew, nr_rekordu,
			kod_uslugi, " Lista budzikow ");
		if (ret == -1) return -1;
	}
	if (kod_decyzji == DEC_TYP_US)
	{
		ustaw_typ_uslugi(A, decyzja); ustaw_czas_budzika2(A, B);
	}
	return kod_uslugi;
}
