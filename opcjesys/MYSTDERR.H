#ifndef _MYSTDERR_
#define _MYSTDERR_
#include <stdio.h>
#ifdef MULTSCR
#define mystderr stderr
#define myfprintf fprintf
#define tmyfprintf fprintf
#else
#undef mystderr
/*#ifdef _DEF_SYS_ */
#ifdef _MAIN_MODUL_
FILE *mystderr=NULL;
char *stderr_name=NULL;
#include <stdarg.h>
int myfprintf(FILE *file, char *format, ...)
 {va_list arg;
	int ret, flaga=0;
/*	if(file==NULL && format[0]=='!')
	 {file=fopen(stderr_name,"wa"); flaga=1;} */
	va_start(arg,format);
	ret=vfprintf(file,format,arg);
	va_end(arg);
//  if(flaga==1) {fclose(file); file=NULL;}
	return ret;
 }
#else
extern FILE *mystderr;
extern char *stderr_name;
#endif
#endif
#endif