#include <stddef.h>

#ifndef TNE_H_
  #define TNE_H_

  typedef struct {
    char *hostname;
    char *path;
    char *protocol;
    unsigned short port;
  } tneurl_t;

  typedef struct {
    char *name;
    char *value;
  } tneheader_t;

  typedef struct {
    int code;
    char *message;
    unsigned int message_length;
  } tnestatus_t;

  struct TNEHeaders {
    tneheader_t *headers;
    size_t header_count;
  };

  typedef struct {
    char *method;
    tneurl_t url;
    struct TNEHeaders headers;
    char *data;
    unsigned long long data_size;
  } tnerequest_t;

  typedef struct {
    struct TNEHeaders headers;
    char *data;
    unsigned long long data_size;
    tnestatus_t status;
  } tneresponse_t;

  tneurl_t tne_parse_url(char *url);
  void tne_free_url(tneurl_t url);

  void tne_add_header(struct TNEHeaders *dest, char *name, char *value);
  void tne_remove_header(struct TNEHeaders *dest, char *name);
  tneheader_t *tne_get_header(struct TNEHeaders dest, char *name);
  void tne_free_headers(struct TNEHeaders dest);

  tneresponse_t *tne_request(tnerequest_t request);
  tnerequest_t tne_prepare_request(char *method, char *url);
  void tne_free_request(tnerequest_t request);
  void tne_free_response(tneresponse_t *response);

  void tne_strncpy(char *dest, char *src, size_t n);
#endif
