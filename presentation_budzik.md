# Prezentacja Demo: Budzik

## Cel
Pokazac, ze `stud_sys` wykonuje zadania czasowe w czasie rzeczywistym i pozwala nimi zarzadzac z poziomu blankietu.

## Widok prezentacyjny
W menu glowym widoczne sa tylko:

- `Budzik`
- `-- Stan systemu --`
- `Zmiana trybu na GRAFICZNY/TEKSTOWY`

Pozostale uslugi sa ukryte, ale logika systemu pozostaje aktywna.

## Scenariusz 3-5 minut

1. Wejdz w `Budzik`.
2. Pokaz liste budzikow i dodaj nowy wpis.
3. Ustaw budzik jednorazowy za `10 s`.
4. Ustaw drugi budzik za `30 s`.
5. Pokaz, ze mozna wrocic do listy i edytowac/usuwac wpisy.
6. Wejdz w `-- Stan systemu --`.
7. Pokaz dla rekordow:
   - `Cykl`
   - `Priorytet`
   - `Opozn`
   - `Stan realizacji`
8. Wroc do `Budzik` i poczekaj na wyzwolenie alarmu.

## Co mowic na prezentacji

### Warstwa RT
- `Sprawdz.otoczenia` dziala w tle jako zadanie nadzorcze.
- `Budzik` jest zadaniem czasowym sterowanym przez harmonogram.
- `Stan systemu` pokazuje aktualny stan kolejkowania i opoznien.

### Czas rzeczywisty
- Budzik za `10 s` pokazuje reakcje w krotkim horyzoncie czasowym.
- Budzik za `30 s` pokazuje, ze system utrzymuje wiele zadan jednoczesnie.
- W `Stan systemu` mozna pokazac, ze zadanie ma okres i priorytet.

## Proponowane ustawienia demo

### Budzik 1
- typ: `t` jednorazowy
- czas do uruchomienia: `10 s`
- priorytet: `80`

### Budzik 2
- typ: `t` jednorazowy
- czas do uruchomienia: `30 s`
- priorytet: `60`

## Najkrotsza wersja demo

1. `Budzik`
2. dodanie wpisu za `10 s`
3. `-- Stan systemu --`
4. powrot do `Budzik`
5. alarm

## Najwazniejszy przekaz

System nie jest tylko lista formularzy. To prosty scheduler RT:

- przyjmuje zadania czasowe,
- utrzymuje je w agendzie,
- wykonuje wedlug czasu i priorytetu,
- pozwala podejrzec stan pracy w raporcie systemowym.

## Wersja 2 min

1. Wejdz w `Budzik`.
2. Dodaj budzik jednorazowy za `10 s`.
3. Wroc do listy budzikow.
4. Wejdz w `-- Stan systemu --`.
5. Pokaz:
   - rekord budzika,
   - `Cykl`,
   - `Priorytet`,
   - `Opozn`.
6. Wroc do `Budzik` i poczekaj na alarm.

### Co powiedziec
- System przyjmuje zadanie czasowe.
- Zadanie trafia do agendy i jest widoczne w raporcie systemowym.
- Po zadanym czasie scheduler uruchamia budzik.

## Wersja 5 min

1. Wejdz w `Budzik`.
2. Pokaz istniejace wpisy lub pusta liste.
3. Dodaj budzik `A` za `10 s`, priorytet `80`.
4. Dodaj budzik `B` za `30 s`, priorytet `60`.
5. Pokaz, ze wpisy mozna edytowac albo usuwac.
6. Wejdz w `-- Stan systemu --`.
7. Pokaz dwa rekordy budzikow i omow:
   - typ zadania,
   - czas/cykl,
   - priorytet,
   - opoznienie.
8. Wroc do `Budzik`.
9. Poczekaj na pierwszy alarm.
10. Pokaz, ze drugi budzik nadal pozostaje aktywny.

### Co powiedziec
- System obsluguje wiele zadan czasowych jednoczesnie.
- Zadania maja rozne czasy uruchomienia i priorytety.
- `Stan systemu` pokazuje wewnetrzny obraz pracy harmonogramu RT.

## Lista klikow

### Demo podstawowe

1. Start programu.
2. `Enter` na `Budzik`.
3. Wybierz `+++ Dodaj nowy budzik +++`.
4. Ustaw:
   - typ: `t`
   - czas: `10 s`
   - priorytet: `80`
5. Zapisz wpis.
6. `Esc` do menu.
7. `Enter` na `-- Stan systemu --`.
8. Pokaz rekord budzika.
9. `Esc` do menu.
10. `Enter` na `Budzik`.
11. Poczekaj na alarm.

### Demo rozszerzone

1. Dodaj pierwszy budzik:
   - czas: `10 s`
   - priorytet: `80`
2. Dodaj drugi budzik:
   - czas: `30 s`
   - priorytet: `60`
3. Wejdz w `-- Stan systemu --`.
4. Przelacz rekord na drugi wpis.
5. Pokaz roznice parametrow.
6. Wroc do `Budzik`.
7. Poczekaj na pierwszy alarm.
8. Pokaz, ze drugi wpis nadal czeka na wykonanie.

## Gotowe wartosci do wpisania

### Budzik A
- typ: `t`
- czas: `10 s`
- priorytet: `80`

### Budzik B
- typ: `t`
- czas: `30 s`
- priorytet: `60`

### Budzik C
- typ: `t`
- czas: `60 s`
- priorytet: `40`
