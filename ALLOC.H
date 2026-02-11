#ifndef ALLOC_H
#define ALLOC_H

// ... existing code ...
#include <stdlib.h>   /* malloc, free, realloc, calloc */

/*
 * malloc.h nie jest przenośny (np. macOS/clang go nie ma).
 * Jeśli bardzo chcesz, możesz go dołączać warunkowo, ale zwykle nie jest potrzebny.
 */
// ... existing code ...
#if defined(__GLIBC__)
#include <malloc.h>
#endif
// ... existing code ...

#endif /* ALLOC_H */