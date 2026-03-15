# Architektura Systemu

## Cel

Ten dokument opisuje techniczna budowe `stud_sys` z perspektywy developera. Koncentruje sie na aktualnym stanie projektu: schedulerze RT, rekordach `agenda`, silniku blankietow oraz glownych modulach funkcjonalnych.

## Warstwy systemu

Projekt ma cztery glowne warstwy:

1. definicja uslug i start systemu,
2. scheduler i agenda zadan,
3. silnik blankietow i raportow,
4. moduly funkcjonalne.

## Start systemu

Punkt wejscia:

- `core.c`

Przebieg startu:

1. `main()` inicjalizuje konsole przez `InitConsole()`.
2. `open_sys()` buduje konfiguracje uslug przez `set_services()`.
3. `open_sys()` inicjalizuje rekordy startowe `agenda` dla uslug RT.
4. menu glowne jest budowane przez `build_presentation_menu()`.
5. wejscie w pozycje menu kieruje do `raporty(...)`.

W aktualnym trybie prezentacyjnym menu pokazuje tylko:

- `Budzik`
- `-- Stan systemu --`
- przelacznik tekst/grafika

## Rejestracja uslug

Plik:

- `konfig.c`

Kazda usluga ma wpis `struct Service`.

Najwazniejsze pola:

- `name` - nazwa uslugi,
- `kod_uslugi` - numer logiczny uslugi,
- `str_size` - rozmiar rekordu,
- `main_modul` - callback RT uruchamiany przez scheduler,
- `def_blankiet` - formularz/raport,
- `dane_rap_bl` - dostarczenie danych do blankietu,
- `wpis_rap_bl` - zapis zmian,
- `decyzje` - obsluga decyzji z blankietu.

Aktywne moduly zarejestrowane w systemie:

- `Sprawdz.otoczenia`
- `Budzik`
- `O systemie...`
- `MQTT Publisher`
- `Czujnik Sim`
- `-- Stan systemu --`

## Model danych RT

Podstawowa struktura:

- `blank/sys_dekl.h`

Centralnym obiektem systemu jest `struct agenda`.

Najwazniejsze pola:

- `name`
- `mode`
- `Interval`
- `delay`
- `prior`
- `prior_plus`
- `cur_prior`
- `number_of_calls`
- `S`

Znaczenie:

- `Interval` okresla cykl uruchamiania,
- `delay` okresla czas do najblizszego uruchomienia,
- `prior` i `cur_prior` steruja wyborem zadania,
- `S` laczy rekord z definicja `struct Service`.

W praktyce rekord `agenda` jest naglowkiem rekordu uslugi. Za nim moze znajdowac sie payload specyficzny dla modulu, np.:

- `struct budzik`
- `struct mqtt_cfg`
- `struct sensor_sim`

## Przeplyw wykonania RT

Glowny przeplyw:

1. rekord znajduje sie w `SysA`,
2. `check_agenda()` zmniejsza `delay`,
3. gotowe rekordy trafiaja do kolejki przez `insert_to_queue()`,
4. `check_queue()` wybiera rekord do wykonania,
5. wywolywany jest `A->S->main_modul(A)`,
6. po wykonaniu rekord wraca do dalszego harmonogramowania albo jest usuwany.

Powiazane funkcje:

- `check_agenda()`
- `insert_to_agenda()`
- `insert_to_queue()`
- `check_queue()`
- `cancel_serv()`

## Pamiec i cykl zycia rekordu

Rekordy uslug sa alokowane dynamicznie.

Glowne miejsca:

- `service_default()` - tworzenie domyslnego rekordu uslugi,
- `insert_to_agenda()` - kopiowanie lub dolaczanie do agendy,
- `free_agenda_mem()` - zwalnianie pamieci pol dynamicznych,
- `free_service()` - zwalnianie calego rekordu.

Wazne:

- `agenda->name` jest alokowane dynamicznie i musi byc zwalniane przez wspolny cleanup,
- rekordy raportowe i rekordy RT nie powinny mieszac wskaznikow do tymczasowych danych.

## Silnik blankietow

Pliki:

- `blank/wewy.c`
- `rap_bl.c`

Silnik odpowiada za:

- rysowanie pol i okien,
- obsluge Enter/Esc,
- przejscia miedzy kontrolkami,
- wykonanie decyzji zdefiniowanych przez modul.

Typowy przeplyw formularza:

1. `raporty(...)` uruchamia raport dla uslugi,
2. `def_blankiet(...)` renderuje i obsluguje pola,
3. kontrolki zwracaja numer kolejnego kroku,
4. decyzja trafia do `decyzje(...)`,
5. wynik decyzji okresla zapis, wyjscie albo przejscie dalej.

To jest najwrazliwsza warstwa projektu. Bledne wskazniki lub zly kontrakt rekordu zwykle koncza sie tu crashem albo zapetleniem.

## Kontrakt modulu blankietu

Kazdy modul raportowy powinien trzymac sie tego kontraktu:

1. `dane_rap_bl()` musi zwrocic poprawny wskaznik do danych i ustawic `*rozmiar_ob`,
2. `def_blankiet()` musi pracowac na poprawnym rekordzie,
3. kontrolki grupowe musza dostac adres rekordu, a nie przypadkowego pola,
4. `decyzje()` musi zwracac kody zgodne z silnikiem raportu,
5. zapis nie moze miec niespodziewanych efektow ubocznych przy zwyklym otwarciu raportu.

To byl glowny obszar ostatnich napraw stabilizacyjnych.

## Modul Budzik

Plik:

- `budz.c`

Rola:

- glowny modul prezentacyjny,
- zarzadzanie lista alarmow,
- formularz dodawania i edycji wpisu,
- przeliczanie czasu do alarmu,
- widok listy z zywa aktualizacja countdownu.

Wazne elementy:

- `budz_main()` - callback RT,
- `budz_blankiet()` - glowny formularz uslugi,
- `budz_menu_live()` - lista z dynamicznym odswiezaniem,
- `ustaw_czas_budzika()` i `ustaw_delay_budzika()` - przeliczenia czasu.

## Raport Stan systemu

Plik:

- `sys_rap.c`

Rola:

- podglad rekordow `agenda`,
- diagnostyka pracy schedulera,
- wglad w okres, priorytet i opoznienie zadan.

Obecna wersja zostala uproszczona tak, aby uniknac bledow grupowej kontrolki rekordowej. Raport korzysta z prostszego renderowania rekordu.

## MQTT i czujnik symulowany

Pliki:

- `mqtt_pub.c`
- `sensor_sim.c`

Rola:

- `MQTT Publisher` publikuje dane aktywnych uslug do brokera,
- `Czujnik Sim` generuje przykladowe dane okresowe.

W trybie prezentacyjnym nie sa eksponowane w menu glownym, ale pozostaja czescia architektury i konfiguracji uslug.

## Warstwa terminala

Pliki:

- `blank/term_ncurses.c`
- `blank/konsola.c`

Rola:

- inicjalizacja `ncurses`,
- mapowanie klawiszy specjalnych,
- operacje ekranowe,
- buforowanie i flush terminala.

Przy pracy z UI trzeba uwazac na:

- pelne repainty calego okna,
- wypisy na `stderr`,
- zbyt czeste odswiezanie w petli wejscia.

To byly zrodla migania i problemow z interfejsem w ostatnich iteracjach.

## Pliki kluczowe

- `core.c` - scheduler, start, glowne menu
- `konfig.c` - definicja uslug
- `blank/sys_dekl.h` - struktury `Service` i `agenda`
- `blank/wewy.c` - silnik blankietow
- `rap_bl.c` - przeplyw raportu
- `budz.c` - glowny modul demonstracyjny
- `sys_rap.c` - raport diagnostyczny

## Zaleznosci runtime

Minimalne:

- `ncurses`
- standardowa biblioteka C

Opcjonalne:

- `paho-mqtt-c` dla `make MQTT=1`

## Rekomendacje dla dalszego rozwoju

1. nie rozpraszac logiki wyjsc blankietow po modulach,
2. pilnowac kontraktu `dane_rap_bl()` i `def_blankiet()`,
3. ograniczac repainty tylko do zmienionych fragmentow UI,
4. trzymac `agenda` jako jedyne zrodlo prawdy dla zadan RT,
5. utrzymywac dokumentacje zgodna z aktualnym stanem menu i konfiguracji.
