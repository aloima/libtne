#include <stddef.h>

#ifndef TNE_H_
  #define TNE_H_

  typedef struct {
    char *hostname;
    char *path;
    char *protocol;
    unsigned int port;
  } tneurl_t;

  tneurl_t tne_parse_url(char *url);
  void tne_free_url(tneurl_t url);

  void tne_strncpy(char *dest, char *src, size_t n);
#endif
