/*
==========================================================
  Plik: ../opcjesys/scr_size.h
  Ustawienia granic ekranu dla systemow:
  - QNX,
  - DOS,
  - WINDOWS.
  Data: 02/07/2001
  Kontakt: gonzo77@poczta.fm
==========================================================
*/
#ifndef _SCR_SIZE_H_
#define _SCR_SIZE_H_
#ifndef WEWY_MAIN
#define WEW_EXT extern
#endif
#ifndef WEW_EXT
int MYR_MAX=24;
int MY_MAX=25;
int MX_MAX=80;
int MXR_MAX=79;
int X_L0=0;
int Y_G0=0;
#else           /* else dla ifdef WEW_EXT */
WEW_EXT int MY_MAX;
WEW_EXT int MYR_MAX;
WEW_EXT int MX_MAX;
WEW_EXT int MXR_MAX;
WEW_EXT int X_L0;
WEW_EXT int Y_G0;
#endif           /* koniec dla ifdef WEW_EXT */
#endif           /* koniec dla ifdef _SCR_SIZE_H_ */
