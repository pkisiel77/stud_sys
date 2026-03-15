# Instrukcja Uzytkownika

## 1. Do czego sluzy system

`stud_sys` sluzy do prezentacji i obslugi prostego systemu czasu rzeczywistego w terminalu. W obecnej konfiguracji glownym zadaniem uzytkowym jest `Budzik`.

System pozwala:

- tworzyc wpisy budzikow,
- obserwowac odliczanie do alarmu,
- przegladac stan schedulera RT,
- pokazac dzialanie zadan czasowych podczas prezentacji.

## 2. Co widzi uzytkownik po uruchomieniu

Po starcie programu widoczne sa trzy pozycje:

- `Budzik`
- `-- Stan systemu --`
- `Zmiana trybu na GRAFICZNY/TEKSTOWY`

Najczesciej uzywane beda pierwsze dwie.

## 3. Sterowanie

Podstawowe klawisze:

- `Strzalki gora/dol` - zmiana zaznaczenia
- `Enter` - wejscie do pozycji lub akceptacja pola
- `Esc` - wyjscie z biezacego okna
- klawisze znakowe, np. `t`, `n`, `m` - wybory w polach decyzyjnych

## 4. Modul Budzik

### Co pokazuje lista

Lista budzikow zawiera:

- godzine alarmu,
- typ wpisu,
- nazwe budzika,
- czas pozostaly do uruchomienia.

Odliczanie na liscie odswieza sie na zywo.

### Co mozna zrobic

W module `Budzik` mozna:

- dodac nowy budzik,
- edytowac istniejacy budzik,
- usunac budzik,
- obserwowac ile czasu zostalo do alarmu.

### Dodawanie budzika

Typowy przebieg:

1. Wejdz w `Budzik`.
2. Wybierz `+++ Dodaj nowy budzik +++`.
3. Ustaw parametry wpisu.
4. Zatwierdz zapis.
5. Wroc do listy i obserwuj odliczanie.

### Co oznacza czas pozostaly

Pole czasu pozostalego pokazuje, ile czasu zostalo do najblizszego uruchomienia danego rekordu.

Format:

- `MM:SS` dla krotszych czasow,
- `HH:MM:SS` dla dluzszych czasow.

## 5. Raport Stan systemu

Raport `-- Stan systemu --` sluzy do sprawdzania, jak system widzi aktywne rekordy RT.

Mozna tam zobaczyc:

- nazwe uslugi,
- typ pracy,
- priorytet,
- cykl uruchomien,
- opoznienie do kolejnego wykonania.

To jest ekran diagnostyczny. Najlepiej uzywac go po dodaniu budzika, aby pokazac, ze wpis zostal poprawnie przyjety przez scheduler.

## 6. Przykladowy scenariusz prezentacji

Najprostszy pokaz:

1. Uruchom program.
2. Wejdz w `Budzik`.
3. Dodaj budzik za `10 s`.
4. Wroc do listy budzikow.
5. Pokaz odliczanie czasu pozostalego.
6. Wejdz w `-- Stan systemu --`.
7. Pokaz rekord budzika z okresem i priorytetem.
8. Wroc do `Budzik`.
9. Poczekaj na alarm.

## 7. Jak interpretowac system podczas demo

Warto tlumaczyc system w trzech punktach:

1. `Budzik` jest zadaniem czasowym.
2. Scheduler RT przechowuje je w agendzie i odlicza czas do uruchomienia.
3. `Stan systemu` pokazuje wewnetrzny stan tego mechanizmu.

## 8. Typowe problemy

### Lista budzikow odswieza sie

Lista odswieza countdown na zywo. System zostal ograniczony do odswiezania przy zmianie sekundy lub zmiany zaznaczenia, ale przy pracy terminalowej moze byc nadal widoczne lekkie przepisywanie tekstu.

### Nie moge wyjsc z okna

Uzyj `Esc`.

### Nie widze innych modulow

To jest zamierzone. Program dziala w trybie prezentacyjnym, gdzie menu glowne pokazuje glownie `Budzik` i raport systemowy.

## 9. Dla kogo jest ten system

System jest przeznaczony do:

- prezentacji prostego schedulera RT,
- pokazu formularzy tekstowych,
- demonstracji zadania czasowego na zywo,
- testow terminalowego interfejsu `ncurses`.
