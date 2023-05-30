#define _DOS_
#ifndef _MOJE_H_
#define _MOJE_H_
#ifdef _QNX_ // QNX
#include <sys/qnxterm.h>
#include <malloc.h>
FILE *fin, *fout, *ferr;
typedef short int shint;
#endif // koniec QNX
#include "../opcjesys/opcje.h"
#include "../blank/sys_dekl.h"
#define DEC_NEW    1
#define DEC_RUN    2
#define DEC_TYP_US 3
#define DEC_TIME   4
#define DEC_DELAY  5
#include <windows.h>
#endif

