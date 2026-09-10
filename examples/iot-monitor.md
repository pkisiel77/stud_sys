# Przykład: terminalowy monitor temperatury

Ten scenariusz wykorzystuje wbudowany symulator czujnika jako źródło danych.
Nie wymaga fizycznego urządzenia ani brokera MQTT.

## Uruchomienie

1. Zbuduj i uruchom aplikację:

   ```bash
   cmake -S . -B build -DSTUDSYS_LANG=PL
   cmake --build build
   ./build/stud_sys
   ```

2. W menu głównym wybierz `Demo IoT: czujnik symulowany`.
3. Aplikacja automatycznie utworzy czujnik i otworzy `Stan systemu`.

Demo używa przebiegu sinusoidalnego, wartości bazowej `20`, amplitudy `10`,
cyklu jednej sekundy i zakresu alarmowego `12..28`. Ponowne wybranie pozycji
menu otwiera istniejący czujnik — nie tworzy jego duplikatu.

Dashboard odświeża się automatycznie. Wiersz czujnika pokazuje bieżącą wartość,
czas do kolejnego wywołania i alarm. Przekroczenie zakresu wyróżnia cały wiersz
na czerwono.

## Nawigacja dashboardu

- `Góra` / `Dół` — wybór zadania,
- `Page Up` / `Page Down` — zmiana strony,
- `Home` / `End` — pierwszy lub ostatni wpis,
- `R` — ręczne odświeżenie,
- `Esc` — powrót bez dodatkowych pytań do menu głównego.

## Rozszerzenie o MQTT

Po zbudowaniu z `-DSTUDSYS_MQTT=ON` można dodać usługę MQTT Publisher.
Publikuje ona wspólne pola `rt_data` czujnika, dzięki czemu ten sam scenariusz
może zasilać Home Assistant, Node-RED lub własny panel bez zmiany symulatora.
