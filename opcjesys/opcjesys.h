#ifndef _OPCJESYS_H_INCLUDED_	//dolaczanie samego siebie
#define _OPCJESYS_H_INCLUDED_
/* ===================== Wybor kompilatora ======================= */
//#define _CBC_  // Borland C
//#define _CWC_  // WATCOM C
#define _CVC_  // Visual C
//#define _GNUC_ // Gnu C
/* ===================== Wybor systemu operacyjnego ============== */
//#define _QNX_  // QNX
//#define _DOS_
#define _WNT_ // Windows NT
//#define _W98_ // Windows'98
//#define _BOSS_SYS_ //to jest zadanie BOSS'a
/* ==================== Ustawienia katalogow ========================== */
#ifdef _QNX_
 #define KAT_GLOWNY "/home/jdu/ver30_11/"   // glowny katalog systemu
 #define KAT_DOSA "/dos/c/qnx/"    // katalog glowny zbiorow dos'a
#else
 #define KAT_GLOWNY "../qnx_cop/"   // glowny katalog systemu
 #define KAT_DOSA "c:/qnx/"         // katalog glowny zbiorow dos'a
#endif
#define KAT_LEN 120                // max.dlugosc nazwy katalogu
#define CONC(A,B) A##B
/* ------------------------ katalog biblioteki wewy ----------------- */
#define _PATH_WEWY_LIB_ "../blank/"		//jako *obj lub *lib
#define _PATH_WEWY_H_   "../blank/"       //jako *h
#define _WEWY1_H_  			"../blank/wewy1.h"
#define _WEWYBL_H_ 			"../blank/wewybl.h"
#define _WEWY_C_ 				"../blank/wewy.c"
/*------------------------- czy jest wiele konsol ------------------ */
#ifdef _QNX_
#define MULTSCR  // opcja wieloterminalowosci (stderr jest oddz.oknem)
#endif
//-- dolaczamy deklaracje typow specjalnych: shint, pid_t, nid_t --------
#include "../opcjesys/shintdef.h"
//-- deklaracje dla pliku logujacego
#include "../opcjesys/mystderr.h"
/* ==================== Ustawienia opcji pliku ====================== */
#ifdef _BOSS_
 #define _RT_MODE_
#endif
//-------------- QNX ------------------------------------------------
#ifdef _QNX_
 #undef _DOS_
 #undef _WNT_
 #undef _W98_
// #define myfprintf fprintf
 #include <sys/console.h>
 #include <sys/dev.h>
 #include <sys/fd.h>
 #include <sys/kernel.h>
 #include <sys/mouse.h>
 #include <sys/name.h>
 #include <sys/osinfo.h>
 #include <sys/proxy.h>
 #include <sys/psinfo.h>
 #include <sys/qnxterm.h>
 #include <sys/qioctl.h>
 #include <sys/qnx_glob.h>
 #include <sys/stat.h>
 #include <sys/seginfo.h>
 #include <sys/types.h>
 //#include <unix.h>      //w C++ gryzie sie z term.h - przeladowania: jedna jest const char*, a druga char*
 //ale gdzies byla uzyta
 #include <sys/vc.h>
 #if defined(_TERM_WEWY_)  |  defined(_TERM_PRINTF_)
	#define _TERM_QNX_
		#include _WEWYBL_H_
//#include "../blank/wewybl.h"
	#include <sys/dev.h>
	#include <sys/qnxterm.h>
	#include <sys/console.h>
	#include <sys/types.h>
	#include <term.h>
	#include <signal.h>
	#include <sys/qioctl.h>
	void exit(shint status), clear_cur(char);
	#include "../aj/dim.h"
 #endif
//------- specyfika innych systemow niz QNX
#else

#ifdef _CVC_
#include <windows.h> // heder œrodowiska WINDOWS u¿ywany dla _TEXT_ & _GRAF_
#define _WIN_GRAF_
// #define _WIN_TEXT_
#ifdef _WIN_GRAF_// tryb okien windows API
#define GRAF
#define getch VCGetch // Visual C Getch
#undef MULTSCR   // log bedzie na dysku
#ifdef _MAIN_MODUL_
HDC hdc; // zmienne globalna uchwyt okna g³ównego (pra pra ojca)
//MSG msg; // struktura komunikatów
#else
extern HDC hdc;
extern MSG msg;
#endif
//#include "../win/graf.c" // funkcje dla trybu graficznego
#endif
#ifdef _WIN_TEXT_// tryb konsoli w systemie windows
#define _DOS_
#include "../win/consol.c" // funkcje dla trybu tekstowego (konsola)
#undef MULTSCR   // log bedzie na dysku
#ifdef _MAIN_MODUL_
HDC hdc; // zmienne globalna uchwyt okna g³ównego (pra pra ojca)
#else
extern HDC hdc;
#endif
#endif
// .....
#endif
 // ---- plik wewy.c dla innych systemow niz QNX
 #undef _TERM_QNX_
 #if defined(_TERM_WEWY_)|defined(_TERM_PRINTF_)
	#include _WEWYBL_H_
 #endif
#endif
#include <process.h>
#ifdef _MAIN_MODUL_
 #define _DEF_SYS_
#endif
/* ---------------------------------------------------------------- */
#ifdef _DOS_
 #undef MULTSCR
typedef int pid_t;
typedef int timer_t;
#endif
#ifdef _TERM_WEWY_
 #undef _TERM_PRINTF_
 #include _WEWYBL_H_
 //#include CONC(_PATH_WEWY_H,scr_size.h)
 #include "../blank/scr_size.h"
#endif
#ifdef _TERM_PRINTF_
 #define RAP wewyRAP
 #define WEWY0_INCL // ---- znak dla wewy.c ze ma juz 'zaincludowane' "wewy.c"
 #include _WEWY_C_
 #undef RAP
#endif

//------------------
#ifdef _BOSS_SYS_
#include "../aj/dim.h"
#endif


/* ------------------------------------------------------------------
	 Stale tekstowe KAT_GLOWNY i KAT_DOSA sa wykorzystywane
	 we wszystkich odwolaniach do katalogow.
	 Dla innego systemu trzeba oczywiscie inaczej zdefiniowac KAT_GLOWNY
	 i KAT_DOSA oraz ewentualnie, zwiekszyc KAT_LEN.
	 W niektorych przypadkach potrzebne jest korzystanie z katalogow
	 w komputerze glownym. Wowczas nazwa katalogu jest tworzona w sposob
	 SPECYFICZNY dla QNX, tak aby np. dla komp.6 bylo:  //6/home/jdu/.itd..
	 Uzyskuje sie to instrukcja:
	 {char .nzb.[KAT_LEN];
		sprintf(.nzb., "//%d%s.nkat.", .nr_komp.., KAT_GLOWNY);
		.nfil.=fopen(.nzb.,"r");
	 Przykladowo, w pliku aj/symul.c:
		{FILE *startf=NULL;
		 char zb_st[KAT_LEN];
			sprintf(zb_st,"//%d%sstart.tim",base_node,KAT_GLOWNY);
			startf=fopen(zb_st,"r");
		 ....
		 }
	 Wszystkie konstrukcje tego typu mozna znalezc dyrektywa systemu oper.:
			grep "\"//%d%s" *.c
	 i zmienic wg potrzeb systemu operacyjnego
	 -------------------------------------------------------------------- */
#define ZAP_MODULO 86400
#endif //koniec dolaczania pliku

