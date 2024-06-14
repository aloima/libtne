#include "../tne.h"

enum TNEErrors err = TNERR_NO;

enum TNEErrors tne_get_last_err() {
  return err;
}

void tne_set_last_err(enum TNEErrors code) {
  err = code;
}
