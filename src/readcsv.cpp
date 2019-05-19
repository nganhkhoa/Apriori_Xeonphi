#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <string>

#include <libcsv/csv.h>

#include "csv_data.h"

using std::string;
using std::vector;

struct csv_parser_args {
  vector<record_t>* records;
  vector<char*> fields;
  vector<int> field_idx;
};

int put_comma;
int lines = 1;
int field = 0;
int new_line = 0;
int BUFLEN = 2048;

void cb1(void* s, size_t i, void* p) {
#ifdef DEBUGGING
  // if (put_comma)
  //   printf(",");
  // printf("%s", (char*)s);
#endif

  csv_parser_args* args = (csv_parser_args*)p;

  if (lines == 1) {
    for (auto f = args->fields.begin(); f != args->fields.end(); f++) {
      if (strcmp(*f, (char*)s) != 0)
        continue;
      args->field_idx.push_back(field);
    }
    put_comma = 1;
    field++;
    return;
  }

  for (auto f = args->field_idx.begin(); f != args->field_idx.end(); f++) {
    if (field == *f) {
      if (new_line) {
        args->records->push_back({});
        new_line = 0;
      }
      record_t& latest_record = args->records->back();
      latest_record.push_back(string((char*)s));
    }
  }

  put_comma = 1;
  field++;
}

void cb2(int c, void* p) {
#ifdef DEBUGGING
  // printf("\nline %d: ", lines);
#endif

  new_line = 1;
  put_comma = 0;
  field = 0;
  lines++;
}

void read_csv(char* filename, vector<record_t>* records, vector<char*> fields) {
  FILE* fp;
  csv_parser p;
  char buf[BUFLEN];
  size_t bytes_read;

  csv_parser_args args = {records, fields};

  if (csv_init(&p, 0) != 0)
    exit(EXIT_FAILURE);
  csv_set_opts(&p, CSV_APPEND_NULL);
  fp = fopen(filename, "rb");
  if (!fp)
    exit(EXIT_FAILURE);

  while ((bytes_read = fread(buf, 1, BUFLEN, fp)) > 0)
    if (csv_parse(&p, buf, bytes_read, cb1, cb2, &args) != bytes_read) {
      fprintf(
          stderr, "Error while parsing file: %s\n",
          csv_strerror(csv_error(&p)));
      exit(EXIT_FAILURE);
    }
  csv_fini(&p, cb1, cb2, &args);
  fclose(fp);
  csv_free(&p);

#ifdef DEBUGGING
  printf("\n");
  int i;
  for (auto c = fields.begin(); c != fields.end(); c++)
    printf("%s\t", *c);
  printf("\n");
  auto record = records->begin();
  for (i = 0; i < 10; i++) {
    for (auto c = (*record).begin(); c != (*record).end(); c++)
      printf("%s\t", (*c).c_str());
    printf("\n");
    record++;
  }
  printf("Rows: %ld\n", records->size());
#endif
}
