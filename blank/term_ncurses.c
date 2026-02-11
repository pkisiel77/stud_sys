/*--------------------------------------
  Plik: term_ncurses.c
  Opis: ncurses terminal implementation
        Replaces Windows console with cross-platform ncurses
  
  Wersja: 1.0.0
  Data:  10.02.2026

  Kontakt: ncurses port
--------------------------------------*/

#include "term_ncurses.h"
#include <stdlib.h>
#include <unistd.h>

/* Global variables */
int xg_akt = 0, yg_akt = 0;
char alarmclock = 0;
unsigned int cursor = 0x0A0C, nocursor = 0x2000;

int x_akt = 0, y_akt = 0;
int x_akt0 = 0, y_akt0 = 0;
int x_maxOkna = 79, y_maxOkna = 24;
int x_min = 0, y_min = 0;
int x_max = 79, y_max = 24;

static int is_cursor = 1;
static int current_color_pair = 0;
static int igraf = 0;

/* Color pair definitions - map Windows colors to ncurses */
#define COLOR_PAIR_COUNT 64

void init_ncurses_colors(void)
{
    if (!has_colors())
    {
        return;
    }

    start_color();

    /* Initialize color pairs for foreground/background combinations */
    /* Color pairs: 1-8 for black bg, 9-16 for blue bg, etc. */
    int pair_idx = 1;
    for (int bg = 0; bg < 8; bg++)
    {
        for (int fg = 0; fg < 8; fg++)
        {
            if (pair_idx < COLOR_PAIRS)
            {
                init_pair(pair_idx++, fg, bg);
            }
        }
    }
}

int map_attr_to_color_pair(unsigned int attr)
{
    /* Extract foreground and background colors from Windows-style attribute */
    int fg = attr & 0x0007; /* Bits 0-2: foreground */
    int bg = (attr >> 8) & 0x0007; /* Bits 8-10: background */
    int hilight = (attr & 0x0008) ? A_BOLD : 0; /* Bit 3: intensity/hilight */
    int blink = (attr & 0x0080) ? A_BLINK : 0; /* Bit 7: blink */
    int inverse = (attr & 0x0040) ? A_REVERSE : 0; /* Bit 6: inverse */

    /* Calculate color pair index */
    int pair_idx = bg * 8 + fg + 1;

    if (pair_idx >= COLOR_PAIRS)
    {
        pair_idx = 0;
    }

    return COLOR_PAIR(pair_idx) | hilight | blink | inverse;
}

void InitConsole(void)
{
    initscr(); /* Initialize ncurses */
    cbreak(); /* Disable line buffering */
    noecho(); /* Don't echo input characters */
    keypad(stdscr, TRUE); /* Enable function keys */
    nodelay(stdscr, TRUE); /* Non-blocking getch() */
    curs_set(1); /* Show cursor by default */

    init_ncurses_colors();

    /* Get screen dimensions */
    getmaxyx(stdscr, y_max, x_max);
    y_max--; /* Adjust for 0-based indexing */
    x_max--;
    x_maxOkna = x_max;
    y_maxOkna = y_max;

    refresh();
}

void CloseConsole(void)
{
    endwin();
}

int Endgraph(void)
{
    igraf = 0;
    return 0;
}

void setcursor(unsigned int shape)
{
    if (shape == nocursor)
    {
        is_cursor = 0;
        curs_set(0);
    }
    else
    {
        is_cursor = 1;
        curs_set(1);
    }
}

int otworz_graf_blank(int Xw_min, int Yw_min, int Xw_max, int Yw_max, int Yz_max, unsigned int Attr)
{
    igraf = 1;
    return 1;
}

void term_clear(int co)
{
    if (co == 1)
    {
        /* TERM_CLS_SCR */
        clear();
        refresh();
    }
}

void term_color(int attr)
{
    current_color_pair = map_attr_to_color_pair(attr);
    attrset(current_color_pair);
}

void term_fill(int attr)
{
    term_color(attr);
    clear();
    refresh();
}

int term_type(int y, int x, char* text, int lenth, unsigned int attr)
{
    int actual_len;

    /* Boundary checks */
    if (x < 0) x = 0;
    if (x > x_max) x = x_max;
    if (y < 0) y = 0;
    if (y > y_max) y = y_max;

    xg_akt = x;
    yg_akt = y;

    /* Set color */
    term_color(attr);

    /* Calculate actual length */
    if (lenth == 0)
    {
        actual_len = strlen(text);
    }
    else
    {
        actual_len = lenth;
    }

    /* Move cursor and print text */
    mvprintw(y, x, "%.*s", actual_len, text);

    xg_akt += actual_len;

    refresh();
    return actual_len;
}

int term_printf(int y, int x, unsigned int attr, char* format, ...)
{
    va_list args;
    char buffer[256];
    int len;

    /* Boundary checks */
    if (x < 0) x = 0;
    if (x > x_max) x = x_max;
    if (y < 0) y = 0;
    if (y > y_max) y = y_max;

    xg_akt = x;
    yg_akt = y;

    /* Format string */
    va_start(args, format);
    len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    /* Print formatted text */
    term_type(y, x, buffer, len, attr);

    return len;
}

int m_wherex(void)
{
    return xg_akt;
}

int m_wherey(void)
{
    return yg_akt;
}

void m_gotoxy(int x, int y)
{
    if (x < x_akt0) x = x_akt0;
    if (x > x_maxOkna) x = x_maxOkna;
    if (y < y_akt0) y = y_akt0;
    if (y > y_maxOkna) y = y_maxOkna;

    x_akt = x;
    y_akt = y;
    xg_akt = x;
    yg_akt = y;

    move(y, x);
    refresh();
}

int GET_char(void)
{
    int ch;

    /* Non-blocking read */
    ch = getch();

    if (ch == ERR)
    {
        /* No character available */
        usleep(10000); /* Sleep 10ms to avoid busy waiting */
        return -1;
    }

    /* Map ncurses keys to application-specific codes */
    switch (ch)
    {
    case KEY_DOWN: return 80; /* DOWN */
    case KEY_UP: return 72; /* UP */
    case KEY_LEFT: return 75; /* LEFT/BBS */
    case KEY_RIGHT: return 77; /* RIGHT/FFR */
    case KEY_PPAGE: return 73; /* PgUp */
    case KEY_NPAGE: return 81; /* PgDn */
    case KEY_HOME: return 71; /* Home */
    case KEY_END: return 79; /* End */
    case KEY_IC: return 82; /* Insert */
    case KEY_DC: return 83; /* Delete */
    case KEY_F(1): return 59; /* F1 */
    case KEY_F(2): return 60; /* F2 */
    case KEY_F(3): return 61; /* F3 */
    case KEY_F(4): return 62; /* F4 */
    case KEY_F(5): return 63; /* F5 */
    case KEY_F(6): return 64; /* F6 */
    case KEY_F(7): return 65; /* F7 */
    case KEY_F(8): return 66; /* F8 */
    case KEY_F(9): return 67; /* F9 */
    case KEY_F(10): return 68; /* F10 */
    case KEY_F(11): return 69; /* F11 */
    case KEY_F(12): return 70; /* F12 */
    case 27: return 27; /* Escape */
    case '\n': return 13; /* Enter (CR) */
    case '\r': return 13; /* Enter (CR) */
    case KEY_BACKSPACE:
    case 127:
    case '\b': return 8; /* Backspace */
    case '\t': return 9; /* Tab */
    default: return ch;
    }
}

void term_cur(int y, int x)
{
    if (is_cursor == 0) return;
    move(y, x);
    refresh();
}

void clear_cur(char z)
{
    return;
}

int term_save_image(int y0, int x0, char* bufor, int dlug)
{
    if (dlug == 0) return 2;
    /* Not implemented for ncurses - would need to save screen region */
    return 1;
}

void term_restore_image(int y0, int x0, char* bufor, int dlug)
{
    /* Not implemented for ncurses */
}

void backspace(char* c)
{
    /* Not implemented */
}

void term_flush(void)
{
    refresh();
}

void ramka_graficzna(int yp, int xp, int ym, int xm, unsigned int attr)
{
    /* Draw a box frame */
    term_color(attr);

    /* Draw corners and borders using line-drawing characters */
    mvaddch(yp, xp, ACS_ULCORNER);
    mvaddch(yp, xm, ACS_URCORNER);
    mvaddch(ym, xp, ACS_LLCORNER);
    mvaddch(ym, xm, ACS_LRCORNER);

    /* Draw horizontal lines */
    for (int x = xp + 1; x < xm; x++)
    {
        mvaddch(yp, x, ACS_HLINE);
        mvaddch(ym, x, ACS_HLINE);
    }

    /* Draw vertical lines */
    for (int y = yp + 1; y < ym; y++)
    {
        mvaddch(y, xp, ACS_VLINE);
        mvaddch(y, xm, ACS_VLINE);
    }

    refresh();
}

void ustaw_okno_graficzne(void)
{
    /* Not implemented */
}

void bar(int x0, int y0, int x1, int y1)
{
    /* Fill a rectangular area */
    for (int y = y0; y <= y1 && y <= y_max; y++)
    {
        for (int x = x0; x <= x1 && x <= x_max; x++)
        {
            mvaddch(y, x, ' ');
        }
    }
    refresh();
}

void sound(unsigned frequency)
{
    /* Use beep() for system bell */
    beep();
}

void nosound(void)
{
    /* Stop sound - ncurses doesn't have continuous sound */
}

void delay(unsigned milliseconds)
{
    usleep(milliseconds * 1000);
}

void Cls(void)
{
    clear();
    refresh();
}

/* MessageBox replacement using ncurses */
int MessageBox(void* hwnd, const char* message, const char* title, unsigned int type)
{
    int msg_len = strlen(message);
    int title_len = strlen(title);
    int width = (msg_len > title_len ? msg_len : title_len) + 4;
    int height = 7;

    if (width < 40) width = 40;
    if (width > x_max - 4) width = x_max - 4;

    int y_start = (y_max - height) / 2;
    int x_start = (x_max - width) / 2;

    /* Draw box */
    attron(A_BOLD);
    ramka_graficzna(y_start, x_start, y_start + height, x_start + width, 0x70);

    /* Draw title */
    mvprintw(y_start, x_start + (width - title_len) / 2, "%s", title);

    /* Draw message */
    mvprintw(y_start + 2, x_start + 2, "%s", message);

    /* Draw OK button */
    mvprintw(y_start + height - 2, x_start + width / 2 - 3, "[ OK ]");

    refresh();

    /* Wait for key press */
    nodelay(stdscr, FALSE); /* Blocking mode */
    int ch;
    do
    {
        ch = getch();
    }
    while (ch != '\n' && ch != '\r' && ch != 27 && ch != ' ');
    nodelay(stdscr, TRUE); /* Back to non-blocking */

    /* Redraw screen */
    clear();
    refresh();

    return 0;
}
