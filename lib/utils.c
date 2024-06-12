#include <string.h>

#include "../tne.h"

// Copies src to dest with exact length and append NULL character.
void tne_strncpy(char *dest, char *src, size_t n) {
  strncpy(dest, src, n);
  dest[n] = 0;
}
