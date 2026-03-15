# Operator Cheatsheet

## Cel

To jest skrócona ściąga do obsługi systemu podczas pokazu lub testu.

## Menu główne

Widoczne pozycje:

- `Budzik`
- `-- Stan systemu --`
- `Zmiana trybu na GRAFICZNY/TEKSTOWY`

## Klawisze

- `Strzałka góra/dół` - przesunięcie zaznaczenia
- `Enter` - otwarcie pozycji / zatwierdzenie pola
- `Esc` - wyjście z okna
- `t`, `n`, `m` - wybory w polach decyzyjnych

## Demo 2 min

1. `Enter` na `Budzik`
2. wybierz `+++ Dodaj nowy budzik +++`
3. ustaw budzik za `10 s`
4. zapisz
5. pokaż odliczanie na liście
6. `Esc`
7. `Enter` na `-- Stan systemu --`
8. pokaż rekord budzika
9. `Esc`
10. wróć do `Budzik`
11. poczekaj na alarm

## Demo 5 min

1. dodaj budzik A za `10 s`
2. dodaj budzik B za `30 s`
3. pokaż dwa wpisy na liście
4. wejdź w `-- Stan systemu --`
5. pokaż okres, priorytet i opóźnienie
6. wróć do `Budzik`
7. pokaż alarm pierwszego wpisu
8. pokaż, że drugi wpis nadal czeka

## Gotowe wartości

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

## Co powiedzieć

- `Budzik` jest zadaniem czasowym.
- System przechowuje je w agendzie RT.
- `Stan systemu` pokazuje wewnętrzny stan schedulera.
- Countdown na liście pokazuje czas do najbliższego uruchomienia.

## Gdy coś pójdzie nie tak

- brak wyjścia z okna: `Esc`
- powrót do menu: `Esc` aż do menu głównego
- chcesz tylko pokazać scheduler: wejdź w `-- Stan systemu --`
