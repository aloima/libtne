#include <string.h>

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

int tne_write(SSL *ssl, int fd, char *message, unsigned long long size) {
  if (ssl == NULL) {
    return send(fd, message, size, 0);
  } else {
    return SSL_write(ssl, message, size);
  }
}

int tne_read(SSL *ssl, int fd, char *buffer, unsigned int size) {
  if (ssl == NULL) {
    return recv(fd, buffer, size, 0);
  } else {
    return SSL_read(ssl, buffer, size);
  }
}
