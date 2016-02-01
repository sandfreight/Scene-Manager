// Miscellaneous utilities

#ifndef MISC_UTIL_H
#define MISC_UTIL_H

#include <stdbool.h>
#include <stddef.h> // to get size_t

// Function used to allocate memory. Returns true on success, false on failure
bool safer_realloc(void **p, size_t increased_size);

// Function used to deallocate memory. Always succeeds!
void safe_dealloc(void **p, size_t decreased_size);

#endif // MISC_UTIL_H
