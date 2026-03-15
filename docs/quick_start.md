# Quick Start

## Cel

Ten dokument służy do szybkiego uruchomienia `stud_sys` bez czytania pełnej dokumentacji.

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
2. Wejdź w `Budzik`.
3. Dodaj nowy budzik za `10 s`.
4. Wróć do listy.
5. Obserwuj countdown.
6. Wejdź w `-- Stan systemu --`.
7. Pokaż rekord budzika.
8. Wróć do `Budzik`.
9. Poczekaj na alarm.

## Podstawowe klawisze

- `Strzałki góra/dół` - zmiana pozycji
- `Enter` - wejście / akceptacja
- `Esc` - wyjście

## Gdzie szukać dalej

- `docs/user_guide.md` - instrukcja obsługi
- `docs/presentation_budzik.md` - scenariusz prezentacji
- `docs/architecture.md` - opis techniczny
