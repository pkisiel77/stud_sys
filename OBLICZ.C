#include <process.h>
#include "time.h"
#include "ramka.h"
#include "../blank/moje.h"
#include "../blank/sys_dekl.h"
#include "dekl_obl.h"
#include "../blank/wewybl.h"
//#include "graphics.h"
void chk_time(void);

int Oblicz_wartosci(char typ,float a,float b,float c,float *wektor_x,int *wektor_y);
void init_plot(int px,int py, struct plot *plt);

extern struct Service *Service;
extern struct agenda *Agenda;
extern unsigned int attr_title;
extern unsigned int attr,at_wpis;
extern unsigned int cursor, nocursor;
extern int X_time, X_tyt, lwmall;

/* ********************************** */
#define IL_WSP 6000
#define L_PLOTS 10

struct plot
{int liczba_danych;
 float *Wek_X;
 int *Wek_Y;
} *p=NULL;


FILE *p_plik=NULL;
int ilosc_wsp=0;
int i_index;
int z;
float Wektor_X[IL_WSP];
int Wektor_Y[IL_WSP];
struct plot *Plots[L_PLOTS]={NULL};
int nr_plots=0;

/* ********************************** */
int l_algor=1;
static struct agenda **SA;
/* -------------------- kody decyzji w moje.h ------------
#define DEC_NEW    1
#define DEC_RUN    2
#define DEC_TYP_US 3
 --------------------------------------------------------- */
#define DEC_FORM   10
#define DEC_COMP   11

#define DEC_BEEP   13

#define DEC_FUN    12
#define DEC_PLOT   22
#define DEC_LOAD   21
#define DEC_NON    20

int dane_agendy(struct agenda *A, struct agenda *An, int cykl_max);
int pobierz_rekord_uslugi(int *nr_rekordu, int kod_uslugi, int ob_konc,
									struct agenda **As, struct agenda *An,
									struct agenda ***SA, char *adres_rek0_uslugi, char *nazwa);
int pokaz_listy_zlecen(struct agenda *Anew, int kod_uslugi, char *tytul);
int decyzje_run(char decyzja, struct agenda **Aserv, struct agenda **Anew,
                  int *nr_rekordu, int kod_uslugi, char *tytul);
void ustaw_typ_uslugi(struct agenda *A, int decyzja);
extern int lwmall;
#define LFORM 6
char *MenuObl[LFORM]={" ",
                      " Funkcje trygonometryczne",
                      " Suma",
									    " Logarytm",
                      " Rownanie kwadratowe",
                      " Komendy systemu operacyjnego "
								     };
/* ======================================================== */
extern unsigned int attryb;
int obl_main(void *DA)
 {struct agenda *A;
  struct oblicz *Ob;
	A=(struct agenda *)DA;
	Ob=(struct oblicz *)(A->data);

 	if(strlen(Ob->komenda)>0)
	 {int yp, xp, ret;
		char text[200];
		yp=MY_MAX; xp=X_L0+1;
		sprintf(text," Wykonac komende: %s <t/Esc> ?????? ",Ob->komenda);
		ret=monit_textowy(yp, xp, ATTR_A, text);
		if(ret=='t') system(Ob->komenda);
	 }

  if((Ob->rys_i_licz=='t')&&(Plots[0]!=NULL))
   {
    int yp, xp, ret;
		char text[200];
		yp=MY_MAX; xp=X_L0+1;
		sprintf(text," Wykonac drukowanie wykresu <t/Esc> ??? ");
		ret=monit_textowy(yp, xp, ATTR_A, text);
		if(ret=='t') init_plot(WSP_X, WSP_Y, Plots[0]);
	 }

  

	A->state=0;
	return A->state;
 };

int obl_blankiet(int nr_rekordu, int ob_pocz,
										int ob_konc, int x_lewy_gorny,
										int y_lewy_gorny, int kod_uslugi,
										char *adres_rek0_uslugi)
 {int ret, ochr=3, ochrf=-1, size, nr_rek, raport;
	int cykl_max=6000;
	struct oblicz *Ob;
  struct agenda *A=NULL;
/* sledzenie(); */
	nr_rek=nr_rekordu;
	ret=pobierz_rekord_uslugi(&nr_rek,kod_uslugi, ob_konc, &A, Anew, &SA,
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
/*********************************************************/
/* FUNKCJE TRYGONOMETRYCZNE */
		 case 1:
       {static char *Fun[]={"s Sinus","c Cosinus","t Tangens","k Cotangens","? WYBIERZ FUNKCJE"};/*break;*/
        ret=dana_decyzyjna(-1,-1,"+ Wybierz funkcje <%s> ??", "s/c/t/k",Fun, 5, &(Ob->funkcja),
                            ochr=1, DEC_NON);
       }
       ret=dana_koment(-1,-1,"+ ");
       ret=dana_koment(-1,-1,"+ Dane obliczeniowe :");
       {static float wmin=0.5,wmax=4.0;
        ret=dana_float(-1,-1,"+ Amplituda <%3.2f-%1.0f> :", &wmin, &wmax, &(Ob->dana_f[0]), 3, 1, ochr,-1);
       }
	     {static float wmin = -10, wmax= 10.0;
		    ret=dana_float(-1,-1,"+ Przesuniecie fi <%3.2f-%1.0f> :", &wmin, &wmax, &(Ob->dana_f[1]), 5, 1, ochr,-1);
       }
	     {static float wmin=0.5,wmax=3.0;
		    ret=dana_float(-1,-1,"+ Okres <%3.2f-%1.0f> :", &wmin, &wmax, &(Ob->dana_f[2]), 3, 1, ochr,-1);
       }
       ret=dana_koment(-1,-1,"+ ");
       {static char *Fun[]={"l Policz wartosci","r Policz wartosci i narysuj","t Rysuj po czasie","? CO ZROBIC"};
        if(((Ob->funkcja)!='s')&&((Ob->funkcja)!='c')&&((Ob->funkcja)!='t')&&((Ob->funkcja)!='k')&&((Ob->funkcja)!='o'))
         {ochr=-1;}
        else{ochr=1;};
        ret=dana_decyzyjna(-1,-1,"+ Operacja  <%s> ??", "l/r/t",Fun, 4, &(Ob->rys_i_licz),
                            ochr, DEC_FUN);
       }
       {static char *uruch[]={"t tak","n nie"};
        ret=dana_decyzyjna(-1,-1," Czy odczytac dane <%s>","t/n", uruch, 2, &(Ob->odczyt),
                           ochr, DEC_LOAD);
       }
        ret=dana_koment(-1,-1,"+ ");
        ret=dana_koment(-1,-1,"+ ");
        break;
/***********************************************************/
/* SUMA */
		 case 2:
 			  ret=dana_koment(-1,-1,"+ ");
        ret=dana_koment(-1,-1,"+ Dane obliczeniowe :");
        {
         for(i_index=0;i_index<5;i_index++)
          {
           static float wmin=-1000,wmax=1000;
		       ret=dana_float(-1,-1,"+ Skladnik sumy  ", &wmin, &wmax, &(Ob->dana_f[i_index]), 3, 1, 5,-1);
          }
        }
        {static char *uruch[]={"t tak","n nie"};
         ret=dana_decyzyjna(-1,-1," Podac wynik <%s>","t/n", uruch, 2, &(Ob->opcja),
                            ochr, DEC_FUN);
         (Ob->dzialanie)='s';
         if((Ob->opcja)=='t')
          {
           ret=dana_koment(-1,-1,"+ ");
           ret=dana_koment(-1,-1,"+ ");
           ret=dana_koment(-1,-1,"+ Wynik :      %5.1f",(Ob->wynik_f[0]));
           (Ob->opcja)='n';
          }
        }
        ret=dana_koment(-1,-1,"+ ");
        {static char *uruch[]={"t tak","n nie"};
         ret=dana_decyzyjna(-1,-1," Czy odczytac dane <%s>","t/n", uruch, 2, &(Ob->odczyt),
                           ochr, DEC_LOAD);
        }
        ret=dana_koment(-1,-1,"+ ");
        ret=dana_koment(-1,-1,"+ ");
        break;
/********************************************************/
/* LOGARYTM */
     case 3:
		   {static char *Fun[]={"d  dziesietny","n  naturalny","? WYBIERZ FUNKCJE"};
        ret=dana_decyzyjna(-1,-1," Wybierz funkcje <%s> ??", "d/n",Fun, 3, &(Ob->funkcja),
                            ochr=1, DEC_NON);
       }
       ret=dana_koment(-1,-1,"+ ");
       if(Ob->funkcja=='n')
        {ret=dana_koment(-1,-1,"+ Dane obliczen logarytmu : y = a*ln(bx)+c ");};
       if(Ob->funkcja=='d')
        {ret=dana_koment(-1,-1,"+ Dane obliczen logarytmu : y = a*log10(bx)+c ");};
       ret=dana_koment(-1,-1,"+ ");
       {static float wmin=-2.0,wmax=5;
        ret=dana_float(-1,-1,"+ Watrosc a=(%3.2f-%1.0f) " ,&wmin ,&wmax, &(Ob->dana_f[0]),3,1,5,-1);
       }
       {static float wmin=0.5,wmax=5;
        ret=dana_float(-1,-1,"+ Wartosc b=(%3.2f-%1.0f) " ,&wmin ,&wmax, &(Ob->dana_f[1]),3,1,5,-1);
       }
       {static float wmin=-2.0,wmax=5;
        ret=dana_float(-1,-1,"+ Wartosc c=(%3.2f-%1.0f) ",&wmin ,&wmax, &(Ob->dana_f[2]),3,1,5,-1);
       }
       ret=dana_koment(-1,-1,"+ ");
       {static char *Fun[]={"l Policz wartosci","r Policz wartosci i narysuj","t Rysuj po czasie","? CO ZROBIC"};
        if(((Ob->funkcja)!='n')&&((Ob->funkcja)!='d'))
         {ochr=-1;}
        else{ochr=1;};
        ret=dana_decyzyjna(-1,-1,"+ Wybierz opcje  <%s> ??", "l/r/t",Fun, 4, &(Ob->rys_i_licz),
                            ochr, DEC_FUN);
       }
       {static char *uruch[]={"t tak","n nie"};
        ret=dana_decyzyjna(-1,-1," Czy odczytac dane <%s>","t/n", uruch, 2, &(Ob->odczyt),
                           ochr, DEC_LOAD);
        }
       ret=dana_koment(-1,-1,"+ ");
       ret=dana_koment(-1,-1,"+ ");
       break;
/*******************************************************************/
/* TR�JMIAN KWADRATOWY */
		 case 4:
       ret=dana_koment(-1,-1,"+ Obliczanie pierwiastkow trojmianu:   y = a*x^2 + b*x + c" );
       {static float wmin=-100,wmax=100;
				ret=dana_float(-1,-1,"+ Watrosc a (%.0f-%.0f) " ,&wmin ,&wmax, &(Ob->dana_f[0]),5,1,5,-1);
       }
       {static float wmin=-100,wmax=100;
				ret=dana_float(-1,-1,"+ Watrosc b (%.0f-%.0f) " ,&wmin ,&wmax, &(Ob->dana_f[1]),5,1,5,-1);
       }
       {static float wmin=-100,wmax=100;
				ret=dana_float(-1,-1,"+ Watrosc c (%.0f-%.0f) " ,&wmin ,&wmax, &(Ob->dana_f[2]),5,1,5,-1);
       }

        (Ob->funkcja)='f';
        (Ob->dzialanie)='t';
        if((Ob->dana_f[0])==0)
         {ret=dana_koment(-1,-1,"+ Je�li wspolczynnik \"a\" rowny jest zero - funkcja nie jest kwadratowa ");
          ret=dana_koment(-1,-1,"+ Miejsce przeciecia z osia x: %5.2f",(Ob->wynik_f[0]));
         }
        if((Ob->dana_f[0])!=0)
         {ret=dana_koment(-1,-1,"+ Dlta trojmianu wynosi: %5.1f",(Ob->wynik_f[0]));
          if((Ob->wynik_f[0])<0)
           {
            ret=dana_koment(-1,-1,"+ Rownanie to ma dwa pierwiastki zespolone X1 i X2: ");
            ret=dana_koment(-1,-1,"+ X1 wynosi : %5.1f - (%1.1f) j ",(Ob->wynik_f[1]),(Ob->wynik_f[2]));
            ret=dana_koment(-1,-1,"+ X2 wynosi : %5.1f + (%1.1f) j ",(Ob->wynik_f[1]),(Ob->wynik_f[2]));
           }
          if((Ob->wynik_f[0])>0)
           {
            ret=dana_koment(-1,-1,"+ Rownanie to ma dwa pierwiastki rzeczywiste X1 i X2: ");
            ret=dana_koment(-1,-1,"+ X1 wynosi : %5.1f ",(Ob->wynik_f[1]));
            ret=dana_koment(-1,-1,"+ X2 wynosi : %5.1f ",(Ob->wynik_f[2]));
           }
          if((Ob->wynik_f[0])==0)
           {
            ret=dana_koment(-1,-1,"+ Rownanie to ma jeden pierwiastek rzeczywisty X0: ");
            ret=dana_koment(-1,-1,"+ X0 wynosi : %5.1f ",(Ob->wynik_f[1]));
           }
         }
       ret=dana_koment(-1,-1,"+ ");
       {static char *Fun[]={"l Policz wartosci","r Policz wartosci i narysuj","t Rysuj po czasie","? CO ZROBIC"};
        ret=dana_decyzyjna(-1,-1,"+ Wybierz opcje  <%s> ??", "l/r/t",Fun, 4, &(Ob->rys_i_licz),
                            ochr, DEC_FUN);
       }
        {static char *uruch[]={"t tak","n nie"};
         ret=dana_decyzyjna(-1,-1," Czy odczytac dane <%s>","t/n", uruch, 2, &(Ob->odczyt),
                           ochr, DEC_LOAD);
        }
        ret=dana_koment(-1,-1,"+ ");
        break;
/************************************************************************/
/* KOMENDY SYSTEMU OPERACYJNEGO */
     case 5:
			 {static char *MenuComp[3]={"copy oblicz.c a:\\zbior.c", "Visual C", "WATCOM C"};
        ret=dana_text_dec(-1,-1,"+ Wpisz komende:  ?? ", Ob->komenda, size=40,
													 MenuComp, 3, ochr=1, DEC_COMP);
       }
       break;
		 default: break;
		}
	}
	return dane_agendy(A, Anew, cykl_max);
 }
/* --------------------------------------------------------------------- */

char *dane_obl(int ob_pocz, int ob_konc, int *rozmiar_ob)
 {static struct Service *S;
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
 }
/****************************************************************/
void wpis_obl(int ob_pocz, int ob_konc,
					    char *D, int rozmiar_ob, char zapis[], char *Kod_op)
  {struct agenda **AG=(struct agenda **)D, *Ag;
   struct oblicz *Ob=NULL;
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
     if((Ag->S)->wpis_rap_bl!=wpis_obl) continue;
     Ob=(struct oblicz *)(Ag->data);
     if(Ob==NULL) continue;
     if(Ob->dana_f[9]==888)
      {for(i_index=0;i_index<(IL_DAN-1);i_index++)
       {fprintf(wsk_plik,"%.2f\n",Ob->dana_f[i_index]);
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
     if((Ag->S)->wpis_rap_bl!=wpis_obl) continue;
     Ob=(struct oblicz *)(Ag->data);
     if(Ob==NULL) continue;
     fwrite(Ob,sizeof(struct oblicz),1,wsk_plik);
    }
   fclose(wsk_plik);
 }
/********************************************************/
int dec_obl(int decyzja, int kod_decyzji, int nr_dec,
              int kod_uslugi, int np, int *nr_rekordu)
 {struct agenda *A0, *A, **Ab;
	struct oblicz *Ob;
	int sek, min, godz, minB, dmin, minp, min_sum;
	int ag_no, p_min, p_max, ret, nast=0;
	 A0=(struct agenda *)czy_zdefiniowany(kod_uslugi, &p_min, &p_max, &ret);
	 A=(struct agenda *)ustal_adres_rek(kod_uslugi,*nr_rekordu);
	 Ob=(struct oblicz *)(A->data);
	 if(kod_decyzji==DEC_NEW && decyzja=='t')
		{if(Anew==NULL)
      {Anew=service_default(dec_obl);
       A=Anew;
       Ob=(struct oblicz *)(A->data); Ob->komenda[0]=EOS;
       Ob->funkcja='?';
       Ob->rys_i_licz='?';
       for(i_index=0;i_index<(IL_DAN-1);i_index++)
       {Ob->dana_f[i_index]=1;}
       (Ob->dana_f[9])=888;
       for(i_index=0;i_index<IL_WYN;i_index++)
       {Ob->wynik_f[i_index]=0;}
      }
		 if(Anew==NULL) return kod_uslugi;
		 A=Anew; Ob=(struct oblicz *)(A->data);
		 czas_zegarowy(&godz, &min, &sek); 	/* oddaje minute od godz.0.0 (stud_sys.c) */
		 Aserv=Anew;
		 return kod_uslugi;
		}
	 else
		{if(Anew!=NULL) A=Anew;
		 else A=Aserv; /* rekord zlecenia agendy */
		 if(A==NULL) return kod_uslugi;
		 Ob=(struct oblicz *)(A->data);
		}
   Ob=(struct oblicz *)(A->data);

	 if(kod_decyzji==DEC_COMP)
		{/*if(strlen(Ob->komenda)>0)
			{term_printf(MY_MAX, X_L0,ATTR_A,"Wykonanie komendy %s <t/Esc>",Ob->komenda);
			 if(GET_char()=='t') system(Ob->komenda);
			}*/
     return kod_uslugi;
    }

	 if(kod_decyzji==DEC_RUN)
    {ret=decyzje_run(decyzja, &A, &Anew, nr_rekordu,
                     kod_uslugi, " Lista zlecen obliczeniowych ");
     if(ret==-1) return -1;
    }
	 if(kod_decyzji==DEC_TYP_US) ustaw_typ_uslugi(A, decyzja);
   if(kod_decyzji==DEC_FUN) {;}

/* ################################################################## */

   if(kod_decyzji==DEC_FUN)
    {switch(Ob->dzialanie)
     {case 's': Ob->wynik_f[0]=0;
                for(i_index=0;i_index<5;i_index++)
                 {(Ob->wynik_f[0])=(Ob->wynik_f[0])+(Ob->dana_f[i_index]);
                 }
                break;

     case 't':  if((Ob->dana_f[0])==0)
                {(Ob->wynik_f[0])=(-(Ob->dana_f[2]))/(Ob->dana_f[1]);
                }
                if((Ob->dana_f[0])!=0)
                 {(Ob->wynik_f[0])=(Ob->dana_f[1])*(Ob->dana_f[1])-(4*(Ob->dana_f[0])*(Ob->dana_f[2]));           ret=dana_koment(-1,-1,"+ Delta tr�jmianu wynosi: %5.1f ", (Ob->wynik_f[0]));
                  if((Ob->wynik_f[0])<0)
                  {(Ob->wynik_f[1])=( -(Ob->dana_f[1]))/(2*(Ob->dana_f[0]));
                   (Ob->wynik_f[2])=(sqrt(-(Ob->wynik_f[0])))/(2*(Ob->dana_f[0]));
                  }
                 }
                if((Ob->wynik_f[0])>0)
                 {(Ob->wynik_f[1])=( -(Ob->dana_f[1])-(sqrt(Ob->wynik_f[0])))/(2*(Ob->dana_f[0]));
                  (Ob->wynik_f[2])=( -(Ob->dana_f[1])+(sqrt(Ob->wynik_f[0])))/(2*(Ob->dana_f[0]));
                 }
                if((Ob->wynik_f[0])==0)
                 {(Ob->wynik_f[1])=( -(Ob->dana_f[1]))/(2*(Ob->dana_f[0]));
                 }
                break;

    }

     if(Ob->rys_i_licz=='l')
      {ilosc_wsp=Oblicz_wartosci(Ob->funkcja,Ob->dana_f[0],Ob->dana_f[1],Ob->dana_f[2],Wektor_X, Wektor_Y);
      }
     if((Ob->rys_i_licz=='r')||(Ob->rys_i_licz=='t'))
      {kod_decyzji=DEC_PLOT;}
  }

   if(kod_decyzji==DEC_PLOT)
    {if((Ob->rys_i_licz=='r')||(Ob->rys_i_licz=='t'))
      {ilosc_wsp=Oblicz_wartosci(Ob->funkcja,Ob->dana_f[0],Ob->dana_f[1],Ob->dana_f[2],Wektor_X, Wektor_Y);
       p=Malloc(sizeof(struct plot));
       if(p!=NULL)
        {for(i_index=0;i_index<L_PLOTS;i_index++)
         {if(Plots[i_index]==NULL) {nr_plots=i_index; Plots[i_index]=p; Plots[i_index+1]=NULL; break;};
         }
         p->Wek_X=Wektor_X;
         p->Wek_Y=Wektor_Y;
         p->liczba_danych=ilosc_wsp;
        }
       (Ob->opcja)='n';
      }
    if((Ob->rys_i_licz=='r')&&(Plots[0]!=NULL))
     {init_plot(WSP_X, WSP_Y, Plots[0]);
     }
    }

   if(kod_decyzji==DEC_LOAD)
    {if(Ob->odczyt=='t')
     {p_plik=fopen("dane_obl.baz","r");
      if(p_plik!=NULL)
       {for(i_index=0;i_index<IL_DAN-1;i_index++)
        {fscanf(p_plik,"%f",&(Ob->dana_f[i_index]));
        }
        fclose(p_plik);
       }
      else
      {int yp, xp, ret;
		   char text[200];
       yp=MY_MAX; xp=X_L0+1;
	 	   sprintf(text," Proba odczytu nieudana !!! ");
		   ret=monit_textowy(yp, xp, ATTR_A, text);
      }
     }
    (Ob->odczyt)='n';
    }

   if(kod_decyzji==DEC_NON) {;}

	 return kod_uslugi;
  }
/*########################################################################*/
extern int if_graf, YZ_max_text, Yz_max_graf;

int Oblicz_wartosci(char typ,float a,float b,float c,float *wektor_x,int *wektor_y)
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
      for(x=-ZAK_X;x<=ZAK_X;x+=PREC_W)
       {zm_x = (x/SKOK_X)*(PI/2);
        y=ROUND( a * SKOK_Y * funkcja ( c * (float)(zm_x) - b));
        *(wektor_x+ilosc_wspol)=ROUND(x);
        *(wektor_y+ilosc_wspol)=y;
        ilosc_wspol++;
       }
     }
    if(typ=='t')
      {ilosc_wspol=0;
       for(x=-ZAK_X;x<=ZAK_X;x+=PREC_W)
	    	{zm_x = (x/SKOK_X)*(PI/2);
	       y=ROUND( a * SKOK_Y * tan ( c * ((float)(zm_x)) - b));
         if(abs((int)y)<ABS)
          {*(wektor_x+ilosc_wspol)=ROUND(x);
           *(wektor_y+ilosc_wspol)=y;
           ilosc_wspol++;
          }
	    	}
      }
     if(typ=='k')
      {ilosc_wspol=0;
       for(x=-ZAK_X;x<=ZAK_X;x+=PREC_W)
	    	{zm_x = (x/SKOK_X)*(PI/2);
	       y= - ROUND( a * SKOK_Y * tan ( c *((float)(zm_x))-b+(SKOK_X*(PI/2))));
         if(abs((int)y)<ABS)
          {*(wektor_x+ilosc_wspol)=ROUND(x);
           *(wektor_y+ilosc_wspol)=y;
           ilosc_wspol++;
          }
        }
      }
     if(typ=='d')
      {ilosc_wspol=0;
       for(x=ZAK_X;x>0;x-=PREC_W)
	      {zm_x = x/SKOK_X;
	       y = ROUND( a * SKOK_Y * log10 ( b * (float)zm_x) + (c * SKOK_Y));
         if(abs((int)y)<ABS)
          {*(wektor_x+ilosc_wspol)=ROUND(x);
           *(wektor_y+ilosc_wspol)=y;
           ilosc_wspol++;
          }
        }
      }

     if(typ=='n')
      {
       for(x=ZAK_X;x>0;x-=PREC_W)
	      {zm_x = x/SKOK_X;
	       y = ROUND( a * SKOK_Y * log ( b * SKOK_Y * (float)zm_x) + (c * SKOK_Y));
         if(abs((int)y)<ABS)
          {*(wektor_x+ilosc_wspol)=ROUND(x);
           *(wektor_y+ilosc_wspol)=y;
           ilosc_wspol++;
          }
        }
      }

     if(typ=='f')
      {
       for(x=-ZAK_X;x<=ZAK_X;x+=PREC_W)
		    {zm_x = x/SKOK_X;
	       y=  ROUND( a*SKOK_Y * (pow(zm_x,2)) + (b * SKOK_Y * zm_x) + (c * SKOK_Y));
         if(abs((int)y)<ABS)
          {*(wektor_x+ilosc_wspol)=ROUND(x);
           *(wektor_y+ilosc_wspol)=y;
           ilosc_wspol++;
          }
        }
      }
return ilosc_wspol;
}

/*#######################################################################*/

void init_plot(int px,int py, struct plot *plt)
{float x;
 int y, old_graf=0, j;

  old_graf=if_graf;
  if(if_graf==0)
   {
    MY_MAX=Yz_max_graf; MYR_MAX=MY_MAX-1;
		if_graf=otworz_graf_blank(0,0.4,-1,-1,Yz_max_graf,TERM_WHITE|TERM_BLACK_BG);
	 }
/* rysowanie ramki i osi wykresu */
#ifdef EEE
    setfillstyle(SOLID_FILL,KOL_TLA);
    bar(px+1,py,px+MAX_X,py+MAX_Y);
	  setcolor(WHITE);
	  rectangle(px+TAB_R,py+TAB_R,px+KON_RAM_X,py+KON_RAM_Y); /* wymiary ramki bia�ej - cie� */
	  setcolor(BLACK);
	  line(px+TAB_R,py+TAB_R,px+KON_RAM_X,py+TAB_R); /* cie� obramowania czarny x */
	  line(px+TAB_R,py+TAB_R,px+TAB_R,py+KON_RAM_Y); /* cie� obramowania czarny y */
	  setcolor(BLACK);

	  line(px+OS_POCZ,py+Y_HALF,px+OS_X_KON,py+Y_HALF);  /* rysowanie osi x */
	  line(px+GROT_X,py+GROT_XG,px+OS_X_KON,py+Y_HALF);  /* rysowanie strza�ki x */
	  line(px+GROT_X,py+GROT_XD,px+OS_X_KON,py+Y_HALF);
	  line(px+X_HALF,py+OS_POCZ,px+X_HALF,py+OS_Y_KON);  /* rysowanie osi y */
	  line(px+GROT_YL,py+GROT_Y,px+X_HALF,py+OS_POCZ);   /* rysowanie strza�ki y */
	  line(px+GROT_YP,py+GROT_Y,px+X_HALF,py+OS_POCZ);

	  for(x=OS_POCZ;x<X_SKAL;x+=SKOK_X)   /* podzia�ka */
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

}