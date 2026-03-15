# Operator Cheatsheet

## Cel

To jest skrocona sciaga do obslugi systemu podczas pokazu lub testu.

## Menu glowne

Widoczne pozycje:

- `Budzik`
- `-- Stan systemu --`
- `Zmiana trybu na GRAFICZNY/TEKSTOWY`

## Klawisze

- `Strzalka gora/dol` - przesuniecie zaznaczenia
- `Enter` - otwarcie pozycji / zatwierdzenie pola
- `Esc` - wyjscie z okna
- `t`, `n`, `m` - wybory w polach decyzyjnych

## Demo 2 min

1. `Enter` na `Budzik`
2. wybierz `+++ Dodaj nowy budzik +++`
3. ustaw budzik za `10 s`
4. zapisz
5. pokaz odliczanie na liscie
6. `Esc`
7. `Enter` na `-- Stan systemu --`
8. pokaz rekord budzika
9. `Esc`
10. wroc do `Budzik`
11. poczekaj na alarm

## Demo 5 min

1. dodaj budzik A za `10 s`
2. dodaj budzik B za `30 s`
3. pokaz dwa wpisy na liscie
4. wejdz w `-- Stan systemu --`
5. pokaz okres, priorytet i opoznienie
6. wroc do `Budzik`
7. pokaz alarm pierwszego wpisu
8. pokaz, ze drugi wpis nadal czeka

## Gotowe wartosci

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

## Co powiedziec

- `Budzik` jest zadaniem czasowym.
- System przechowuje je w agendzie RT.
- `Stan systemu` pokazuje wewnetrzny stan schedulera.
- Countdown na liscie pokazuje czas do najblizszego uruchomienia.

## Gdy cos pojdzie nie tak

- brak wyjscia z okna: `Esc`
- powrot do menu: `Esc` az do menu glownego
- chcesz tylko pokazac scheduler: wejdz w `-- Stan systemu --`
