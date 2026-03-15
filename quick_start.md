# Quick Start

## Cel

Ten dokument sluzy do szybkiego uruchomienia `stud_sys` bez czytania pelnej dokumentacji.

## Wymagania

- `gcc`
- `ncurses`

Opcjonalnie:

- `paho-mqtt-c` przy budowaniu z `MQTT=1`

## Build

```bash
make
```

Lub z MQTT:

```bash
make MQTT=1
```

## Start

```bash
./stud_sys
```

## Co zobaczysz

W aktualnym trybie prezentacyjnym menu pokazuje:

- `Budzik`
- `-- Stan systemu --`
- `Zmiana trybu na GRAFICZNY/TEKSTOWY`

## Najszybsze demo

1. Uruchom `stud_sys`.
2. Wejdz w `Budzik`.
3. Dodaj nowy budzik za `10 s`.
4. Wroc do listy.
5. Obserwuj countdown.
6. Wejdz w `-- Stan systemu --`.
7. Pokaz rekord budzika.
8. Wroc do `Budzik`.
9. Poczekaj na alarm.

## Podstawowe klawisze

- `Strzalki gora/dol` - zmiana pozycji
- `Enter` - wejscie / akceptacja
- `Esc` - wyjscie

## Gdzie szukac dalej

- `user_guide.md` - instrukcja obslugi
- `presentation_budzik.md` - scenariusz prezentacji
- `architecture.md` - opis techniczny
