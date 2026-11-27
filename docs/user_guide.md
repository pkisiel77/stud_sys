# Instrukcja użytkownika

## 1. Do czego służy system

`stud_sys` służy do prezentacji i obsługi prostego systemu czasu rzeczywistego w terminalu. W obecnej konfiguracji głównym zadaniem użytkowym jest `Budzik`.

System pozwala:

- tworzyć wpisy budzików,
- obserwować odliczanie do alarmu,
- przeglądać stan schedulera RT,
- pokazać działanie zadań czasowych podczas prezentacji.

## 2. Co widzi użytkownik po uruchomieniu

Po starcie programu widoczne są trzy pozycje:

- `Budzik` - lista budzików i formularz budzika,
- `Demo IoT: czujnik symulowany` - uruchamia symulowany czujnik i pokazuje jego pomiary w `Stanie systemu`,
- `-- Stan systemu --` - podgląd na żywo wszystkich zadań schedulera RT.

Po powrocie z dowolnej pozycji kursor zostaje na niej, więc `Enter` otwiera ją ponownie.

## 3. Sterowanie

Podstawowe klawisze:

- `Strzałki góra/dół` - zmiana zaznaczenia
- `Enter` - wejście do pozycji lub akceptacja pola
- `Esc` - wyjście z bieżącego okna (w polu wpisu: anulowanie zmiany)
- cyfry i `Backspace` - wpisywanie wartości w polu wpisu na dole ekranu

## 4. Moduł Budzik

### Co pokazuje lista

Każdy wiersz listy budzików zawiera:

- godzinę budzika (`HH:MM`),
- w nawiasie czas pozostały do najbliższego monitu,
- nazwę budzika (pole `Sprawa`).

Pod budzikami są pozycje `+++ Dodaj nowy budzik +++` i `Wyjdz`.

Odliczanie na liście odświeża się na żywo.

### Co można zrobić

W module `Budzik` można:

- dodać nowy budzik,
- edytować istniejący budzik,
- usunąć budzik,
- obserwować ile czasu zostało do alarmu.

### Formularz budzika

Formularz ma trzy pola:

| Pole | Znaczenie |
|---|---|
| `Sprawa` | nazwa budzika |
| `Godzina` | godzina i minuta budzika |
| `Co ile` / `wyprzedz` | od ilu minut przed godziną budzika zaczynają się monity (`Wyprzedzenie`, min. 1) i co ile minut się powtarzają (`Co ile`) |

Aby zmienić pole, ustaw na nim kursor i naciśnij `Enter`. Na dole ekranu pojawi się pole wpisu:
wpisz wartość, `Enter` zatwierdza, `Esc` anuluje. `Godzina` pyta kolejno o godzinę i minutę,
a `Co ile` - o odstęp monitów i o wyprzedzenie.

### Dodawanie budzika

1. Wejdź w `Budzik`.
2. Wybierz `+++ Dodaj nowy budzik +++`.
3. Ustaw `Sprawa`, `Godzina` oraz `Co ile` / `wyprzedz`.
4. Wybierz `>>> Wpisz do agendy <<<`.
5. Na liście obserwuj odliczanie do pierwszego monitu.

Budzik jest zapisywany w pliku `~/.stud_sys_budz.dat` i wraca po ponownym uruchomieniu programu.

### Edycja i usuwanie

Wybierz budzik na liście i naciśnij `Enter`. Formularz ma wtedy pozycje
`>>> Zatwierdz zmiany <<<`, `>>> Usun budzik <<<` i `Wyjdz (bez zmian)`.

### Alarm

Gdy zaczyna się okno wyprzedzenia, pojawia się okno `BUDZIK` z nazwą budzika, godziną
i czasem do następnego monitu. `Enter` wyłącza sygnał, `Esc` zamyka monit.

### Co oznacza czas pozostały

Pole czasu pozostałego pokazuje, ile czasu zostało do najbliższego uruchomienia danego rekordu.

Format:

- `MM:SS` dla krótszych czasów,
- `HH:MM:SS` dla dłuższych czasów.

## 5. Raport Stan systemu

Ekran `-- Stan systemu --` pokazuje na żywo wszystkie zadania w agendzie schedulera RT.
W nagłówku jest liczba aktywnych zadań, długość kolejki i liczba alarmów, a w tabeli:

| Kolumna | Znaczenie |
|---|---|
| `USLUGA` | nazwa usługi (np. `Sprawdz.otoczenia`, `Budzik`, `Czujnik Sim`) |
| `STAN` | stan zadania, np. `CZEKA` |
| `NAST.` | sekundy do następnego wywołania |
| `CYKL` | okres wywołań w sekundach |
| `PRIOR` | priorytet |
| `WARTOSC` | ostatni pomiar zadania (np. czujnika) |
| `ALARM` | stan alarmu pomiaru |

Pod tabelą są szczegóły zaznaczonego zadania. Klawisze: `Góra/Dół` - wybór zadania,
`PgUp/PgDn` - strona, `R` - odświeżenie, `Esc` - powrót do menu głównego.

Najlepiej wejść tu po dodaniu budzika: rekord `Budzik` pokazuje, że wpis trafił do schedulera.

## 6. Demo IoT

`Demo IoT: czujnik symulowany` dodaje do agendy zadanie `Czujnik Sim` (symulowany pomiar co sekundę)
i od razu otwiera `Stan systemu`, gdzie w kolumnie `WARTOSC` widać zmieniający się pomiar.

## 7. Przykładowy scenariusz prezentacji

Najprostszy pokaz (ok. 3 min):

1. Uruchom program.
2. Wejdź w `Budzik` i wybierz `+++ Dodaj nowy budzik +++`.
3. Ustaw: `Sprawa` = `Demo`, `Godzina` = bieżąca godzina i minuta **+3 min**, `Co ile` = `1`, `Wyprzedzenie` = `1`.
4. Wybierz `>>> Wpisz do agendy <<<` - na liście widać odliczanie do pierwszego monitu (ok. 1-2 min).
5. `Esc` do menu głównego i wejdź w `-- Stan systemu --`.
6. Pokaż rekord `Budzik`: `NAST.` (sekundy do wywołania) i `CYKL`.
7. `Esc` i wróć do `Budzik`.
8. Poczekaj na okno alarmu.

Budzik nie może zadzwonić "za 10 sekund": czas ustawia się z dokładnością do minuty, a monity zaczynają się
na początku okna wyprzedzenia (min. 1 min przed godziną budzika).

## 8. Jak interpretować system podczas demo

Warto tłumaczyć system w trzech punktach:

1. `Budzik` jest zadaniem czasowym.
2. Scheduler RT przechowuje je w agendzie i odlicza czas do uruchomienia.
3. `Stan systemu` pokazuje wewnętrzny stan tego mechanizmu.

## 9. Typowe problemy

### Lista budzików odświeża się

Lista odświeża countdown na żywo. System został ograniczony do odświeżania przy zmianie sekundy lub zmiany zaznaczenia, ale przy pracy terminalowej może być nadal widoczne lekkie przepisywanie tekstu.

### Nie mogę wyjść z okna

Użyj `Esc`.

### Nie widzę innych modułów

To jest zamierzone. Program działa w trybie prezentacyjnym: menu główne pokazuje `Budzik`, `Demo IoT` i `Stan systemu`, a pozostałe moduły (np. `MQTT Publisher`) zostają w kodzie.

### Alarm pojawił się od razu po zapisaniu budzika

Godzina budzika minus wyprzedzenie wypadła w bieżącej minucie lub wcześniej, więc monit startuje natychmiast. Ustaw godzinę co najmniej 2-3 minuty do przodu.

## 10. Dla kogo jest ten system

System jest przeznaczony do:

- prezentacji prostego schedulera RT,
- pokazu formularzy tekstowych,
- demonstracji zadania czasowego na żywo,
- testów terminalowego interfejsu `ncurses`.
