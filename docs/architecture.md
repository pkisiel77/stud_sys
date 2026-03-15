# Architektura systemu

## Cel

Ten dokument opisuje techniczną budowę `stud_sys` z perspektywy developera. Koncentruje się na aktualnym stanie projektu: schedulerze RT, rekordach `agenda`, silniku blankietów oraz głównych modułach funkcjonalnych.

## Warstwy systemu

Projekt ma cztery główne warstwy:

1. definicja usług i start systemu,
2. scheduler i agenda zadań,
3. silnik blankietów i raportów,
4. moduły funkcjonalne.

## Start systemu

Punkt wejścia:

- `core.c`

Przebieg startu:

1. `main()` inicjalizuje konsolę przez `InitConsole()`.
2. `open_sys()` buduje konfigurację usług przez `set_services()`.
3. `open_sys()` inicjalizuje rekordy startowe `agenda` dla usług RT.
4. menu główne jest budowane przez `build_presentation_menu()`.
5. wejście w pozycję menu kieruje do `raporty(...)`.

W aktualnym trybie prezentacyjnym menu pokazuje tylko:

- `Budzik`
- `-- Stan systemu --`
- przełącznik tekst/grafika

## Rejestracja usług

Plik:

- `konfig.c`

Każda usługa ma wpis `struct Service`.

Najważniejsze pola:

- `name` - nazwa usługi,
- `kod_uslugi` - numer logiczny usługi,
- `str_size` - rozmiar rekordu,
- `main_modul` - callback RT uruchamiany przez scheduler,
- `def_blankiet` - formularz/raport,
- `dane_rap_bl` - dostarczenie danych do blankietu,
- `wpis_rap_bl` - zapis zmian,
- `decyzje` - obsługa decyzji z blankietu.

Aktywne moduły zarejestrowane w systemie:

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

Najważniejsze pola:

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

- `Interval` określa cykl uruchamiania,
- `delay` określa czas do najbliższego uruchomienia,
- `prior` i `cur_prior` sterują wyborem zadania,
- `S` łączy rekord z definicją `struct Service`.

W praktyce rekord `agenda` jest nagłówkiem rekordu usługi. Za nim może znajdować się payload specyficzny dla modułu, np.:

- `struct budzik`
- `struct mqtt_cfg`
- `struct sensor_sim`

## Przepływ wykonania RT

Główny przepływ:

1. rekord znajduje się w `SysA`,
2. `check_agenda()` zmniejsza `delay`,
3. gotowe rekordy trafiają do kolejki przez `insert_to_queue()`,
4. `check_queue()` wybiera rekord do wykonania,
5. wywoływany jest `A->S->main_modul(A)`,
6. po wykonaniu rekord wraca do dalszego harmonogramowania albo jest usuwany.

Powiązane funkcje:

- `check_agenda()`
- `insert_to_agenda()`
- `insert_to_queue()`
- `check_queue()`
- `cancel_serv()`

## Pamięć i cykl życia rekordu

Rekordy usług są alokowane dynamicznie.

Główne miejsca:

- `service_default()` - tworzenie domyślnego rekordu usługi,
- `insert_to_agenda()` - kopiowanie lub dołączanie do agendy,
- `free_agenda_mem()` - zwalnianie pamięci pól dynamicznych,
- `free_service()` - zwalnianie całego rekordu.

Ważne:

- `agenda->name` jest alokowane dynamicznie i musi być zwalniane przez wspólny cleanup,
- rekordy raportowe i rekordy RT nie powinny mieszać wskaźników do tymczasowych danych.

## Silnik blankietów

Pliki:

- `blank/wewy.c`
- `rap_bl.c`

Silnik odpowiada za:

- rysowanie pól i okien,
- obsługę Enter/Esc,
- przejścia między kontrolkami,
- wykonanie decyzji zdefiniowanych przez moduł.

Typowy przepływ formularza:

1. `raporty(...)` uruchamia raport dla usługi,
2. `def_blankiet(...)` renderuje i obsługuje pola,
3. kontrolki zwracają numer kolejnego kroku,
4. decyzja trafia do `decyzje(...)`,
5. wynik decyzji określa zapis, wyjście albo przejście dalej.

To jest najwrażliwsza warstwa projektu. Błędne wskaźniki lub zły kontrakt rekordu zwykle kończą się tu crashem albo zapętleniem.

## Kontrakt modułu blankietu

Każdy moduł raportowy powinien trzymać się tego kontraktu:

1. `dane_rap_bl()` musi zwrócić poprawny wskaźnik do danych i ustawić `*rozmiar_ob`,
2. `def_blankiet()` musi pracować na poprawnym rekordzie,
3. kontrolki grupowe muszą dostać adres rekordu, a nie przypadkowego pola,
4. `decyzje()` musi zwracać kody zgodne z silnikiem raportu,
5. zapis nie może mieć niespodziewanych efektów ubocznych przy zwykłym otwarciu raportu.

To był główny obszar ostatnich napraw stabilizacyjnych.

## Moduł Budzik

Plik:

- `budz.c`

Rola:

- główny moduł prezentacyjny,
- zarządzanie listą alarmów,
- formularz dodawania i edycji wpisu,
- przeliczanie czasu do alarmu,
- widok listy z żywą aktualizacją countdownu.

Ważne elementy:

- `budz_main()` - callback RT,
- `budz_blankiet()` - główny formularz usługi,
- `budz_menu_live()` - lista z dynamicznym odświeżaniem,
- `ustaw_czas_budzika()` i `ustaw_delay_budzika()` - przeliczenia czasu.

## Raport Stan systemu

Plik:

- `sys_rap.c`

Rola:

- podgląd rekordów `agenda`,
- diagnostyka pracy schedulera,
- wgląd w okres, priorytet i opóźnienie zadań.

Obecna wersja została uproszczona tak, aby uniknąć błędów grupowej kontrolki rekordowej. Raport korzysta z prostszego renderowania rekordu.

## MQTT i czujnik symulowany

Pliki:

- `mqtt_pub.c`
- `sensor_sim.c`

Rola:

- `MQTT Publisher` publikuje dane aktywnych usług do brokera,
- `Czujnik Sim` generuje przykładowe dane okresowe.

W trybie prezentacyjnym nie są eksponowane w menu głównym, ale pozostają częścią architektury i konfiguracji usług.

## Warstwa terminala

Pliki:

- `blank/term_ncurses.c`
- `blank/konsola.c`

Rola:

- inicjalizacja `ncurses`,
- mapowanie klawiszy specjalnych,
- operacje ekranowe,
- buforowanie i flush terminala.

Przy pracy z UI trzeba uważać na:

- pełne repainty całego okna,
- wypisy na `stderr`,
- zbyt częste odświeżanie w pętli wejścia.

To były źródła migania i problemów z interfejsem w ostatnich iteracjach.

## Pliki kluczowe

- `core.c` - scheduler, start, główne menu
- `konfig.c` - definicja usług
- `blank/sys_dekl.h` - struktury `Service` i `agenda`
- `blank/wewy.c` - silnik blankietów
- `rap_bl.c` - przepływ raportu
- `budz.c` - główny moduł demonstracyjny
- `sys_rap.c` - raport diagnostyczny

## Zależności runtime

Minimalne:

- `ncurses`
- standardowa biblioteka C

Opcjonalne:

- `paho-mqtt-c` dla `make MQTT=1`

## Rekomendacje dla dalszego rozwoju

1. nie rozpraszać logiki wyjść blankietów po modułach,
2. pilnować kontraktu `dane_rap_bl()` i `def_blankiet()`,
3. ograniczać repainty tylko do zmienionych fragmentów UI,
4. trzymać `agenda` jako jedyne źródło prawdy dla zadań RT,
5. utrzymywać dokumentację zgodną z aktualnym stanem menu i konfiguracji.
