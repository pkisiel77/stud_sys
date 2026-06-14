/* loc.h — compile-time language selection for stud_sys TUI.
 *
 * Default language is Polish. Build with `make LANG=EN` to switch to English.
 * Add a new language by defining LANG_<XX> in the makefile and a matching
 * block below. Keep all user-facing literals here; sources should reference
 * only the L_* macros.
 */
#ifndef STUD_SYS_LOC_H
#define STUD_SYS_LOC_H

#if defined(LANG_EN)

/* ===== Budzik / Alarm ===== */
#define L_BUDZ_MAIN_TITLE    " ALARM: <Ent> - silence signal; <Esc> - dismiss "
#define L_BUDZ_REPORTED      "     Set for %02d:%02d"
#define L_BUDZ_NEXT_PING     "     Next ping in %dmin.%02dsec."
#define L_BUDZ_PROMPT_INT    " %s [%d-%d]: %-8s  (Enter=confirm  Esc=cancel)"
#define L_BUDZ_PROMPT_TXT    " %s: %-44s  (Enter=confirm  Esc=cancel)"
#define L_BUDZ_FIELD_NAME    "  Subject : %.52s"
#define L_BUDZ_FIELD_TIME    "  Time    : %02d:%02d"
#define L_BUDZ_FIELD_EVERY   "  Every   : %d min  (lead: %d min)"
#define L_BUDZ_ASK_NAME      "Alarm name"
#define L_BUDZ_ASK_HOUR      "Hour"
#define L_BUDZ_ASK_MIN       "Minute"
#define L_BUDZ_ASK_EVERY     "Every N minutes"
#define L_BUDZ_ASK_LEAD      "Lead time [min]"
#define L_BUDZ_LIST_TITLE    " ALARM LIST "
#define L_BUDZ_NEW_TITLE     " NEW ALARM "
#define L_BUDZ_EDIT_TITLE    " ALARM "
#define L_BUDZ_MENU_SAVE_NEW "  >>> Save to agenda <<<"
#define L_BUDZ_MENU_CANCEL   "  Cancel"
#define L_BUDZ_MENU_APPLY    "  >>> Apply changes <<<"
#define L_BUDZ_MENU_DELETE   "  >>> Delete alarm <<<"
#define L_BUDZ_MENU_EXIT_NC  "  Exit (no changes)"
#define L_BUDZ_MENU_ADD      "  +++ Add new alarm +++"
#define L_BUDZ_MENU_EXIT     "  Exit"
#define L_BUDZ_AGENDA_TITLE  " Alarm list "

#else /* LANG_PL — default */

/* ===== Budzik ===== */
#define L_BUDZ_MAIN_TITLE    " BUDZIK: <Ent> - wylaczenie sygnalu; <Esc> - usuniecie monitu "
#define L_BUDZ_REPORTED      "     Zgloszono na godz.%02d.%02d"
#define L_BUDZ_NEXT_PING     "     Najblizszy monit za %dmin.%02dsek."
#define L_BUDZ_PROMPT_INT    " %s [%d-%d]: %-8s  (Enter=zatwierdz  Esc=anuluj)"
#define L_BUDZ_PROMPT_TXT    " %s: %-44s  (Enter=zatwierdz  Esc=anuluj)"
#define L_BUDZ_FIELD_NAME    "  Sprawa  : %.52s"
#define L_BUDZ_FIELD_TIME    "  Godzina : %02d:%02d"
#define L_BUDZ_FIELD_EVERY   "  Co ile  : %d min  (wyprzedz: %d min)"
#define L_BUDZ_ASK_NAME      "Nazwa budzika"
#define L_BUDZ_ASK_HOUR      "Godzina"
#define L_BUDZ_ASK_MIN       "Minuta"
#define L_BUDZ_ASK_EVERY     "Co ile minut"
#define L_BUDZ_ASK_LEAD      "Wyprzedzenie [min]"
#define L_BUDZ_LIST_TITLE    " LISTA BUDZIKOW "
#define L_BUDZ_NEW_TITLE     " NOWY BUDZIK "
#define L_BUDZ_EDIT_TITLE    " BUDZIK "
#define L_BUDZ_MENU_SAVE_NEW "  >>> Wpisz do agendy <<<"
#define L_BUDZ_MENU_CANCEL   "  Anuluj"
#define L_BUDZ_MENU_APPLY    "  >>> Zatwierdz zmiany <<<"
#define L_BUDZ_MENU_DELETE   "  >>> Usun budzik <<<"
#define L_BUDZ_MENU_EXIT_NC  "  Wyjdz (bez zmian)"
#define L_BUDZ_MENU_ADD      "  +++ Dodaj nowy budzik +++"
#define L_BUDZ_MENU_EXIT     "  Wyjdz"
#define L_BUDZ_AGENDA_TITLE  " Lista budzikow "

#endif

#endif /* STUD_SYS_LOC_H */
