/*---------------------------------
Data:  18.11.2001

Autor: gonzo77@poczta.fm
---------------------------------*/

#include "blank/moje.h"
#include "admin.h"
#include <mmsystem.h>

void chk_time(void);
void PrzegladajLog(void);//char *buffor[20]);
int InfoSystem(int ret);

int nr_poz=-1;
extern struct Service *Service;
extern struct agenda *Agenda;
extern unsigned int attr_title;
extern int X_time, X_tyt;
extern unsigned int cursor, nocursor;

/* ================================================================ */
int admin_blankiet(int nr_rekordu, int ob_pocz,	int ob_konc,
									   int x_lewy_gorny,	int y_lewy_gorny,
									   int anim_pid, char *D)
{
	int ret=-1, ochr=5, ochrf=-1, size, nr_rek;
	struct admin *d0, *dp, *d;
  // kolory menu administratora 
  unsigned int attr=TERM_WHITE_BG|TERM_BLACK|TERM_FLUSH;
  unsigned int at_wpis=TERM_WHITE|MTERM_HILIGHT|TERM_BLUE_BG|TERM_FLUSH;

  setcursor(cursor);
  d0=(struct admin *)D;
  nr_rek=nr_rekordu;
  dp=d0+nr_rek;
  d=(struct admin *)dane_raportowanego_rekordu(admin_blankiet,&nr_rek);
	if(d==NULL || d!=dp)
	 {term_printf(MY_MAX,X_L0,ATTR_A," Niezgodnosc adresow w RAP_ZM(d=%p dp=%p). <Ent> ",d,dp);
		GET_char();  return -1;
	 }
	rekord_danych_do_naglowka(nr_rek);
    ret=dana_koment(-1,-1,"+ ");  
/*
	if(nr_poz==-1)
	{ char *admin_menu[]={"Przegladanie pliku log.dat","Informacje o systemie"};
	  int l_poz_menu = 2;
      nr_poz=okno_menu(admin_menu,l_poz_menu,nr_poz,attr, at_wpis, 10,25,-1," WYBIERZ OPCJE ",1);
	}
	switch(nr_poz)
	{ case 0: PrzegladajLog();break;
	  case 1: InfoSystem(ret);break;
	default:break;
	}
*/
	InfoSystem(ret);
	return ret;
 }

char *dane_admin(int ob_pocz, int ob_konc, int *rozmiar_ob)
 {
	static struct Service *S;
	static struct agenda *A;
    struct admin *St;
	int x,y;
    S=Service;
	*rozmiar_ob=S->str_size;
	St=Malloc(S->l_rek_max*(*rozmiar_ob));
	x=m_wherex(); y=m_wherey(); setcursor(nocursor);
	term_printf(Y_G0,X_tyt,attr_title,"%s",S->name);
	term_cur(y,x); setcursor(cursor);
	return (char *)St;
}

void wpis_admin(int ob_pocz, int ob_konc, char *D, int rozmiar_ob, char zapis[], char *Kod_op)
{
}

int dec_admin(int decyzja, int kod_decyzji, int nr_dec, int kod_raportu, int np, int *nr_rekordu)
{
    struct admin *ad;
	int ret,i;
	int suma=0;
	char buf[128];

	switch(kod_decyzji)
	 {case DEC_GEN_KOD:
	 	 switch(decyzja)
		 {case 't': 
		  break;
		  default: break;
		 }
		break;
		default: break;
	}
	return kod_raportu;
}
/*------------------------DODATKOWE FUNKCJIE------------------------------*/
void PrzegladajLog(void)
{
	FILE *fp;
	char buf[20];
	char znak;
    fp=fopen("c:\\pacjent\\log\\log.dat","r");
	if(fp==NULL) dana_koment(MY_MAX,X_L0,"Blad otawrcia pliku LOG.DAT");
	 else
	 {
		 while((znak=fgetc(fp))!=EOF)
		 {printf("%c",znak);}
		 
		 fclose(fp);
	 }
}

/*-------------------------
  informacje o systemie
  ...podstawowe informacjie 
  o systemie na kórym 
  pracuje podsystem
-------------------------*/

int InfoSystem(int ret)
{
	char *text;
	char buffor[128];
	int zwrot,retu;
	// struktury 
	OSVERSIONINFO vi;
	SYSTEM_INFO   si;
    // pocz¹tek
    ret=dana_koment(-1,20,"+ Informacje o systemie");
    ret=dana_koment(-1,-1,"+ ");
    ret=dana_koment(-1,-1,"+ System:");
	 vi.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	 zwrot=GetVersionEx(&vi);
      if(zwrot) 
	  {
		if(vi.dwPlatformId==VER_PLATFORM_WIN32s)        text="3.1";  
        if(vi.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS) text="98";
        if(vi.dwPlatformId==VER_PLATFORM_WIN32_NT)      text="NT" ;
        ret=dana_koment(-1,-1,"+  Windows %s",text);	    
	    ret=dana_koment(-1,-1,"+  %d.%.2d.%d",vi.dwMajorVersion,vi.dwMinorVersion,vi.dwBuildNumber);
	    ret=dana_koment(-1,-1,"+  %s",vi.szCSDVersion);
	  }
	 ret=dana_koment(-1,-1,"+ ");
	 ret=dana_koment(-1,-1,"+ Procesor:");
	  GetSystemInfo(&si);
	   if(si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_INTEL) text=" Architektura: INTEL";
	   if(si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_MIPS) text=" Architektura: MIPS";
	   if(si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_ALPHA) text=" Architektura: ALPHA";
	   if(si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_PPC) text=" Architektura: PPC";
       if(si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_UNKNOWN) text=" Architektura: Nieznana";
	    ret=dana_koment(-1,-1,"+ %s",text);
       if(si.dwProcessorType==PROCESSOR_INTEL_386) text=" Typ: INTEL_386";
       if(si.dwProcessorType==PROCESSOR_INTEL_486) text=" Typ: INTEL_486";
	   if(si.dwProcessorType==PROCESSOR_INTEL_PENTIUM) text=" Typ: INTEL_PENTIUM";
       if(si.dwProcessorType==PROCESSOR_MIPS_R4000) text=" Typ: MIPS_R4000";
       if(si.dwProcessorType==PROCESSOR_ALPHA_21064) text=" Typ: ALPHA_21064";
	    ret=dana_koment(-1,-1,"+ %s",text);
	   ret=dana_koment(-1,-1,"+  Liczba w systemie: %d",si.dwNumberOfProcessors);   
	 ret=dana_koment(-1,-1,"+ ");
	 ret=dana_koment(-1,-1,"+ Pamiec:");
	 ret=dana_koment(-1,-1,"+  196 084 KB");
	// koniec
	retu=ret;
	return retu;
}
