/*--------------------------------------
  Plik: term_ncurses.c
  Opis: ncurses terminal implementation
        Replaces Windows console with cross-platform ncurses
  
  Wersja: 1.0.0
  Data:  10.02.2026

  Kontakt: ncurses port
--------------------------------------*/

#include "term_ncurses.h"
#include <locale.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

/* Scheduler jest kooperacyjny: podczas oczekiwania na klawisz trzeba
   regularnie odliczac agende i uruchamiac gotowe uslugi. */
void check_services(void);

extern int MY_MAX, MYR_MAX, MX_MAX, MXR_MAX;

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
static int terminal_initialized = 0;
static int cleanup_registered = 0;
static volatile sig_atomic_t stop_requested = 0;
static int available_color_pairs = 1;

/* Color pair definitions - map Windows colors to ncurses */
#define COLOR_PAIR_COUNT 64

static void terminal_signal_handler(int signal_number)
{
    (void)signal_number;
    stop_requested = 1;
}

static void update_terminal_size(void)
{
    int rows, columns;

    getmaxyx(stdscr, rows, columns);
    if (rows < 1) rows = 1;
    if (columns < 1) columns = 1;

    y_max = rows - 1;
    x_max = columns - 1;
    x_maxOkna = x_max;
    y_maxOkna = y_max;

    MX_MAX = columns;
    MXR_MAX = x_max;
    MY_MAX = y_max;
    MYR_MAX = (y_max > 0) ? y_max - 1 : 0;
}

short ncurses_color_from_dos(int color)
{
    static const short colors[8] = {
        COLOR_BLACK, COLOR_BLUE, COLOR_GREEN, COLOR_CYAN,
        COLOR_RED, COLOR_MAGENTA, COLOR_YELLOW, COLOR_WHITE
    };

    if (color < 0 || color >= 8) return COLOR_BLACK;
    return colors[color];
}

void init_ncurses_colors(void)
{
    if (!has_colors())
    {
        available_color_pairs = 1;
        return;
    }

    start_color();
    available_color_pairs = COLOR_PAIRS;

    /* Initialize color pairs for foreground/background combinations */
    /* Color pairs: 1-8 for black bg, 9-16 for blue bg, etc. */
    int pair_idx = 1;
    for (int bg = 0; bg < 8; bg++)
    {
        for (int fg = 0; fg < 8; fg++)
        {
            if (pair_idx < COLOR_PAIRS)
            {
                init_pair((short)pair_idx++, ncurses_color_from_dos(fg),
                          ncurses_color_from_dos(bg));
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

    if (pair_idx >= available_color_pairs || pair_idx >= COLOR_PAIR_COUNT)
    {
        pair_idx = 0;
    }

    return COLOR_PAIR(pair_idx) | hilight | blink | inverse;
}

void InitConsoleV1(void)
{
    InitConsole();
}

void InitConsole(void)
{
    if (terminal_initialized) return;

    setlocale(LC_ALL, "");
    initscr(); /* Initialize ncurses */
    terminal_initialized = 1;
    cbreak(); /* Disable line buffering */
    noecho(); /* Don't echo input characters */
    keypad(stdscr, TRUE); /* Enable function keys */

    /* Zamiast nodelay(TRUE) ustaw mały timeout:
       getch() będzie czekał max N ms, potem zwróci ERR. */
    timeout(50);

#ifdef NCURSES_VERSION
    set_escdelay(25);
#endif

    curs_set(1); /* Show cursor by default */

    init_ncurses_colors();

    update_terminal_size();

    signal(SIGINT, terminal_signal_handler);
    signal(SIGTERM, terminal_signal_handler);
    if (!cleanup_registered)
    {
        atexit(CloseConsole);
        cleanup_registered = 1;
    }

    refresh();
}

void CloseConsole(void)
{
    if (!terminal_initialized) return;
    timeout(-1);
    curs_set(1);
    echo();
    nocbreak();
    endwin();
    terminal_initialized = 0;
}

int Endgraph(void)
{
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
    (void)Xw_min; (void)Yw_min; (void)Xw_max;
    (void)Yw_max; (void)Yz_max; (void)Attr;
    return 0;
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

    if (text == NULL) return 0;

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
    if (actual_len < 0) actual_len = 0;
    if (actual_len > x_max - x + 1) actual_len = x_max - x + 1;

    /* Move cursor and print text */
    mvprintw(y, x, "%.*s", actual_len, text);

    xg_akt += actual_len;
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

    if (len < 0) return len;
    if (len >= (int)sizeof(buffer)) len = (int)sizeof(buffer) - 1;

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
}

int GET_charV1(void)
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

int GET_char(void)
{
    static int pending_scan_code = -1;
    int ch;
    int mapped = -1;

    if (stop_requested) return 27;

    check_services();
    refresh();
    if (stop_requested) return 27;

    /* Emulate DOS two-byte keys: first SPEC(0), then scan code. */
    if (pending_scan_code >= 0)
    {
        ch = pending_scan_code;
        pending_scan_code = -1;
        return ch;
    }

    ch = getch();
    if (ch == ERR)
    {
        return -1;
    }

    if (ch == KEY_RESIZE)
    {
        update_terminal_size();
        clearok(stdscr, TRUE);
        touchwin(stdscr);
        refresh();
        return -1;
    }

    /* Fallback for raw ESC sequences sent by some terminals. */
    if (ch == 27)
    {
        int ch2;
        int ch3;

        timeout(0);
        ch2 = getch();
        if (ch2 != '[')
        {
            if (ch2 != ERR) ungetch(ch2);
            timeout(50);
            return 27;
        }

        ch3 = getch();
        timeout(50);
        switch (ch3)
        {
        case 'A': mapped = 72; break; /* UP */
        case 'B': mapped = 80; break; /* DOWN */
        case 'C': mapped = 77; break; /* RIGHT */
        case 'D': mapped = 75; break; /* LEFT */
        default: break;
        }
        if (mapped >= 0)
        {
            pending_scan_code = mapped;
            return 0;
        }
        if (ch3 != ERR) ungetch(ch3);
        ungetch(ch2);
        return 27;
    }

    switch (ch)
    {
    case KEY_DOWN:  mapped = 80; break;
    case KEY_UP:    mapped = 72; break;
    case KEY_LEFT:  mapped = 75; break;
    case KEY_RIGHT: mapped = 77; break;
    case KEY_PPAGE: mapped = 73; break;
    case KEY_NPAGE: mapped = 81; break;
    case KEY_HOME:  mapped = 71; break;
    case KEY_END:   mapped = 79; break;
    case KEY_IC:    mapped = 82; break;
    case KEY_DC:    mapped = 83; break;
    case KEY_F(1):  mapped = 59; break;
    case KEY_F(2):  mapped = 60; break;
    case KEY_F(3):  mapped = 61; break;
    case KEY_F(4):  mapped = 62; break;
    case KEY_F(5):  mapped = 63; break;
    case KEY_F(6):  mapped = 64; break;
    case KEY_F(7):  mapped = 65; break;
    case KEY_F(8):  mapped = 66; break;
    case KEY_F(9):  mapped = 67; break;
    case KEY_F(10): mapped = 68; break;
    case KEY_F(11): mapped = 69; break;
    case KEY_F(12): mapped = 70; break;
    case '\n':
    case '\r':      return 13;
    case KEY_BACKSPACE:
    case 127:
    case '\b':      return 8;
    case '\t':      return 9;
    default:        return ch;
    }

    pending_scan_code = mapped;
    return 0;
}

int Get_char(void)
{
    return GET_char();
}

int Get_Char(void)
{
    return GET_char();
}

void term_cur(int y, int x)
{
    if (is_cursor == 0) return;
    if (x < 0) x = 0;
    if (x > x_max) x = x_max;
    if (y < 0) y = 0;
    if (y > y_max) y = y_max;
    move(y, x);
}

void clear_cur(char z)
{
    (void)z;
}

int term_save_image(int y0, int x0, char* bufor, int dlug)
{
    int i;
    chtype *cells = (chtype *)bufor;

    if (dlug == 0) return (int)sizeof(chtype);
    if (bufor == NULL || dlug < 0) return (int)sizeof(chtype);

    for (i = 0; i < dlug; i++)
    {
        int x = x0 + i;
        cells[i] = (y0 >= 0 && y0 <= y_max && x >= 0 && x <= x_max)
                   ? mvinch(y0, x) : (chtype)' ';
    }
    return (int)sizeof(chtype);
}

void term_restore_image(int y0, int x0, char* bufor, int dlug)
{
    int i;
    chtype *cells = (chtype *)bufor;

    if (bufor == NULL || dlug <= 0 || y0 < 0 || y0 > y_max) return;
    for (i = 0; i < dlug; i++)
    {
        int x = x0 + i;
        if (x >= 0 && x <= x_max) mvaddch(y0, x, cells[i]);
    }
}

void backspace(char* c)
{
    (void)c;
    if (xg_akt <= 0 || yg_akt < 0 || yg_akt > y_max) return;
    xg_akt--;
    mvaddch(yg_akt, xg_akt, ' ');
    move(yg_akt, xg_akt);
}

void term_flush(void)
{
    refresh();
}

void ramka_graficzna(int yp, int xp, int ym, int xm, unsigned int attr)
{
    if (xp < 0) xp = 0;
    if (yp < 0) yp = 0;
    if (xm > x_max) xm = x_max;
    if (ym > y_max) ym = y_max;
    if (xp >= xm || yp >= ym) return;

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

}

void ustaw_okno_graficzne(void)
{
    touchwin(stdscr);
}

void bar(int x0, int y0, int x1, int y1)
{
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > x_max) x1 = x_max;
    if (y1 > y_max) y1 = y_max;
    if (x0 > x1 || y0 > y1) return;

    /* Fill a rectangular area */
    for (int y = y0; y <= y1 && y <= y_max; y++)
    {
        for (int x = x0; x <= x1 && x <= x_max; x++)
        {
            mvaddch(y, x, ' ');
        }
    }
}

void sound(unsigned frequency)
{
    /* Use beep() for system bell */
    (void)frequency;
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
    WINDOW *saved_screen;
    int msg_len = strlen(message);
    int title_len = strlen(title);
    int width = (msg_len > title_len ? msg_len : title_len) + 4;
    int height = 7;

    (void)hwnd;
    (void)type;

    saved_screen = dupwin(stdscr);

    if (width < 40) width = 40;
    if (width > x_max - 2) width = x_max - 2;
    if (width < 4 || y_max < height + 2)
    {
        if (saved_screen != NULL) delwin(saved_screen);
        return 0;
    }

    int y_start = (y_max - height) / 2;
    int x_start = (x_max - width) / 2;

    /* Draw box */
    ramka_graficzna(y_start, x_start, y_start + height, x_start + width, 0x70);

    /* Draw title */
    attron(A_BOLD);
    mvprintw(y_start, x_start + (width - title_len) / 2, "%s", title);
    attroff(A_BOLD);

    /* Draw message */
    mvprintw(y_start + 2, x_start + 2, "%s", message);

    /* Draw OK button */
    mvprintw(y_start + height - 2, x_start + width / 2 - 3, "[ OK ]");

    term_flush();

    /* Wait for key press */
    int ch;
    do
    {
        ch = GET_char();
    }
    while (ch != '\n' && ch != '\r' && ch != 27 && ch != ' ');

    if (saved_screen != NULL)
    {
        overwrite(saved_screen, stdscr);
        delwin(saved_screen);
    }
    touchwin(stdscr);
    refresh();

    return 0;
}
