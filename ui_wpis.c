/* ui_wpis.c - okna menu i pole wpisu dla modulow uslug (opis: ui_wpis.h) */
#include "blank/moje.h"
#include "loc.h"
#include "core.h"
#include "ui_wpis.h"

void ui_rysuj_okno(int yp, int xp, int ym, int xm, char *tytul,
                   char *wiersze[], int n, int sel, int dx)
{
    int i, x, y;
    unsigned int att;
    Okno(yp, xp, ym, xm, attr);
    maluj_ramke(yp, xp, ym, xm, attr);
    if (tytul != NULL) {
        int xt = xp + ((xm - xp + 1) - (int)strlen(tytul)) / 2;
        if (xt < xp + 1) xt = xp + 1;
        term_type(yp, xt, tytul, 0, attr);
    }
    /* czyscimy cale wnetrze: po zmianie rozmiaru nie moze zostac
       fragment poprzedniego okna */
    for (i = 0; i < ym - yp - 1; i++) {
        y = yp + 1 + i;
        att = (i == sel) ? at_wpis : attr;
        for (x = xp + 1; x < xm; x++) term_type(y, x, " ", 1, att);
        if (i < n) term_type(y, xp + dx, wiersze[i], xm - xp - dx, att);
    }
    if (sel >= 0) term_cur(yp + 1 + sel, xp + dx + 1);
}

int ui_czekaj_klawisz(ui_odswiez_fn odswiez)
{
    int ch;
    if (odswiez != NULL) odswiez(1);
    while ((ch = GET_char()) == -1)
        if (odswiez != NULL) odswiez(0);
    return ch;
}

int ui_klawisz_menu(int ch, int *sel, int count, ui_odswiez_fn odswiez)
{
    ch &= 0x00ff;
    switch (ch) {
    case NLCR:
    case CR:
        return *sel;
    case Esc:
        return -1;
    case SPEC:
        ch = GETchar() & 0x00ff;
        switch (ch) {
        case UP:
        case BBS:  if (--(*sel) < 0) *sel = count - 1; break;
        case DOWN:
        case FFR:  if (++(*sel) >= count) *sel = 0; break;
        case Home: *sel = 0; break;
        case End:  *sel = count - 1; break;
        default:   break;
        }
        if (odswiez != NULL) odswiez(1);
        break;
    default:
        break;
    }
    return UI_NIC;
}

/* Wspolna petla pola wpisu. cyfry != 0: tylko cyfry. Zwraca dlugosc
   wpisu albo -1 (Esc). */
static int ui_wpis(char *buf, int rozmiar, int cyfry, ui_odswiez_fn odswiez,
                   const char *prompt, int min_val, int max_val)
{
    int len, ch, nowa;
    len = (int)strlen(buf);
    nowa = 1; /* pierwszy znak zastepuje stara wartosc, Backspace ja edytuje */
    setcursor(cursor);
    for (;;) {
        if (odswiez != NULL) odswiez(0); /* okno modulu zostaje na ekranie */
        if (cyfry)
            term_printf(MY_MAX, X_L0, ATTR_A, L_BUDZ_PROMPT_INT, prompt, min_val, max_val, buf);
        else
            term_printf(MY_MAX, X_L0, ATTR_A, L_BUDZ_PROMPT_TXT, prompt, buf);
        ch = GET_char();   /* GET_char wola check_services(): scheduler dziala w trakcie wpisu */
        if (ch < 0) continue;                  /* timeout - nic nie wcisnieto */
        if (ch == 0) { GET_char(); continue; } /* klawisz specjalny (strzalka, F1..): pomijamy */
        if (ch == '\n' || ch == '\r' || ch == KEY_ENTER) break;
        if (ch == 27) { len = -1; break; }
        if ((ch == KEY_BACKSPACE || ch == 127 || ch == 8) && len > 0) {
            buf[--len] = 0; nowa = 0;
        } else if ((cyfry ? (ch >= '0' && ch <= '9') : (ch >= 32 && ch < 256))
                   && (nowa || len < rozmiar - 1)) {
            if (nowa) { len = 0; nowa = 0; }
            buf[len++] = (char)ch; buf[len] = 0;
        }
    }
    term_printf(MY_MAX, X_L0, ATTR_A, "%-*s", MX_MAX, " ");
    setcursor(nocursor);
    return len;
}

int ui_czytaj_int(const char *prompt, int min_val, int max_val, int current,
                  ui_odswiez_fn odswiez)
{
    char buf[12];
    int val;
    snprintf(buf, sizeof(buf), "%d", current);
    if (ui_wpis(buf, (int)sizeof(buf) - 1, 1, odswiez, prompt, min_val, max_val) <= 0)
        return current;
    val = atoi(buf);
    if (val < min_val) val = min_val;
    if (val > max_val) val = max_val;
    return val;
}

void ui_czytaj_text(char *buf, int max_len, const char *prompt, ui_odswiez_fn odswiez)
{
    char tmp[128];
    if (max_len > (int)sizeof(tmp)) max_len = (int)sizeof(tmp);
    snprintf(tmp, sizeof(tmp), "%s", buf);
    tmp[max_len - 1] = 0;
    if (ui_wpis(tmp, max_len, 0, odswiez, prompt, 0, 0) > 0)
        snprintf(buf, (size_t)max_len, "%s", tmp);
}

void ui_zapamietaj_tlo(struct ui_tlo *t, int y, int n)
{
    int i, kom = term_save_image(0, 0, "a", 0);
    t->y = y; t->x = X_L0; t->dl = MXR_MAX - X_L0 + 1;
    if (y + n - 1 > MYR_MAX) n = MYR_MAX - y + 1;
    t->n = n > 0 ? n : 0;
    t->buf = (char *)malloc((size_t)t->n * t->dl * kom + 1);
    if (t->buf == NULL) return;
    for (i = 0; i < t->n; i++)
        term_save_image(t->y + i, t->x, t->buf + (size_t)i * t->dl * kom, t->dl);
}

void ui_przywroc_tlo(const struct ui_tlo *t)
{
    int i, kom = term_save_image(0, 0, "a", 0);
    if (t->buf == NULL) return;
    for (i = 0; i < t->n; i++)
        term_restore_image(t->y + i, t->x, t->buf + (size_t)i * t->dl * kom, t->dl);
}

void ui_zwolnij_tlo(struct ui_tlo *t)
{
    free(t->buf);
    t->buf = NULL;
}
