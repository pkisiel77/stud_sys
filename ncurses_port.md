# Warstwa terminala (ncurses)

Cały interfejs `stud_sys` rysuje się przez cienką warstwę w `blank/term_ncurses.c` / `blank/term_ncurses.h`.
Zachowuje ona API pierwotnej wersji DOS/Windows (`term_*`, `GET_char()`, atrybuty kolorów w stylu konsoli),
dzięki czemu silnik formularzy (`blank/wewy.c`) i moduły działają bez zmian na Linuksie i macOS.
Build zawsze definiuje `_NCURSES_` (makefile i CMake); gałęzie DOS/QNX/Windows zostały usunięte.

## Najważniejsze funkcje

| Obszar | Funkcje |
|---|---|
| Ekran | `InitConsole()`, `CloseConsole()`, `term_clear()`, `term_fill()`, `Cls()` |
| Tekst | `term_type()`, `term_printf()`, `term_color()`, `ramka_graficzna()` |
| Kursor | `setcursor()`, `m_gotoxy()`, `m_wherex()` / `m_wherey()`, `term_cur()` |
| Klawiatura | `GET_char()` |
| Zapis fragmentu ekranu | `term_save_image()`, `term_restore_image()` |
| Zgodność | `sound()` / `nosound()` (beep), `delay()`, `MessageBox()` (tekstowy) |

## Klawiatura i scheduler

`GET_char()` jest jedynym miejscem oczekiwania na klawisz. Przy każdym wywołaniu woła `check_services()`
(`scheduler.c`), więc scheduler RT działa kooperacyjnie: agenda jest odliczana i gotowe usługi są uruchamiane,
dopóki użytkownik nie naciśnie klawisza.

Klawisze specjalne są zwracane jak w DOS - najpierw `0`, przy następnym wywołaniu kod skanowania:

| Klawisz | Kod | Klawisz | Kod |
|---|---|---|---|
| F1 ... F12 | 59 ... 70 | Home / End | 71 / 79 |
| Strzałka w górę / w dół | 72 / 80 | PgUp / PgDn | 73 / 81 |
| Strzałka w lewo / w prawo | 75 / 77 | Insert / Delete | 82 / 83 |

Zwykłe klawisze: Esc = 27, Enter = 13, Backspace = 8, Tab = 9.
SIGINT/SIGTERM ustawiają flagę zatrzymania i kolejne `GET_char()` zwracają Esc, co zamyka program porządnie.

## Atrybuty kolorów

Atrybut to liczba w formacie konsoli DOS (stałe `TERM_*` w `blank/wewybl.h`):

| Bity | Znaczenie |
|---|---|
| 0-2 | kolor tekstu (0-7: black, blue, green, cyan, red, magenta, yellow, white) |
| 3 | rozjaśnienie (`HILIGHT`) → `A_BOLD` |
| 6 | odwrócenie (`INVERSE`) → `A_REVERSE` |
| 7 | miganie (`BLINK`) → `A_BLINK` |
| 8-10 | kolor tła |

`init_ncurses_colors()` tworzy 64 pary kolorów (8 tła × 8 tekstu), a `map_attr_to_color_pair()` zamienia atrybut
na parę i atrybuty ncurses.

## Ograniczenia

- Tryb graficzny jest tylko szczątkowy (`otworz_graf_blank()`, `Endgraph()` nic nie rysują).
- Dźwięk to wyłącznie `beep()`, bez częstotliwości.
- `MessageBox()` jest prostym oknem tekstowym.
- Ramki rysowane są znakami ACS ncurses; terminal musi obsługiwać UTF-8 lub ACS.
