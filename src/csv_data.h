struct csv_data_t {
  int Pclass;
  int Sex;
  int Age;
  int Survived;
};
struct Age_Sex{
  int Age;
  int Sex;
  int count;
};

struct Age_Pclass{
  int Pclass;
  int Age;
  int count;
};

struct Age_Survived{
  int Age;
  int Survived;
  int count;
};

struct Sex_Pclass{
  int Sex;
  int Pclass;
  int count;
};

struct Sex_Survived{
  int Sex;
  int Survived;
  int count;
};
struct Pclass_Survived{
  int Pclass;
  int Survived;
  int count;
};

struct Age_Sex_Survived{
  int Age;
  int Sex;
  int Survived;
  int count;
};
struct Age_Sex_Pclass{
  int Age;
  int Sex;
  int Pclass;
  int count;
};

struct Sex_Survived_Pclass{
  int Sex;
  int Survived;
  int Pclass;
  int count;
};

void read_csv(char* filename, void* returned_data, int* total_data_count);
