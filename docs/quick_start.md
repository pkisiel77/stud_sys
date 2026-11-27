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

Język interfejsu (domyślnie `PL`):

```bash
make TUI_LANG=EN
```

Zmiana `TUI_LANG` lub `MQTT` sama przebudowuje cały program - nie trzeba robić `make clean`.

Testy:

```bash
make test
```

## Start

```bash
./stud_sys
```

## Co zobaczysz

W aktualnym trybie prezentacyjnym menu pokazuje:

- `Budzik`
- `Demo IoT: czujnik symulowany`
- `-- Stan systemu --`

## Najszybsze demo (ok. 3 min)

1. Uruchom `stud_sys`.
2. `Enter` na `Budzik`, potem `+++ Dodaj nowy budzik +++`.
3. Ustaw pola (kursor na polu, `Enter`, wpisz wartość, `Enter`):
   - `Sprawa`: `Demo`
   - `Godzina`: bieżąca godzina i minuta **+3 min**
   - `Co ile`: `1`, `Wyprzedzenie`: `1`
4. Wybierz `>>> Wpisz do agendy <<<` - na liście widać odliczanie do pierwszego monitu.
5. `Esc` do menu, wejdź w `-- Stan systemu --` i pokaż rekord `Budzik`.
6. `Esc`, wróć do `Budzik` i poczekaj na okno alarmu (ok. 1-2 min od zapisu).

## Podstawowe klawisze

- `Strzałki góra/dół` - zmiana pozycji
- `Enter` - wejście / akceptacja
- `Esc` - wyjście (w polu wpisu: anulowanie)

## Gdzie szukać dalej

- `docs/user_guide.md` - instrukcja obsługi
- `docs/presentation_budzik.md` - scenariusz prezentacji
- `docs/architecture.md` - opis techniczny
