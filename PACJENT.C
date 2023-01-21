#include "../blank/moje.h"
#include "../blank/sys_dekl.h"
void chk_time(void);
extern struct Service *Service;
extern struct agenda *Agenda;
extern unsigned int attr_title;
extern int X_time, X_tyt;
extern unsigned int cursor, nocursor;

//int l_stud=1;
#include "pacjent.h"
#define DEC_USUN 1
char **Nazwiska=NULL, **Imiona, **Miasta, **Kod;
#define DEC_NAZW 22
#define DEC_IMIE 23
#define DEC_KOD 24
#define DEC_POMIAR 25
#define NLEN 22
static char txt[NLEN], tx1[NLEN];
int lnazw=0,limion=0, lmiast=0;
/* ================================================================ */
// Plan
//
#define L_POZ_DANYCH_OSO
char *text_dane_osobowe[L_POZ_DANYCH_OSO]={"Nr",
					   "Imie",
					   "Nazwisko",
					   "Miejsce zamieszkania","Ulica","Nr domu","Nr mieszkania",
					   "Data urodzenia","Dzien","Miesiac","Rok",
						 "Oplata","Skierowanie","Prywatnie"
					   "Nr skierowania","Cena",
					   "Nr kasy chorych",
					  };
// ende

int pacjent_blankiet(int nr_rekordu, int ob_pocz,	int ob_konc,
		int x_lewy_gorny,	int y_lewy_gorny,
		int anim_pid, char *D)
 {int ret, ochr, ochrf=-1, size, nr_rek;
	struct pacjent *d0, *dp, *d;
	static char txt_kod[15];
	int lkod=15;
	txt[0]=0; txt[1]=0;
	setcursor(cursor);
/*sledzenie(); */
	d0=(struct pacjent *)D;
	nr_rek=nr_rekordu;
	dp=d0+nr_rek;
	d=(struct pacjent *)dane_raportowanego_rekordu(pacjent_blankiet,&nr_rek);
	if(d==NULL || d!=dp)
	 {term_printf(MY_MAX,X_L0,ATTR_A," Niezgodnosc adresow w RAP_ZM(d=%p dp=%p). <Ent> ",d,dp);
		GET_char();  return -1;
	 }
	rekord_danych_do_naglowka(nr_rek);

// gonzo77@poczta.fm
// WIZYTA
#define OCHR_WIZYTA 5
#define OCHR_POMIAR 5
#define DL_IMIE 25
#define DL_NAZW 40
#define DL_WIEK 3
#define DL_OX 3
#define DL_OY 3

	{static int nr_ewd_min=1, nr_ewd_max=9999;
	 static int czas_pom_min=1, czas_pom_max=9999;
     static int wiek_min=1, wiek_max=150;
	 static int ox_min=-100, ox_max=100, oy_min=-100, oy_max=100;
	 static char nazw_txt[DL_NAZW],imie_txt[DL_IMIE];
	 static char *dec_pom[]={"a aktywny","n nieaktywny"};
	 static char  dec='n';
d->nr_ewd=0;
d->ox=0;
d->oy=0;
d->wiek=0;
d->czas_pom=0;
nazw_txt[0]=0;
imie_txt[0]=0;

 
     ret=dana_koment(-1,-1,"+ ----Dane pacjenta----");
	 ret=dana_int(-1, -1,"+ Numer ewidencyjny [%d...%d]: ",&nr_ewd_min,&nr_ewd_max,
								&d->nr_ewd, 4, OCHR_WIZYTA, -1);
	 ret=dana_text_dec(-1, -1,"+ Imie ", imie_txt, size=15, Imiona, limion,OCHR_POMIAR,DEC_IMIE);
	 ret=dana_text_dec(-1, -1,"+ Nazwisko ", nazw_txt, DL_NAZW-1, Nazwiska, lnazw,OCHR_POMIAR,DEC_NAZW);
	 ret=dana_int(-1, -1,"+ Wiek ",&wiek_min,&wiek_max, &d->wiek, DL_WIEK, OCHR_POMIAR, -1);
	 ret=dana_koment(-1,-1,"+ ----Ustawienia pomiaru----");
	 ret=dana_int(-1, -1,"+ Czas pomiaru [%d...%d]: ",&czas_pom_min,&czas_pom_max,
			      &d->czas_pom, 4, OCHR_POMIAR, -1);
     ret=dana_text_dec(-1, -1,"+ Zapis do pliku ", imie_txt, size=15, Imiona, limion,OCHR_POMIAR,DEC_IMIE);
	 ret=dana_decyzyjna(-1,-1,"+ Pomiar EKG - %s  ?? ", "a/n", dec_pom, 2,
					    &dec, OCHR_POMIAR, DEC_POMIAR);

	 if(dec=='a')
	 {
#ifndef GRAF
		 ret=dana_koment(-1,-1,"+ ----Pomiar w trybie TEKSTOWYM----");
#else
		 ret=dana_koment(-1,-1,"+ ----Pomiarw trybie GRAFICZNYM----");
#endif
		 //OknoInfo(10,10," Uwaga uruchomienie pomiaru",ATTR_A,0);
		 ret=dana_int(-1, -1,"+ Dana OX ",&ox_min,&ox_max, &d->ox, DL_OX, OCHR_POMIAR, -1);
         ret=dana_int(-1, -1,"+ Dana OY ",&oy_min,&oy_max, &d->oy, DL_OY, OCHR_POMIAR, -1);
         ret=dana_int(-1, -1,"+ Procentowe zaawansowanie pomiaru [%] ",&oy_min,&oy_max, &d->oy, DL_OY, OCHR_POMIAR, -1);
		 // odczyt z pliku serce.dat
     }



	}
//	ret=dana_text(-1, -1," Nazwisko ?? "
/*
	ret=dana_koment(-1,20,"+ Wpis danych pojedynczego studenta ");
	ret=dana_koment(-1,-1,"+ ");
	{if(d->nr_nazw<0) txt[0]=0;
	 else strcpy(txt,Nazwiska[d->nr_nazw]);
	 ret=dana_text_dec(-1, -1," Nazwisko  ?? ", txt, size=NLEN-1, Nazwiska, lnazw,ochr=1,DEC_NAZW);
	 ret=dana_text_dec(-1, -1,"+ Imie  ?? ", tx1, size=15, Imiona, limion,ochr=1,DEC_IMIE);

	 // lkod jest pobierany z pliku indeksow nazwiska index.dat
	 ret=dana_text_dec(-1,-1,"+ Kod ?? ",txt_kod, size=15, Kod, lkod,ochr=1,DEC_KOD);

	}
	ret=dana_koment(-1,-1,"+ ");
	{static char *usun[4]={"d wpis danych","z ZAPIS do bazy","m menu glowne","u usuwamy"}, dec='d';
	 ret=dana_decyzyjna(-1,-1," Co robimy <%s>  ?? ", "d/z/m/u", usun, 4,
					 &dec, ochr=0, DEC_USUN);
	}
	// opis dodatkowych komend
	ret=dana_koment(18,1,"+ F2 - Lista pacjentow");
*/
	return ret;
 }

char *dane_pacjent(int ob_pocz, int ob_konc, int *rozmiar_ob)
{
	static struct Service *S;
	static struct agenda *A;
	struct pacjent *St;
	int x,y;
  
	S=Service;
	*rozmiar_ob=S->str_size;
	St=Malloc(S->l_rek_max*(*rozmiar_ob));
	x=m_wherex(); y=m_wherey(); setcursor(nocursor);
	term_printf(Y_G0,X_tyt,attr_title,"%s",S->name);
	term_cur(y,x); setcursor(cursor);
	return (char *)St;
}

void wpis_pacjent(int ob_pocz, int ob_konc, char *D, int rozmiar_ob, char zapis[], char *Kod_op)
{
}

int dec_pacjent(int decyzja, int kod_decyzji, int nr_dec, int kod_raportu, int np, int *nr_rekordu)
 {int ret;
 switch(kod_decyzji)
	 {case DEC_USUN:
		 switch(decyzja)
			 {case 'd': return kod_raportu;
				case 'u':
					break;
				case 'z':
					break;
				case 'm': return -1;
				default: break;
			 }
			break;
		 case DEC_NAZW:// ret=wpis_tekstu(txt,Nazwiska,lnazw);
										if(ret>0) lnazw++;

/* case DEC_POMIAR:OknoInf(10,10,"Pomiar start",ATTR_A,1);
			             break;
*/
		default: break;
	 }
	return kod_raportu;
 }
/* --------------- Przyklady danych --------------------------------
	ret=grupa_danych(-1,-1, "+ Tor ", &tmin, &tmax, &d->nr_rek,
			 size=3, ochr=-3, sizeof(struct zm_proc), raport=-1,
   		   ": %4s %8s[%z13]%-8s[%a15]%-11s[%a46] [%s[%z8]]",
				 d0->nazwa,pomiary,d0->nazwa,medium,&(d0->naz1),
                   aparat,&(d0->naz2), units, &d0->jedn);
	 ret=grupa_danych(-1,-1, "+ Inne tory (%d-%d) ", &tmin, &tmax, &nr_zm,
		 size=3, ochr=3, sizeof(struct zm_proc), raport=RAPORT_ZM_PROC,
	          ": %d %4s %8s[%z13]%-8s[%a15]%-11s[%a46] [%s[%z8]]",
									&d0->nr_rek,d0->nazwa,pomiary,d0->nazwa,medium,&(d0->naz1),
									aparat,&(d0->naz2), units, &d0->jedn);
	 S=(struct proces_signal *)czy_zdefiniowany(RAPORT_SYGNALU, &s_min, &s_max,
   ret=dana_float(-1,-1,"+   Precyzja=", &przyr_wzgl_min, &przyr_wzgl_max,
								 &d->skala, size=6, prec=4, ochr1-1, kod_an=1);
    if(ochr>0)
	 {ret=dana_text(-1, -1," Nazwa zm.  ?? ", d->nazwa, size=4, nazwy_zm, 4,
						   ochr);
  	  nrm_min=0; nrm_max=14;
			ret=dana_int_menu(-1, -1,"Medium  ?? ",
				 &nrm_min, &nrm_max, &d->naz1, size=2, ochr, raport=-1,
				 nrm_max+1, medium);
	  nra_min=0; nra_max=45;
			ret=dana_int_menu(-1, -1,"Aparat  ?? ",
 				 &nra_min, &nra_max, &d->naz2, size=2, ochr, raport=-1,
				 nra_max+1, aparat);
     }
	ret=dana_float(-1,-1," Wart.nominalna (%1.0f-%2.0f)   ?? ", &d->sens_d, &d->sens_g,
				 &d->wart_nom, size=6, prec=2, 3, kod_an=1);
	ret=dana_znak(-1, -1," Jednostka {%s} ","1-8",
								 units, 8, &d->jedn, ochr);
	ret=dana_float_dec(-1,-1," Ogr.sens.dolne (%2.0f-%2.0f)  ?? ", &wmin, &wmax,
					 &d->sens_d, size=5, prec=2, ochr=5, kod_an=2, kod_dec=1);
	ret=dana_float_dec(-1,-1," Ogr.sens.gorne (%2.0f-%2.0f)  ?? ", &d->sens_d, &wmax,
					 &d->sens_g, size=5, prec=2, ochr=5, kod_an=2,kod_dec=1);
	ret=dana_int(23, -1," Okno obserwacji dla sr.krotk.i dlugich (%d-%d): (5sek)*", &nrm_min, &dziew, &d->okno, size=1, ochr=5, raport=-1);
			   &size);
	---------------------------------------------------------------- */