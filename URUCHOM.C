#include "blank/moje.h"
#include "blank/sys_dekl.h"
void chk_time(void);
extern struct Service *Service;
extern struct agenda *Agenda;
extern unsigned int attr_title;
extern int X_time, X_tyt;
extern unsigned int cursor, nocursor;
extern unsigned int attr,at_wpis;
//int l_stud=1;
#include "uruchom.h"
#define DEC_USUN 1
//char **Nazwiska=NULL, **Imiona, **Miasta, **Kod;
char **Sciezka, **Roz;

char **Dysk;

int szukanie=0;

/*
#define DEC_NAZW 22
#define DEC_IMIE 23
#define DEC_KOD 24
#define DEC_POMIAR 25
*/
#define DEC_SCIEZKA 26
#define DEC_ROZ 27
#define DEC_DYSK 28
/*
#define NLEN 22
static char txt[NLEN], tx1[NLEN];
int lnazw=0,limion=0, lmiast=0;
*/
int lsciezka=0;
int lroz=0;
int ldysk=0;
/* ================================================================ */
// Plan
/*
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
*/
int uruchom_blankiet(int nr_rekordu, int ob_pocz, int ob_konc, int x_lewy_gorny, int y_lewy_gorny, int anim_pid, char *D)
{
	int ret, ochr, ochrf=-1, size, nr_rek;
	struct uruchom *d0, *dp, *d;
    static char  decy='n';
	char buf[128];
	static char txt_kod[15];
	int lkod=15;
//	txt[0]=0; txt[1]=0;
	setcursor(cursor);
    /*sledzenie(); */
	d0=(struct uruchom *)D;
	nr_rek=nr_rekordu;
	dp=d0+nr_rek;
	d=(struct uruchom *)dane_raportowanego_rekordu(uruchom_blankiet,&nr_rek);
	if(d==NULL || d!=dp)
	 {term_printf(MY_MAX,X_L0,ATTR_A," Niezgodnosc adresow w RAP_ZM(d=%p dp=%p). <Ent> ",d,dp);
		GET_char();  return -1;
	 }
	rekord_danych_do_naglowka(nr_rek);

#define OCHR_WIZYTA 5
#define OCHR_POMIAR 5
#define DL_IMIE 25
#define DL_NAZW 40
#define DL_WIEK 3
#define DL_OX 3
#define DL_OY 3

#define DL_SCIEZKI 20
#define DL_ROZ 3
#define DL_DYSK 1



	{	static char dysk_txt[DL_DYSK];
		int wynik;

		
		ret=dana_text_dec(-1, -1,"+ Wpisz litere dysku", dysk_txt, DL_DYSK, Dysk, ldysk,OCHR_POMIAR,DEC_DYSK);
         sprintf(buf,"%s:\\",dysk_txt);
		wynik=GetDriveType(buf);
		switch(wynik)
		{ case DRIVE_UNKNOWN:ret=dana_koment(-1,-1,"+ <DRIVE_UNKNOWN>");break;
          case DRIVE_NO_ROOT_DIR:ret=dana_koment(-1,-1,"+ <DRIVE_NO_ROOT_DIR>");break;
		  case DRIVE_REMOVABLE: ret=dana_koment(-1,-1,"+ <DRIVE_REMOVABLE>");break;
          case DRIVE_FIXED: ret=dana_koment(-1,-1,"+ <DRIVE_FIXED>");break;
          case DRIVE_REMOTE: ret=dana_koment(-1,-1,"+ <DRIVE_REMOTE>");break;
          case DRIVE_CDROM: ret=dana_koment(-1,-1,"+ <DRIVE_CDROM>");break;
          case DRIVE_RAMDISK :ret=dana_koment(-1,-1,"+ <DRIVE_RAMDISK>");break;
		  default:break;
		}


			
	}
	 
	{ static char *dec_dysk[]={"t tak","n nie"};


	 ret=dana_decyzyjna(-1,-1,"+ Pokazac zawartosc katalogu glownego %s  ?? ", "t/n", dec_dysk, 2,
					    &decy, OCHR_POMIAR, DEC_DYSK);
	}

     if(decy=='t')
     {
		 static char *nazw_plik[100],*temp;
		 int opcje, i=0;
		 WIN32_FIND_DATA nazwa;
		 BOOL n_opcje;
		 

		 {
		 char buff[128];
		 sprintf(buff,"%s*.*",buf);
		  opcje=FindFirstFile(buff,&nazwa);
		  if(opcje==-1) MessageBox(NULL,"B³¹d znajdowania plików","B³¹d",MB_OK);
		   else
		   {   temp=(char*)malloc(15);
		       sprintf(temp,"%s",nazwa.cFileName);
	           nazw_plik[i]=temp;
		       do
			   {temp=(char*)malloc(15);
				   n_opcje=FindNextFile(opcje,&nazwa);
				   if(opcje!=-1) 
				   {i++;
                    sprintf(temp,"%s",nazwa.cFileName);
				    nazw_plik[i]=temp;
				   }
			   }while(n_opcje);
			   FindClose(opcje);
		   }
          opcje=okno_menu(nazw_plik,i,0,attr,at_wpis, 4,25,-1,NULL,1);
		  if(opcje!=-1) {sprintf(buff,"%s\\%s",buf,nazw_plik[opcje]);WinExec(buff,SW_SHOW);}
		  decy='n';
		 }
        
	 }
 
/*
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
	ret=dana_koment(18,1,"+ F2 - Lista uruchomow");
*/
	return ret;
 }

char *dane_uruchom(int ob_pocz, int ob_konc, int *rozmiar_ob)
{
	static struct Service *S;
	static struct agenda *A;
	struct uruchom *St;
	int x,y;
  
	S=Service;
	*rozmiar_ob=S->str_size;
	St=Malloc(S->l_rek_max*(*rozmiar_ob));
	x=m_wherex(); y=m_wherey(); setcursor(nocursor);
	term_printf(Y_G0,X_tyt,attr_title,"%s",S->name);
	term_cur(y,x); setcursor(cursor);
	return (char *)St;
}

void wpis_uruchom(int ob_pocz, int ob_konc, char *D, int rozmiar_ob, char zapis[], char *Kod_op)
{
}

int dec_uruchom(int decyzja, int kod_decyzji, int nr_dec, int kod_raportu, int np, int *nr_rekordu)
{
	int ret;
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
		 case DEC_SCIEZKA: szukanie++;break;
         case DEC_ROZ: szukanie++;break;
         case DEC_DYSK:break;
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