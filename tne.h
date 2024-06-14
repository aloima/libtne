#include <stddef.h>

#include <openssl/crypto.h>

#ifndef TNE_H_
  #define TNE_H_

  #define TNE_VERSION "1.1"

  enum TNEErrors {
    TNERR_NO,
    TNERR_CMIS,
    TNERR_IV,
    TNERR_SSL,
    TNERR_SOCK,
    TNERR_SOCKSSL,
    TNERR_URL
  };

  typedef struct {
    char *hostname;
    char *path;
    char *protocol;
    unsigned short port;
  } tneurl_t;

  typedef struct {
    char *name;
    unsigned int name_len;

    char *value;
    unsigned int value_len;
  } tneheader_t;

  typedef struct {
    int code;
    char *message;
    unsigned int message_len;
  } tnestatus_t;

  struct TNEHeaders {
    tneheader_t *data;
    unsigned int count;
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

  void tne_add_header(struct TNEHeaders *headers, char *name, char *value, unsigned int name_len, unsigned int value_len);
  void tne_remove_header(struct TNEHeaders *headers, char *name);
  tneheader_t *tne_get_header(struct TNEHeaders headers, char *name);
  void tne_free_headers(struct TNEHeaders headers);

  tneresponse_t *tne_request(tnerequest_t request);
  tnerequest_t tne_prepare_request(char *method, char *url);
  void tne_free_request(tnerequest_t request);
  void tne_free_response(tneresponse_t *response);

  enum TNEErrors tne_get_last_err();
  void tne_set_last_err(enum TNEErrors code);

  void tne_strncpy(char *dest, char *src, size_t n);
  void tne_cleanup_openssl(SSL *ssl, SSL_CTX *ctx);
  int tne_write(SSL *ssl, int fd, char *buf, unsigned long long size);
  int tne_read(SSL *ssl, int fd, char *buf, unsigned int size);
#endif
