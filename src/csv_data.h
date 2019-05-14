struct csv_data_t {
  int Pclass;
  int Sex;
  int Age;
  int Survived;
};

void read_csv(char* filename, void* returned_data, int* total_data_count);
