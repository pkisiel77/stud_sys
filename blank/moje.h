/* Platform selection - define one of: _NCURSES_, _DOS_, _QNX_ */
#ifndef _NCURSES_
#ifndef _QNX_
#define _DOS_
#endif
#endif

#ifndef _MOJE_H_
#define _MOJE_H_

#ifdef _NCURSES_ // ncurses (Linux/Unix)
#include "term_ncurses.h"
typedef short int shint;
#define _CVC_  /* Enable console mode */
#endif // koniec NCURSES

#ifdef _QNX_ // QNX
#include <sys/qnxterm.h>
#include <malloc.h>
FILE *fin, *fout, *ferr;
typedef short int shint;
#endif // koniec QNX

#ifdef _DOS_ // Windows/DOS
#include <windows.h>
#endif // koniec DOS

#include "../opcjesys/opcje.h"
#include "../blank/SYS_DEKL.H"
#define DEC_NEW    1
#define DEC_RUN    2
#define DEC_TYP_US 3
#define DEC_TIME   4
#define DEC_DELAY  5

#endif

