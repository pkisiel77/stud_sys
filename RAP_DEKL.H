/* ======================= tu sie zaczyna rap_bl1.c ---------------- */
#ifndef _RAP_INCL_
#define _DOS_
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
/* #include <sys/qnxterm.h> */
#include <math.h>
#include <time.h>

#ifdef _DOS_
#include <dos.h>
#include <alloc.h>
#define Malloc farmalloc
#define Free farfree
#define TERM_CLS_SCR 1
typedef  int shint;
typedef  int pid_t;
#else
#include <malloc.h>
#include <sys/qnxterm.h>
typedef  short int shint;
#endif
#include "wewybl.h"
#include "msor_bl.h"
extern shint nr_wegla, nr_ukladu;
extern struct nazwa_sorbatu NSOR[];
extern struct dane_sorbatu DSOR[];
/* ------------------------------------------------------------- */
void term_clear(int co);
extern unsigned short int NODE_NO;
extern shint czy_z_dysku;
extern signed char _TYP_KONSOLI_;
extern shint lmall;
extern char nastawy, *pathd, *pathf;
#endif
extern char katalog[], *sorwm[2];
