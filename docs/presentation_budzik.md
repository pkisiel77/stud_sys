# Prezentacja demo: Budzik

## Cel

Pokazać, że `stud_sys` wykonuje zadania czasowe w czasie rzeczywistym i pozwala nimi zarządzać z poziomu blankietu.

## Widok prezentacyjny

W menu głównym widoczne są:

- `Budzik`
- `Demo IoT: czujnik symulowany`
- `-- Stan systemu --`

Pozostałe usługi (np. `MQTT Publisher`) są ukryte, ale logika systemu pozostaje aktywna.

## Jak ustawić budzik na pokaz

Formularz budzika ma pola `Sprawa`, `Godzina` oraz `Co ile` / `wyprzedz`.
Kursor na polu, `Enter`, wpisz wartość na dole ekranu, `Enter`.

Monity zaczynają się `Wyprzedzenie` minut przed `Godziną` i powtarzają się co `Co ile` minut.
Czas ustawia się z dokładnością do minuty, a wyprzedzenie to co najmniej 1 minuta, dlatego:

- `Godzina` = teraz + 3 min, `Co ile` = 1, `Wyprzedzenie` = 1 → pierwszy monit po ok. 1-2 min, odliczanie widać na liście;
- `Godzina` = teraz + 1 min → alarm od razu po zapisie (okno wyprzedzenia już trwa).

## Scenariusz 3-5 minut

1. Wejdź w `Budzik`.
2. Pokaż listę budzików i dodaj budzik A (teraz + 3 min).
3. Dodaj budzik B (teraz + 6 min).
4. Pokaż, że wpis można otworzyć `Enter` i zmienić albo usunąć.
5. `Esc` do menu i wejdź w `-- Stan systemu --`.
6. Pokaż dla rekordu `Budzik`:
   - `STAN`
   - `NAST.` - sekundy do wywołania
   - `CYKL` - okres wywołań
   - `PRIOR`
7. Wróć do `Budzik` i poczekaj na okno alarmu.

## Co mówić na prezentacji

### Warstwa RT

- `Sprawdz.otoczenia` działa w tle jako zadanie nadzorcze (cykl 1 s, priorytet 200).
- `Budzik` jest zadaniem czasowym sterowanym przez harmonogram.
- `Stan systemu` pokazuje aktualny stan agendy, kolejki i opóźnień.

### Czas rzeczywisty

- Odliczanie na liście i kolumna `NAST.` maleją co sekundę - scheduler działa także wtedy, gdy program czeka na klawisz.
- Dwa budziki z różnymi godzinami pokazują, że system utrzymuje wiele zadań jednocześnie.
- `Demo IoT` dokłada trzecie zadanie (`Czujnik Sim`, cykl 1 s) z pomiarem w kolumnie `WARTOSC`.

## Proponowane ustawienia demo

### Budzik A

- `Sprawa`: `Demo A`
- `Godzina`: teraz + 3 min
- `Co ile`: `1`, `Wyprzedzenie`: `1`

### Budzik B

- `Sprawa`: `Demo B`
- `Godzina`: teraz + 6 min
- `Co ile`: `1`, `Wyprzedzenie`: `1`

## Najkrótsza wersja demo

1. `Budzik`
2. dodanie budzika A
3. `-- Stan systemu --`
4. powrót do `Budzik`
5. alarm

## Najważniejszy przekaz

System nie jest tylko listą formularzy. To prosty scheduler RT:

- przyjmuje zadania czasowe,
- utrzymuje je w agendzie,
- wykonuje według czasu i priorytetu,
- pozwala podejrzeć stan pracy w raporcie systemowym.

## Wersja 2 min

Najlepiej przygotować budzik A tuż przed pokazem (np. teraz + 3 min), żeby alarm zmieścił się w czasie.

1. Wejdź w `Budzik` i pokaż odliczanie budzika A.
2. `Esc`, wejdź w `-- Stan systemu --`.
3. Pokaż:
   - rekord `Budzik`,
   - `NAST.`,
   - `CYKL`.
4. Wróć do `Budzik` i poczekaj na alarm.

### Co powiedzieć

- System przyjmuje zadanie czasowe.
- Zadanie trafia do agendy i jest widoczne w raporcie systemowym.
- O zadanym czasie scheduler uruchamia budzik.

## Wersja 5 min

1. Wejdź w `Budzik`.
2. Pokaż istniejące wpisy lub pustą listę.
3. Dodaj budzik A (teraz + 3 min).
4. Dodaj budzik B (teraz + 6 min).
5. Pokaż, że wpisy można edytować albo usuwać.
6. `Esc`, wejdź w `Demo IoT: czujnik symulowany`.
7. Pokaż w `Stanie systemu` rekordy `Budzik` i `Czujnik Sim` i omów:
   - `NAST.` i `CYKL`,
   - priorytet,
   - pomiar w `WARTOSC`.
8. Wróć do `Budzik`.
9. Poczekaj na alarm budzika A.
10. Pokaż, że budzik B nadal czeka.

### Co powiedzieć

- System obsługuje wiele zadań czasowych jednocześnie.
- Zadania mają różne czasy uruchomienia i okresy.
- `Stan systemu` pokazuje wewnętrzny obraz pracy harmonogramu RT.

## Lista klików

### Demo podstawowe

1. Start programu.
2. `Enter` na `Budzik`.
3. `Enter` na `+++ Dodaj nowy budzik +++`.
4. Ustaw:
   - `Sprawa`: `Demo A`
   - `Godzina`: teraz + 3 min
   - `Co ile`: `1`, `Wyprzedzenie`: `1`
5. `Enter` na `>>> Wpisz do agendy <<<`.
6. `Esc` do menu.
7. `Enter` na `-- Stan systemu --`.
8. Pokaż rekord `Budzik`.
9. `Esc` do menu.
10. `Enter` na `Budzik`.
11. Poczekaj na okno alarmu (`Enter` wyłącza sygnał, `Esc` zamyka monit).

### Demo rozszerzone

1. Dodaj budzik A (teraz + 3 min).
2. Dodaj budzik B (teraz + 6 min).
3. `Esc`, `Enter` na `Demo IoT: czujnik symulowany`.
4. `Góra/Dół` - przełączaj zaznaczony rekord i pokaż szczegóły pod tabelą.
5. `Esc`, wróć do `Budzik`.
6. Poczekaj na alarm budzika A.
7. Pokaż, że budzik B nadal czeka na wykonanie.

## Gotowe wartości do wpisania

`T` = bieżąca godzina i minuta.

| Budzik | `Sprawa` | `Godzina` | `Co ile` | `Wyprzedzenie` | Pierwszy monit |
|---|---|---|---|---|---|
| A | `Demo A` | `T + 3 min` | `1` | `1` | po ok. 1-2 min |
| B | `Demo B` | `T + 6 min` | `1` | `1` | po ok. 4-5 min |
| C | `Demo C` | `T + 10 min` | `2` | `4` | po ok. 5-6 min, potem co 2 min |
