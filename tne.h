#include <stddef.h>

#ifndef TNE_H_
  #define TNE_H_

  typedef struct {
    char *hostname;
    char *path;
    char *protocol;
    unsigned int port;
  } tneurl_t;

  typedef struct {
    char *name;
    char *value;
  } tneheader_t;

  struct TNEHeaders {
    tneheader_t *headers;
    size_t header_count;
  };

  tneurl_t tne_parse_url(char *url);
  void tne_free_url(tneurl_t url);

  void tne_add_header(struct TNEHeaders *dest, char *name, char *value);
  void tne_remove_header(struct TNEHeaders *dest, char *name);
  tneheader_t *tne_get_header(struct TNEHeaders dest, char *name);
  void tne_free_headers(struct TNEHeaders dest);

  void tne_strncpy(char *dest, char *src, size_t n);
#endif
