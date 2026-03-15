# stud_sys

`stud_sys` to terminalowy system czasu rzeczywistego z prostym harmonogramem RT, formularzami typu `blankiet` i raportem stanu pracy. Projekt wywodzi sie ze starszego systemu badawczego, ale aktualna wersja zostala uproszczona i dostosowana do pracy w terminalu `ncurses`.

Obecny wariant repo jest przygotowany glownie do prezentacji zadania `Budzik`, przy zachowaniu pelnego silnika harmonogramu i raportowania.

## Co robi system

System uruchamia zestaw uslug zapisanych jako rekordy `agenda`. Kazda usluga ma:

- nazwe,
- tryb pracy,
- priorytet,
- okres wywolania (`Interval`),
- opoznienie do najblizszego uruchomienia (`delay`).

Scheduler w `core.c` cyklicznie:

- odlicza opoznienia zadan,
- wybiera gotowe rekordy,
- przenosi je do kolejki wykonania,
- uruchamia przypisany modul RT.

W praktyce system pozwala:

- definiowac zadania czasowe,
- obserwowac ich stan,
- modyfikowac rekordy przez formularze tekstowe,
- sprawdzac stan pracy schedulera w raporcie systemowym.

## Aktualny widok programu

Repo jest obecnie ustawione w tryb prezentacyjny. W menu glownym widoczne sa tylko:

- `Budzik`
- `-- Stan systemu --`
- przelacznik `GRAFICZNY/TEKSTOWY`

Pozostale moduly pozostaja w kodzie, ale nie sa eksponowane w glownym menu prezentacyjnym.

## Glowny scenariusz uzycia

Najwazniejszy aktywny modul to `Budzik`.

Umozliwia on:

- dodawanie nowych budzikow,
- edycje istniejacych wpisow,
- usuwanie wpisow,
- podglad czasu pozostalego do uruchomienia,
- obserwacje odliczania na zywo na liscie budzikow.

Raport `-- Stan systemu --` sluzy do pokazania:

- jakie rekordy sa aktywne,
- jaki maja okres,
- jaki maja priorytet,
- jakie jest ich biezace opoznienie.

To jest podstawowy ekran diagnostyczny systemu RT.

## Architektura

Projekt sklada sie z czterech warstw:

### 1. Scheduler i stan systemu

Pliki:

- `core.c`
- `konfig.c`
- `konfig.h`
- `blank/sys_dekl.h`

Odpowiedzialnosc:

- inicjalizacja uslug,
- utrzymanie tablicy `SysA`,
- kolejka zadan gotowych do wykonania,
- wybor zadania do uruchomienia,
- wspolne operacje na rekordach `agenda`.

### 2. Silnik formularzy i raportow

Pliki:

- `blank/wewy.c`
- `rap_bl.c`
- `blank/konsola.c`
- `blank/term_ncurses.c`

Odpowiedzialnosc:

- rysowanie okien i kontrolek,
- obsluga klawiatury,
- raporty i blankiety,
- wspolna logika Enter/Esc/decyzji.

### 3. Moduly funkcjonalne

Pliki:

- `budz.c` - budziki i lista odliczania
- `sys_rap.c` - raport stanu systemu
- `sprawdz.c` - zadanie nadzorcze
- `mqtt_pub.c` - publikacja danych do MQTT
- `sensor_sim.c` - prosty symulator czujnika
- `admin.c` - ekran informacji o systemie

### 4. Warstwa terminala

Pliki:

- `blank/term_ncurses.c`
- `blank/term_ncurses.h`

Odpowiedzialnosc:

- inicjalizacja `ncurses`,
- mapowanie klawiszy,
- operacje na ekranie,
- tryb tekstowy dla macOS/Linux/Unix.

## Jak zbudowany jest rekord RT

Kazde aktywne zadanie jest osadzone w strukturze `agenda` zdefiniowanej w `blank/sys_dekl.h`.

Najwazniejsze pola:

- `name` - nazwa zadania,
- `mode` - typ pracy,
- `Interval` - okres uruchamiania,
- `delay` - czas do najblizszego uruchomienia,
- `prior` - priorytet bazowy,
- `prior_plus` - wzrost priorytetu przy oczekiwaniu,
- `number_of_calls` - licznik wywolan,
- `S` - wskaznik do definicji uslugi.

Na tej strukturze opieraja sie:

- scheduler,
- lista uslug,
- raport `Stan systemu`,
- moduly takie jak `Budzik` i `MQTT Publisher`.

## Budowanie

Wymagania:

- kompilator `gcc`
- biblioteka `ncurses`

Budowanie podstawowe:

```bash
make
```

Budowanie z obsluga MQTT:

```bash
make MQTT=1
```

Uruchomienie:

```bash
./stud_sys
```

## MQTT

Modul `MQTT Publisher` pozostaje w projekcie i moze publikowac stan aktywnych uslug do brokera MQTT. W trybie prezentacyjnym nie jest pokazany w menu glownym, ale kod i konfiguracja nadal sa obecne.

Aby budowac z obsluga MQTT, potrzebna jest biblioteka `paho-mqtt-c`.

Przyklad:

```bash
# macOS
brew install paho-mqtt-c

# Debian/Ubuntu
sudo apt-get install libpaho-mqtt-dev
```

## Najwazniejsze pliki

- `core.c` - scheduler, start systemu, menu glowne
- `konfig.c` - rejestracja uslug
- `budz.c` - glowny modul demonstracyjny
- `sys_rap.c` - raport diagnostyczny
- `presentation_budzik.md` - scenariusz prezentacji
- `user_guide.md` - instrukcja obslugi dla uzytkownika

## Ograniczenia

- interfejs jest terminalowy i oparty na starym modelu formularzy,
- tryb graficzny jest historyczny i ma ograniczone znaczenie w wersji `ncurses`,
- projekt nie jest nowoczesnym frameworkiem RT; to lekki system demonstracyjny z wlasnym schedulerem i formularzami.

## Dokumenty pomocnicze

- opis prezentacji: `presentation_budzik.md`
- instrukcja obslugi: `user_guide.md`
