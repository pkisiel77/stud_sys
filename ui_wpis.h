#ifndef UI_WPIS_H
#define UI_WPIS_H
/* Proste okna menu i pole wpisu w wierszu statusu dla modulow uslug
 * (budz.c, mqtt_pub.c).
 *
 * Zamiast okno_menu() (ktore zamyka sie przed wpisem wartosci) modul
 * rysuje swoje okno sam w funkcji odswiezania (ui_odswiez_fn) i podaje
 * ja tutaj: ui_klawisz_menu() i ui_czytaj_*() wywoluja ja w kazdym
 * obiegu petli. Z wymus == 0 funkcja powinna rysowac raz na sekunde,
 * z wymus != 0 - zawsze. GET_char() wola przy tym check_services(),
 * wiec scheduler pracuje caly czas. */

typedef void (*ui_odswiez_fn)(int wymus);

#define UI_NIC (-2) /* ui_klawisz_menu: klawisz obsluzony, nic nie wybrano */

/* Okno z ramka i tytulem; wiersze od x = xp + dx, podswietlony wiersz sel
   (sel < 0: bez podswietlenia). */
void ui_rysuj_okno(int yp, int xp, int ym, int xm, char *tytul,
                   char *wiersze[], int n, int sel, int dx);

/* Czeka na klawisz, odswiezajac ekran (odswiez moze byc NULL). */
int ui_czekaj_klawisz(ui_odswiez_fn odswiez);

/* Obsluga klawiszy menu: strzalki, Home/End zmieniaja *sel.
   Zwraca *sel (Enter), -1 (Esc) albo UI_NIC. */
int ui_klawisz_menu(int ch, int *sel, int count, ui_odswiez_fn odswiez);

/* Pole wpisu w wierszu statusu. Pierwszy znak zastepuje stara wartosc,
   Backspace ja edytuje; Esc zostawia stara wartosc. */
int  ui_czytaj_int(const char *prompt, int min_val, int max_val, int current,
                   ui_odswiez_fn odswiez);
void ui_czytaj_text(char *buf, int max_len, const char *prompt,
                    ui_odswiez_fn odswiez);

/* Zapamietany fragment ekranu (pelne wiersze y .. y+n-1). */
struct ui_tlo {
    char *buf;
    int y, n, x, dl;
};
void ui_zapamietaj_tlo(struct ui_tlo *t, int y, int n);
void ui_przywroc_tlo(const struct ui_tlo *t);
void ui_zwolnij_tlo(struct ui_tlo *t);

#endif
