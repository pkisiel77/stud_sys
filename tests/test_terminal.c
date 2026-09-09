#include "blank/moje.h"

#include <stdio.h>

static int failures = 0;

#define CHECK_EQ(actual, expected)                                              \
    do {                                                                        \
        int actual_value = (int)(actual);                                       \
        int expected_value = (int)(expected);                                   \
        if (actual_value != expected_value) {                                   \
            fprintf(stderr, "FAIL %s:%d: %s=%d, expected %d\n",               \
                    __FILE__, __LINE__, #actual, actual_value, expected_value);  \
            failures++;                                                        \
        }                                                                       \
    } while (0)

int main(void)
{
    CHECK_EQ(ncurses_color_from_dos(0), COLOR_BLACK);
    CHECK_EQ(ncurses_color_from_dos(1), COLOR_BLUE);
    CHECK_EQ(ncurses_color_from_dos(2), COLOR_GREEN);
    CHECK_EQ(ncurses_color_from_dos(3), COLOR_CYAN);
    CHECK_EQ(ncurses_color_from_dos(4), COLOR_RED);
    CHECK_EQ(ncurses_color_from_dos(5), COLOR_MAGENTA);
    CHECK_EQ(ncurses_color_from_dos(6), COLOR_YELLOW);
    CHECK_EQ(ncurses_color_from_dos(7), COLOR_WHITE);
    CHECK_EQ(ncurses_color_from_dos(-1), COLOR_BLACK);
    CHECK_EQ(ncurses_color_from_dos(8), COLOR_BLACK);

    if (failures != 0) {
        fprintf(stderr, "%d terminal test(s) failed\n", failures);
        return 1;
    }
    puts("terminal tests: OK");
    return 0;
}
