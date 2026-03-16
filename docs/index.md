# Dokumentacja `stud_sys`

**Wersja dokumentacji:** `0384673`

To jest strona startowa dokumentacji projektu.

## Dla kogo są te materiały

- dla użytkownika końcowego, który chce uruchomić system i dodać budzik,
- dla operatora, który prowadzi pokaz lub prezentację,
- dla developera, który chce zrozumieć architekturę i przepływ RT.

## Dokumenty

### Start i obsługa

- `docs/quick_start.md` - najszybsza ścieżka uruchomienia i demo
- `docs/user_guide.md` - pełna instrukcja użytkownika

### Prezentacja

- `docs/operator_cheatsheet.md` - skrócona ściąga operatora
- `docs/presentation_budzik.md` - scenariusz prezentacji `Budzik`
- `docs/pdf/stud_sys_presentation_pack.pdf` - krótki pakiet PDF do samej prezentacji
- `docs/pdf/stud_sys_defense_pack.pdf` - bardzo krótki pakiet pod obronę / pokaz 2-4 min

### Techniczne

- `docs/architecture.md` - architektura systemu i przepływ `agenda -> queue -> main_modul`

## Eksport do PDF

Gotowy eksport można wykonać poleceniem:

```bash
make docs-pdf
```

Po wykonaniu pliki pojawią się w katalogu:

```bash
docs/pdf/
```

Domyślnie generowane są:

- `docs/pdf/operator_cheatsheet.pdf`
- `docs/pdf/user_guide.pdf`
- `docs/pdf/presentation_budzik.pdf`
- `docs/pdf/stud_sys_docs_bundle.pdf`
- `docs/pdf/stud_sys_presentation_pack.pdf`

Każdy PDF ma stopkę z:

- datą eksportu,
- wersją dokumentacji opartą o aktualny commit,
- numerem strony.

Scalony PDF `docs/pdf/stud_sys_docs_bundle.pdf` zawiera dodatkowo:

- stronę tytułową,
- automatyczny spis treści.

W katalogu `docs/assets/` znajduje się także logo dokumentacji:

- `docs/assets/stud_sys_logo.tex` - źródło logo
- `docs/assets/stud_sys_logo.pdf` - wygenerowany asset do PDF
