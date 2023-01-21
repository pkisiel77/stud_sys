 // pacjnet.c
#include "../blank/moje.h"
#include "../blank/sys_dekl.h"
void chk_time(void);

// ustawiena dla blankietu
extern struct Service *Service;
extern struct agenda *Agenda;
extern unsigned int attr_title;
extern shint X_time, X_tyt;
extern unsigned int cursor, nocursor;
// kolory
unsigned int attr0_menu=TERM_WHITE_BG|TERM_BLACK|TERM_FLUSH;
unsigned int attr1_menu=TERM_WHITE|MTERM_HILIGHT|TERM_MAGENTA_BG|TERM_FLUSH;
//


// chwilowo
#define DLNAZW 128
#define Ill_przedm 10
//

// dodatkowe funkcje dla blankietu pacjnet
void wpisz_pacjent(shint nr_rekordu,shint ret,shint size,shint ochr);
void edyt_pacjent(shint nr_rekordu,shint ret,shint size,shint ochr);
void zapis_pacjent(shint ret,char plik[DLNAZW],struct student **tb);
void otworz_pacjent(shint ret,char plik[DLNAZW]);
shint wpisz(void);
shint wypisz(void);
//
// dla blankietu

//
shint ll_stud=1;
// kolory 
//
shint nnr_opcji=-1;
char *opcje_menu[5]={"Dodaj","Wyswietl baze","Przeszukiwanie","Zapisz","Otworz"};
unsigned char ll_opcji_stud=5;
char *ttytul={"Baza Studentow"};

#include "pacjnet.h"

//------ s³owniki dla bazy pacjentów ------
//
// s³ownik kasy chorych
char *kasa_chorych[L_KASY_CHORYCH]={"1","2","1","2","1","2","1","2","1","2","1","2","1"};
//-----------------------------------------

#define DEC_MENU 80
#define DEC_USUN 21
#define DEC_NAZW   22 
#define DEC_IMIE   23
#define DEC_MIASTO 24
#define DEC_WZOR_T 25
#define DEC_ZAP    27
#define DEC_DATAD  28
#define DEC_DATAM  29
#define DEC_DATAR  30
#define DEC_SZUKAJ 31
#define DEC_WZOR_L 32
#define DEC_NSTUD  33
#define DEC_INDEX  34
#define DEC_ROK_ST 35
#define DEC_LPRZ   36
#define DEC_PRZEDM 37
#define DEC_OTW    38
#define DLN 20
#define DLIM 15
#define DLNAZW 15
#define DLPRZ 15
#define SIZE 15
#define LMIAST 50
#define LNAZW 50
#define LIMION 15
#define LPRZEDM 30
#define ROKL 4
#define MIESIACL 30
#define DZIENL 50
#define LDZIEN 50

shint nr_indexu,rok_stu,dat_dzie,data_mie,data_rok;
char txt_nazw[DLN+1], txt_imie[DLIM+1], ttxt_miast[DLIM+1]={0},txt_przedm[Ill_przedm][DLPRZ+1],tmp_przedm[Ill_przedm][DLPRZ+1];
char txt_dzi[DZIENL+1],txt_mie[MIESIACL+1],txt_rok[ROKL+1];
shint ll_nazw=0, ll_imion=0, ll_miast=0,ll_przedm=0,ll_prz_tab=0;
shint sstary_rekord=-1,ttest=-1,*tb;
char plik[DLNAZW];
#define MAX 50
struct student *sstudenci[MAX]={NULL};
struct student *dStud;
struct student **dPrzedmioty;
#define STO 100
#define TYSIAC 100
char *Nazwiska[LNAZW], *Imiona[LIMION], *Miasta[LMIAST], *Przedm[LPRZEDM];
char *pDzien[LDZIEN],*pMiesiac[LDZIEN],*pRok[LDZIEN];
shint i,in,ii,im,l_mall,ttempor=-1;
#include "szukaj.c"

/* ================================================================ */
shint pacjent_blankiet(shint nr_rekordu, shint ob_pocz,	shint ob_konc,
										shint x_lewy_gorny,	shint y_lewy_gorny,
										shint anim_pid, char *D)
 {shint ret, ochr=0, ochrf=-1, size=0, nr_rek;
  struct pacjnet *d0, *dp, *d;
  shint licznik;
  static tbw[MAX]={-1,0};
  setcursor(cursor);
  /*sledzenie(); */
   d0=(struct student *)D;
   nr_rek=nr_rekordu;
   dp=d0+nr_rek;
   d=(struct student *)dane_raportowanego_rekordu(pacjent_blankiet,&nr_rek);
   if(d==NULL || d!=dp)
	{term_printf(MY_MAX,X_L0,ATTR_A," Niezgodnosc adresow w RAP_ZM(d=%p dp=%p). <Ent> ",d,dp);
	 GET_char();  return -1;
	}
   rekord_danych_do_naglowka(nr_rek);

   // blankiet
   ret=dana_koment(-1,-1,"+ ");
   if(nnr_opcji==-1)
   {nnr_opcji=okno_menu(opcje_menu,ll_opcji_stud,0,attr0_menu,attr1_menu,10,30,-1,ttytul,1);
   }
   switch(nnr_opcji)
   {case 0:dodaj_pacjent(nr_rekordu,ret,size,ochr);
           {static char *powrot_menu[2]={"m menu glowne","b menu bazy"},dec='m';
            ret=dana_decyzyjna(-1,-1," Co robimy <%s>  ?? ", "m/b",powrot_menu, 2,
										 &dec, ochr=0, DEC_MENU);
           }
           break;
    case 1:ret=dana_koment(-1,20,"+ Wyswietlanie Bazy");
           licznik=show(tbw);
	       if(licznik>=0)
		   {dStud=sstudenci[licznik];
            edyt_pacjent(nr_rekordu,ret,size,ochr);
            ll_przedm=0;
		   }
		   {static char *powrot_menu[2]={"m menu glowne","b menu bazy"},dec='m';
            ret=dana_decyzyjna(-1,-1," Co robimy <%s>  ?? ", "m/b",powrot_menu, 2,
										 &dec, ochr=0, DEC_MENU);
           }
	       break;
    case 2:ret=dana_koment(-1,20,"+ Przeszukiwanie bazy studentow ");
           ret=dana_koment(-1,-1,"+ ");
           ret=dana_koment(-1,-1,"+ ");
           {static char *opis[4]={"n Nazwisko","i Imie","x nr indexu","s rok studiow"};
            ret=dana_decyzyjna(-1,-1,"Pole <%s>  ?? ", "n/i/x/s", opis, 4,&Sz.pole, ochr=0, DEC_SZUKAJ);
           }
           if(Sz.text)
		   {ret=dana_text_dec(-1, -1," Wzor  ??", Sz.wzor_t, size=DLN-1, NULL, 0,ochr=1,DEC_WZOR_T);
            if(ttempor>=0)
			{dStud=sstudenci[tb[ttempor]];
             //edyt_pacjent(nr_rekordu,ret,size,ochr);
             ll_przedm=0;
			 ttempor=-1;
			 free(tb);
			}
			}
            if(Sz.liczby)
			{static char *how[3]={"> wieksze","< mniejsze","= rowne"};
             static shint wmin=0, wmax=32767;
             ret=dana_znak(-1, -1,"   ??",">/</=", how, 3, &Sz.znak,ochr=0);
             ret=dana_int_dec(-1, -1," Wzor  ??", &wmin, &wmax, &(Sz.wzor_l), 5,ochr=1,-1,DEC_WZOR_L);
             if(ttempor>=0)
			 {dStud=sstudenci[tb[ttempor]];
              //edyt_pacjent(nr_rekordu,ret,size,ochr);
              ll_przedm=0;
	          ttempor=-1;
			  free(tb);    
			 }
			}
            ret=dana_koment(-1,-1,"+ ");
            ret=dana_koment(-1,-1,"+ ");
            ret=dana_koment(-1,-1,"+ ");
            {static char *powrot_menu[2]={"m menu glowne","b menu bazy"}, dec='m';
             ret=dana_decyzyjna(-1,-1," Co robimy <%s>  ?? ", "m/b",powrot_menu, 2,&dec, ochr=0, DEC_MENU);
            }
            break;
    case 3: ret=dana_koment(-1,20,"+Zapisywanie danych");
            ret=dana_koment(-1,-1,"+ ");
            {static char **pm=NULL;
			 ret=dana_text_dec(-1,-1,"+Nazwa pliku:  ??",plik,14,pm,0,ochr=0,DEC_ZAP);
		     if(plik[0]!=0)
			 {zapis_pacjent(ret,plik,sstudenci);
			 }
			}
            ret=dana_koment(-1,-1,"+ ");
            {static char *powrot_menu[2]={"m menu glowne","b menu bazy"}, dec='m';
             ret=dana_decyzyjna(-1,-1," Co robimy <%s>  ?? ", "m/b",powrot_menu, 2,&dec, ochr=0, DEC_MENU);
            }
            break;
    case 4: ret=dana_koment(-1,20,"Otworz");
            ret=dana_koment(-1,-1,"+ ");
            ret=dana_koment(-1,-1,"+ ");
			{static char **pm=NULL;
             ret=dana_text_dec(-1,-1,"+Nazwa pliku:  ??",plik,14,pm,0,ochr=0,DEC_OTW);
		     if(plik[0]!=0)
			 {otworz_pacjent(ret,plik);
			 }
			}
            ret=dana_koment(-1,-1,"+ ");
            {static char *powrot_menu[2]={"m menu glowne","b menu bazy"}, dec='m';
             ret=dana_decyzyjna(-1,-1," Co robimy <%s>  ?? ", "m/b",powrot_menu, 2,&dec, ochr=0, DEC_MENU);
            }
            break;
    default:break;
   }

	return ret;
 }

char *dane_pacjent(shint ob_pocz, shint ob_konc, shint *rozmiar_ob)
 {static struct Service *S;
  static struct agenda *A;
/*
  struct student *St,*st;
  shint x,y;
   Sz.text=1;
   Sz.liczby=0;
   Sz.pole='n';
   Sz.znak='=';
   sstudenci[0]=NULL;
   S=Service;
    *rozmiar_ob=S->str_size;
	St=Malloc(S->l_rek_max*(*rozmiar_ob));
    for(i=0, st=St;i<S->l_rek_max;i++,st++)
    {st->nazw_no=-1; st->imie_no=-1; st->miasto_no=-1;}
    for(i=0;i<LMIAST;i++) Miasta[i]=NULL;
    for(i=0;i<LNAZW;i++) Nazwiska[i]=NULL;
    for(i=0;i<LDZIEN;i++) pDzien[i]=NULL;
    for(i=0;i<LDZIEN;i++) pMiesiac[i]=NULL;
    for(i=0;i<LDZIEN;i++) pRok[i]=NULL;
    for(i=0;i<MAX;i++) sstudenci[i]=NULL;

    /* tu wczytamy rekordy bazy */
/*
	x=m_wherex(); y=m_wherey(); setcursor(nocursor);
	term_printf(Y_G0,X_tyt,attr_title,"%s",S->name);
	term_cur(y,x); setcursor(cursor);
  nnr_opcji=-1;
  ll_przedm=0;
  ttest=-1;
  plik[0]=0;
  nr_indexu=0;
  rok_stu=1;
  dat_dzie=1;
  data_mie=1;
  data_rok=1901;
    for(i=0;i<Ill_przedm;i++)txt_przedm[i][0]='0\n';
    for(i=0;i<Ill_przedm;i++)tmp_przedm[i][0]='0\n';
  return (char *)St;
  */
  return 0;
 }

void wpis_pacjent(shint ob_pocz, shint ob_konc,
					 char *D, shint rozmiar_ob, char zapis[], char *Kod_op)
 {
  /*
  nnr_opcji=-1;
  ll_przedm=0;
  ttest=-1;
  */
}
#include <string.h>
#include <ctype.h>

shint ppopraw_text(char text[])
 {shint d_str, nz, kz, r_zn;
  d_str=strlen(text);
  for(nz=0, kz=0; kz<d_str; kz++)
   {if(isalpha(text[kz])) {text[nz]=text[kz]; nz++;}
   }
  if(nz==0) return -1;
  if(text[0]>='a' && text[0]<='z')
   {r_zn=('A'-'a');
    text[0]+=r_zn;
   }

  d_str=nz; text[nz]=0;
  return d_str;
 }
int sszukaj_tekstu(char *baza[], char txt[], shint l_rek, shint max)
 {shint wynik, i;
  if((strlen(txt))!=0)
   {i=ppopraw_text(txt); if(i<=0) return -1;
    for(i=0;i<l_rek;i++)
     {wynik=strcmp(baza[i],txt); if(wynik==0) return i;
     }
   }
  if(l_rek==max) return -1;
  baza[l_rek]=Malloc(strlen(txt)+1);
  przepisz(baza[l_rek], txt, strlen(txt)+1);
  return l_rek;
 }

void dodaj_pacjent(shint nr_rekordu,shint ret,shint size,shint ochr)
{
  shint licznik,temp;
  if(ttest)
  {ttest=0;
  for(licznik=0;licznik<MAX-1;licznik++)
  {if(sstudenci[licznik]==NULL)
    {sstudenci[licznik]=(struct student*)malloc(sizeof (struct student));
     dStud=sstudenci[licznik];
     sstudenci[licznik+1]=NULL;
     dStud->nazw_no=-1;
	 dStud->imie_no=-1;
     dStud->miasto_no=-1;
     break;
    }
  }
 }
  if(dStud->nazw_no<0) txt_nazw[0]=0;
  else
   {przepisz(txt_nazw,Nazwiska[dStud->nazw_no],strlen(Nazwiska[dStud->nazw_no])+1);
   }
  if(dStud->imie_no<0) txt_imie[0]=0;
  else
   {przepisz(txt_imie,Imiona[dStud->imie_no],strlen(Imiona[dStud->imie_no])+1);
   }
  if(dStud->miasto_no<0) ttxt_miast[0]=0;
  else
   {przepisz(ttxt_miast,Miasta[dStud->miasto_no],strlen(Miasta[dStud->miasto_no])+1);
   }
  sstary_rekord=nr_rekordu;
  ret=dana_text_dec(-1, -1," Nazwisko  ?? ", txt_nazw, size=DLN, Nazwiska, ll_nazw, ochr=1,DEC_NAZW);
  ret=dana_text_dec(-1, -1," Imie  ??", txt_imie, size=DLIM, Imiona, ll_imion, ochr=1,DEC_IMIE);
  ret=dana_koment(-1,-1,"+ ");
  ret=dana_text_dec(-1,-2," Miasto  ??", ttxt_miast, size=DLIM, Miasta, ll_miast, ochr=1, DEC_MIASTO);
  ret=dana_koment(-1,-1,"+ ");
  {static shint wmin=0,wmax=3200;
   ret=dana_int_dec(-1,-1,"+ Numer indeksu ", &wmin, &wmax,&nr_indexu,4, 1,1,DEC_INDEX);
   dStud->index_no=nr_indexu;

  }
  {static shint wmin=0,wmax=10;
   ret=dana_int_dec(-1,-1,"+ Rok studiow ", &wmin, &wmax,&rok_stu,2, 1,1,DEC_ROK_ST);
   dStud->rok_stud=rok_stu;
  }
  {static shint dmin=1,dmax=31;          /* dzien*/
   static shint mmin=1,mmax=12;        /*miesiac*/
   static shint rmin=1900,rmax=2100;   /*rok*/
    ret=dana_int_dec(-1,-1," Data urodzenia: dzi ",&dmin,&dmax,&dat_dzie,2,1,1,DEC_DATAD);
    ret=dana_int_dec(-1,-1,"  mie ",&mmin,&mmax,&data_mie,2,1,1,DEC_DATAM);
    ret=dana_int_dec(-1,-1,"  rok ",&rmin,&rmax,&data_rok,4,1,1,DEC_DATAR);
    dStud->dzien_u=dat_dzie;
    dStud->mies_u=data_mie;
    dStud->rok_u=data_rok;
  }
  ret=dana_koment(-1,-1,"+ ");
  {if(ll_przedm==0)
  {static shint wmin=1,wmax=Ill_przedm;
   (dStud->ll_przedm_stu)=0;
	ret=dana_int_dec(-1,-1,"+ Podaj liczbe przedmiotow",&wmin,&wmax,&ll_przedm,2,1,1,DEC_LPRZ);

  }
   else
   {ret=dana_koment(-1,-1,"+  Przedmiot:      Egzamin-ocena    Cwiczenia-ocena  Laboratorium-ocena");
    for(temp=0;temp<ll_przedm;temp++)
	{static shint gemin=0,gemax=100;
     static shint gcmin=0,gcmax=100;
     static shint glmin=0,glmax=100;
     static float oemin=1.0,oemax=6.1;
     static float ocmin=1.0,ocmax=6.1;
     static float olmin=1.0,olmax=6.1;
      ret=dana_text_dec(-1,-1,"  ",txt_przedm[temp],DLPRZ,Przedm,ll_prz_tab,1,DEC_PRZEDM);
      ret=dana_koment(-1,-1,"    ");
	  ret=dana_float_dec(-1,-1,"  ",&oemin,&oemax,&((dStud->przedmioty[temp])->ocena_egz),4,1,1,1,DEC_DATAM);
	  ret=dana_koment(-1,-1,"           ");
      ret=dana_float_dec(-1,-1,"  ",&ocmin,&ocmax,&((dStud->przedmioty[temp])->ocena_cwi),4,1,1,1,DEC_DATAD);
      ret=dana_koment(-1,-1,"             ");
	  ret=dana_float_dec(-1,-1,"  ",&olmin,&olmax,&((dStud->przedmioty[temp])->ocena_lab),4,1,1,1,DEC_DATAR);
      ret=dana_koment(-1,-1,"+ ");
	}
   }
  }
#endif
}


void edyt_pacjent(shint nr_rekordu,shint ret,shint size,shint ochr)
{
#ifdef EEE
	shint licznik,temp,i=0;
{przepisz(txt_nazw,Nazwiska[dStud->nazw_no],strlen(Nazwiska[dStud->nazw_no])+1);
 przepisz(txt_imie,Imiona[dStud->imie_no],strlen(Imiona[dStud->imie_no])+1);
 przepisz(ttxt_miast,Miasta[dStud->miasto_no],strlen(Miasta[dStud->miasto_no])+1);
 for(i=0;i<dStud->ll_przedm_stu;i++)
 {strcpy(txt_przedm[i],Przedm[ (dStud->przedmioty[i])->nazwa_przedm] );
  strcpy(tmp_przedm[i],Przedm[(dStud->przedmioty[i])->nazwa_przedm]);
 }
}
 sstary_rekord=nr_rekordu;
 ret=dana_text_dec(-1, -1," Nazwisko  ?? ", txt_nazw, size=DLN, Nazwiska, ll_nazw, ochr=1,DEC_NAZW);
 ret=dana_text_dec(-1, -1," Imie  ??", txt_imie, size=DLIM, Imiona, ll_imion, ochr=1,DEC_IMIE);
 ret=dana_koment(-1,-1,"+ ");
 ret=dana_text_dec(-1,-2," Miasto ", ttxt_miast, size=SIZE, Miasta, ll_miast, ochr=1, DEC_MIASTO);
 ret=dana_koment(-1,-1,"+ ");
 {static shint wmin=0,wmax=3200;
  ret=dana_int_dec(-1,-1,"+ Numer indeksu ", &wmin, &wmax,&(dStud->index_no),4, 1,1,DEC_INDEX);
 }
 {static shint wmin=0,wmax=10;
  ret=dana_int_dec(-1,-1,"+ Rok studiow ", &wmin, &wmax,&(dStud->rok_stud),2, 1,1,DEC_ROK_ST);
 }
 {static shint dmin=1,dmax=31;          /* dzien*/
  static shint mmin=1,mmax=12;        /*miesiac*/
  static shint rmin=1900,rmax=2100;   /*rok*/
   ret=dana_int_dec(-1,-1," Data urodzenia: dzi ",&dmin,&dmax,&(dStud->dzien_u),2,1,1,DEC_DATAD);
   ret=dana_int_dec(-1,-1,"  mie ",&mmin,&mmax,&(dStud->mies_u),2,1,1,DEC_DATAM);
   ret=dana_int_dec(-1,-1,"  rok ",&rmin,&rmax,&(dStud->rok_u),4,1,1,DEC_DATAR);
 }
 ret=dana_koment(-1,-1,"+ ");
 {ll_przedm=(dStud->ll_przedm_stu);
  ret=dana_koment(-1,-1,"+  Przedmiot:      Egzamin-ocena    Cwiczenia-ocena  Laboratorium-ocena");
  for(temp=0;temp<ll_przedm;temp++)
  { static shint gemin=0,gemax=100;
    static shint gcmin=0,gcmax=100;
    static shint glmin=0,glmax=100;
    static float oemin=1.0,oemax=6.1;
    static float ocmin=1.0,ocmax=6.1;
    static float olmin=1.0,olmax=6.1;
     ret=dana_text_dec(-1,-1,"  ",txt_przedm[temp],DLPRZ,Przedm,ll_prz_tab,1,DEC_PRZEDM);
     ret=dana_koment(-1,-1,"    ");
	 ret=dana_float_dec(-1,-1,"  ",&oemin,&oemax,&((dStud->przedmioty[temp])->ocena_egz),4,1,1,1,DEC_DATAM);
	 ret=dana_koment(-1,-1,"           ");
     ret=dana_float_dec(-1,-1,"  ",&ocmin,&ocmax,&((dStud->przedmioty[temp])->ocena_cwi),4,1,1,1,DEC_DATAD);
     ret=dana_koment(-1,-1,"             ");
	 ret=dana_float_dec(-1,-1,"  ",&olmin,&olmax,&((dStud->przedmioty[temp])->ocena_lab),4,1,1,1,DEC_DATAR);
     ret=dana_koment(-1,-1,"+ ");
  }
 }
#endif
}

/***************************************************************/

void zapis_pacjent(shint ret,char plik[DLNAZW],struct student **tb)
{
	
	
#ifdef EEE	
	FILE *plikstud=NULL;
 char c,d;
 int licz=0,licz2,i;
  plikstud=fopen(plik,"w");
  if(!plikstud)
  {ret=dana_koment(-1,-1,"blad przy otwieraniu pliku docelowego");
  }
  else
  {fprintf(plikstud,"%d\n",licz);
   for(licz=0;licz<MAX;licz++)
   {if(tb[licz]==NULL) {break;}
	fprintf(plikstud,"%d %d %d %d %d %d %d %d %d %d\n",licz,tb[licz]->imie_no,
           tb[licz]->nazw_no,tb[licz]->index_no,tb[licz]->dzien_u,
           tb[licz]->mies_u,tb[licz]->rok_u,tb[licz]->rok_stud,
           tb[licz]->miasto_no,tb[licz]->ll_przedm_stu);
   }
   for(i=0;i<licz;i++)
   {for(licz2=0;licz2<(tb[i]->ll_przedm_stu);licz2++)
	 {fprintf(plikstud,"  %d %d %3.1f %3.1f %3.1f\n",licz2,
	                (tb[i]->przedmioty[licz2])->nazwa_przedm,
                    (tb[i]->przedmioty[licz2])->ocena_egz,
					(tb[i]->przedmioty[licz2])->ocena_cwi,
					(tb[i]->przedmioty[licz2])->ocena_lab);
	 }
	}
   fprintf(plikstud,"%d\n",ll_nazw);
   for(i=0;i<ll_nazw;i++)
   {fprintf(plikstud,"%s\n",Nazwiska[i]);
   }
   fprintf(plikstud,"%d\n",ll_imion);
   for(i=0;i<ll_imion;i++)
   {fprintf(plikstud,"%s\n",Imiona[i]);
   }
   fprintf(plikstud,"%d\n",ll_miast);
   for(i=0;i<ll_miast;i++)
   {fprintf(plikstud,"%s\n",Miasta[i]);
   }
   fprintf(plikstud,"%d\n",ll_prz_tab);
   for(i=0;i<ll_prz_tab;i++)
   {fprintf(plikstud,"%s\n",Przedm[i]);
   }
   rewind(plikstud);
   fprintf(plikstud,"%d",licz);
   if(fclose(plikstud))
   {ret=dana_koment(-1,-1,"blad przy zamykaniu pliku docelowego");
   }
  }
#endif
}
/******************************************************************/
void otworz_pacjent(shint ret,char plik[DLNAZW])
{
	
#ifdef EEE	
	FILE *plikstud=NULL;
 char temp[DLPRZ];
 int licz=0,licz2,licz3,i,poczatek;
  plikstud=fopen(plik,"r");
  if(!plikstud)
  {ret=dana_koment(-1,-1,"blad przy otwieraniu pliku docelowego");
  }
  else
  {poczatek=0;
   while(sstudenci[poczatek]!=NULL){ free(sstudenci[poczatek]); poczatek++; }
   fscanf(plikstud,"%d",&licz); 
   for(i=0;i<licz;i++)
   {sstudenci[i]=malloc(sizeof(struct student)); 
	sstudenci[i+1]=NULL;
	fscanf(plikstud,"%d %d %d %d %d %d %d %d %d %d",&licz2,
		   &sstudenci[i]->imie_no,&sstudenci[i]->nazw_no,
		   &sstudenci[i]->index_no,&sstudenci[i]->dzien_u,
		   &sstudenci[i]->mies_u,&sstudenci[i]->rok_u,
		   &sstudenci[i]->rok_stud,&sstudenci[i]->miasto_no,
		   &sstudenci[i]->ll_przedm_stu);
   }
   for(i=0;i<licz;i++)
   {for(licz2=0;licz2<(sstudenci[i]->ll_przedm_stu);licz2++)
   {(sstudenci[i]->przedmioty[licz2])=malloc(sizeof(struct kurs));
    fscanf(plikstud,"  %d%d%f%f%f",&licz3,&(sstudenci[i]->przedmioty[licz2])->nazwa_przedm,
                    &(sstudenci[i]->przedmioty[licz2])->ocena_egz,
					&(sstudenci[i]->przedmioty[licz2])->ocena_cwi,
					&(sstudenci[i]->przedmioty[licz2])->ocena_lab);
   }
   }
   fscanf(plikstud,"%d",&ll_nazw);
   for(i=0;i<ll_nazw;i++)
   {fscanf(plikstud,"%s",txt_nazw);
    Nazwiska[i]=malloc(strlen(txt_nazw)+1);
    strcpy(Nazwiska[i],txt_nazw);
   }
   fscanf(plikstud,"%d",&ll_imion);
   for(i=0;i<ll_imion;i++)
   {fscanf(plikstud,"%s",txt_imie);
	Imiona[i]=malloc(strlen(txt_imie)+1);
	strcpy(Imiona[i],txt_imie);
   }
   fscanf(plikstud,"%d",&ll_miast);
   for(i=0;i<ll_miast;i++)
   {fscanf(plikstud,"%s",ttxt_miast);
	Miasta[i]=malloc(strlen(ttxt_miast)+1);
	strcpy(Miasta[i],ttxt_miast);
   }
   fscanf(plikstud,"%d",&ll_prz_tab);
   for(i=0;i<ll_prz_tab;i++)
   {fscanf(plikstud,"%s",temp);
	Przedm[i]=malloc(strlen(temp)+1);
	strcpy(Przedm[i],temp);
   }
   if(fclose(plikstud))
   {ret=dana_koment(-1,-1,"blad przy zamykaniu pliku docelowego");
   }
  }
#endif
}
/*********************************************************************/
shint dec_pacjent(shint decyzja, shint kod_decyzji, shint nr_dec,shint kod_raportu, shint np, shint *nr_rekordu)
 {
	
#ifdef EEE
	
	shint pozycja,temp;
   switch(kod_decyzji)
   {case DEC_NAZW:pozycja=sszukaj_tekstu(Nazwiska, txt_nazw, ll_nazw, LNAZW);
                   dStud->nazw_no=pozycja;
                   if(pozycja>=ll_nazw) ll_nazw=pozycja+1;
                   break;
    case DEC_IMIE:pozycja=sszukaj_tekstu(Imiona, txt_imie, ll_imion, LIMION);
                  dStud->imie_no=pozycja;
                  if(pozycja>=ll_imion) ll_imion=pozycja+1;
                  break;
    case DEC_MIASTO:pozycja=sszukaj_tekstu(Miasta, ttxt_miast, ll_miast, LMIAST);
                    dStud->miasto_no=pozycja;
                    if(pozycja>=ll_miast) ll_miast=pozycja+1;
                    break;
    case DEC_ZAP:break;
    case DEC_OTW:break;
    case DEC_SZUKAJ:switch(decyzja)
					{case 'i':Sz.text=1;
                              Sz.liczby=0;
                              break;
                     case 'n':Sz.text=1;
                              Sz.liczby=0;
                              break;
                     case 'x':Sz.liczby=1;
                              Sz.text=0;
                              break;
                     case 's':Sz.liczby=1;
                              Sz.text=0;
                              break;
                     default:break;
					}
                    break;

    case DEC_WZOR_T:tb=pprzeszukaj_t(Sz.pole,Sz.wzor_t);
			        ttempor=show(tb);
				    break;
    case DEC_WZOR_L:tb=pprzeszukaj_l(Sz.pole,Sz.znak,Sz.wzor_l);
			        ttempor=show(tb);
				    break;

    case DEC_USUN:switch(decyzja)
				  {case 'd': return kod_raportu;
                   case 'u':break;
                   case 'z':break;
                   case 'm': return -1;
                   default: break;
				  }
                  break;
    case DEC_MENU:switch(decyzja)
				  {case 'm': return -1;
                   case 'b': nnr_opcji=-1;
                   default: break;
				  }
                  break;
    case DEC_INDEX:break;
    case DEC_DATAD:break;
	case DEC_DATAM:break;
    case DEC_DATAR:break;
	case DEC_ROK_ST:break;
	case DEC_LPRZ:for(temp=0;temp<ll_przedm;temp++)
				  {dStud->przedmioty[temp]=(struct kurs*)malloc(sizeof(struct kurs));
	               (dStud->przedmioty[temp])->ocena_egz=5.0;
                   (dStud->przedmioty[temp])->ocena_cwi=5.0;
                   (dStud->przedmioty[temp])->ocena_lab=5.0;
				   txt_przedm[temp][0]='\0';
				   tmp_przedm[temp][0]='\0';
				  }
			      (dStud->ll_przedm_stu)=ll_przedm;
			      break;
	case DEC_PRZEDM:temp=0;
			        while( strcmp(txt_przedm[temp],tmp_przedm[temp])==0 ){temp++;}
			        pozycja=sszukaj_tekstu(Przedm, txt_przedm[temp], ll_prz_tab, LPRZEDM);
                    (dStud->przedmioty[temp])->nazwa_przedm=pozycja;
                    if(pozycja>=ll_prz_tab) ll_prz_tab=pozycja+1;
                    strcpy(tmp_przedm[temp],txt_przedm[temp]);
			        break;
    default: break;
   }
#endif
  return kod_raportu;
}
