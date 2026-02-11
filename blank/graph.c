/*
   wersja: 1.0.3
   data:04.01.2002
*/

/* B��dy:
   1. W sesji jeden raz do trybu graficznego
   2. Kolory
*/
#include "graph.h"

COLORREF kolor_wypelnienia; // ta zmienna powinna byc na ka�dym etapie prowadzenia
HWND hwnd;
HDC hdc;

#define GRAF_X_MAX 800
#define GRAF_Y_MAX 600

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void initgraph(int *graphdriver, int *graphmode, char *pathtodriver)
{
    
	static WNDCLASS wndclass;
	HINSTANCE hInstance;
	int okno_x, okno_y;
	
	//okno_x=GetSystemMetrics(SM_CXSCREEN);
    //okno_y=GetSystemMetrics(SM_CYSCREEN);
//	if(okno_x>=GRAF_X_MAX) okno_x=GRAF_X_MAX;
//  if(okno_y>=GRAF_Y_MAX) okno_y=GRAF_Y_MAX;

	wndclass.style         = CS_SAVEBITS;
	wndclass.lpfnWndProc   = WndProc;
	wndclass.cbClsExtra    = 0;
	wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
	wndclass.hIcon         = NULL;
	wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszMenuName  = NULL;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszClassName = "WewyGraf";
	
	if (!RegisterClass(&wndclass)) {;}
	 else
	 {	hwnd = CreateWindow("WewyGraf", 
	                        "Tryb graficzny wersja 1.0.0 [gonzo77@poczta.fm]", 
							WS_OVERLAPPEDWINDOW,
							CW_USEDEFAULT,CW_USEDEFAULT,
							CW_USEDEFAULT,CW_USEDEFAULT,    
			                NULL,NULL,0, NULL);
	 }
    if (hwnd == NULL) {;}
	else
	{   // ShowWindow(hwnd, SW_SHOWMINIMIZED | SW_SHOWNORMAL);
        ShowWindow(hwnd,SW_SHOWMAXIMIZED);
	    UpdateWindow(hwnd);
	}
	hdc = GetDC(hwnd);
	SelectObject(hdc,GetStockObject(OEM_FIXED_FONT));
}

void closegraph(void)
{
	 ReleaseDC(hwnd,hdc);
	 PostQuitMessage(0);
	 SendMessage(hwnd,WM_CLOSE,0,0);
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int cxClient,cyClient;
	HDC hdcClient,hdcWindow;
    PAINTSTRUCT ps;
	
	
	switch(message)
	 { 
	case WM_SIZE:
		cxClient=LOWORD(lParam);
        cyClient=HIWORD(lParam);
		return 0;
	case WM_PAINT:
		hdcClient=BeginPaint(hwnd,&ps);
		hdcWindow=GetWindowDC(hwnd);
		BitBlt(hdcClient,0,0,cxClient,cyClient,hdcWindow,0,0,SRCAND);
		ReleaseDC(hwnd,hdcWindow);
		EndPaint(hwnd,&ps);
		return 0;
	case WM_DESTROY:
          PostQuitMessage(0);
	  	  ReleaseDC(hwnd,hdc);
	      SendMessage(hwnd,WM_CLOSE,0,0);
          return 0 ;
	 };
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}

/*-----------------FUNKCJE Z BGI---------------------------*/

void moveto(int x, int y)
{ MoveToEx(hdc, x, y, NULL);}

void outtextxy(int x, int y, char *textstring)
{ TextOut(hdc, x, y, textstring, strlen(textstring));}

// funkcja dla _CVC_
// FOREGRAUND
COLORREF F2F(int liczba)
{
   double cal=0.0,resz=0.0,ret=0.0;
   int i=0,tab[24],x;
// rozbieranie liczby bit po bice 
   do{
      ret=liczba/2.0;
      resz=modf(ret,&cal);
      liczba=cal;
      if(resz>0) tab[i]=1;
       else tab[i]=0;
      i++;
   }while(cal>0);
// zamina na RGB
   return RGB(tab[2]*255,tab[1]*255,tab[0]*255);
}

void setcolor(int color)
{
	COLORREF retu;
    retu=F2F(color);
	SetTextColor( hdc, retu);
}

void setfillstyle(int pattern, int color)
{
    // ustala kolor wype�nienia dla funkcji bar
	COLORREF retu;
	retu=F2F(color);
	kolor_wypelnienia = retu;
	SetBkColor(hdc,retu);
}

void bar(int left, int top, int right, int bottom)
{

	RECT rect;
	HBRUSH brush;
	
	brush = CreateSolidBrush(kolor_wypelnienia);
	rect.left   = left;
	rect.top    = top;
	rect.right  = right;
	rect.bottom = bottom;
   	FillRect( hdc, &rect,brush);

}

int textheight(char far *textstring)
{
    SIZE size;
	GetTextExtentPoint32( hdc, textstring, 1, &size);
	return size.cy;
}

int textwidth(char far *textstring)
{
    SIZE size;
	GetTextExtentPoint32( hdc, textstring, 1, &size);
	return size.cx+2;
}

unsigned imagesize(int left, int top, int right, int bottom)
{;}

void getimage(int left, int top, int right, int bottom,void far *bitmap)
{;}

void putimage(int left, int top, void far *bitmap, int op)
{;}

char *grapherrormsg(int errorcode)
{;}

int graphresult(void)
{;}

int getmaxx(void)
{ 
    int okno_x;
	okno_x=GetSystemMetrics(SM_CXSCREEN);
	 if(okno_x>GRAF_X_MAX) okno_x=GRAF_X_MAX;
	return okno_x;
}
 
int getmaxy(void)
{    
	int okno_y;
	okno_y=GetSystemMetrics(SM_CXSCREEN);
	 if(okno_y>GRAF_Y_MAX) okno_y=GRAF_Y_MAX;
	return okno_y;
}
	
void settextstyle(int font, int direction, int charsize)
{;}

void setviewport(int left, int top, int right, int bottom, int clip)
{;}

void clearviewport(void)
{;}

void lineto(int x, int y)
{ LineTo(hdc, x, y);}

void rectangle(int left, int top, int right, int bottom)
{ Rectangle(hdc, left, top, right, bottom);}

void setlinestyle(int linestyle, unsigned upattern, int thickness)
{;}

/*-----------------KONIEC---------------------------*/








