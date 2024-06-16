#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../tne.h"

tneurl_t tne_parse_url(char *url) {
  tneurl_t res = {0};

  const uint32_t len = strlen(url);
  uint32_t i = 0;
  uint32_t protocol_len = 0;
  uint32_t port_at = 0;

  while (i < len) {
    if (url[i] == ':') {
      if (res.protocol == NULL && strncmp(url + i, "://", 3) == 0) {
        protocol_len = i;
        res.protocol = malloc(i + 1);
        tne_strncpy(res.protocol, url, i);
        i += 3;
      } else if (port_at == 0) {
        port_at = i + 1;
        char sport[6];

        while (i < len) {
          if (url[i] == '/') {
            const uint32_t port_len = i - port_at;

            if (port_len > 5 || port_len < 1) {
              tne_set_last_err(TNERR_URL);
              free(res.protocol);
              return res;
            }

            tne_strncpy(sport, url + port_at, port_len);
            res.port = atoi(sport);

            res.hostname = malloc(i - protocol_len - port_len - 3);
            tne_strncpy(res.hostname, url + protocol_len + 3, i - protocol_len - port_len - 4);

            res.path = malloc(len - i + 1);
            tne_strncpy(res.path, url + i, len - i);

            return res;
          }

          ++i;
        }
      } else {
        tne_set_last_err(TNERR_URL);
        free(res.protocol);
        return res;
      }
    }

    if (url[i] == '/') {
      res.port = res.protocol[protocol_len - 1] == 's' ? 443 : 80;

      res.hostname = malloc(i - protocol_len - 2);
      tne_strncpy(res.hostname, url + protocol_len + 3, i - protocol_len - 3);

      res.path = malloc(len - i + 1);
      tne_strncpy(res.path, url + i, len - i);

      return res;
    }

    ++i;
  }

  if (res.path == NULL) {
    res.port = res.protocol[protocol_len - 1] == 's' ? 443 : 80;

    res.hostname = malloc(len - protocol_len - 2);
    tne_strncpy(res.hostname, url + protocol_len + 3, len - protocol_len - 3);

    res.path = malloc(2);
    tne_strncpy(res.path, "/", 1);
  }

  return res;
}

void tne_free_url(tneurl_t url) {
  free(url.hostname);
  free(url.path);
  free(url.protocol);
}
