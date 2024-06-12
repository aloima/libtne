#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../tne.h"

// TODO: change res.path to /, when no path
tneurl_t tne_parse_url(char *url) {
  tneurl_t res = {0};

  unsigned int i = 0;
  unsigned int protocol_length = 0;
  int port_at = -1;
  const unsigned int length = strlen(url);

  while (i < length) {
    if (url[i] == ':') {
      if (res.protocol == NULL && strncmp(url + i, "://", 3) == 0) {
        protocol_length = i;
        res.protocol = malloc(i + 1);
        tne_strncpy(res.protocol, url, i);
        i += 3;
      } else if (port_at == -1) {
        port_at = i + 1;
        char sport[6];

        while (i < length) {
          if (url[i] == '/') {
            const unsigned int port_length = i - port_at;

            if (port_length > 5 || port_length < 1) {
              fputs("tne_parse_url: invalid url scheme, port length must be between 1 and 5\n", stderr);
              return res;
            }

            tne_strncpy(sport, url + port_at, port_length);
            res.port = atoi(sport);

            res.hostname = malloc(i - protocol_length - port_length - 3);
            tne_strncpy(res.hostname, url + protocol_length + 3, i - protocol_length - port_length - 4);

            res.path = malloc(length - i + 1);
            tne_strncpy(res.path, url + i, length - i);

            return res;
          }

          ++i;
        }
      } else {
        fputs("tne_parse_url: invalid url scheme\n", stderr);
        return res;
      }
    }

    if (url[i] == '/') {
      res.port = res.protocol[protocol_length - 1] == 's' ? 443 : 80;

      res.hostname = malloc(i - protocol_length - 2);
      tne_strncpy(res.hostname, url + protocol_length + 3, i - protocol_length - 3);

      res.path = malloc(length - i + 1);
      tne_strncpy(res.path, url + i, length - i);

      return res;
    }

    ++i;
  }

  return res;
}

void tne_free_url(tneurl_t url) {
  free(url.hostname);
  free(url.path);
  free(url.protocol);
}
