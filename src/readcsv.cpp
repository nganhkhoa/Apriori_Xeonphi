#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <libcsv/csv.h>

#include "csv_data.h"

int put_comma;
int lines = 0;
int field = 0;
int BUFLEN = 2048;

void cb1(void* s, size_t i, void* p) {
  if (lines == 0)
    return;
#ifdef DEBUGGING
  if (put_comma)
    printf(",");
  printf("%s", (char*)s);
#endif

  struct csv_data_t* pp = (struct csv_data_t*)p;
  switch (field) {
  case 1:
    pp[lines - 1].Survived = atoi((char*)s);
    break;
  case 2:
    pp[lines - 1].Pclass = atoi((char*)s);
    break;
  case 4:
    pp[lines - 1].Sex = strcmp((char*)s, "female") == 0 ? 1 : 0;
    break;
  case 5:
    pp[lines - 1].Age = atoi((char*)s);
    break;
  default:
    break;
  }
  put_comma = 1;
  field++;
}

void cb2(int c, void* p) {
  put_comma = 0;
  field = 0;
  lines++;

#ifdef DEBUGGING
  printf("\nline %d\n", lines);
#endif
}

void read_csv(char* filename, void* returned_data, int* total_data_count) {
  FILE* fp;
  struct csv_parser p;
  char buf[BUFLEN];
  size_t bytes_read;

  if (csv_init(&p, 0) != 0)
    exit(EXIT_FAILURE);
  csv_set_opts(&p, CSV_APPEND_NULL);
  fp = fopen(filename, "rb");
  if (!fp)
    exit(EXIT_FAILURE);

  while ((bytes_read = fread(buf, 1, BUFLEN, fp)) > 0)
    if (csv_parse(&p, buf, bytes_read, cb1, cb2, returned_data) != bytes_read) {
      fprintf(
          stderr, "Error while parsing file: %s\n",
          csv_strerror(csv_error(&p)));
      exit(EXIT_FAILURE);
    }
  csv_fini(&p, cb1, cb2, returned_data);
  fclose(fp);
  csv_free(&p);

#ifdef DEBUGGING
  int i;
  struct csv_data_t* c = (struct csv_data_t*)returned_data;
  for (i = 0; i < 10; i++)
    printf("%d %d %d %d\n", c[i].Survived, c[i].Pclass, c[i].Sex, c[i].Age);
  printf("Lines: %d\n", lines);
#endif

  *total_data_count = lines - 1;
}
