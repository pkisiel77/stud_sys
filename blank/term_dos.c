#include <windows.h>
/*0.0.3*/
int x_cur, y_cur;
Uint char_width, char_height, odstep;
int x0_cur = 0, y0_cur = 0;
int xmax_cur = 5000, ymax_cur = 5000;
void chk_time(void);
void check_services(void);
int cur_bgkcol, cur_color, Cursor_color, is_cursor;
unsigned int textkol, bgkol;
int Xl = 5000, dc = 1, fsize = 5;
unsigned int textkol, bgkol;
int xg_min = 0, yg_min = 0, xd_max, yd_max;

#define _DOS_RT_

extern HWND handle_window_device;
extern int X_time;
/* funkcje koloru i wyswietlenia tekstu */

int term_new_getch_char(int znak)
{
    static unsigned int n_znak = -1;
    if (znak >= 0) n_znak = znak;
    return n_znak;
}

/*1*/
int Endgraph(void) /* ZAMKNIECIE grafiki */
{
    return 0;
}

/*2*/
void setcursor(unsigned int shape)
{
    if (shape == nocursor) is_cursor = 0;
    else is_cursor = 1;
#ifdef GRAF
    if (igraf != 0) return;
#endif
}

/*3*/
int otworz_graf_blank(int Xw_min, int Yw_min, int Xw_max, int Yw_max, int Yz_max, unsigned int Attr)
{
    return 1;
}

/*4*/
void term_clear(int co)
{
    // clrscr();
    ;
}

/*5*/
// funkcja dla _CVC_
// FOREGRAUND
COLORREF F2F(unsigned int liczba)
{
    double cal = 0.0, resz = 0.0, ret = 0.0;
    unsigned int i = 0, tab[24], x;
    // rozbieranie liczby bit po bice
    do
    {
        ret = liczba / 2.0;
        resz = modf(ret, &cal);
        liczba = cal;
        if (resz > 0) tab[i] = 1;
        else tab[i] = 0;
        i++;
    }
    while (cal > 0);
    // zamina na RGB
    return RGB(tab[2] * 255, tab[1] * 255, tab[0] * 255);
}

void setcolor(unsigned int color)
{
    F2F(color);
}

void term_color(int attr)
{
    HDC hdc;
    int cc, blink;
    textkol = attr & 0x00ff;
    blink = attr & TERM_BLINK;
    bgkol = attr & 0xff00;
    bgkol = bgkol >> 8;
    if (blink != 0) bgkol = bgkol | 128;
    if ((attr & INVERSE) == INVERSE)
    {
        cc = bgkol;
        bgkol = textkol & (~INVERSE);
        textkol = cc;
    }
    hdc = GetDC(handle_window_device);
    SetTextColor(hdc, F2F(textkol));
    SetBkColor(hdc, F2F(bgkol));
    ReleaseDC(handle_window_device, hdc);
}

/*6*/
void term_char_color(unsigned int attr, HDC hdc)
{
    int cc, blink;
    textkol = attr & 0x00ff;
    blink = attr & TERM_BLINK;
    bgkol = attr & 0xff00;
    bgkol = bgkol >> 8;
    if (blink != 0) bgkol = bgkol | 128;
    if ((attr & INVERSE) == INVERSE)
    {
        cc = bgkol;
        bgkol = textkol & (~INVERSE);
        textkol = cc;
    }

    SetTextColor(hdc, F2F(textkol));
    SetBkColor(hdc, F2F(bgkol));
}

void term_fill(int attr)
{
    term_color(attr);
    term_clear(3);
}

/*7*/
int term_type(int y, int x, char* text, int lenth, unsigned int attr)
{
    HDC hdc;
    TEXTMETRIC tm;
    int cx_char, cy_char;
    int l, lzn;

    if (x < X_L0) x = X_L0;
    if (x > MX_MAX) x = MX_MAX;
    if (y < Y_G0) y = Y_G0;
    if (y > MY_MAX) y = MY_MAX;

    xg_akt = x;
    yg_akt = y;
    x_cur = x;
    y_cur = y;

    m_gotoxy(x, y);
    if (lenth == 0)
    {
        lenth = strlen(text);
        xg_akt += lenth;
    }
    else
    {
        if (lenth == 1)
        {
            x_cur += 1;
            xg_akt++;
        }
        else
        {
            l = strlen(text);
            lzn = strlen(text);
            xg_akt += lzn;
            if (l < lenth)
            {
                lzn = (lenth - l);
                xg_akt += lzn;
                lzn *= 1;
                x_cur += lzn;
            }
        }
    }

    hdc = GetDC(handle_window_device);
    term_char_color(attr, hdc);
    SelectObject(hdc, GetStockObject(OEM_FIXED_FONT));
    GetTextMetrics(hdc, &tm);
    cx_char = tm.tmAveCharWidth;
    cy_char = tm.tmHeight;

    TextOut(hdc, x * cx_char, y * cy_char, text, lenth);
    m_gotoxy(x, y);

    DeleteObject(SelectObject(hdc, GetStockObject(OEM_FIXED_FONT)));
    ReleaseDC(handle_window_device, hdc);

    return lenth;
}

/*8*/
int term_printf(int y, int x, unsigned int attr, char* format, ...)
{
    va_list arg;


    if (x < X_L0) x = X_L0;
    if (x > MX_MAX) x = MX_MAX;
    if (y < Y_G0) y = Y_G0;
    if (y > MY_MAX) y = MY_MAX;
    xg_akt = x;
    yg_akt = y;

    {
        int l, i, lzn;
        char c = EOS, *cd = NULL, *bf, text[120], *bufor;
        HDC hdc;
        TEXTMETRIC tm;
        int cx_char;

        SelectObject(hdc, GetStockObject(OEM_FIXED_FONT));
        GetTextMetrics(hdc, &tm);
        cx_char = tm.tmAveCharWidth;

        DeleteObject(SelectObject(hdc, GetStockObject(OEM_FIXED_FONT)));
        ReleaseDC(handle_window_device, hdc);

        x_cur = xg_akt;
        y_cur = yg_akt;
        do
        {
            if (cd != NULL)
            {
                format = cd;
                format[0] = '\n';
                cd = NULL;
            }
            va_start(arg, format);
            if (format[0] == '\n')
            {
                y_cur += 1;
                if (y_cur > yd_max) y_cur -= 1;
                else yg_akt++;
                x_cur = x0_cur;
                xg_akt = x_min;
                format++;
            }
            if (format[0] == '\r')
            {
                x_cur = x0_cur;
                xg_akt = x_min;
                format++;
            }
            l = strlen(format);
            for (i = 0; i < l; i++)
            {
                if (format[i] == '\n')
                {
                    cd = &format[i];
                    format[i] = EOS;
                    break;
                }
            }
            vsnprintf(text, sizeof(text), format, arg);
            bufor = &text[0];
            do
            {
                bf = NULL;
                do
                {
                    l = cx_char;
                    i = x_cur + l;
                    if (i < xd_max) break;
                    l = strlen(bufor);
                    if (bf != NULL) bufor[l] = c;
                    l = l - 1;
                    bf = &bufor[l];
                    c = bufor[l];
                    bufor[l] = EOS;
                    i = x_cur;
                }
                while (l > 0);
                lzn = strlen(bufor);
                term_type(y_cur, x_cur, bufor, lzn, attr);

                x_cur = i;
                xg_akt += lzn;
                if (bf != NULL)
                {
                    y_cur += 1;
                    yg_akt++;
                    if (y_cur > yd_max)
                    {
                        y_cur -= 1;
                        yg_akt--;
                    }
                    Xl = x_cur;
                    x_cur = x0_cur;
                    bufor = bf;
                    bufor[0] = c;
                }
            }
            while (bf != NULL);
            va_end(arg);
        }
        while (cd != NULL);
        return strlen(text);
    }
}

/*9*/
int m_wherex(void)
{
    return xg_akt;
} // !!! Uwaga powinno by� x_akt le nie dzia�a wi�c xg_akt;
/*10*/
int m_wherey(void)
{
    return yg_akt;
}

/*11*/
void term_cur(int y, int x)
{
    if (is_cursor == 0) return;
#ifdef GRAF
    if (igraf != 0)
    {
        cur_bgkcol = bgkol; /*|128; blinkowanie */
        setcolor(cur_color);
        x_cur = x_akt * char_width;
        y_cur = y_akt * odstep;
        // definitywnie wy��czy�em kursor
        setcolor(textkol);
        m_gotoxy(x_cur, y_cur);
    }
#endif
    term_flush();
}

/*12*/
int GET_char(void)
{
    MSG term_msg;
    static int znak, n_znak = -1, x, y;
    unsigned int attr_old;
    static time_t tickp = 0, ticka = 0;
    static time_t lticks_sound = 0.2 * CLK_TCK; /* sygnal co 0.2sek */
    static int flaga = 0;
    time_t ticks, dtck;
    while (1)
    {
        znak = 1;
        check_services(); /* tu dziala system interakcyjny */
        if (alarmclock > 0)
        {
            ticks = clock(); /* czas w taktach zegara CLK_TCK l.taktow na sek*/
            dtck = ticks - ticka;
            if (dtck >= lticks_sound)
            {
                if (flaga == 0)
                {
                    flaga = 1;
                    sound(500);
                }
                else
                {
                    flaga = 0;
                    nosound();
                }
                ticka = ticks;
            }
        }
        PeekMessage(&term_msg, NULL, 0, 0, PM_REMOVE);
        TranslateMessage(&term_msg);
        DispatchMessage(&term_msg);
        if (term_msg.message == WM_KEYDOWN)
        {
            znak = term_msg.wParam;
            // znaki steruj�ce <->
            if (znak == 38)
            {
                znak = 0;
                n_znak = UP;
            }
            if (znak == 40)
            {
                znak = 0;
                n_znak = DOWN;
            }
            if (znak == 39)
            {
                znak = 0;
                n_znak == FFR;
            }
            if (znak == 37)
            {
                znak = 0;
                n_znak == BBS;
            }
            // F1...F12
            if (znak == 112)
            {
                znak = 0;
                n_znak = FONT1;
            }
            if (znak == 113)
            {
                znak = 0;
                n_znak = FONT2;
            }
            if (znak == 114)
            {
                znak = 0;
                n_znak = FONT3;
            }
            if (znak == 115)
            {
                znak = 0;
                n_znak = FONT4;
            }
            if (znak == 116)
            {
                znak = 0;
                n_znak = FONT5;
            }
            if (znak == 117)
            {
                znak = 0;
                n_znak = FONT6;
            }
            if (znak == 118)
            {
                znak = 0;
                n_znak = FONT7;
            }
            if (znak == 119)
            {
                znak = 0;
                n_znak = FONT8;
            }
            if (znak == 120)
            {
                znak = 0;
                n_znak = FONT9;
            }
            if (znak == 121)
            {
                znak = 0;
                n_znak = FONT10;
            }
            if (znak == 122)
            {
                znak = 0;
                n_znak = FONT11;
            }
            if (znak == 123)
            {
                znak = 0;
                n_znak = FONT12;
            }
        }
        // 2. MyszWM_RBUTTONDOWN
        if (term_msg.message == WM_LBUTTONDOWN)
        {
            znak = 0;
            n_znak = UP;
        }
        if (term_msg.message == WM_RBUTTONDOWN)
        {
            znak = 0;
            n_znak = DOWN;
        }
        // Enter i Esc ???
        if (znak != 1)
        {
            ticks = clock(); // czas w taktach zegara CLK_TCK l.taktow na sek
            dtck = ticks - tickp;
            if (dtck >= lticks_min)
            {
                tickp = ticks;
                term_new_getch_char(n_znak);
                return znak;
            }
        }
    }
    return;
}

/*13*/

/*
void m_gotoxy(int x, int y)
{if(x<x_akt0) x=x_akt0; if(x>x_maxOkna) x=x_maxOkna;
	if(y<y_akt0) y=y_akt0; if(y>y_maxOkna) y=y_maxOkna;

	x_akt=x; y_akt=y; xg_akt=x; yg_akt=y;
#ifdef GRAF
	if(igraf!=0) {x_cur=x_akt*char_width; y_cur=y_akt*odstep;}
	else
#endif
	 {gotoxy(x_akt,y_akt);}
	term_flush();
	return;
}
*/
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
    x_cur = x_akt * char_width;
    y_cur = y_akt * odstep;
    //	x_cur=x_akt;y_cur=y_akt;
    term_flush();
}

/*14*/
void clear_cur(char z)
{
    return;
}

/*15*/
int term_save_image(int y0, int x0, char* bufor, int dlug)
{
    if (dlug == 0) return 2;
    return 1; //gettext(x0,y0,x0+dlug-1,y0,bufor);
}

/*16*/
void term_restore_image(int y0, int x0, char* bufor, int dlug)
{
    //puttext(x0,y0,x0+dlug-1,y0,bufor);
    ;
}

/*17*/
void backspace(char* c)
{
    ;
}

/*18*/
void term_flush(void)
{
    ;
}

/*19*/
/*20*/
void ramka_graficzna(int yp, int xp, int ym, int xm, unsigned int attr)
{
    ;
}

/*21*/
void ustaw_okno_graficzne(void)
{
    ;
}

/*22*/
void bar(int x0, int y0, int x1, int y1)
{
    ;
}
