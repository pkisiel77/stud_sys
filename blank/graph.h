/* 
 win_graf.h
*/

#include <windows.h>
// zmienne globalne specjalnie dla trybu _CVC_
// koniec

#define VGAHI 2

#define DETECT   0
#define CGA      1
#define MCGA     2
#define EGA      3
#define EGA64    4
#define EGAMONO  5
#define IBM8514  6
#define HERCMONO 7
#define ATT400   8
#define VGA      9
#define PC3270   10

#define SOLID_FILL 1
#define COPY_PUT 1
#define SMALL_FONT 1
#define HORIZ_DIR 1
#define SOLID_LINE 1
#define DEFAULT_FONT 1
#define grOk 1



LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
void initgraph(int *graphdriver, int *graphmode, char *pathtodriver);
void closegraph(void);





