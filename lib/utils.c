#include <string.h>
#include <stdint.h>

#include <sys/socket.h>

#include <openssl/ssl.h>
#include <openssl/crypto.h>
#include <openssl/evp.h>

#include "../tne.h"

void tne_strncpy(char *dest, char *src, size_t n) {
  strncpy(dest, src, n);
  dest[n] = 0;
}

void tne_cleanup_openssl(SSL *ssl, SSL_CTX *ctx) {
  if (ssl != NULL) SSL_free(ssl);
  if (ctx != NULL) SSL_CTX_free(ctx);
  EVP_cleanup();
}

int tne_write(SSL *ssl, int fd, char *buf, uint64_t size) {
  if (ssl == NULL) {
    return send(fd, buf, size, 0);
  } else {
    return SSL_write(ssl, buf, size);
  }
}

int tne_read(SSL *ssl, int fd, char *buf, uint32_t size) {
  if (ssl == NULL) {
    return recv(fd, buf, size, 0);
  } else {
    return SSL_read(ssl, buf, size);
  }
}
