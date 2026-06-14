# stud_sys

**Wersja dokumentacji:** `0384673`

`stud_sys` to terminalowy system czasu rzeczywistego z prostym harmonogramem RT, formularzami typu `blankiet` i raportem stanu pracy. Projekt wywodzi się ze starszego systemu badawczego, ale aktualna wersja została uproszczona i dostosowana do pracy w terminalu `ncurses`.

Obecny wariant repo jest przygotowany głównie do prezentacji zadania `Budzik`, przy zachowaniu pełnego silnika harmonogramu i raportowania.

## Co robi system

System uruchamia zestaw usług zapisanych jako rekordy `agenda`. Każda usługa ma:

- nazwę,
- tryb pracy,
- priorytet,
- okres wywołania (`Interval`),
- opóźnienie do najbliższego uruchomienia (`delay`).

Scheduler w `core.c` cyklicznie:

- odlicza opóźnienia zadań,
- wybiera gotowe rekordy,
- przenosi je do kolejki wykonania,
- uruchamia przypisany moduł RT.

W praktyce system pozwala:

- definiować zadania czasowe,
- obserwować ich stan,
- modyfikować rekordy przez formularze tekstowe,
- sprawdzać stan pracy schedulera w raporcie systemowym.

## Aktualny widok programu

Repo jest obecnie ustawione w tryb prezentacyjny. W menu głównym widoczne są tylko:

- `Budzik`
- `-- Stan systemu --`
- przełącznik `GRAFICZNY/TEKSTOWY`

Pozostałe moduły pozostają w kodzie, ale nie są eksponowane w głównym menu prezentacyjnym.

## Główny scenariusz użycia

Najważniejszy aktywny moduł to `Budzik`.

Umożliwia on:

- dodawanie nowych budzików,
- edycję istniejących wpisów,
- usuwanie wpisów,
- podgląd czasu pozostałego do uruchomienia,
- obserwację odliczania na żywo na liście budzików.

Raport `-- Stan systemu --` służy do pokazania:

- jakie rekordy są aktywne,
- jaki mają okres,
- jaki mają priorytet,
- jakie jest ich bieżące opóźnienie.

To jest podstawowy ekran diagnostyczny systemu RT.

## Architektura

Projekt składa się z czterech warstw:

### 1. Scheduler i stan systemu

Pliki:

- `core.c`
- `konfig.c`
- `konfig.h`
- `blank/sys_dekl.h`

Odpowiedzialność:

- inicjalizacja usług,
- utrzymanie tablicy `SysA`,
- kolejka zadań gotowych do wykonania,
- wybór zadania do uruchomienia,
- wspólne operacje na rekordach `agenda`.

### 2. Silnik formularzy i raportów

Pliki:

- `blank/wewy.c`
- `rap_bl.c`
- `blank/konsola.c`
- `blank/term_ncurses.c`

Odpowiedzialność:

- rysowanie okien i kontrolek,
- obsługa klawiatury,
- raporty i blankiety,
- wspólna logika Enter/Esc/decyzji.

### 3. Moduły funkcjonalne

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

Odpowiedzialność:

- inicjalizacja `ncurses`,
- mapowanie klawiszy,
- operacje na ekranie,
- tryb tekstowy dla macOS/Linux/Unix.

## Jak zbudowany jest rekord RT

Każde aktywne zadanie jest osadzone w strukturze `agenda` zdefiniowanej w `blank/sys_dekl.h`.

Najważniejsze pola:

- `name` - nazwa zadania,
- `mode` - typ pracy,
- `Interval` - okres uruchamiania,
- `delay` - czas do najbliższego uruchomienia,
- `prior` - priorytet bazowy,
- `prior_plus` - wzrost priorytetu przy oczekiwaniu,
- `number_of_calls` - licznik wywołań,
- `S` - wskaźnik do definicji usługi.

Na tej strukturze opierają się:

- scheduler,
- lista usług,
- raport `Stan systemu`,
- moduły takie jak `Budzik` i `MQTT Publisher`.

## Budowanie

Wymagania:

- kompilator `gcc`
- biblioteka `ncurses`

Budowanie podstawowe:

```bash
make
```

Budowanie z obsługą MQTT:

```bash
make MQTT=1
```

Wybór języka interfejsu TUI (domyślnie `PL`, aktualnie obsługiwany jest też `EN`):

```bash
make TUI_LANG=EN
```

Flagi można łączyć, np. `make MQTT=1 TUI_LANG=EN`. Stringi zlokalizowane są w `loc.h` — nowy język dodaje się przez kolejny blok `#if defined(LANG_XX)`.

Uruchomienie:

```bash
./stud_sys
```

## MQTT

Moduł `MQTT Publisher` pozostaje w projekcie i może publikować stan aktywnych usług do brokera MQTT. W trybie prezentacyjnym nie jest pokazany w menu głównym, ale kod i konfiguracja nadal są obecne.

Aby budować z obsługą MQTT, potrzebna jest biblioteka `paho-mqtt-c`.

Przykład:

```bash
# macOS
brew install paho-mqtt-c

# Debian/Ubuntu
sudo apt-get install libpaho-mqtt-dev
```

## Najważniejsze pliki

- `core.c` - scheduler, start systemu, menu główne
- `konfig.c` - rejestracja usług
- `budz.c` - główny moduł demonstracyjny
- `sys_rap.c` - raport diagnostyczny
- `docs/presentation_budzik.md` - scenariusz prezentacji
- `docs/user_guide.md` - instrukcja obsługi dla użytkownika
- `docs/architecture.md` - dokumentacja techniczna

## Ograniczenia

- interfejs jest terminalowy i oparty na starym modelu formularzy,
- tryb graficzny jest historyczny i ma ograniczone znaczenie w wersji `ncurses`,
- projekt nie jest nowoczesnym frameworkiem RT; to lekki system demonstracyjny z własnym schedulerem i formularzami.

## Dokumentacja

- `docs/index.md` - strona startowa dokumentacji
- `docs/quick_start.md` - szybkie uruchomienie
- `docs/user_guide.md` - instrukcja użytkownika
- `docs/operator_cheatsheet.md` - skrócona ściąga do prezentacji
- `docs/presentation_budzik.md` - scenariusz demo
- `docs/architecture.md` - opis techniczny
