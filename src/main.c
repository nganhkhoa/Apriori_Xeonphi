#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>

#include <libcsv/csv.h>

#include <omp.h>

#include "csv_data.h"

int main(int argc, char** argv) {
  struct csv_data_t* csv_data = malloc(sizeof(struct csv_data_t) * 900);
  int rows = 0;
  read_csv(argv[1], csv_data, &rows);
  return 0;
}
