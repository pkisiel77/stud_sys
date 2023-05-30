#include <process.h>
#include "time.h"
#include "ramkaa.h"
#include "blank/moje.h"
#include "blank/sys_dekl.h"
#include "pomiar.h"
#include "blank/wewybl.h"
//#include "blank/graph.c"
//#include "term_graf.h"

void chk_time(void);

int OOblicz_wartosci(char typ,float a,float b,float c,float *wektor_x,int *wektor_y);
void iinit_plot(int px,int py, struct plot *plt);
int CzasPobraniaDanych(int dec);

extern struct Service *Service;
extern struct agenda *Agenda;
extern struct _dane_pacjenta_ dane_pacjenta;

extern unsigned int attr_title;
extern unsigned int attr,at_wpis;
extern unsigned int cursor, nocursor;

extern int X_time, X_tyt, lwmall;

/* ********************************** */
#define IL_WSP 6000
#define L_PLOTS 10

#define POM_START 0
#define POM_STOP  1

struct plot
{int liczba_danych;
 float *Wek_X;
 int *Wek_Y;
} *pp=NULL;


FILE *p_plikk=NULL;
int ilosc_wspp=0;
int i_index;
int z;
float Wektor_X[IL_WSP];
int Wektor_Y[IL_WSP];
struct plot *Plotss[L_PLOTS]={NULL};

int nr_plotss=0;

/* ********************************** */
int l_algory=1;
static struct agenda **SA;
/* -------------------- kody decyzji w moje.h ------------
#define DEC_NEW    1
#define DEC_RUN    2
#define DEC_TYP_US 3
 --------------------------------------------------------- */
#define DEC_FORM   10
#define DEC_COMP   11
#define DEC_FUN    12

#define DEC_DANE   23
#define DEC_PLOT   22
#define DEC_LOAD   21
#define DEC_NON    20

#define DEC_BEEP      23
#define DEC_AMPLITUDA 24
#define DEC_CZAS      25
#define DEC_ALARM     26

// Funkcje i procedury dziêki którym dzia³ poprawnie us³uga POMIAR
int dane_agendy(struct agenda *A, struct agenda *An, int cykl_max);
int pobierz_rekord_uslugi(int *nr_rekordu, int kod_uslugi, int ob_konc,
									struct agenda **As, struct agenda *An,
									struct agenda ***SA, char *adres_rek0_uslugi, char *nazwa);
int pokaz_listy_zlecen(struct agenda *Anew, int kod_uslugi, char *tytul);
int decyzje_run(char decyzja, struct agenda **Aserv, struct agenda **Anew,
                  int *nr_rekordu, int kod_uslugi, char *tytul);
void ustaw_typ_uslugi(struct agenda *A, int decyzja);

// ...dodatkowe funkcje blankietu
int ZapiszDane(char *sciezka);

extern int lwmall;
#define LFORM 3
char *MenuPomiar[LFORM]={" ",
                         " Pomiar",
                         " Pomiar i wykres (samodzielne okno)"};
			//		     " Pomiar i wykres (wykres pod blankietem)"};
/* ======================================================== */
/* POMIAR MAIN */
extern unsigned int attryb;
int pomiar_main(void *DA)
 {struct agenda *A;
  struct pomiar*P;
  
  // zmienne pomocnicze
  int ret, ekg_max=10, ekg_min=-10;
  static int krok=-1;
  static float i=0.0;

	A=(struct agenda *)DA;
	P=(struct pomiar *)(A->data);

    if(P->typ_pom==POMIAR_DYNAMICZNY)
	{;}
    if(P->typ_pom==POMIAR_STATYCZNY)
	{
		//odczyt danych z tablicy pomiarowej wgranej w blankiecie
		krok++;
		if(P->usluga_pomiar==POMIAR) // wyœwietlanie danych A[V] i t[s]
		{ if(krok>=P->czas_pomiaru) krok=0;// koniec pomiaru
		   else
		   { A->dana=(float)P->tablica_pomiarowa[krok];
		     A->czas+=1.0;
			 if(A->dana<=A->wart_min) A->wart_min=A->dana;
             if(A->dana>=A->wart_max) A->wart_max=A->dana;
			 // obsluga b³êdów
			 // zapis do pliku ../tmp/raport.dat
			 // nr próbki i wartoœæ i nr pacjenta
			 // i Beep
			 // przekroczony zakres
			 if((A->dana>=ekg_max) || (A->dana<=ekg_min))
			 {
				 A->alarm=2;
				 Beep(100,100);
			 }
			 else
				 A->alarm=0;//powrót do normalnoœci
			 
		   }
		}
		if(P->usluga_pomiar==POMIAR_1) // ... wykres pod blankietem
		{
			//if init_graf
			//if(igraf!=0){;}
			//else{;} // nie jesteœ w trybie graficznym
		}
		if(P->usluga_pomiar==POMIAR_2) // ... wykres poza blankietem
		{
			//if(igraf!=0){;}
			//else{;} // nie jesteœ w trybie graficznym
		}
	}
	// czas_zegarowy(&tab[0][0],&tab[1][0],&tab[2][0]);
	// data_systemowa(&tab[3][0],&	tab[4][0],&	tab[5][0]);
	// wykonanie instrukcji
	//dana_koment(-1,-1,"Kisiel");
/*
	{static float i=0.0;
    A->dana=i++;
    Beep(100,100);   
	}
*/	
	// koniec wykonywania instrukcji
	// czas_zegarowy(&tab[0][1],&tab[1][1],&tab[2][1]);
	// data_systemowa(&tab[3][1],&	tab[4][1],&	tab[5][1]);
	A->state=0;
	return A->state;
 };
/* ======================================================== */
/* POMIAR BLANKIET */

int pomiar_blankiet(int nr_rekordu, int ob_pocz,
										int ob_konc, int x_lewy_gorny,
										int y_lewy_gorny, int kod_uslugi,
										char *adres_rek0_uslugi)
 {int ret, ochr=3, ochrf=-1, size, nr_rek, raport;
	int cykl_max=6000;
	struct pomiar*P;
    struct agenda *A=NULL;
    struct _dane_pacjenta_ dane_pacjenta;
	static int dysk=0;
	
// zmienne które powinne byc przekazywane do blankietu
	int pacjent_nr=1;//dane_pacjenta.pac_nr_ew;
	char *pacjent_imie="Jan";//dane_pacjenta.pac_imie;
	char *pacjent_nazwisko="Kowalski";//dane_pacjenta.pac_nazwisko;

	if(pacjent_nr<0) 
	{ MessageBox(NULL,"Uwaga wchodzisz bez podania pacjenta","Uwaga",MB_ICONWARNING|MB_OK);
	  pacjent_nr=0;
	  pacjent_imie="NULL";
	  pacjent_nazwisko="NULL";
	}

//    pacjent_nr=PobierzFix(PATH_FIX);

	nr_rek=nr_rekordu;
    
	ret=dana_koment(-1, -1, " Pacjent[%d]:%s %s",pacjent_nr,pacjent_imie,pacjent_nazwisko);
	ret=pobierz_rekord_uslugi(&nr_rek,kod_uslugi, ob_konc, &A, Anew, &SA,
														adres_rek0_uslugi, "zlecenia pomiaru");
  Aserv=A;
	if(A==NULL)
	 {if(nr_rek<0) return ret;
		ret=dana_koment(-1, -1, " Brak zlecenia oblicz. na pozycji %d w agendzie ",nr_rek);
		return ret;
	 }
  P=(struct pomiar*)(A->data);

	rekord_danych_do_naglowka(nr_rek);


	if(Anew==NULL)
	 {ret=dana_koment(-1,13,"+ Modyfikacja danych POMIAR juz zgloszonych ");}
	else
	 {ret=dana_koment(-1,-1,"+  ");}
	{static int nmin=0, nmax=LFORM-1;
	 if(Anew==NULL) ochr=-2; else ochr=4;
		ret=dana_int_menu_dec(-1,-1,"+ Zadanie pomiarowe (%d-%d)  ?? ", &nmin, &nmax,
								 &(P->nr_wzoru), size=1, ochr, raport=-1, LFORM, MenuPomiar,DEC_FORM);
		

	if(P->nr_wzoru==1) P->typ_pom=POMIAR_STATYCZNY;
    if(P->nr_wzoru==2) P->typ_pom=POMIAR_STATYCZNY;//{P->typ_pom=POMIAR_DYNAMICZNY;P->usluga_pomiar=POMIAR;}
    if(P->nr_wzoru==3) {P->typ_pom=POMIAR_DYNAMICZNY;P->usluga_pomiar=POMIAR_1;}

	if(P->nr_wzoru!=0) 
	{
      	// to co dotyczy wszystkie opcje
		ret=dana_koment(-1,-1,"+ -------------------------------------------------------------------------");      
		// 1. Ÿród³o danych
		{static char *menu_text[3]={"b brak","d dysk","k karta dzwiekowa"};
		 static char *zrodlo_danych='b';
         ret=dana_decyzyjna(-1,-1,"+ Zrodlo danych  <%s> ??", "b/d/k",menu_text, 3, &(zrodlo_danych),ochr, DEC_DANE);
		 P->zrodlo_danych=zrodlo_danych; 
		// 2. nazwa pliku do zapisu lub odczytu
		// dane statyczne czyli odczyt z pliku danego pacjenta
 	    if((P->zrodlo_danych=='d') && (dysk!=1))
		{ 
          static char *nazw_plik[100],*temp;
		  int opcje,i=0;
		  WIN32_FIND_DATA nazwa;
		  BOOL n_opcje;
		  // uruchomienie okna wykresu
		  if(P->nr_wzoru==2) WinExec("c:\\MenuDemo.exe",SW_SHOW);
     	  // ³adowanie plików z katalogu 22 do nazw_plik
		  opcje=FindFirstFile("c:\\pacjent\\dane\\1\\*.dat",&nazwa);
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
		   if(opcje!=-1)
		   {static char **help=NULL;
		    P->nazwa_pliku=nazw_plik[opcje];
            ret=dana_text(-1,-1,"+ Odczyt pliku:  ??",P->nazwa_pliku,12,help,0,ochr=-1);
			{ int retu,i=0,x;
			  static char *menu_czas_pom[2]={"Czas pomiaru",""};
			  char buf[128];
	          FILE *fp;
			  CzasPobraniaDanych(POM_START);
			  sprintf(buf,"c:\\pacjent\\dane\\1\\%s",nazw_plik[opcje]);
	          fp=fopen(buf,"r");
	           if(fp==NULL) {;}
	            else
				{
					do{ retu=fscanf(fp,"%d",&x);
			            P->tablica_pomiarowa[i]=(0.001*x)+1.3;
					    i++;
					}while(retu!=EOF);
					P->czas_pomiaru=i;
		           fclose(fp);
				}
			  retu=CzasPobraniaDanych(POM_STOP);              
			  sprintf(buf,"Czas pobrania=%d[s] | Czas max.=%d[s]",retu,A->Interval);
			  menu_czas_pom[1]=buf;
			  okno_menu(menu_czas_pom,2,0,attr,at_wpis,2,1,-1,NULL,1);
			}
		   }
		   dysk=1;
        } 
        if(P->zrodlo_danych=='k') 
		{ static char **help=NULL;
		  static char *MenuText[2]={"t tak","n nie"}; 
		  int i;
		  ret=dana_koment(-1,-1,"Pomiar dynamiczny");
		  MessageBox(NULL,"Brak karty dŸwiêkowej","B³¹d I/0",MB_ICONWARNING|MB_OK);
		  //ret=dana_text(-1,-1,"+ Zapis do pliku:  ??",P->nazwa_pliku,11,help,0,ochr);
		}
		}
    ret=dana_koment(-1,-1,"+ -------------------------------------------------------------------------");      
	 
	}	
    switch(P->nr_wzoru)
		{case 0: break;
/*********************************************************/
/* POMIAR */
/* Pobrane dane s¹ przedstawiane jako wartoœæ (A)mplituda i (c)zas*/
/* Pola: Amplituda[]; czas[]; */
		 case 1:
		 case 2:
      {static float wmin=0.0,wmax=5.0;// 0-5[V]
	   static float wart_min=0.0, wart_max=0.0;
	  	A->dana=0.0;
		A->wart_max=0.0;
		A->wart_min=wmax;
        ret=dana_float_dec(-1,-1," Amplituda <%.1f-%.1f> :", &wmin, &wmax, &(A->dana), 5, 3, ochr,0,DEC_AMPLITUDA);
		ret=dana_koment(-1,-1,"+  [V]");
		// max.
        ret=dana_float_dec(-1,-1,"+ Wartosc max. :", &wmin, &wmax, &(A->wart_max), 5, 3, ochr,0,DEC_AMPLITUDA);
		// min.
		ret=dana_float_dec(-1,-1,"+ Wartosc min. :", &wmin, &wmax, &(A->wart_min), 5, 3, ochr,0,DEC_AMPLITUDA);
      }
      {static float wmin = 0.0, wmax= 1000.0;// 0-60[s]
	    A->czas=0.0;
	    ret=dana_koment(-1,-1," [1p/100ms] <==> [1p/ %d s]",(A->Interval));
		ret=dana_koment(-1,-1,"+ Ilosc pobranych probek = %d ",(P->czas_pomiaru));
		ret=dana_float_dec(-1,-1," Probka nr : ", &wmin, &wmax, &(A->czas), 2, 2, ochr,0,DEC_CZAS);
		ret=dana_koment(-1,-1,"+  [%d s]",(A->Interval));
      }
	  { static char *menu_alarm[3]={"Wszystko w porzadku","Uwaga przekroczony czas pobierania danych <zobacz raport.dat>","Uwaga przekroczony zakres pomiaru <zobacz raport.dat>"}; 
	    static int w_min=0, w_max=3;
		A->alarm=0;
		ret=dana_int_menu(-1,-1," Alarm nr ",&w_min,&w_max,&(A->alarm),2,ochr,-1,3,menu_alarm);
	  }
        break;
		 default: break;
		}
	}
	ret=dana_koment(-1,-1,"+ -------------------------------------------------------------------------");
	return dane_agendy(A, Anew, cykl_max);
 }
/* --------------------------------------------------------------------- */
char *dane_pomiar(int ob_pocz, int ob_konc, int *rozmiar_ob)
 {static struct Service *S;
	static struct agenda AS, *A;
    struct pomiar POM, *P;
	int x,y;
  P=&POM; A=&AS; S=Service;
	A=(struct agenda *)Malloc(sizeof(struct agenda)+l_algory*sizeof(struct pomiar));
  A->S=S; A->data=(A+1);
  x=m_wherex(); y=m_wherey(); setcursor(nocursor);
	term_printf(Y_G0,X_tyt,attr_title,"%s",S->name);
	term_cur(y,x); setcursor(cursor);

	return (char *)A;
 }
/****************************************************************/
void wpis_pomiar(int ob_pocz, int ob_konc,
					    char *D, int rozmiar_ob, char zapis[], char *Kod_op)
  {struct agenda **AG=(struct agenda **)D, *Ag;
   struct pomiar*P=NULL;
   FILE *wsk_plik=NULL;
   char *plik="dane_obl.baz";
   char *plikb="dane_obl.dat";
   int nru;

   if(D==NULL)
    {term_printf(MY_MAX,X_L0,ATTR_A," Brak bazy"); GET_char(); return;}
   wsk_plik=fopen(plik,"w");
   if(wsk_plik==NULL)
    {term_printf(MY_MAX,X_L0,ATTR_A," Blad pliku"); GET_char(); return;}
   for(nru=0;nru<AG_SIZE; nru++)
    {Ag=AG[nru];
     if(Ag==NULL) continue;
     if((Ag->S)->wpis_rap_bl!=wpis_pomiar) continue;
     P=(struct pomiar*)(Ag->data);
     if(P==NULL) continue;
     if(P->dana_f[9]==888)
      {for(i_index=0;i_index<(IL_DAN-1);i_index++)
       {fprintf(wsk_plik,"%.2f\n",P->dana_f[i_index]);
       }
      }
    }
  fclose(wsk_plik);
  wsk_plik=fopen(plikb,"wb");
   if(wsk_plik==NULL)
    {term_printf(MY_MAX,X_L0,ATTR_A," Blad pliku"); GET_char(); return;}
   for(nru=0;nru<AG_SIZE; nru++)
    {Ag=AG[nru];
     if(Ag==NULL) continue;
     if((Ag->S)->wpis_rap_bl!=wpis_pomiar) continue;
     P=(struct pomiar*)(Ag->data);
     if(P==NULL) continue;
     fwrite(P,sizeof(struct pomiar),1,wsk_plik);
    }
   fclose(wsk_plik);
 
 }
/********************************************************/
int dec_pomiar(int decyzja, int kod_decyzji, int nr_dec,
              int kod_uslugi, int np, int *nr_rekordu)
 {struct agenda *A0, *A, **Ab;
	struct pomiar*P;
	int sek, min, godz, minB, dmin, minp, min_sum;
	int ag_no, p_min, p_max, ret, nast=0;
	 A0=(struct agenda *)czy_zdefiniowany(kod_uslugi, &p_min, &p_max, &ret);
	 A=(struct agenda *)ustal_adres_rek(kod_uslugi,*nr_rekordu);
	 P=(struct pomiar*)(A->data);
	 if(kod_decyzji==DEC_NEW && decyzja=='t')
		{if(Anew==NULL)
      {Anew=service_default(dec_pomiar);
       A=Anew;
       P=(struct pomiar*)(A->data); P->komenda[0]=EOS;
       P->funkcja='?';
       P->rys_i_licz='?';
       for(i_index=0;i_index<(IL_DAN-1);i_index++)
       {P->dana_f[i_index]=1;}
       (P->dana_f[9])=888;
       for(i_index=0;i_index<IL_WYN;i_index++)
       {P->wynik_f[i_index]=0;}
      }
		 if(Anew==NULL) return kod_uslugi;
		 A=Anew; P=(struct pomiar*)(A->data);
		 czas_zegarowy(&godz, &min, &sek); 	/* oddaje minute od godz.0.0 (stud_sys.c) */
		 Aserv=Anew;
		 return kod_uslugi;
		}
	 else
		{if(Anew!=NULL) A=Anew;
		 else A=Aserv; /* rekord zlecenia agendy */
		 if(A==NULL) return kod_uslugi;
		 P=(struct pomiar*)(A->data);
		}
   P=(struct pomiar*)(A->data);

	 if(kod_decyzji==DEC_DANE)
	 {P->zrodlo_danych='d';}

	 if(kod_decyzji==DEC_NON)
		{/*if(strlen(P->komenda)>0)
			{term_printf(MY_MAX, X_L0,ATTR_A,"Wykonanie komendy %s <t/Esc>",Ob->komenda);
			 if(GET_char()=='t') system(Ob->komenda);
			}*/
		 //Beep(100,100);
     return kod_uslugi;
    }


	 if(kod_decyzji==DEC_RUN)
    {ret=decyzje_run(decyzja, &A, &Anew, nr_rekordu,
                     kod_uslugi, " Lista zlecen obliczeniowych ");
     if(ret==-1) return -1;
    }
	 if(kod_decyzji==DEC_TYP_US) ustaw_typ_uslugi(A, decyzja);
   //if(kod_decyzji==DEC_FUN) return kod_uslugi;

/* ################################################################## */

   if(kod_decyzji==DEC_FUN)
    {switch(P->dzialanie)
     {case 's': P->wynik_f[0]=0;
                for(i_index=0;i_index<5;i_index++)
                 {(P->wynik_f[0])=(P->wynik_f[0])+(P->dana_f[i_index]);
                 }
                break;

     case 't':  if((P->dana_f[0])==0)
                {(P->wynik_f[0])=(-(P->dana_f[2]))/(P->dana_f[1]);
                }
                if((P->dana_f[0])!=0)
                 {(P->wynik_f[0])=(P->dana_f[1])*(P->dana_f[1])-(4*(P->dana_f[0])*(P->dana_f[2]));           ret=dana_koment(-1,-1,"+ Delta tr¢jmianu wynosi: %5.1f ", (P->wynik_f[0]));
                  if((P->wynik_f[0])<0)
                  {(P->wynik_f[1])=( -(P->dana_f[1]))/(2*(P->dana_f[0]));
                   (P->wynik_f[2])=(sqrt(-(P->wynik_f[0])))/(2*(P->dana_f[0]));
                  }
                 }
                if((P->wynik_f[0])>0)
                 {(P->wynik_f[1])=( -(P->dana_f[1])-(sqrt(P->wynik_f[0])))/(2*(P->dana_f[0]));
                  (P->wynik_f[2])=( -(P->dana_f[1])+(sqrt(P->wynik_f[0])))/(2*(P->dana_f[0]));
                 }
                if((P->wynik_f[0])==0)
                 {(P->wynik_f[1])=( -(P->dana_f[1]))/(2*(P->dana_f[0]));
                 }
                break;

    }

     if(P->rys_i_licz=='l')
      {ilosc_wspp=OOblicz_wartosci(P->funkcja,P->dana_f[0],P->dana_f[1],P->dana_f[2],Wektor_X, Wektor_Y);
      }
     if((P->rys_i_licz=='r')||(P->rys_i_licz=='t'))
      {kod_decyzji=DEC_PLOT;}
  }


   if(kod_decyzji==DEC_LOAD)
    {if(P->odczyt=='t')
     {p_plikk=fopen("dane_obl.baz","r");
      if(p_plikk!=NULL)
       {for(i_index=0;i_index<IL_DAN-1;i_index++)
        {fscanf(p_plikk,"%f",&(P->dana_f[i_index]));
        }
        fclose(p_plikk);
       }
      else
      {int yp, xp, ret;
		   char text[200];
       yp=MY_MAX; xp=X_L0+1;
	 	   sprintf(text," Proba odczytu nieudana !!! ");
		   ret=monit_textowy(yp, xp, ATTR_A, text);
      }
     }
    (P->odczyt)='n';
    }

   if(kod_decyzji==DEC_NON) {;}


	 return kod_uslugi;
  }

/*########################################################################*/
extern int if_graf, YZ_max_text, Yz_max_graf;

int OOblicz_wartosci(char typ,float a,float b,float c,float *wektor_x,int *wektor_y)
{float x;
 int y,ilosc_wspol=0;
 double (* funkcja)(double);

    for(i_index=0;i_index<IL_WSP;i_index++)
     {*(wektor_x+i_index)=0;
      *(wektor_y+i_index)=0;
     }

    switch(typ)
     {case 's':funkcja=sin; break;
      case 'c':funkcja=cos; break;
      default: break;
     }
    if(typ=='s'||typ=='c')
     {ilosc_wspol=0;
      for(x=-ZAK_XX;x<=ZAK_XX;x+=PREC_W)
       {zm_x = (x/SKOK_XX)*(PI/2);
        y=ROUND( a * SKOK_YY * funkcja ( c * (float)(zm_x) - b));
        *(wektor_x+ilosc_wspol)=ROUND(x);
        *(wektor_y+ilosc_wspol)=y;
        ilosc_wspol++;
       }
     }
    if(typ=='t')
      {ilosc_wspol=0;
       for(x=-ZAK_XX;x<=ZAK_XX;x+=PREC_W)
	    	{zm_x = (x/SKOK_XX)*(PI/2);
	       y=ROUND( a * SKOK_YY * tan ( c * ((float)(zm_x)) - b));
         if(abs((int)y)<ABSS)
          {*(wektor_x+ilosc_wspol)=ROUND(x);
           *(wektor_y+ilosc_wspol)=y;
           ilosc_wspol++;
          }
	    	}
      }
     if(typ=='k')
      {ilosc_wspol=0;
       for(x=-ZAK_XX;x<=ZAK_XX;x+=PREC_W)
	    	{zm_x = (x/SKOK_XX)*(PI/2);
	       y= - ROUND( a * SKOK_YY * tan ( c *((float)(zm_x))-b+(SKOK_XX*(PI/2))));
         if(abs((int)y)<ABSS)
          {*(wektor_x+ilosc_wspol)=ROUND(x);
           *(wektor_y+ilosc_wspol)=y;
           ilosc_wspol++;
          }
        }
      }
     if(typ=='d')
      {ilosc_wspol=0;
       for(x=ZAK_XX;x>0;x-=PREC_W)
	      {zm_x = x/SKOK_XX;
	       y = ROUND( a * SKOK_YY * log10 ( b * (float)zm_x) + (c * SKOK_YY));
         if(abs((int)y)<ABSS)
          {*(wektor_x+ilosc_wspol)=ROUND(x);
           *(wektor_y+ilosc_wspol)=y;
           ilosc_wspol++;
          }
        }
      }

     if(typ=='n')
      {
       for(x=ZAK_XX;x>0;x-=PREC_W)
	      {zm_x = x/SKOK_XX;
	       y = ROUND( a * SKOK_YY * log ( b * SKOK_YY * (float)zm_x) + (c * SKOK_YY));
         if(abs((int)y)<ABSS)
          {*(wektor_x+ilosc_wspol)=ROUND(x);
           *(wektor_y+ilosc_wspol)=y;
           ilosc_wspol++;
          }
        }
      }

     if(typ=='f')
      {
       for(x=-ZAK_XX;x<=ZAK_XX;x+=PREC_W)
		    {zm_x = x/SKOK_XX;
	       y=  ROUND( a*SKOK_YY * (pow(zm_x,2)) + (b * SKOK_YY * zm_x) + (c * SKOK_YY));
         if(abs((int)y)<ABSS)
          {*(wektor_x+ilosc_wspol)=ROUND(x);
           *(wektor_y+ilosc_wspol)=y;
           ilosc_wspol++;
          }
        }
      }
return ilosc_wspol;
}

/*#######################################################################*/

void iinit_plot(int px,int py, struct plot *plt)
{float x;
 int y, old_graf=0, j;

  old_graf=if_graf;
  if(if_graf==0)
   {
    MY_MAX=Yz_max_graf; MYR_MAX=MY_MAX-1;
		if_graf=otworz_graf_blank(0,0.4,-1,-1,Yz_max_graf,TERM_WHITE|TERM_BLACK_BG);
	 }
/* rysowanie ramki i osi wykresu */

#ifdef EEEE
    setfillstyle(SOLID_FILL,KOL_TLA);
    bar(px+1,py,px+MAX_X,py+MAX_Y);
	  setcolor(TERM_WHITE);
	  rectangle(px+TAB_R,py+TAB_R,px+KON_RAM_X,py+KON_RAM_Y); /* wymiary ramki biaˆej - cieä */
	  setcolor(TERM_BLACK);
	  line(px+TAB_R,py+TAB_R,px+KON_RAM_X,py+TAB_R); /* cieä obramowania czarny x */
	  line(px+TAB_R,py+TAB_R,px+TAB_R,py+KON_RAM_Y); /* cieä obramowania czarny y */
	  setcolor(TERM_BLACK);

	  line(px+OS_POCZ,py+Y_HALF,px+OS_X_KON,py+Y_HALF);  /* rysowanie osi x */
	  line(px+GROT_X,py+GROT_XG,px+OS_X_KON,py+Y_HALF);  /* rysowanie strzaˆki x */
	  line(px+GROT_X,py+GROT_XD,px+OS_X_KON,py+Y_HALF);
	  line(px+X_HALF,py+OS_POCZ,px+X_HALF,py+OS_Y_KON);  /* rysowanie osi y */
	  line(px+GROT_YL,py+GROT_Y,px+X_HALF,py+OS_POCZ);   /* rysowanie strzaˆki y */
	  line(px+GROT_YP,py+GROT_Y,px+X_HALF,py+OS_POCZ);

	  for(x=OS_POCZ;x<X_SKAL;x+=SKOK_X)   /* podziaˆka */
		{
		  line(px+x,py+Y_HALF-PODZ,px+x,py+Y_HALF+PODZ);
		}
	  for(y=OS_Y_KON;y>OS_POCZ;y-=SKOK_Y)
		{
		  line(px+X_HALF-PODZ,py+y,px+X_HALF+PODZ,py+y);
		}

    for(j=0;j<(plt->liczba_danych);j++)  /* rysowanie wykresu */
     {putpixel(px + ( *( (plt->Wek_X)+j ) ) + X_HALF,py - ( *( (plt->Wek_Y)+j ) ) + Y_HALF,KOL_WYK);
     }

#endif
    GET_char();
    if(old_graf==0)
     {if_graf=Endgraph(); /* ------ przechodzimy na tekstowy ---- */
		  MY_MAX=YZ_max_text; MYR_MAX=MY_MAX-1;
	   }


}

/*
int PobierzFix(char *sciezka)
{
	int dana=-1;
	FILE *fp;
	fp=fopen(sciezka,"r");
	if(fp==NULL) MessageBox(NULL,"Fix!!!!","B³¹d",MB_OK);
	 else
	 { fscanf(fp,"%d",&dana);
	   fclose(fp);
	 }
	 return dana;
}
*/

int CzasPobraniaDanych(int dec)
{
	static int tab[3];
	int i=0;
	if(dec==POM_START) czas_zegarowy(&tab[0],&tab[1],&tab[2]);
	 else
	 { czas_zegarowy(&tab[0],&tab[1],&i);
	   return i-tab[2];
	 }
	 return -1;
}	   
