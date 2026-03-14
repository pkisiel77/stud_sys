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
