# Operator Cheatsheet

## Cel

To jest skrócona ściąga do obsługi systemu podczas pokazu lub testu.

## Menu główne

Widoczne pozycje:

- `Budzik`
- `Demo IoT: czujnik symulowany`
- `-- Stan systemu --`

## Klawisze

- Strzałka góra/dół - przesunięcie zaznaczenia
- `Enter` - otwarcie pozycji / zatwierdzenie pola
- `Esc` - wyjście z okna / anulowanie wpisu
- cyfry, `Backspace` - pole wpisu na dole ekranu (pierwsza cyfra zastępuje starą wartość)
- w Stanie systemu: `PgUp/PgDn` strona, `R` odświeżenie

## Demo 3 min

1. `Enter` na `Budzik`
2. wybierz `+++ Dodaj nowy budzik +++`
3. ustaw budzik A (wartości niżej)
4. `>>> Wpisz do agendy <<<`
5. pokaż odliczanie na liście
6. `Esc`
7. `Enter` na `-- Stan systemu --`
8. pokaż rekord `Budzik` (`NAST.`, `CYKL`)
9. `Esc`
10. wróć do `Budzik`
11. poczekaj na okno alarmu (ok. 1-2 min od zapisu)

## Demo 5 min

1. dodaj budzik A i budzik B
2. pokaż dwa wpisy na liście z różnym odliczaniem
3. `Esc`, `Enter` na `Demo IoT: czujnik symulowany`
4. pokaż `Czujnik Sim` i zmieniającą się `WARTOSC` obok rekordu `Budzik`
5. `Esc`, wróć do `Budzik`
6. pokaż alarm budzika A
7. pokaż, że budzik B nadal czeka

## Gotowe wartości

Godzinę liczysz od bieżącej (`T` = teraz, z dokładnością do minuty).

### Budzik A

- `Sprawa`: `Demo A`
- `Godzina`: `T + 3 min`
- `Co ile`: `1`, `Wyprzedzenie`: `1`

### Budzik B

- `Sprawa`: `Demo B`
- `Godzina`: `T + 6 min`
- `Co ile`: `1`, `Wyprzedzenie`: `1`

Monity zaczynają się `Wyprzedzenie` minut przed Godziną. Godzina ustawiona zbyt blisko (np. `T + 1`) daje alarm od razu po zapisie.

## Co powiedzieć

- `Budzik` jest zadaniem czasowym.
- System przechowuje je w agendzie RT.
- `Stan systemu` pokazuje wewnętrzny stan schedulera.
- Countdown na liście pokazuje czas do najbliższego monitu.

## Gdy coś pójdzie nie tak

- brak wyjścia z okna: `Esc`
- powrót do menu: `Esc` aż do menu głównego
- chcesz tylko pokazać scheduler: wejdź w `-- Stan systemu --`
