/* Wspolne makra testow jednostkowych (ctest). */
#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>   /* mkdtemp (macOS), rmdir */

static int failures = 0;

#define CHECK(condition)                                                        \
    do {                                                                        \
        if (!(condition)) {                                                     \
            fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__,           \
                    #condition);                                                \
            failures++;                                                        \
        }                                                                       \
    } while (0)

#define CHECK_STR(actual, expected)                                             \
    do {                                                                        \
        const char *a_ = (actual), *e_ = (expected);                            \
        if (strcmp(a_, e_) != 0) {                                              \
            fprintf(stderr, "FAIL %s:%d: \"%s\" != \"%s\"\n", __FILE__,        \
                    __LINE__, a_, e_);                                          \
            failures++;                                                        \
        }                                                                       \
    } while (0)

/* Tymczasowy katalog jako HOME: pliki ~/.stud_sys_* testow nie dotykaja
   prawdziwego katalogu domowego. */
static char test_home[256];

static inline void test_ustaw_home(void)
{
    const char *tmp = getenv("TMPDIR");
    snprintf(test_home, sizeof(test_home), "%s/stud_sys_test_XXXXXX",
             (tmp && *tmp) ? tmp : "/tmp");
    if (mkdtemp(test_home) == NULL) {
        perror("mkdtemp");
        exit(2);
    }
    setenv("HOME", test_home, 1);
}

static inline int test_wynik(const char *nazwa)
{
    if (failures != 0) {
        fprintf(stderr, "%d %s test(s) failed\n", failures, nazwa);
        return 1;
    }
    printf("%s tests: OK\n", nazwa);
    return 0;
}

#endif /* TEST_UTIL_H */
