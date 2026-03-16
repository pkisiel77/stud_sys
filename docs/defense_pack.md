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
3. Dodaj budzik za `10 s`.
4. Wróć do listy i pokaż countdown.
5. Wejdź w `-- Stan systemu --`.
6. Pokaż rekord budzika.
7. Wróć do `Budzik`.
8. Poczekaj na alarm.

## Co powiedzieć

- System przyjmuje zadanie czasowe od użytkownika.
- Zadanie trafia do wewnętrznej agendy RT.
- Scheduler odlicza czas do uruchomienia.
- Raport systemowy pokazuje bieżący stan zadania.

## Gotowe ustawienie

- typ: `t`
- czas: `10 s`
- priorytet: `80`

## Najważniejszy przekaz

To nie jest tylko formularz terminalowy. To działający model prostego systemu czasu rzeczywistego z harmonogramem, zadaniami i raportem diagnostycznym.
