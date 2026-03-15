# Prezentacja demo: Budzik

## Cel

Pokazać, że `stud_sys` wykonuje zadania czasowe w czasie rzeczywistym i pozwala nimi zarządzać z poziomu blankietu.

## Widok prezentacyjny

W menu głównym widoczne są tylko:

- `Budzik`
- `-- Stan systemu --`
- `Zmiana trybu na GRAFICZNY/TEKSTOWY`

Pozostałe usługi są ukryte, ale logika systemu pozostaje aktywna.

## Scenariusz 3-5 minut

1. Wejdź w `Budzik`.
2. Pokaż listę budzików i dodaj nowy wpis.
3. Ustaw budzik jednorazowy za `10 s`.
4. Ustaw drugi budzik za `30 s`.
5. Pokaż, że można wrócić do listy i edytować/usunąć wpisy.
6. Wejdź w `-- Stan systemu --`.
7. Pokaż dla rekordów:
   - `Cykl`
   - `Priorytet`
   - `Opóźn`
   - `Stan realizacji`
8. Wróć do `Budzik` i poczekaj na wyzwolenie alarmu.

## Co mówić na prezentacji

### Warstwa RT

- `Sprawdz.otoczenia` działa w tle jako zadanie nadzorcze.
- `Budzik` jest zadaniem czasowym sterowanym przez harmonogram.
- `Stan systemu` pokazuje aktualny stan kolejkowania i opóźnień.

### Czas rzeczywisty

- Budzik za `10 s` pokazuje reakcję w krótkim horyzoncie czasowym.
- Budzik za `30 s` pokazuje, że system utrzymuje wiele zadań jednocześnie.
- W `Stan systemu` można pokazać, że zadanie ma okres i priorytet.

## Proponowane ustawienia demo

### Budzik 1

- typ: `t` jednorazowy
- czas do uruchomienia: `10 s`
- priorytet: `80`

### Budzik 2

- typ: `t` jednorazowy
- czas do uruchomienia: `30 s`
- priorytet: `60`

## Najkrótsza wersja demo

1. `Budzik`
2. dodanie wpisu za `10 s`
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

1. Wejdź w `Budzik`.
2. Dodaj budzik jednorazowy za `10 s`.
3. Wróć do listy budzików.
4. Wejdź w `-- Stan systemu --`.
5. Pokaż:
   - rekord budzika,
   - `Cykl`,
   - `Priorytet`,
   - `Opóźn`.
6. Wróć do `Budzik` i poczekaj na alarm.

### Co powiedzieć

- System przyjmuje zadanie czasowe.
- Zadanie trafia do agendy i jest widoczne w raporcie systemowym.
- Po zadanym czasie scheduler uruchamia budzik.

## Wersja 5 min

1. Wejdź w `Budzik`.
2. Pokaż istniejące wpisy lub pustą listę.
3. Dodaj budzik `A` za `10 s`, priorytet `80`.
4. Dodaj budzik `B` za `30 s`, priorytet `60`.
5. Pokaż, że wpisy można edytować albo usuwać.
6. Wejdź w `-- Stan systemu --`.
7. Pokaż dwa rekordy budzików i omów:
   - typ zadania,
   - czas/cykl,
   - priorytet,
   - opóźnienie.
8. Wróć do `Budzik`.
9. Poczekaj na pierwszy alarm.
10. Pokaż, że drugi budzik nadal pozostaje aktywny.

### Co powiedzieć

- System obsługuje wiele zadań czasowych jednocześnie.
- Zadania mają różne czasy uruchomienia i priorytety.
- `Stan systemu` pokazuje wewnętrzny obraz pracy harmonogramu RT.

## Lista klików

### Demo podstawowe

1. Start programu.
2. `Enter` na `Budzik`.
3. Wybierz `+++ Dodaj nowy budzik +++`.
4. Ustaw:
   - typ: `t`
   - czas: `10 s`
   - priorytet: `80`
5. Zapisz wpis.
6. `Esc` do menu.
7. `Enter` na `-- Stan systemu --`.
8. Pokaż rekord budzika.
9. `Esc` do menu.
10. `Enter` na `Budzik`.
11. Poczekaj na alarm.

### Demo rozszerzone

1. Dodaj pierwszy budzik:
   - czas: `10 s`
   - priorytet: `80`
2. Dodaj drugi budzik:
   - czas: `30 s`
   - priorytet: `60`
3. Wejdź w `-- Stan systemu --`.
4. Przełącz rekord na drugi wpis.
5. Pokaż różnice parametrów.
6. Wróć do `Budzik`.
7. Poczekaj na pierwszy alarm.
8. Pokaż, że drugi wpis nadal czeka na wykonanie.

## Gotowe wartości do wpisania

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
