/*---------------------------------
Data:  18.11.2001

Autor: gonzo77@poczta.fm
---------------------------------*/

#include "blank/moje.h"
#include "admin.h"

void chk_time(void);
void PrzegladajLog(void);//char *buffor[20]);
int InfoSystem(int ret);

int nr_poz=-1;
extern struct Service *Service;
extern struct agenda *Agenda;
extern unsigned int attr_title;
extern unsigned int attr, at_wpis;
extern int X_time, X_tyt;
extern unsigned int cursor, nocursor;

/* ================================================================ */
int admin_blankiet(int nr_rekordu, int ob_pocz,	int ob_konc,
									   int x_lewy_gorny,	int y_lewy_gorny,
									   int anim_pid, char *D)
{
	int ret=-1, nr_rek;
	struct admin *d0, *dp, *d;

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
	{ char *admin_menu[]={"Przegladanie pliku log.dat","Informacje o systemie"};
	  int l_poz_menu = 2;
      nr_poz=okno_menu(admin_menu,l_poz_menu,nr_poz,attr, at_wpis, 10,25,-1," WYBIERZ OPCJE ",1);
	}
	switch(nr_poz)
	{ case 0: PrzegladajLog(); break;
	  case 1: InfoSystem(ret); break;
	  default: break;
	}
	nr_poz=-1;
	return ret;
 }

char *dane_admin(int ob_pocz, int ob_konc, int *rozmiar_ob)
 {
	static struct Service *S;
    struct admin *St;
	int x,y;
    S=Service;
	*rozmiar_ob=S->str_size;
	St=(struct admin *)Malloc(S->l_rek_max*(*rozmiar_ob));
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
    char line[256];
    const char *logpath = "/tmp/stud_sys.log";
    fp = fopen(logpath, "r");
    if (fp == NULL)
    {
        dana_koment(MY_MAX, X_L0, "+ Blad otwarcia logu: %s", logpath);
        return;
    }
    dana_koment(-1, -1, "+ --- Log: %s ---", logpath);
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') line[len-1] = '\0';
        dana_koment(-1, -1, "+ %s", line);
    }
    fclose(fp);
}

/*-------------------------
  informacje o systemie
  ...podstawowe informacjie 
  o systemie na k�rym 
  pracuje podsystem
-------------------------*/

int InfoSystem(int ret)
{
	ret=dana_koment(-1,20,"+ Informacje o systemie");
	ret=dana_koment(-1,-1,"+ ");
	ret=dana_koment(-1,-1,"+ System: Linux/Unix (ncurses)");
	ret=dana_koment(-1,-1,"+ ");
	return ret;
}
