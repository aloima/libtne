#include <stddef.h>
#include <stdint.h>

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
    TNERR_URL,
    TNERR_HMETH,
    TNERR_HN,
    TNERR_RDM
  };

  typedef struct {
    char *hostname;
    char *path;
    char *protocol;
    uint16_t port;
  } tneurl_t;

  typedef struct {
    char *name;
    uint32_t name_len;

    char *value;
    uint32_t value_len;
  } tneheader_t;

  typedef struct {
    int code;
    char *message;
    uint32_t message_len;
  } tnestatus_t;

  struct TNEHeaders {
    tneheader_t *data;
    uint32_t count;
  };

  typedef struct {
    char *method;
    uint8_t method_len;

    tneurl_t url;
    struct TNEHeaders headers;

    char *data;
    uint64_t data_size;
  } tnerequest_t;

  typedef struct {
    struct TNEHeaders headers;
    char *data;
    uint64_t data_size;
    tnestatus_t status;
  } tneresponse_t;

  tneurl_t tne_parse_url(char *url);
  void tne_free_url(tneurl_t url);

  tneheader_t *tne_add_header(struct TNEHeaders *headers, char *name, char *value, uint32_t name_len, uint32_t value_len);
  void tne_remove_header(struct TNEHeaders *headers, char *name);
  tneheader_t *tne_get_header(struct TNEHeaders headers, char *name);
  void tne_free_headers(struct TNEHeaders headers);

  tneresponse_t *tne_request(tnerequest_t *request);
  tnerequest_t *tne_prepare_request(char *method, char *url);
  void tne_add_request_data(tnerequest_t *request, char *data, uint64_t data_size, char *type);
  void tne_free_request(tnerequest_t *request);
  void tne_free_response(tneresponse_t *response);

  enum TNEErrors tne_get_last_err();
  void tne_set_last_err(enum TNEErrors code);
  char *tne_get_err_msg(enum TNEErrors code);

  void tne_strncpy(char *dest, char *src, size_t n);
  void tne_cleanup_openssl(SSL *ssl, SSL_CTX *ctx);
  int tne_write(SSL *ssl, int fd, char *buf, uint64_t size);
  int tne_read(SSL *ssl, int fd, char *buf, uint32_t size);
#endif
