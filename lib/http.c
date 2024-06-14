#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include "../tne.h"

tneresponse_t *tne_request(tnerequest_t request) {
  tneresponse_t *response = malloc(sizeof(tneresponse_t));
  memset(response, 0, sizeof(tneresponse_t));

  struct hostent *host = gethostbyname(request.url.hostname);
  int fd;

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    fputs("tne_request: socket opening error\n", stderr);
    return NULL;
  }

  struct sockaddr_in addr = {
    .sin_family = AF_INET,
    .sin_port = htons(request.url.port),
    .sin_addr = {
      .s_addr = *(long *) (host->h_addr)
    }
  };

  if (connect(fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
    fputs("tne_request: connecting error\n", stderr);
    return NULL;
  }

  unsigned long long request_message_length = 0;
  unsigned long long request_headers_length = 0;

  for (unsigned int i = 0; i < request.headers.count; ++i) {
    tneheader_t header = request.headers.data[i];
    request_headers_length += header.name_length + header.value_length + 4;
  }

  char *request_message = malloc(request_headers_length + request.data_size + 1024);
  request_message_length = sprintf(request_message, "%s %s HTTP/1.1\r\n", request.method, request.url.path);
  
  for (unsigned int i = 0; i < request.headers.count; ++i) {
    tneheader_t header = request.headers.data[i];
    char line[header.name_length + header.value_length + 5];
    request_message_length += sprintf(line, "%s: %s\r\n", header.name, header.value);
    strcat(request_message, line);
  }

  request_message_length += 21;
  strcat(request_message, "Connection: close\r\n\r\n");

  request_message = realloc(request_message, request_message_length + 1);

  if (send(fd, request_message, request_message_length, 0) == -1) {
    fputs("tne_request: sending error\n", stderr);
    return NULL;
  }

  free(request_message);

  unsigned char received[2048];
  unsigned int received_size = recv(fd, received, sizeof(received), 0);
  unsigned long long response_message_size = received_size;
  char *response_message = malloc(response_message_size);

  memcpy(response_message, received, received_size);

  while ((received_size = recv(fd, received, sizeof(received), 0)) > 0) {
    int new_size = response_message_size + received_size;
    response_message = realloc(response_message, new_size);
    memcpy(response_message + response_message_size, received, received_size);
    response_message_size = new_size;
  }

  char status_string[4];
  tne_strncpy(status_string, response_message + 9, 3);
  response->status.code = atoi(status_string);

  char *tresponse_message = response_message + 13;

  while (1) {
    if (*tresponse_message == '\r') {
      response->status.message_length = tresponse_message - response_message - 13;
      response->status.message = malloc(response->status.message_length + 1);
      tne_strncpy(response->status.message, response_message + 13, response->status.message_length);
      tresponse_message += 2; // pass "\r\n"
      break;
    }

    ++tresponse_message;
  }

  char *presponse_message = tresponse_message;
  char *nat = NULL;

  while (1) {
    if (*tresponse_message == ':' && nat == NULL) nat = tresponse_message;
    if (*tresponse_message == '\r') {
      if (nat != NULL) {
        tne_add_header(&response->headers, presponse_message, nat + 2, nat - presponse_message, tresponse_message - nat - 2);
        ++tresponse_message; // pass '\n'
        presponse_message = tresponse_message + 1;
        nat = NULL;
      } else {
        tresponse_message += 2;
        response->data_size = response_message_size - (tresponse_message - response_message);

        tneheader_t *content_length = tne_get_header(response->headers, "content-length");

        if (content_length && (unsigned long long) atoll(content_length->value) != response->data_size) {
          tne_set_last_error(TNERR_CMIS);
          free(response_message);
          close(fd);
          return NULL;
        }

        response->data = malloc(response->data_size);
        memcpy(response->data, tresponse_message, response->data_size);
        break;
      }
    }

    ++tresponse_message;
  }

  free(response_message);
  close(fd);

  return response;
}

tnerequest_t tne_prepare_request(char *method, char *url) {
  unsigned int method_size = strlen(method) + 1;
  tnerequest_t request = {
    .method = malloc(method_size),
    .url = tne_parse_url(url)
  };

  memcpy(request.method, method, method_size);

  struct TNEHeaders *headers = &request.headers;
  tne_add_header(headers, "Host", request.url.hostname, 4, strlen(request.url.hostname));
  tne_add_header(headers, "User-Agent", "libtne " TNE_VERSION, 10, 10);
  tne_add_header(headers, "Accept", "*/*", 6, 3);

  return request;
}

void tne_free_request(tnerequest_t request) {
  tne_free_headers(request.headers);
  tne_free_url(request.url);
  free(request.method);
}

void tne_free_response(tneresponse_t *response) {
  tne_free_headers(response->headers);
  free(response->status.message);
  free(response->data);
  free(response);
}
