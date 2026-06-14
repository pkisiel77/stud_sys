# Plan semestralny — rozwój `stud_sys`

## Profil

- **Format**: 1 student, praca solo, ok. 8–12 h tygodniowo
- **Czas**: 15 tygodni (13 roboczych + 2 zaliczeniowe)
- **Poziom**: średni — student zna C i podstawy systemów operacyjnych, nie miał wcześniej kontaktu z tym repo
- **Cel**: dydaktyczny — poznanie schedulera RT, formularzy `blankiet`, persystencji, IPC

## Cele dydaktyczne (sylabus minimum)

Po semestrze student powinien umieć:

1. Czytać i nawigować średniej wielkości codebase w C z function pointers i strukturami callbackowymi.
2. Dodawać nowy moduł do istniejącego silnika usług (rejestracja w `konfig.c`, blankiet, persystencja).
3. Pracować z prostym schedulerem RT: rozumieć cykl `Interval`/`delay`/`number_of_calls`, dopisać własną usługę RT.
4. Integrować persystencję plikową, IPC (MQTT) i testy jednostkowe.
5. Prowadzić projekt w `git` z review co tydzień (1 PR/tydzień jest twardym wymogiem).

## Materiały startowe

- `readme.md` — wprowadzenie
- `docs/architecture.md` — model warstw, scheduler, blankiety
- `docs/user_guide.md` — perspektywa użytkownika
- `docs/operator_cheatsheet.md` — klawisze
- `loc.h` — przykład compile-time configu

## Plan tygodniowy

### Tydzień 1 — Setup i pierwszy build

- **Cel**: środowisko, narzędzia, workflow.
- **Zadanie**:
  - sklonować repo, postawić toolchain (`gcc`, `ncurses`, ew. `paho-mqtt-c`),
  - zbudować `make` i `make TUI_LANG=EN`, uruchomić obie wersje,
  - znaleźć 1 drobny fix (literówka w komentarzu, dead `(void)x;`, niespójne wcięcie),
  - otworzyć PR z poprawką.
- **Deliverable**: PR scalony + krótki screencast (2 min) pokazujący menu PL i EN.
- **Pass**: PR przechodzi build w obu językach, zawiera sensowny commit message.
- **Szacunek**: 4–6 h.

### Tydzień 2 — Mapa kodu

- **Cel**: rozumienie architektury.
- **Zadanie**: wybrać jeden moduł (`budz.c`, `sprawdz.c` lub `sys_rap.c`) i napisać o nim notatkę techniczną w `docs/module_<nazwa>.md`:
  - ASCII diagram cyklu rekordu od `dane_*` → `*_blankiet` → `dec_*` → `*_main`,
  - opis każdego callbacka kontraktu `Service`,
  - 5 kluczowych pól struktury danych usługi.
- **Deliverable**: 1–2 strony Markdowna + PR.
- **Pass**: dokument tłumaczy moduł osobie, która go nie czytała.
- **Szacunek**: 6–8 h.

### Tydzień 3 — Pierwsza zmiana funkcjonalna

- **Cel**: ingerencja w istniejący moduł, persystencja.
- **Zadanie**: dodać do Budzika pole **kategoria** (np. `praca`/`dom`/`sport`/`inne`) jako jednoznakowy wybór z menu w `budz_formularz`. Pole musi:
  - zapisywać się w `~/.stud_sys_budz.dat` (rozszerzyć `struct budz_rekord_pliku`),
  - pokazywać się jako ikona/litera w liście budzików (`budz_menu_live`),
  - mieć wartość domyślną przy migracji starych rekordów.
- **Deliverable**: PR + krótki opis decyzji o migracji.
- **Pass**: stare pliki `.stud_sys_budz.dat` nadal się otwierają.
- **Szacunek**: 8–10 h.

### Tydzień 4 — Walidacja i robustness

- **Cel**: defensywne programowanie.
- **Zadanie**:
  - dodać walidacje wszystkich pól wpisywanych w Budziku (limity, sensowne wartości, zbiór dopuszczalnych znaków),
  - poprawić sytuacje brzegowe znalezione w trakcie ćwiczenia: brak HOME, plik tylko-do-odczytu, dwa procesy `stud_sys` jednocześnie,
  - opisać 6 ręcznych przypadków testowych w `docs/test_budzik.md`.
- **Deliverable**: PR + dokument z testami manualnymi (krok / oczekiwany rezultat / OK).
- **Pass**: 6/6 testów daje zielone w demo na review.
- **Szacunek**: 6–8 h.

### Tydzień 5 — Konfiguracja globalna

- **Cel**: file I/O, format konfiguracji, lifecycle inicjalizacji.
- **Zadanie**: dodać `~/.stud_sys.conf` (key=value, jak `mqtt_pub.c`) z:
  - `default_lang` (PL/EN) — runtime override `TUI_LANG`,
  - `budz_max` — limit liczby budzików,
  - `auto_save_interval_sec`.
- Wczytać przy starcie (`open_sys()`), zapisać przy zmianie z dedykowanego ekranu w "O systemie".
- **Deliverable**: nowy moduł `konfig_user.c`/`.h` + użycia w `core.c` i `admin.c`.
- **Pass**: brak pliku → bezpieczne defaulty, brak crasha.
- **Szacunek**: 8–10 h.

### Tydzień 6 — Nowy moduł cz. 1: Stoper

- **Cel**: pełen lifecycle modułu od zera, wzorzec usługi RT.
- **Zadanie**: zaimplementować `stoper.c` wzorowany na `budz.c`:
  - rejestracja w `konfig.c`,
  - `stoper_main` jako RT callback (zwiększa licznik co `Interval`),
  - blankiet ze start / stop / reset / lista czasów,
  - struktura danych w `dek_stoper.h`.
- **Deliverable**: PR z modułem (jeszcze bez persystencji ani live updates).
- **Pass**: stoper liczy sekundy zgodnie z `Interval`.
- **Szacunek**: 10–12 h.

### Tydzień 7 — Stoper cz. 2: lap / historia / live UI

- **Cel**: praktyka `live menu` (analog do `budz_menu_live`).
- **Zadanie**:
  - dodać lap (round) — lista międzyczasów,
  - dodać "live view" odświeżający się co sekundę bez czyszczenia ekranu,
  - persystencja sesji do `~/.stud_sys_stoper.dat`.
- **Deliverable**: PR + sekcja w `docs/architecture.md` o Stoperze.
- **Pass**: live view nie miga, lapy się zapisują.
- **Szacunek**: 8–10 h.

### Tydzień 8 — Symulator i callbacki RT

- **Cel**: zrozumienie cyklu wywołań RT.
- **Zadanie**: rozszerzyć `sensor_sim.c` o:
  - nowy waveform `t` (trójkątny),
  - tryb "burst" (np. 5 próbek z `Interval=200ms`, potem 10 s ciszy) — wymaga modyfikacji `delay` i `number_of_calls` z poziomu `*_main`,
  - dokumentacja matematyczna w komentarzu nad funkcją generującą próbkę.
- **Deliverable**: PR + porównawczy wykres ASCII obu trybów w opisie PR.
- **Pass**: burst widoczny w `sprawdz` jako gwałtowna zmiana licznika próbek.
- **Szacunek**: 6–8 h.

### Tydzień 9 — Integracja modułów (sensor → budzik)

- **Cel**: cross-module wiring, model "event-driven" w obrębie agendy.
- **Zadanie**: dodać do Budzika tryb **alarmu warunkowego**:
  - budzik nie ma godziny, tylko nasłuchuje stanu `A->rt.alarm` w usłudze sensora,
  - gdy sensor zgłasza alarm zakresu, budzik się uruchamia.
- Skutek: użytkownik tworzy budzik typu "alarm z czujnika XYZ".
- **Deliverable**: PR + krótki sequence diagram w PR description.
- **Pass**: trójkątny waveform z W8 daje cykliczne alarmy budzika w demo.
- **Szacunek**: 8–10 h.

### Tydzień 10 — MQTT subscribe

- **Cel**: IPC, model pub/sub, protokół tekstowy.
- **Zadanie**:
  - rozszerzyć `mqtt_pub.c` o subscribe na temat `<topic>/cmd/add_budzik`,
  - obsłużyć payload JSON typu `{"nazwa":"...","godz":7,"min":30,"co_ile":2,"wyprzedz":15}` — dodawanie budzika zdalnie,
  - bezpieczeństwo: limit długości pól, walidacja zakresów, rate-limit.
- **Deliverable**: PR + dokumentacja protokołu w `docs/mqtt_protocol.md`.
- **Pass**: `mosquitto_pub -t ... -m '{...}'` dodaje budzik widoczny w UI.
- **Szacunek**: 10–12 h.

### Tydzień 11 — Testy jednostkowe

- **Cel**: dyscyplina testowa.
- **Zadanie**:
  - zbudować szkielet testów: `tests/` + `make test`,
  - napisać 8–10 testów dla `ustaw_czas_budzika` i `ustaw_delay_budzika` (różne dni doby, granice 00:00 i 23:59, ujemne `delay`),
  - 1 test integracyjny: utwórz budzik, zapisz, wczytaj, porównaj.
- **Deliverable**: PR + raport pokrycia (ręczny — które funkcje).
- **Pass**: `make test` zwraca 0, wszystkie testy zielone.
- **Szacunek**: 8–10 h.

### Tydzień 12 — Refaktoring i jakość

- **Cel**: czytelność, redukcja długu.
- **Zadanie**:
  - znaleźć 3 funkcje > 100 linii, rozbić na mniejsze ze znaczącymi nazwami,
  - dopisać krótkie komentarze "intencji" (NIE "co robi") tylko tam gdzie jest niesoczywiście,
  - znaleźć i naprawić ≥ 1 realną drobną luką (race, błędny limit, brakujące `free`).
- **Deliverable**: PR z refaktorem + lista przed/po (liczba linii / cyklomatyka jeśli zna `lizard`).
- **Pass**: kod nadal kompiluje się PL i EN, demo działa.
- **Szacunek**: 6–8 h.

### Tydzień 13 — Proposal własnego tematu

- **Cel**: samodzielne zaplanowanie pracy.
- **Zadanie**: napisać `docs/proposal_<student>.md` (1–2 strony):
  - problem,
  - propozycja modułu (np. logger zdarzeń RT, planer cykliczny, monitor zasobów),
  - decyzje techniczne,
  - rozkład prac na W14–W15.
- **Deliverable**: PR z proposalem; review na review-spotkaniu.
- **Pass**: zakres realistyczny w 2 tygodnie pracy solo.
- **Szacunek**: 4–6 h.

### Tydzień 14 — Implementacja proposalu (cz. 1)

- **Cel**: pełna autonomia.
- **Zadanie**: prototyp działającego rozwiązania z proposalu (W13). Minimalna ścieżka happy-path.
- **Deliverable**: PR (może być draft).
- **Pass**: demo na review pokazuje główny przepływ.
- **Szacunek**: 10–12 h.

### Tydzień 15 — Implementacja cz. 2 + obrona

- **Cel**: dokumentacja i prezentacja.
- **Zadanie**:
  - dokończyć moduł (walidacja, persystencja, edge cases),
  - dopisać `docs/module_<own>.md`,
  - przygotować 10-minutową prezentację (slajdy lub live demo).
- **Deliverable**: PR scalony + prezentacja.
- **Pass**: kod scalony do `main`, demo działa, prezentacja pokazuje wybór projektowy z W13.
- **Szacunek**: 12–15 h.

## Kryteria zaliczenia

| Element | Waga |
|---|---|
| 15 tygodniowych PR (pass/fail) | 60% |
| Jakość kodu (review, brak regresji, czytelność) | 15% |
| Dokumentacja (`docs/`) | 10% |
| Obrona / prezentacja końcowa | 15% |

Próg zaliczenia: **60% punktów**. Stopień: 60–74% → 3, 75–89% → 4, 90+ → 5.

**Twarde minima**:

- 12 z 15 PR scalonych (max 3 nieudane tygodnie),
- testy `make test` zielone na koniec semestru,
- proposal i jego implementacja oddane.

## Wskazówki dla prowadzącego

- **Cotygodniowy review**: 30 min, code review + dyskusja kolejnego tygodnia. Najlepiej w PR-ze (komentarze inline) plus krótka rozmowa.
- **Wczesny sygnał**: jeśli student wymięka na W3–W4, prawdopodobnie nie zna `function pointers` i `void*` w C — zatrzymać i nadrobić, zanim wejdzie w moduły.
- **Nie przyspieszać**: tempo 1 PR/tydzień jest celem dydaktycznym, nie organizacyjnym. Lepiej żeby student dobrze zrobił W3 niż w pośpiechu skoczył do W6.
- **Demo zamiast slajdów**: w trakcie review wymagać uruchomienia `./stud_sys` z nowym kodem, nie tylko diffu.

## Backup tematy (gdy student wyrobi się szybciej)

Jeśli student kończy zadanie tygodnia w 4–5 h, można dorzucić jedno z:

- przeniesienie `loc.h` na trzeci język (DE / UK / dowolny),
- generator scen testowych dla `make test` (np. property-based dla `ustaw_czas_budzika`),
- profiling: `gprof`/`perf` schedulera i raport,
- rozszerzenie sensora o czytanie z pliku zamiast generatora,
- integracja CI (GitHub Actions: build PL+EN+MQTT, `make test`).

## Backup tematy (gdy student utknie)

Jeśli student nie daje rady z modułem RT (W6–W10), zamienić na:

- **Stoper light**: bez RT callback, tylko prosty formularz wpisu czasu ręcznie,
- **MQTT light**: tylko publish, bez subscribe,
- **Brak własnego proposalu**: dostać "z półki" — np. eksporter rekordów agendy do CSV.

Każda zamiana to ½ wagi zadania w ocenie końcowej.
