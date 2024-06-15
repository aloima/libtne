#include "../tne.h"

enum TNEErrors err = TNERR_NO;

enum TNEErrors tne_get_last_err() {
  return err;
}

void tne_set_last_err(enum TNEErrors code) {
  err = code;
}

char *tne_get_err_msg(enum TNEErrors code) {
  switch (code) {
    case TNERR_NO:
      return "There is no error.";

    case TNERR_CMIS:
      return "Content-Length header and received response data length is mismatch.";

    case TNERR_IV:
      return "Specified an invalid URL protocol, this protocol cannot be used for this function.";

    case TNERR_SSL:
      return "An OpenSSL error occured, should be checked using OpenSSL error methods.";

    case TNERR_SOCK:
      return "A socket error occured, should be checked with errno in libc.";

    case TNERR_SOCKSSL:
      return "A socket or an OpenSSL error occured, should be checked with errno in libc or using OpenSSL error methods.";

    case TNERR_URL:
      return "An error occured when parsing URL. Specified URL does not match URL scheme.";

    case TNERR_HMETH:
      return "Specified an invalid HTTP method, it should not include an white-space character.";
  }
}
