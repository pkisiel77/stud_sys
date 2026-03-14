#ifndef KONFIG_H
#define KONFIG_H

#include "blank/moje.h"   /* struct Service, struct agenda */

#define L_SYS 6

extern struct agenda *SysA[];

extern char *Menu[L_SYS + 3];
extern int (*Fun_sys[L_SYS + 1])();
extern int (*dialog[L_SYS + 1])();
extern struct Service *Serv[L_SYS + 1];

int set_services(void);

#endif /* KONFIG_H */
