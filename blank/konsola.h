/*-----------------------------------------------------

  Plik: konsola.h             
  Opis: W pliku znajduja sie:
        - definicje funkcji, stale itd...
          dla systemu WEWY kompilowanego
          w srodowisku WINDOWS NT
        jako konsola.
  
  Wersja: 1.0.4
  Data:  20.02.2002

  Kontakt: gonzo77@poczta.fm

-----------------------------------------------------*/

#ifndef _WIN_CONSOLE_H_
#define _WIN_CONSOLE_H_
// hedery
#include <windows.h>
// stale symboliczne
#define MAX_WART_BUFORA_BLEDU 10 // do funkcji SprawdzFunkcje
// inicjacja konsoli
#define _WIN_CONSOLE_
#ifdef _WIN_CONSOLE_
#define _DOS_
#undef GRAF
// deklaracje zmiennikow (funkcje brakujace) 
void sound(unsigned frequency);
void nosound(void);
void delay(unsigned milliseconds);
void Cls(void);
void SprawdzFunkcje(BOOL ret, PSTR nazwa_funkcji);
void check_services(void); // funckja dla trybu RT 
#endif
#endif

