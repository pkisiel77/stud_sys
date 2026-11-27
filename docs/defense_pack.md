# Pakiet obronowy

## Cel

Ten materiał służy do bardzo krótkiego pokazu działania systemu podczas obrony lub szybkiej prezentacji.

Zakładany czas:

- od `2` do `4` minut.

## Co pokazać

1. System działa jako prosty scheduler RT.
2. `Budzik` jest przykładem zadania czasowego.
3. `Stan systemu` pokazuje, że zadanie trafia do agendy i jest nadzorowane przez scheduler.

## Minimalny scenariusz

1. Uruchom `stud_sys`.
2. Wejdź w `Budzik`.
3. Dodaj budzik (wartości niżej) i wybierz `>>> Wpisz do agendy <<<`.
4. Pokaż countdown na liście.
5. Wejdź w `-- Stan systemu --`.
6. Pokaż rekord `Budzik` (`NAST.` - sekundy do wywołania, `CYKL`).
7. Wróć do `Budzik`.
8. Poczekaj na okno alarmu (ok. 1-2 min od zapisu).

## Co powiedzieć

- System przyjmuje zadanie czasowe od użytkownika.
- Zadanie trafia do wewnętrznej agendy RT.
- Scheduler odlicza czas do uruchomienia.
- Raport systemowy pokazuje bieżący stan zadania.

## Gotowe ustawienie

- `Sprawa`: `Demo`
- `Godzina`: bieżąca godzina i minuta + 3 min
- `Co ile`: `1`, `Wyprzedzenie`: `1`

## Najważniejszy przekaz

To nie jest tylko formularz terminalowy. To działający model prostego systemu czasu rzeczywistego z harmonogramem, zadaniami i raportem diagnostycznym.
