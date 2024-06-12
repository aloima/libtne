#include <string.h>
#include <stdlib.h>

#include "../tne.h"

void tne_add_header(struct TNEHeaders *dest, char *name, char *value) {
  ++dest->header_count;

  if (dest->headers == NULL) {
    dest->headers = malloc(sizeof(tneheader_t));
  } else {
    dest->headers = realloc(dest->headers, dest->header_count * sizeof(tneheader_t));
  }

  tneheader_t *header = &dest->headers[dest->header_count - 1];

  header->name = malloc(strlen(name) + 1);
  strcpy(header->name, name);

  header->value = malloc(strlen(value) + 1);
  strcpy(header->value, value);
}

void tne_remove_header(struct TNEHeaders *dest, char *name) {
  for (unsigned int i = 0; i < dest->header_count; ++i) {
    tneheader_t *header = &dest->headers[i];

    if (strcmp(header->name, name) == 0) {
      tneheader_t last_header = dest->headers[dest->header_count - 1];

      header->name = realloc(header->name, strlen(last_header.name) + 1);
      strcpy(header->name, last_header.name);
      free(last_header.name);

      header->value = realloc(header->value, strlen(last_header.value) + 1);
      strcpy(header->value, last_header.value);
      free(last_header.value);

      --dest->header_count;
      dest->headers = realloc(dest->headers, dest->header_count * sizeof(tneheader_t));

      break;
    }
  }
}

tneheader_t *tne_get_header(struct TNEHeaders dest, char *name) {
  for (unsigned int i = 0; i < dest.header_count; ++i) {
    tneheader_t *header = &dest.headers[i];
    if (strcmp(header->name, name) == 0) return header;
  }

  return NULL;
}

void tne_free_headers(struct TNEHeaders dest) {
  for (unsigned int i = 0; i < dest.header_count; ++i) {
    tneheader_t header = dest.headers[i];
    free(header.name);
    free(header.value);
  }

  free(dest.headers);
}