#include "blank/moje.h"
#include "blank/sys_dekl.h"
#ifdef _NCURSES_
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
#ifdef _NCURSES_
// Define Windows drive type constants for ncurses
#define DRIVE_UNKNOWN 0
#define DRIVE_NO_ROOT_DIR 1
#define DRIVE_REMOVABLE 2
#define DRIVE_FIXED 3
#define DRIVE_REMOTE 4
#define DRIVE_CDROM 5
#define DRIVE_RAMDISK 6
#endif
void chk_time(void);
extern struct Service* Service;
extern struct agenda* Agenda;
extern unsigned int attr_title;
extern int X_time, X_tyt;
// extern unsigned int cursor, nocursor;
extern unsigned int attr, at_wpis;
//int l_stud=1;
#include "uruchom.h"
#define DEC_USUN 1
//char **Nazwiska=NULL, **Imiona, **Miasta, **Kod;
char **Sciezka, **Roz;

char** Dysk;

int szukanie = 0;

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
int lsciezka = 0;
int lroz = 0;
int ldysk = 0;
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
int uruchom_blankiet(int nr_rekordu, int ob_pocz, int ob_konc, int x_lewy_gorny, int y_lewy_gorny, int anim_pid,
                     char* D)
{
    int ret, nr_rek;
    // int size, ochr, ochrf = -1;
    struct uruchom *d0, *dp, *d;
    static char decy = 'n';
    char buf[128];
    // static char txt_kod[15];
    // int lkod = 15;
    //	txt[0]=0; txt[1]=0;
    setcursor(cursor);
    /*sledzenie(); */
    d0 = (struct uruchom*)D;
    nr_rek = nr_rekordu;
    dp = d0 + nr_rek;
    d = (struct uruchom*)dane_raportowanego_rekordu(uruchom_blankiet, &nr_rek);
    if (d == NULL || d != dp)
    {
        term_printf(MY_MAX, X_L0,ATTR_A, " Niezgodnosc adresow w RAP_ZM(d=%p dp=%p). <Ent> ", d, dp);
        GET_char();
        return -1;
    }
    rekord_danych_do_naglowka(nr_rekordu);

#define OCHR_WIZYTA 5
#define OCHR_POMIAR 5
#define DL_IMIE 25
#define DL_NAZW 40
#define DL_WIEK 3
#define DL_OX 3
#define DL_OY 3

#define DL_SCIEZKI 20
#define DL_ROZ 3
#define DL_DYSK_CHARS 1
#define DL_DYSK_STR (DL_DYSK_CHARS + 1)

    {
        static char dysk_txt[DL_DYSK_STR] = "C";
        static char* dysk_menu[] = { "C", "D" };
        const int dysk_menu_count = (int)(sizeof(dysk_menu) / sizeof(dysk_menu[0]));
        int wynik;

        ret = dana_text_dec(-1, -1, "+ Wpisz litere dysku", dysk_txt, DL_DYSK_CHARS,
                            dysk_menu, dysk_menu_count, OCHR_POMIAR, DEC_DYSK);

        if (dysk_txt[0] == EOS)
        {
            ret = dana_koment(-1, -1, "+ Nie podano litery dysku");
            return ret;
        }

#ifdef _NCURSES_
        {
            struct stat st;
            char unix_root[PATH_MAX];

            if (dysk_txt[0] == 'C' || dysk_txt[0] == 'c')
            {
                snprintf(unix_root, sizeof(unix_root), "/");
            }
            else
            {
                snprintf(unix_root, sizeof(unix_root), "/Volumes/%c", dysk_txt[0]);
            }

            snprintf(buf, sizeof(buf), "%s", unix_root);
            if (stat(unix_root, &st) == 0 && S_ISDIR(st.st_mode))
            {
                wynik = DRIVE_FIXED;
            }
            else
            {
                wynik = DRIVE_NO_ROOT_DIR;
            }
        }
#else
        snprintf(buf, sizeof(buf), "%c:\\", dysk_txt[0]);
        wynik = GetDriveType(buf);
#endif
        switch (wynik)
        {
        case DRIVE_UNKNOWN:     ret = dana_koment(-1, -1, "+ <DRIVE_UNKNOWN>"); break;
        case DRIVE_NO_ROOT_DIR: ret = dana_koment(-1, -1, "+ <DRIVE_NO_ROOT_DIR>"); break;
        case DRIVE_REMOVABLE:   ret = dana_koment(-1, -1, "+ <DRIVE_REMOVABLE>"); break;
        case DRIVE_FIXED:       ret = dana_koment(-1, -1, "+ <DRIVE_FIXED>"); break;
        case DRIVE_REMOTE:      ret = dana_koment(-1, -1, "+ <DRIVE_REMOTE>"); break;
        case DRIVE_CDROM:       ret = dana_koment(-1, -1, "+ <DRIVE_CDROM>"); break;
        case DRIVE_RAMDISK:     ret = dana_koment(-1, -1, "+ <DRIVE_RAMDISK>"); break;
        default: break;
        }
    }

    {
        static char* dec_dysk[] = {"t tak", "n nie"};


        ret = dana_decyzyjna(-1, -1, "+ Pokazac zawartosc katalogu glownego %s  ?? ", "t/n", dec_dysk, 2,
                             &decy, OCHR_POMIAR, DEC_DYSK);
    }

    if (decy == 't')
    {
#ifdef _NCURSES_
        char* nazw_plik[256];
        int i = 0;
        int opcje;
        int j;
        DIR* dir;
        struct dirent* wpis;
        const int max_poz = (int)(sizeof(nazw_plik) / sizeof(nazw_plik[0]));

        dir = opendir(buf);
        if (dir == NULL)
        {
            ret = dana_koment(-1, -1, "+ Blad katalogu: %s (%s)", buf, strerror(errno));
            decy = 'n';
            return ret;
        }

        while ((wpis = readdir(dir)) != NULL && i < max_poz)
        {
            size_t len;
            char* temp;

            if (strcmp(wpis->d_name, ".") == 0 || strcmp(wpis->d_name, "..") == 0)
            {
                continue;
            }

            len = strlen(wpis->d_name);
            temp = (char*)malloc(len + 1);
            if (temp == NULL)
            {
                break;
            }
            memcpy(temp, wpis->d_name, len + 1);
            nazw_plik[i] = temp;
            i++;
        }
        closedir(dir);

        if (i == 0)
        {
            ret = dana_koment(-1, -1, "+ Katalog pusty: %s", buf);
        }
        else
        {
            opcje = okno_menu(nazw_plik, i, 0, attr, at_wpis, 4, 25, -1, NULL, 1);
            if (opcje >= 0 && opcje < i)
            {
                ret = dana_koment(-1, -1, "+ Wybrano: %s/%s", buf, nazw_plik[opcje]);
            }
        }

        for (j = 0; j < i; j++)
        {
            free(nazw_plik[j]);
        }
        decy = 'n';
#else
        static char *nazw_plik[100], *temp;
        int opcje, i = 0;
        WIN32_FIND_DATA nazwa;
        BOOL n_opcje;


        {
            char buff[128];
            snprintf(buff, sizeof(buff), "%s*.*", buf);
            opcje = FindFirstFile(buff, &nazwa);
            if (opcje == -1) MessageBox(NULL, "B��d znajdowania plik�w", "B��d", MB_OK);
            else
            {
                temp = (char*)malloc(strlen(nazwa.cFileName) + 1);
                if (temp == NULL) { FindClose(opcje); return ret; }
                snprintf(temp, strlen(nazwa.cFileName) + 1, "%s", nazwa.cFileName);
                nazw_plik[i] = temp;
                do
                {
                    n_opcje = FindNextFile(opcje, &nazwa);
                    if (opcje != -1)
                    {
                        temp = (char*)malloc(strlen(nazwa.cFileName) + 1);
                        if (temp == NULL) break;
                        i++;
                        snprintf(temp, strlen(nazwa.cFileName) + 1, "%s", nazwa.cFileName);
                        nazw_plik[i] = temp;
                    }
                }
                while (n_opcje);
                FindClose(opcje);
            }
            opcje = okno_menu(nazw_plik, i, 0, attr, at_wpis, 4, 25, -1, NULL, 1);
            if (opcje != -1)
            {
                snprintf(buff, sizeof(buff), "%s\\%s", buf, nazw_plik[opcje]);
                WinExec(buff, SW_SHOW);
            }
            decy = 'n';
        }
#endif // _NCURSES_
    }


    return ret;
}

char* dane_uruchom(int ob_pocz, int ob_konc, int* rozmiar_ob)
{
    static struct Service* S;
    // static struct agenda* A;
    struct uruchom* St;
    int x, y;

    S = Service;
    *rozmiar_ob = S->str_size;
    St = (struct uruchom*)Malloc(S->l_rek_max * (*rozmiar_ob));
    x = m_wherex();
    y = m_wherey();
    setcursor(nocursor);
    term_printf(Y_G0, X_tyt, attr_title, "%s", S->name);
    term_cur(y, x);
    setcursor(cursor);
    return (char*)St;
}

void wpis_uruchom(int ob_pocz, int ob_konc, char* D, int rozmiar_ob, char zapis[], char* Kod_op)
{
}

int dec_uruchom(int decyzja, int kod_decyzji, int nr_dec, int kod_raportu, int np, int* nr_rekordu)
{
    // int ret;
    switch (kod_decyzji)
    {
    case DEC_USUN:
        switch (decyzja)
        {
        case 'd': return kod_raportu;
        case 'u':
        case 'z':
            break;
        case 'm': return -1;
        default: break;
        }
        break;
    case DEC_SCIEZKA:
    case DEC_ROZ: szukanie++;
        break;
    case DEC_DYSK:
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
