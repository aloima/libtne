/*
  To compile:
  gcc http.c -ltne -lssl -lcrypto
*/

#include <stdio.h>
#include <tne.h>

int main() {
  tnerequest_t *request = tne_prepare_request("GET", "https://example.com");
  tneresponse_t *response = tne_request(request);

  puts("Response headers:");

  for (unsigned int i = 0; i < response->headers.count; ++i) {
    tneheader_t header = response->headers.data[i];
    printf("%s: %s\n", header.name, header.value);
  }

  printf("\nResponse data size: %lld\n", response->data_size);
  puts("Response data:");
  puts(response->data);

  tne_free_request(request);
  tne_free_response(response);

  return 0;
}
