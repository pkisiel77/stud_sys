/*-----------------------------------------------------

  Plik: konsola.c             
  Opis: W pliku znajduj� si�:
        - deklaracje funkcji,
          dla systemu WEWY kompilowanego
          w �rodowisku WINDOWS NT lub ncurses
        jako konsola.
  
  Wersja: 1.0.5
  Data:  10.02.2026

  Kontakt: gonzo77@poczta.fm

-----------------------------------------------------*/

#ifndef _WIN_CONSOLE_C_
#define _WIN_CONSOLE_C_

#ifdef _NCURSES_
/* ncurses implementation */
#include "term_ncurses.h"
#include <stdio.h>

void InitConsole() {
    /* ncurses initialization is handled in term_ncurses.c */
    extern void InitConsole(void);
    InitConsole();
}

#else
/* Windows implementation */
#include "konsola.h"
#include <stdio.h>

#define BUFFER(x,y) *(ptr_buffer+y*cx_buffer+x)

// zmienna do okna konsoli
HWND handle_window_device;
//HDC handle_device_context;
/*---DEKLARACJE FUNKCJI KONSOLI---*/

void InitConsole() {

}
#endif  /* _NCURSES_ */

#ifndef _NCURSES_
/* Windows-specific functions */
void sound(unsigned frequency)
{
 BOOL ret;
 DWORD dwDuration = 1; // minimalna warto�� 1ms 

 ret = Beep(frequency, dwDuration);
 SprawdzFunkcje(ret,"Beep");
}

void nosound(void) {/* procedura pusta*/;} 
void delay(unsigned milliseconds) { Sleep(milliseconds);}
void Cls(void)
{
}
void window(int left, int top, int right, int bottom){;}
void SprawdzFunkcje(BOOL ret, PSTR nazwa_funkcji)
{
 TCHAR bufor[MAX_WART_BUFORA_BLEDU];
 int nr_bledu;

  if(ret!=TRUE)
   {nr_bledu = GetLastError();
    sprintf(bufor,"%d",nr_bledu);
//    MessageBox(NULL,TEXT("Numer b��du:\n")TEXT(bufor),"B��d procedury: " TEXT(nazwa_funkcji),MB_OK|MB_ICONERROR);
   }
}
#endif  /* _NCURSES_ */

#endif  /* _WIN_CONSOLE_C_ */
/*-----KONIEC----------*/
/* to dopiero poczatek */

#ifndef _NCURSES_
/*---------------------------------- 
  Biblioteka DLL dla podsystemu BOSS
  wersja 0.0.1 - 24.08.2002
  wersja 0.0.2 - 25.08.2002
  wersja 0.0.3 - 26.08.2002
  autor: gonzo77@poczta.fm
  opis: funkcja dla podsystemu BOSS w �rodowisku Windows 2000
        (plik term_dos.c)
----------------------------------*/

//#include "konsola.h"


/*---OPIS-----------

---KONIEC OPISU---*/

void term_console_read_events(void)
{
  MSG msg;
  PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
  TranslateMessage (&msg) ;
  DispatchMessage (&msg) ;
  //return msg;
}
/*
EXPORT int CALLBACK term_printf(HDC hdc,int x, int y, int attr, char *format,...)
{
 char buf_out[128];
 va_list arg;
 va_start(arg,format);
 vsprintf(buf_out,format,arg);
 TextOut(hdc,x,y,buf_out,strlen(buf_out));
 va_end(arg);
 return 0;
}

EXPORT HWND CALLBACK term_console_set_hwnd(HWND hwnd)
{
	static HWND new_hwnd=NULL;
	new_hwnd=hwnd;
	return new_hwnd;
}
*/
LRESULT CALLBACK term_console_events(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND term_console_create(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
     static TCHAR szAppName[] = TEXT ("Boss") ;
     HWND         hwnd ;
     MSG          msg ;
     WNDCLASS     wndclass ;
	 HDC hdc;



     wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
     wndclass.lpfnWndProc   = term_console_events ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = 0 ;
     wndclass.hInstance     = hInstance ;
     wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
     wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
     wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
     wndclass.lpszMenuName  = NULL ;
     wndclass.lpszClassName = szAppName ;
     
     if (!RegisterClass (&wndclass))
     {
          MessageBox (NULL, TEXT ("Ten program wymaga Windows NT/2000!"),
                      szAppName, MB_ICONERROR) ;
          return 0 ;
     }
     
     hwnd = CreateWindow (szAppName, TEXT ("Konsola dla podsystemu BOSS... wersja 0.0.3n 21-11-2002"),
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          81*8,26*13, // !!! tak nie mo�e by� 
                          NULL, NULL, hInstance, NULL) ;
     ShowWindow (hwnd, iCmdShow) ;
     UpdateWindow (hwnd) ;
     
	 hdc = GetDC(hwnd);
     SelectObject(hdc,GetStockObject(OEM_FIXED_FONT));


     return hwnd;
}

LRESULT CALLBACK term_console_events(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
     HDC         hdc;
	 TEXTMETRIC  tm;

	 static int cx_char,cy_char, cx_buffer, cy_buffer,
		        cx_client, cy_client;
	            

	 static TCHAR *ptr_buffer=NULL;
	 int x,y;
	 char buf[128];
		        

     
     switch (message)
     { 
	 case WM_CREATE:
		 hdc = GetDC(hwnd);
	     SelectObject(hdc,GetStockObject(OEM_FIXED_FONT));
		 GetTextMetrics(hdc,&tm);
		 cx_char=tm.tmAveCharWidth;
		 cy_char=tm.tmHeight;

		  cx_buffer=cx_char*80;
		  cy_buffer=cy_char*25;

//		  sprintf(buf,"x=%d y=%d",cx_char,cy_char);
//		  MessageBox(NULL,buf,"",MB_OK);

    	 DeleteObject(SelectObject(hdc,GetStockObject(OEM_FIXED_FONT)));
		 ReleaseDC(hwnd,hdc);
		 
	 case WM_SIZE:
		 // pobie� rozmiar okna w pikselach
		 if(message==WM_SIZE)
		 {
			 cx_client=LOWORD(lParam);
			 cy_client=HIWORD(lParam);
		 }

    	 // wyznacz rozmiar okna w znakach
		 cx_buffer=max(1,cx_client/cx_char);
		 cy_buffer=max(1,cy_client/cy_char);

		 // przydziel pami�� na bufor i wykasuj j�
		 
		 if(ptr_buffer != NULL)
			 free(ptr_buffer);

		 ptr_buffer = (TCHAR*) malloc(cx_buffer*cy_buffer*sizeof(TCHAR));

		 for(y=0;y<cy_buffer;y++)
			 for(x=0;x<cx_buffer;x++)
				 BUFFER(x,y)=' ';

		if(hwnd == GetFocus())
		InvalidateRect(hwnd,NULL,TRUE);
		return 0;

	 case WM_SETFOCUS:
		 // zapis obrazu
		 return 0;
	 case WM_KILLFOCUS:
		 // od�wierzenie obrazu
		 return 0;

/*
     case WM_PAINT:
          hdc = BeginPaint (hwnd, &ps) ;
          GetClientRect (hwnd, &rect) ;
		  term_printf(hdc,10,100,10,"BOSS");
          EndPaint (hwnd, &ps) ;
          return 0 ;
*/
     case WM_DESTROY:
          PostQuitMessage (0) ;
          return 0 ;
     }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
}
	
/*
EXPORT int CALLBACK term_delete_console(void)
{
}

*/
/*
   KONIEC
*/
#endif  /* _NCURSES_ */

