/* Jedyna obslugiwana platforma: terminal ncurses (build ustawia -D_NCURSES_) */

#ifndef _MOJE_H_
#define _MOJE_H_

#include "term_ncurses.h"
/* typedef short int; */  /* Invalid typedef - removed */
#define _CVC_  /* Enable console mode */

#include "../opcjesys/opcje.h"
#include "../blank/sys_dekl.h"
#define DEC_NEW    1
#define DEC_RUN    2
#define DEC_TYP_US 3
#define DEC_TIME   4
#define DEC_DELAY  5

#endif

