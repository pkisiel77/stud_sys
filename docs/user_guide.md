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

- `Budzik`
- `-- Stan systemu --`
- `Zmiana trybu na GRAFICZNY/TEKSTOWY`

Najczęściej używane będą pierwsze dwie.

## 3. Sterowanie

Podstawowe klawisze:

- `Strzałki góra/dół` - zmiana zaznaczenia
- `Enter` - wejście do pozycji lub akceptacja pola
- `Esc` - wyjście z bieżącego okna
- klawisze znakowe, np. `t`, `n`, `m` - wybory w polach decyzyjnych

## 4. Moduł Budzik

### Co pokazuje lista

Lista budzików zawiera:

- godzinę alarmu,
- typ wpisu,
- nazwę budzika,
- czas pozostały do uruchomienia.

Odliczanie na liście odświeża się na żywo.

### Co można zrobić

W module `Budzik` można:

- dodać nowy budzik,
- edytować istniejący budzik,
- usunąć budzik,
- obserwować ile czasu zostało do alarmu.

### Dodawanie budzika

Typowy przebieg:

1. Wejdź w `Budzik`.
2. Wybierz `+++ Dodaj nowy budzik +++`.
3. Ustaw parametry wpisu.
4. Zatwierdź zapis.
5. Wróć do listy i obserwuj odliczanie.

### Co oznacza czas pozostały

Pole czasu pozostałego pokazuje, ile czasu zostało do najbliższego uruchomienia danego rekordu.

Format:

- `MM:SS` dla krótszych czasów,
- `HH:MM:SS` dla dłuższych czasów.

## 5. Raport Stan systemu

Raport `-- Stan systemu --` służy do sprawdzania, jak system widzi aktywne rekordy RT.

Można tam zobaczyć:

- nazwę usługi,
- typ pracy,
- priorytet,
- cykl uruchomień,
- opóźnienie do kolejnego wykonania.

To jest ekran diagnostyczny. Najlepiej używać go po dodaniu budzika, aby pokazać, że wpis został poprawnie przyjęty przez scheduler.

## 6. Przykładowy scenariusz prezentacji

Najprostszy pokaz:

1. Uruchom program.
2. Wejdź w `Budzik`.
3. Dodaj budzik za `10 s`.
4. Wróć do listy budzików.
5. Pokaż odliczanie czasu pozostałego.
6. Wejdź w `-- Stan systemu --`.
7. Pokaż rekord budzika z okresem i priorytetem.
8. Wróć do `Budzik`.
9. Poczekaj na alarm.

## 7. Jak interpretować system podczas demo

Warto tłumaczyć system w trzech punktach:

1. `Budzik` jest zadaniem czasowym.
2. Scheduler RT przechowuje je w agendzie i odlicza czas do uruchomienia.
3. `Stan systemu` pokazuje wewnętrzny stan tego mechanizmu.

## 8. Typowe problemy

### Lista budzików odświeża się

Lista odświeża countdown na żywo. System został ograniczony do odświeżania przy zmianie sekundy lub zmiany zaznaczenia, ale przy pracy terminalowej może być nadal widoczne lekkie przepisywanie tekstu.

### Nie mogę wyjść z okna

Użyj `Esc`.

### Nie widzę innych modułów

To jest zamierzone. Program działa w trybie prezentacyjnym, gdzie menu główne pokazuje głównie `Budzik` i raport systemowy.

## 9. Dla kogo jest ten system

System jest przeznaczony do:

- prezentacji prostego schedulera RT,
- pokazu formularzy tekstowych,
- demonstracji zadania czasowego na żywo,
- testów terminalowego interfejsu `ncurses`.
