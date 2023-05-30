/*
==========================================================
  Plik: ../opcjesys/scr_size.h
  Ustawienia granic ekranu dla systemów:
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
#ifdef _QNX_  
shint MY_MAX=24;
shint MYR_MAX=23;
shint MX_MAX= 80;
shint MXR_MAX=79;
shint X_L0=0;
shint Y_G0=0;
#endif  // koniec dla QNX
#ifdef _DOS_
#ifndef _CVC_
shint MYR_MAX=24;
shint MY_MAX=25;
shint MX_MAX=80;
shint MXR_MAX=79;
shint X_L0=1;
shint Y_G0=1;
#endif
#endif  // koniec dla DOS
#ifdef _CVC_
shint MYR_MAX=24;
shint MY_MAX=25;
shint MX_MAX=80;
shint MXR_MAX=79;
shint X_L0=0;
shint Y_G0=0;
#endif
#else           /* else dla ifdef WEW_EXT */
WEW_EXT shint MY_MAX;
WEW_EXT shint MYR_MAX;
WEW_EXT shint MX_MAX;
WEW_EXT shint MXR_MAX;
WEW_EXT shint X_L0;
WEW_EXT shint Y_G0;
#endif           /* koniec dla ifdef WEW_EXT */
#endif           /* koniec dla ifdef _SCR_SIZE_H_ */