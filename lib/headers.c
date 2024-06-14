#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "../tne.h"

void tne_add_header(struct TNEHeaders *headers, char *name, char *value, unsigned int name_length, unsigned int value_length) {
  ++headers->count;

  if (headers->data == NULL) {
    headers->data = malloc(sizeof(tneheader_t));
  } else {
    headers->data = realloc(headers->data, headers->count * sizeof(tneheader_t));
  }

  tneheader_t *header = &headers->data[headers->count - 1];

  header->name = malloc(name_length + 1);
  header->name_length = name_length;
  tne_strncpy(header->name, name, name_length);

  header->value = malloc(value_length + 1);
  header->value_length = value_length;
  tne_strncpy(header->value, value, value_length);
}

void tne_remove_header(struct TNEHeaders *headers, char *name) {
  for (unsigned int i = 0; i < headers->count; ++i) {
    tneheader_t *header = &headers->data[i];

    if (strcmp(header->name, name) == 0) {
      tneheader_t last_header = headers->data[headers->count - 1];

      header->name = realloc(header->name, last_header.name_length + 1);
      tne_strncpy(header->name, last_header.name, last_header.name_length);

      header->name_length = last_header.name_length;
      free(last_header.name);


      header->value = realloc(header->value, last_header.value_length + 1);
      tne_strncpy(header->value, last_header.value, last_header.value_length);

      header->value_length = last_header.value_length;
      free(last_header.value);

      --headers->count;
      headers->data = realloc(headers->data, headers->count * sizeof(tneheader_t));

      break;
    }
  }
}

tneheader_t *tne_get_header(struct TNEHeaders headers, char *name) {
  for (unsigned int i = 0; i < headers.count; ++i) {
    tneheader_t *header = &headers.data[i];
    unsigned int k = 0;
    while (name[k] != '\0' && header->name[k] != '\0' && tolower(name[k]) == tolower(header->name[k])) ++k;
    if (k == header->name_length) return header;
  }

  return NULL;
}

void tne_free_headers(struct TNEHeaders headers) {
  for (unsigned int i = 0; i < headers.count; ++i) {
    tneheader_t header = headers.data[i];
    free(header.name);
    free(header.value);
  }

  free(headers.data);
}
