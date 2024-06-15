#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include <openssl/ssl.h>
#include <openssl/crypto.h>
#include <openssl/evp.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include "../tne.h"

tneresponse_t *tne_request(tnerequest_t *request) {
  tneresponse_t *response = malloc(sizeof(tneresponse_t));
  memset(response, 0, sizeof(tneresponse_t));

  tneurl_t url = request->url;

  struct hostent *host = gethostbyname(url.hostname);
  int fd;

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    tne_set_last_err(TNERR_SOCK);
    tne_free_response(response);
    return NULL;
  }

  struct sockaddr_in addr = {
    .sin_family = AF_INET,
    .sin_port = htons(url.port),
    .sin_addr = {
      .s_addr = *(long *) (host->h_addr)
    }
  };

  if (connect(fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
    tne_set_last_err(TNERR_SOCK);
    tne_free_response(response);
    close(fd);
    return NULL;
  }

  SSL_CTX *ctx = NULL;
  SSL *ssl = NULL;
  bool is_https = strcmp(url.protocol, "https") == 0;

  if (is_https) {
    SSL_library_init();
    OpenSSL_add_ssl_algorithms();

    if ((ctx = SSL_CTX_new(TLS_method())) == NULL) {
      tne_set_last_err(TNERR_SSL);
      tne_cleanup_openssl(ssl, ctx);
      tne_free_response(response);
      close(fd);
      return NULL;
    }

    if ((ssl = SSL_new(ctx)) == NULL) {
      tne_set_last_err(TNERR_SSL);
      tne_cleanup_openssl(ssl, ctx);
      tne_free_response(response);
      close(fd);
      return NULL;
    }

    SSL_set_fd(ssl, fd);

    if (SSL_connect(ssl) <= 0) {
      tne_set_last_err(TNERR_SSL);
      tne_cleanup_openssl(ssl, ctx);
      tne_free_response(response);
      close(fd);
      return NULL;
    }
  } else if (strcmp(url.protocol, "http") != 0) {
    tne_set_last_err(TNERR_IV);
    tne_free_response(response);
    close(fd);
    return NULL;
  }

  unsigned long long req_headers_len = 0;

  for (unsigned int i = 0; i < request->headers.count; ++i) {
    tneheader_t header = request->headers.data[i];
    req_headers_len += header.name_len + header.value_len + 4;
  }

  char *req_msg = malloc(req_headers_len + request->data_size + 1024);
  unsigned long long req_msg_len = sprintf(req_msg, "%s %s HTTP/1.1\r\n", request->method, url.path);

  for (unsigned int i = 0; i < request->headers.count; ++i) {
    tneheader_t header = request->headers.data[i];
    char line[header.name_len + header.value_len + 5];
    req_msg_len += sprintf(line, "%s: %s\r\n", header.name, header.value);
    strcat(req_msg, line);
  }

  req_msg_len += 21;
  strcat(req_msg, "Connection: close\r\n\r\n");

  req_msg = realloc(req_msg, req_msg_len + 1);

  if (tne_write(ssl, fd, req_msg, req_msg_len) == -1) {
    tne_set_last_err(TNERR_SOCKSSL);
    tne_free_response(response);
    free(req_msg);
    close(fd);
    return NULL;
  }

  free(req_msg);

  char received[2048];
  unsigned int received_size = tne_read(ssl, fd, received, sizeof(received));
  unsigned long long res_msg_size = received_size;
  char *res_msg = malloc(res_msg_size);

  memcpy(res_msg, received, received_size);

  while ((received_size = tne_read(ssl, fd, received, sizeof(received))) > 0) {
    int new_size = res_msg_size + received_size;
    res_msg = realloc(res_msg, new_size);
    memcpy(res_msg + res_msg_size, received, received_size);
    res_msg_size = new_size;
  }

  char status_str[4];
  tne_strncpy(status_str, res_msg + 9, 3);
  response->status.code = atoi(status_str);

  char *tres_msg = res_msg + 13;

  while (1) {
    if (*tres_msg == '\r') {
      response->status.message_len = tres_msg - res_msg - 13;
      response->status.message = malloc(response->status.message_len + 1);
      tne_strncpy(response->status.message, res_msg + 13, response->status.message_len);
      tres_msg += 2; // pass "\r\n"
      break;
    }

    ++tres_msg;
  }

  char *pres_msg = tres_msg;
  char *nat = NULL;

  while (1) {
    if (*tres_msg == ':' && nat == NULL) nat = tres_msg;
    if (*tres_msg == '\r') {
      if (nat != NULL) {
        tne_add_header(&response->headers, pres_msg, nat + 2, nat - pres_msg, tres_msg - nat - 2);
        ++tres_msg; // pass '\n'
        pres_msg = tres_msg + 1; // pass '\r'
        nat = NULL;
      } else {
        tres_msg += 2;
        response->data_size = res_msg_size - (tres_msg - res_msg);

        tneheader_t *content_length = tne_get_header(response->headers, "content-length");

        if (content_length && (unsigned long long) atoll(content_length->value) != response->data_size) {
          tne_set_last_err(TNERR_CMIS);
          tne_free_response(response);
          free(res_msg);
          close(fd);
          return NULL;
        }

        response->data = malloc(response->data_size);
        memcpy(response->data, tres_msg, response->data_size);
        break;
      }
    }

    ++tres_msg;
  }

  if (is_https) {
    SSL_shutdown(ssl);
    tne_cleanup_openssl(ssl, ctx);
  }

  free(res_msg);
  close(fd);

  return response;
}

tnerequest_t *tne_prepare_request(char *method, char *url) {
  unsigned int method_size = 0;

  while (method[method_size] != '\0') {
    if (isspace(method[method_size])) {
      tne_set_last_err(TNERR_HMETH);
      return NULL;
    } else ++method_size;
  }

  method_size += 1;

  tnerequest_t *request = malloc(sizeof(tnerequest_t));
  memset(request, 0, sizeof(tnerequest_t));

  request->method = malloc(method_size);
  memcpy(request->method, method, method_size);

  request->url = tne_parse_url(url);

  struct TNEHeaders *headers = &request->headers;
  tne_add_header(headers, "Host", request->url.hostname, 4, strlen(request->url.hostname));
  tne_add_header(headers, "User-Agent", "libtne " TNE_VERSION, 10, 10);
  tne_add_header(headers, "Accept", "*/*", 6, 3);

  return request;
}

void tne_free_request(tnerequest_t *request) {
  tne_free_headers(request->headers);
  tne_free_url(request->url);
  free(request->method);
  free(request);
}

void tne_free_response(tneresponse_t *response) {
  tne_free_headers(response->headers);
  if (response->status.message != NULL) free(response->status.message);
  if (response->data != NULL) free(response->data);
  free(response);
}
