#include "../tne.h"

enum TNEErrors error = TNERR_NO;

enum TNEErrors tne_get_last_error() {
  return error;
}

void tne_set_last_error(enum TNEErrors code) {
  error = code;
}
