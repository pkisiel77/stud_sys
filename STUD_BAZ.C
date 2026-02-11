#include "../blank/moje.h"
#include "../blank/sys_dekl.h"
void chk_time(void);
void WpiszPacjenta(int nr_rekordu,int ret,int size,int ochr);
void edyt_stud(int nr_rekordu,int ret,int size,int ochr);
int PobierzFix(char *sciezka);
int ZapiszFix(char *sciezka, int dana);

extern struct Service *Service;
extern struct agenda *Agenda;
extern struct _dane_pacjenta_ dane_pacjenta;

extern unsigned int attr_title;
extern int X_time, X_tyt;
extern unsigned int cursor, nocursor;
int l_stud=1;
unsigned int attr_baz=TERM_WHITE_BG|TERM_BLACK|TERM_FLUSH;
unsigned int attr0_baz=TERM_WHITE|MTERM_HILIGHT|TERM_BLUE_BG|TERM_FLUSH;
int nr_opcji=-1;
char *opcje_bazy[2]={"Dodaj pacjenta","Szukaj pacjenta"};
unsigned char l_opcji_stud=2;
char *tytul={" Pacjent"};
#include "baza_stu.h"
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

#define DEC_DODAJ  39
#define DEC_POMIAR 40

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
int nr_ewidencyjny,nr_kasy_chorych,dat_dzie,data_mie,data_rok;
void zapis_baz(int ret,char plik[DLNAZW],struct student **tb);
void otworz_baz(int ret,char plik[DLNAZW]);
char txt_nazw[DLN+1], txt_imie[DLIM+1], txt_miast[DLIM+1]={0},txt_przedm[IL_PRZEDM][DLPRZ+1],tmp_przedm[IL_PRZEDM][DLPRZ+1];
char txt_dzi[DZIENL+1],txt_mie[MIESIACL+1],txt_rok[ROKL+1];
int l_nazw=0, l_imion=0, l_miast=0,l_przedm=0,l_prz_tab=0;
int stary_rekord=-1,test=-1,*tb;
char plik[DLNAZW];
#define MAX 50
struct student *studenci[MAX]={NULL};
struct student *dStud;
struct student **dPrzedmioty;
#define STO 100
#define TYSIAC 100
char *Nazwiska[LNAZW], *Imiona[LIMION], *Miasta[LMIAST], *Przedm[LPRZEDM];
char *pDzien[LDZIEN],*pMiesiac[LDZIEN],*pRok[LDZIEN];
int i,in,ii,im,l_mall,tempor=-1;
int wpisz(void);
int wypisz(void);
#include "szukaj.c"
/* ================================================================ */
int pacjent_blankiet(int nr_rekordu, int ob_pocz,	int ob_konc,
										int x_lewy_gorny,	int y_lewy_gorny,
										int anim_pid, char *D)
 {int ret, ochr=0, ochrf=-1, size=0, nr_rek;
  int pomiar=0; // je�eli pomiar = 1 to skok do blankietu nr ... do pomiaru
  struct student *d0, *dp, *d;
  int licznik;
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
   // automatyczne �adowanie bazy danych pacjenta
   ret=dana_koment(-1,-1,"+ ");
   otworz_baz(ret,PATH_BAZA);
   // ...dalej
   if(nr_opcji==-1)
   {nr_opcji=okno_menu(opcje_bazy,l_opcji_stud,1,attr_baz,attr0_baz,10,30,-1,tytul,1);
   }
   switch(nr_opcji)
   {case 0:WpiszPacjenta(nr_rekordu,ret,size,ochr);
           {static char *powrot_menu[2]={"m menu glowne","b menu bazy"},dec='m';
            ret=dana_decyzyjna(-1,-1," Co robimy <%s>  ?? ", "m/b",powrot_menu, 2,
									 &dec, ochr=0, DEC_MENU);
		   }
           break;
/*
    case 1:ret=dana_koment(-1,20,"+ Wyswietlanie Bazy");
           licznik=show(tbw);
	       if(licznik>=0)
		   {dStud=studenci[licznik];
            edyt_stud(nr_rekordu,ret,size,ochr);
            l_przedm=0;
		   }
		   {static char *powrot_menu[2]={"m menu glowne","b menu bazy"},dec='m';
            ret=dana_decyzyjna(-1,-1," Co robimy <%s>  ?? ", "m/b",powrot_menu, 2,
										 &dec, ochr=0, DEC_MENU);
			pomiar=0;
           }
	       break;
*/
    case 1:ret=dana_koment(-1,20,"+ Przeszukiwanie bazy pacjentow ");
           ret=dana_koment(-1,-1,"+ ");
           ret=dana_koment(-1,-1,"+ ");
           {static char *opis[4]={"n Nazwisko","i Imie","r Rok urodzenia","e Nr ewidencyjny"};
            ret=dana_decyzyjna(-1,-1,"Pole <%s>  ?? ", "n/i/r/e", opis, 4,&Sz.pole, ochr=0, DEC_SZUKAJ);
           }
           if(Sz.text)
		   {ret=dana_text_dec(-1, -1," Wzor  ??", Sz.wzor_t, size=DLN-1, NULL, 0,ochr=1,DEC_WZOR_T);
            if(tempor>=0)
			{dStud=studenci[tb[tempor]];
             edyt_stud(nr_rekordu,ret,size,ochr);
             l_przedm=0;
			 tempor=-1;
			 free(tb);
			}
			}
            if(Sz.liczby)
			{static char *how[3]={"> wieksze","< mniejsze","= rowne"};
             static int wmin=0, wmax=32767;
             ret=dana_znak(-1, -1,"   ??",">/</=", how, 3, &Sz.znak,ochr=0);
             ret=dana_int_dec(-1, -1," Wzor  ??", &wmin, &wmax, &(Sz.wzor_l), 5,ochr=1,-1,DEC_WZOR_L);
             if(tempor>=0)
			 {dStud=studenci[tb[tempor]];
              edyt_stud(nr_rekordu,ret,size,ochr);
              l_przedm=0;
	          tempor=-1;
			  free(tb);    
			 }
			}
            ret=dana_koment(-1,-1,"+ ");
			pomiar=1;
            {static char *powrot_menu[2]={"m menu glowne","b menu bazy"}, dec='m';
             ret=dana_decyzyjna(-1,-1," Co robimy <%s>  ?? ", "m/b",powrot_menu, 2,&dec, ochr=0, DEC_MENU);
            }
            break;
/*
    case 3: ret=dana_koment(-1,20,"+Zapisywanie danych");
            ret=dana_koment(-1,-1,"+ ");
            {static char **pm=NULL;
			 ret=dana_text_dec(-1,-1,"+Nazwa pliku:  ??",plik,14,pm,0,ochr=0,DEC_ZAP);
		     if(plik[0]!=0)
			 {zapis_baz(ret,plik,studenci);
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
			 {otworz_baz(ret,plik);
			 }
			}
            ret=dana_koment(-1,-1,"+ ");
            {static char *powrot_menu[2]={"m menu glowne","b menu bazy"}, dec='m';
             ret=dana_decyzyjna(-1,-1," Co robimy <%s>  ?? ", "m/b",powrot_menu, 2,&dec, ochr=0, DEC_MENU);
            }
            break;
			*/
//	case 3: break;
   default:break;
   }
   return ret;
 }

char *dane_pacjent(int ob_pocz, int ob_konc, int *rozmiar_ob)
 {static struct Service *S;
  static struct agenda *A;
  struct student *St,*st;
  int x,y;
   Sz.text=1;
   Sz.liczby=0;
   Sz.pole='n';
   Sz.znak='=';
   studenci[0]=NULL;
   S=Service;
    *rozmiar_ob=S->str_size;
	St=Malloc(S->l_rek_max*(*rozmiar_ob));
    for(i=0, st=St;i<S->l_rek_max;i++,st++)
    {st->pac_nazwisko=-1; st->pac_imie=-1; st->pac_miejsce_zam=-1;}
    for(i=0;i<LMIAST;i++) Miasta[i]=NULL;
    for(i=0;i<LNAZW;i++) Nazwiska[i]=NULL;
    for(i=0;i<LDZIEN;i++) pDzien[i]=NULL;
    for(i=0;i<LDZIEN;i++) pMiesiac[i]=NULL;
    for(i=0;i<LDZIEN;i++) pRok[i]=NULL;
    for(i=0;i<MAX;i++) studenci[i]=NULL;
    /* tu wczytamy rekordy bazy */
	x=m_wherex(); y=m_wherey(); setcursor(nocursor);
	term_printf(Y_G0,X_tyt,attr_title,"%s",S->name);
	term_cur(y,x); setcursor(cursor);
  nr_opcji=-1;
  l_przedm=0;
  test=-1;
  plik[0]=0;
  nr_ewidencyjny=0;
  nr_kasy_chorych=1;
  dat_dzie=1;
  data_mie=1;
  data_rok=1901;
    for(i=0;i<IL_PRZEDM;i++)txt_przedm[i][0]='0\n';
    for(i=0;i<IL_PRZEDM;i++)tmp_przedm[i][0]='0\n';
  return (char *)St;
 }

void wpis_pacjent(int ob_pocz, int ob_konc,
					 char *D, int rozmiar_ob, char zapis[], char *Kod_op)
 {nr_opcji=-1;
  l_przedm=0;
  test=-1;
}
#include <string.h>
#include <ctype.h>

int popraw_text(char text[])
 {int d_str, nz, kz, r_zn;
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
int szukaj_tekstu(char *baza[], char txt[], int l_rek, int max)
 {int wynik, i;
  if((strlen(txt))!=0)
   {i=popraw_text(txt); if(i<=0) return -1;
    for(i=0;i<l_rek;i++)
     {wynik=strcmp(baza[i],txt); if(wynik==0) return i;
     }
   }
  if(l_rek==max) return -1;
  baza[l_rek]=Malloc(strlen(txt)+1);
  przepisz(baza[l_rek], txt, strlen(txt)+1);
  return l_rek;
 }

void WpiszPacjenta(int nr_rekordu,int ret,int size,int ochr)
{int licznik,temp;
 struct _dane_pacjenta_ dane_pacjenta;
  if(test)
  {test=0;
  for(licznik=0;licznik<MAX-1;licznik++)
  {if(studenci[licznik]==NULL)
    {studenci[licznik]=(struct student*)malloc(sizeof (struct student));
     dStud=studenci[licznik];
     studenci[licznik+1]=NULL;
     dStud->pac_nazwisko=-1;
	 dStud->pac_imie=-1;
     dStud->pac_miejsce_zam=-1;
     break;
    }
  }
 }
  if(dStud->pac_nazwisko<0) txt_nazw[0]=0;
  else
   {przepisz(txt_nazw,Nazwiska[dStud->pac_nazwisko],strlen(Nazwiska[dStud->pac_nazwisko])+1);
   }
  if(dStud->pac_imie<0) txt_imie[0]=0;
  else
   {przepisz(txt_imie,Imiona[dStud->pac_imie],strlen(Imiona[dStud->pac_imie])+1);
   }
  if(dStud->pac_miejsce_zam<0) txt_miast[0]=0;
  else
   {przepisz(txt_miast,Miasta[dStud->pac_miejsce_zam],strlen(Miasta[dStud->pac_miejsce_zam])+1);
   }
  stary_rekord=nr_rekordu;
  // pola blankietu
 {
  // odczytywany z fix'a i na ko�cu zwiekszany o jeden
  nr_ewidencyjny=PobierzFix(PATH_FIX);
   if(nr_ewidencyjny < 0) return ret;
  nr_ewidencyjny+=1;
   ret=dana_koment(-1,-1,"+ Numer pacjenta w bazie [%d]",nr_ewidencyjny);
   ret=dana_koment(-1,-1,"+ -------------------------------------------------------------------------");      
   dStud->pac_nr_ew=nr_ewidencyjny;
  }

  ret=dana_text_dec(-1, -1," Nazwisko  ?? ", txt_nazw, size=DLN, Nazwiska, l_nazw, ochr=1,DEC_NAZW);
  ret=dana_text_dec(-1, -1," Imie  ??", txt_imie, size=DLIM, Imiona, l_imion, ochr=1,DEC_IMIE);
  ret=dana_koment(-1,-1,"+ ");
  ret=dana_text_dec(-1,-2," Miejsce zamieszkania  ??", txt_miast, size=DLIM, Miasta, l_miast, ochr=1, DEC_MIASTO);
  ret=dana_koment(-1,-1,"+ ");
  
  {static int wmin=0,wmax=13;
   static char *dec_opcje="0/1/2/3/4/5/6/7/8/9/10/11/12/13/14/15/16";
   static char *dec_obj[17]={"",
	                         "Kasa dolnoslaska",
	                         "Kasa kujawsko-pomorska",
							 "Kasa lubelska",
							 "Kasa lubuska",
							 "Kasa malopolska",
							 "Kasa mazowiecka",
							 "Kasa opolska",
							 "Kasa podkarpacka",
							 "Kasa podlaska",
							 "Kasa pomorska",
							 "Kasa slaska",
							 "Kasa swietokrzyska",
							 "Kasa warminsko-mazurska",
							 "Kasa wielkopolska",
							 "Kasa zachodniopomorska",
							 "Kasa branzowa dla sluzb mundurowych"};
   nr_kasy_chorych=0;
   ret=dana_int_menu(-1,-1,"+ Numer kasy chorych ",&wmin,&wmax,&nr_kasy_chorych,2,ochr,-1,14,dec_obj);
   dStud->pac_nr_kasy=nr_kasy_chorych;
  }
  {static int dmin=1,dmax=31;     
   static int mmin=1,mmax=12;     
   static int rmin=1900,rmax=2100; 
    ret=dana_int_dec(-1,-1," Data urodzenia: dzien ",&dmin,&dmax,&dat_dzie,2,1,1,DEC_DATAD);
    ret=dana_int_dec(-1,-1,"  miesiac ",&mmin,&mmax,&data_mie,2,1,1,DEC_DATAM);
    ret=dana_int_dec(-1,-1,"  rok ",&rmin,&rmax,&data_rok,4,1,1,DEC_DATAR);
    dStud->pac_ur_dzien=dat_dzie;
    dStud->pac_ur_rok=data_mie;
    dStud->pac_ur_rok=data_rok;
  }
  ret=dana_koment(-1,-1,"+ -------------------------------------------------------------------------");      
  {static char *dec_obj[]={"t tak","n nie"};
   static char *dec='n';
    ret=dana_decyzyjna(-1,-1,"+ Dodac do bazy <%s>  ??","t/n",dec_obj,2,&dec,ochr,DEC_DODAJ);
     if(dec=='t') 
	 { 
		 int zwrot=-1;
		 char buf[128];
		 ret=dana_koment(-1,-1,"+ Katalog ../pacjent/dane/%d",nr_ewidencyjny);
		 // zapis do fix'a nr nast�pnego pacjenta
		 zwrot=ZapiszFix(PATH_FIX,nr_ewidencyjny);
		 if(zwrot!=nr_ewidencyjny) {MessageBox(NULL,TEXT(" B��d zapisu do pliku fix.dat \n")
			                                        TEXT(" Skontaktuj si� z administratorem "),
													"Powa�ny b��d",MB_OK|MB_ICONERROR);return ret;}
		  else {;}
         // zapis do bazy
		 zapis_baz(ret,PATH_BAZA,studenci);
		 // nowy katalog
		 wsprintf(buf,TEXT("c:\\proba\\pacjent\\pacjent\\dane\\%d"),nr_ewidencyjny);
		 zwrot=CreateDirectory(buf,NULL);
		 if(!zwrot) {RemoveDirectory(buf);CreateDirectory(buf,NULL);}
		 else {;}

	 dane_pacjenta.pac_imie=Nazwiska;
	 dane_pacjenta.pac_nazwisko=Imiona;
	 dane_pacjenta.pac_nr_ew=dStud->pac_nr_ew;

	 

	{static char *dec_obj[]={"t tak","n nie"};
     static char *dec='n';
     ret=dana_decyzyjna(-1,-1,"+ Wykonac pomiar <%s>  ??","t/n",dec_obj,2,&dec,ochr,DEC_POMIAR);
	 ret=dana_koment(-1,-1,"+ -------------------------------------------------------------------------");      
	}
	 }
  }
}

void edyt_stud(int nr_rekordu,int ret,int size,int ochr)
{int licznik,temp,i=0;
 struct _dane_pacjenta_ dane_pacjenta;

{przepisz(txt_nazw,Nazwiska[dStud->pac_nazwisko],strlen(Nazwiska[dStud->pac_nazwisko])+1);
 przepisz(txt_imie,Imiona[dStud->pac_imie],strlen(Imiona[dStud->pac_imie])+1);
 przepisz(txt_miast,Miasta[dStud->pac_miejsce_zam],strlen(Miasta[dStud->pac_miejsce_zam])+1);
 for(i=0;i<dStud->l_przedm_stu;i++)
 {strcpy(txt_przedm[i],Przedm[ (dStud->przedmioty[i])->nazwa_przedm] );
  strcpy(tmp_przedm[i],Przedm[(dStud->przedmioty[i])->nazwa_przedm]);
 }
}
 stary_rekord=nr_rekordu;
 ret=dana_koment(-1,-1,"+ ");
 {static int wmin=0,wmax=3200;
  ret=dana_int_dec(-1,-1,"+ Numer ewidencyjny ", &wmin, &wmax,&(dStud->pac_nr_ew),4, 1,1,DEC_INDEX);
 }
 ret=dana_text_dec(-1, -1," Nazwisko  ?? ", txt_nazw, size=DLN, Nazwiska, l_nazw, ochr=1,DEC_NAZW);
 ret=dana_text_dec(-1, -1," Imie  ??", txt_imie, size=DLIM, Imiona, l_imion, ochr=1,DEC_IMIE);
 ret=dana_koment(-1,-1,"+ ");
 ret=dana_text_dec(-1,-2," Miejsce zamieszkania ", txt_miast, size=SIZE, Miasta, l_miast, ochr=1, DEC_MIASTO);
 ret=dana_koment(-1,-1,"+ ");
 
 {static int wmin=0,wmax=10;
  ret=dana_int_dec(-1,-1,"+ Numer kasy chorych ", &wmin, &wmax,&(dStud->pac_nr_kasy),2, 1,1,DEC_ROK_ST);
 }
 {static int dmin=1,dmax=31;          /* dzien*/
  static int mmin=1,mmax=12;        /*miesiac*/
  static int rmin=1900,rmax=2100;   /*rok*/
   ret=dana_int_dec(-1,-1," Data urodzenia: dzien ",&dmin,&dmax,&(dStud->pac_ur_dzien),2,1,1,DEC_DATAD);
   ret=dana_int_dec(-1,-1,"  miesiac: ",&mmin,&mmax,&(dStud->pac_ur_miesiac),2,1,1,DEC_DATAM);
   ret=dana_int_dec(-1,-1,"  rok: ",&rmin,&rmax,&(dStud->pac_ur_rok),4,1,1,DEC_DATAR);
 }
 ret=dana_koment(-1,-1,"+ ");
 /*
 {l_przedm=(dStud->l_przedm_stu);
  ret=dana_koment(-1,-1,"+  Przedmiot:      Egzamin-ocena    Cwiczenia-ocena  Laboratorium-ocena");
  for(temp=0;temp<l_przedm;temp++)
  { static int gemin=0,gemax=100;
    static int gcmin=0,gcmax=100;
    static int glmin=0,glmax=100;
    static float oemin=1.0,oemax=6.1;
    static float ocmin=1.0,ocmax=6.1;
    static float olmin=1.0,olmax=6.1;
     ret=dana_text_dec(-1,-1,"  ",txt_przedm[temp],DLPRZ,Przedm,l_prz_tab,1,DEC_PRZEDM);
     ret=dana_koment(-1,-1,"    ");
	 ret=dana_float_dec(-1,-1,"  ",&oemin,&oemax,&((dStud->przedmioty[temp])->ocena_egz),4,1,1,1,DEC_DATAM);
	 ret=dana_koment(-1,-1,"           ");
     ret=dana_float_dec(-1,-1,"  ",&ocmin,&ocmax,&((dStud->przedmioty[temp])->ocena_cwi),4,1,1,1,DEC_DATAD);
     ret=dana_koment(-1,-1,"             ");
	 ret=dana_float_dec(-1,-1,"  ",&olmin,&olmax,&((dStud->przedmioty[temp])->ocena_lab),4,1,1,1,DEC_DATAR);
     ret=dana_koment(-1,-1,"+ ");
  }
 }
 */
 dane_pacjenta.pac_nazwisko=Nazwiska;
 dane_pacjenta.pac_imie=Imiona;
 dane_pacjenta.pac_nr_ew=dStud->pac_nr_ew;
 Show(dane_pacjenta.pac_nr_ew);
     

{static char *dec_obj[]={"t tak","n nie"};
 static char *dec='n';
 ret=dana_decyzyjna(-1,-1,"+ Wykonac pomiar <%s>  ??","t/n",dec_obj,2,&dec,ochr,DEC_POMIAR);
}


}

/***************************************************************/
void zapis_baz(int ret,char plik[DLNAZW],struct student **tb)
{FILE *plikstud=NULL;
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
	fprintf(plikstud,"%d %d %d %d %d %d %d %d %d %d\n",licz,tb[licz]->pac_imie,
           tb[licz]->pac_nazwisko,tb[licz]->pac_nr_ew,tb[licz]->pac_ur_dzien,
           /*tb[licz]->pac_ur_miesiac*/6,/*tb[licz]->pac_ur_rok*/1956,tb[licz]->pac_nr_kasy,
           tb[licz]->pac_miejsce_zam,0/*tb[licz]->l_przedm_stu*/);
   }
   for(i=0;i<licz;i++)
   {for(licz2=0;licz2<(tb[i]->l_przedm_stu);licz2++)
	 {fprintf(plikstud,"  %d %d %3.1f %3.1f %3.1f\n",licz2,
	                (tb[i]->przedmioty[licz2])->nazwa_przedm,
                    (tb[i]->przedmioty[licz2])->ocena_egz,
					(tb[i]->przedmioty[licz2])->ocena_cwi,
					(tb[i]->przedmioty[licz2])->ocena_lab);
	 }
	}
   fprintf(plikstud,"%d\n",l_nazw);
   for(i=0;i<l_nazw;i++)
   {fprintf(plikstud,"%s\n",Nazwiska[i]);
   }
   fprintf(plikstud,"%d\n",l_imion);
   for(i=0;i<l_imion;i++)
   {fprintf(plikstud,"%s\n",Imiona[i]);
   }
   fprintf(plikstud,"%d\n",l_miast);
   for(i=0;i<l_miast;i++)
   {fprintf(plikstud,"%s\n",Miasta[i]);
   }
   fprintf(plikstud,"%d\n",l_prz_tab);
   for(i=0;i<l_prz_tab;i++)
   {fprintf(plikstud,"%s\n",Przedm[i]);
   }
   rewind(plikstud);
   fprintf(plikstud,"%d",licz);
   if(fclose(plikstud))
   {ret=dana_koment(-1,-1,"blad przy zamykaniu pliku docelowego");
   }
  }
}
/******************************************************************/
void otworz_baz(int ret,char plik[DLNAZW])
{FILE *plikstud=NULL;
 char temp[DLPRZ];
 int licz=0,licz2,licz3,i,poczatek;
  plikstud=fopen(plik,"r");
  if(!plikstud)
  {
#ifndef _CVC_	  
	  ret=dana_koment(-1,-1,"blad przy otwieraniu pliku docelowego");
#else
	  MessageBox(NULL,TEXT("Otworzenie bazy jest nie mo�liwe <brak pliku>\n")
		              TEXT("Zostanie utowrzony pusty plik baza.dat"),"B��d I/O",
                      MB_ICONWARNING|MB_OK);
#endif
  }
  else
  {poczatek=0;
   while(studenci[poczatek]!=NULL){ free(studenci[poczatek]); poczatek++; }
   fscanf(plikstud,"%d",&licz); 
   for(i=0;i<licz;i++)
   {studenci[i]=malloc(sizeof(struct student)); 
	studenci[i+1]=NULL;
	fscanf(plikstud,"%d %d %d %d %d %d %d %d %d %d",&licz2,
		   &studenci[i]->pac_imie,&studenci[i]->pac_nazwisko,
		   &studenci[i]->pac_nr_ew,&studenci[i]->pac_ur_dzien,
		   &studenci[i]->pac_ur_miesiac,&studenci[i]->pac_ur_rok,
		   &studenci[i]->pac_nr_kasy,&studenci[i]->pac_miejsce_zam,
		   &studenci[i]->l_przedm_stu);
   }
   for(i=0;i<licz;i++)
   {for(licz2=0;licz2<(studenci[i]->l_przedm_stu);licz2++)
   {(studenci[i]->przedmioty[licz2])=malloc(sizeof(struct kurs));
    fscanf(plikstud,"  %d%d%f%f%f",&licz3,&(studenci[i]->przedmioty[licz2])->nazwa_przedm,
                    &(studenci[i]->przedmioty[licz2])->ocena_egz,
					&(studenci[i]->przedmioty[licz2])->ocena_cwi,
					&(studenci[i]->przedmioty[licz2])->ocena_lab);
   }
   }
   fscanf(plikstud,"%d",&l_nazw);
   for(i=0;i<l_nazw;i++)
   {fscanf(plikstud,"%s",txt_nazw);
    Nazwiska[i]=malloc(strlen(txt_nazw)+1);
    strcpy(Nazwiska[i],txt_nazw);
   }
   fscanf(plikstud,"%d",&l_imion);
   for(i=0;i<l_imion;i++)
   {fscanf(plikstud,"%s",txt_imie);
	Imiona[i]=malloc(strlen(txt_imie)+1);
	strcpy(Imiona[i],txt_imie);
   }
   fscanf(plikstud,"%d",&l_miast);
   for(i=0;i<l_miast;i++)
   {fscanf(plikstud,"%s",txt_miast);
	Miasta[i]=malloc(strlen(txt_miast)+1);
	strcpy(Miasta[i],txt_miast);
   }
   fscanf(plikstud,"%d",&l_prz_tab);
   for(i=0;i<l_prz_tab;i++)
   {fscanf(plikstud,"%s",temp);
	Przedm[i]=malloc(strlen(temp)+1);
	strcpy(Przedm[i],temp);
   }
   if(fclose(plikstud))
   {ret=dana_koment(-1,-1,"blad przy zamykaniu pliku docelowego");
   }
  }
}
/*********************************************************************/
int dec_pacjent(int decyzja, int kod_decyzji, int nr_dec,int kod_raportu, int np, int *nr_rekordu)
 {int pozycja,temp;
   switch(kod_decyzji)
   {case DEC_NAZW:pozycja=szukaj_tekstu(Nazwiska, txt_nazw, l_nazw, LNAZW);
                   dStud->pac_nazwisko=pozycja;
                   if(pozycja>=l_nazw) l_nazw=pozycja+1;
                   break;
    case DEC_IMIE:pozycja=szukaj_tekstu(Imiona, txt_imie, l_imion, LIMION);
                  dStud->pac_imie=pozycja;
                  if(pozycja>=l_imion) l_imion=pozycja+1;
                  break;
    case DEC_MIASTO:pozycja=szukaj_tekstu(Miasta, txt_miast, l_miast, LMIAST);
                    dStud->pac_miejsce_zam=pozycja;
                    if(pozycja>=l_miast) l_miast=pozycja+1;
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
                     case 'r':Sz.liczby=1;
                              Sz.text=0;
                              break;
                     case 'e':Sz.liczby=1;
                              Sz.text=0;
                              break;
                     default:break;
					}
                    break;
    case DEC_WZOR_T:tb=przeszukaj_t(Sz.pole,Sz.wzor_t);
			        tempor=show(tb);
				    break;
    case DEC_WZOR_L:tb=przeszukaj_l(Sz.pole,Sz.znak,Sz.wzor_l);
			        tempor=show(tb);
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
                   case 'b': nr_opcji=-1;
                   default: break;
				  }
                  break;
    case DEC_INDEX:break;
    case DEC_DATAD:break;
	case DEC_DATAM:break;
    case DEC_DATAR:break;
	case DEC_ROK_ST:break;
	case DEC_LPRZ:for(temp=0;temp<l_przedm;temp++)
				  {dStud->przedmioty[temp]=(struct kurs*)malloc(sizeof(struct kurs));
	               (dStud->przedmioty[temp])->ocena_egz=5.0;
                   (dStud->przedmioty[temp])->ocena_cwi=5.0;
                   (dStud->przedmioty[temp])->ocena_lab=5.0;
				   txt_przedm[temp][0]='\0';
				   tmp_przedm[temp][0]='\0';
				  }
			      (dStud->l_przedm_stu)=l_przedm;
			      break;
	case DEC_PRZEDM:temp=0;
			        while( strcmp(txt_przedm[temp],tmp_przedm[temp])==0 ){temp++;}
			        pozycja=szukaj_tekstu(Przedm, txt_przedm[temp], l_prz_tab, LPRZEDM);
                    (dStud->przedmioty[temp])->nazwa_przedm=pozycja;
                    if(pozycja>=l_prz_tab) l_prz_tab=pozycja+1;
                    strcpy(tmp_przedm[temp],txt_przedm[temp]);
			        break;
	case DEC_POMIAR: return 5;// pomiar RT
		            break;
    default: break;
   }
  return kod_raportu;
}


int PobierzFix(char *sciezka)
{
	int dana=-1;
	FILE *fp;
	fp=fopen(sciezka,"r");
	if(fp==NULL) MessageBox(NULL,"Brak pliku fix.dat","B��d I/O",MB_OK);
	 else
	 { fscanf(fp,"%d",&dana);
	   fclose(fp);
	 }
	 return dana;
}

int ZapiszFix(char *sciezka, int dana)
{
	FILE *fp;
	fp=fopen(sciezka,"w");
	if(fp==NULL) MessageBox(NULL,"Fix!!!!","B��d",MB_OK);
	 else
	 { fprintf(fp,"%d",dana);
	   fclose(fp);
	 }
	 return dana;
}