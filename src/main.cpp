#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <libcsv/csv.h>
#include<vector>
#include <omp.h>

#include "csv_data.h"

using namespace std;
enum AGE{ child,adult,old};

enum SEX {male, female};

float min_sup =0.05;
float min_cof = 0.5;

int CountDataSet(csv_data_t* csv_data, int rows, bool check_Age, bool check_Sex, bool check_Survived, bool check_Pclass, int age, int sex, int survived, int pclass){
  int count =0;
  for (int i=0; i < rows; i++){
      if (check_Age==1 && age != csv_data[i].Age) continue;
      if (check_Sex==1 && sex != csv_data[i].Sex) continue;
      if (check_Pclass==1 && pclass != csv_data[i].Pclass) continue;
      if (check_Survived ==1 && survived != csv_data[i].Survived) continue;
      count ++;
  }
  return count;
}
int main(int argc, char** argv) {
  struct csv_data_t* csv_data = new  csv_data_t [900];
  int rows = 0;
  read_csv(argv[1], csv_data, &rows);
  int Age[3] ={};
  int Sex[2]={};
  int Pclass[5]={};
  int Survived[2]={};
  int i, j;
  // Tim L1
  int count_age_L1=0;
  int count_pclass_L1 =0;
  int count_sex_L1 =0;
  int count_survived_L1 =0;

  for (i=0; i < rows; i++){
    if (csv_data[i].Age < 18){
      Age[child]++;
      csv_data[i].Age = 0;
    }
    else if (csv_data[i].Age < 50){
      Age[adult]++;
      csv_data[i].Age = 1;
    }
    else {
      Age[old]++;
      csv_data[i].Age = 2;
    } 
    
    Sex[csv_data[i].Sex]++;
    Pclass[csv_data[i].Pclass]++;
    Survived[csv_data[i].Survived]++;
  }

  // feature Age
  int age_L1[3]={};
  for (i =0; i <3; i++)
    if ((Age[i] > 0) && ((float)Age[i]/ rows > min_sup)){
      age_L1[i] =1;
      count_age_L1++;
    }
  
  for (i=0; i < 3 ; i++) {
    printf("Age: %d ; count: %d ; check: %d \n", i, Age[i], age_L1[i]);
  }
  // feature Pclass
  
  int pclass_L1[5] ={};
  for (i=0; i < 5; i++){
    if ((Pclass[i] > 0) && ((float)Pclass[i] / rows > min_sup )){
      count_pclass_L1++;
      pclass_L1[i] =1;
    }
  }  
  for (i=0; i < 5 ; i++) {
    printf("Pclass: %d ; count: %d ; check: %d \n",i, Pclass[i], pclass_L1[i]);
  }
  
  // feature Sex
  if (Sex[0] > 0 && (float)Sex[0]/ rows > min_sup) count_sex_L1 ++;
  if (Sex[1] > 0 && (float)Sex[1]/ rows > min_sup) count_sex_L1 ++;
  int sex_L1[2]={};
  if (count_sex_L1 == 2){
    sex_L1[0] = 1;
    sex_L1[1] = 1;
  }
  else {

    if (Sex[0] > 0 && ((float)Sex[0]/ rows > min_sup))  sex_L1[0] = 1;
    else sex_L1[1] = 1;
  }
  for (i=0; i < 2 ; i++) {
    printf("Sex: %d ; count: %d ; check: %d \n", i, Sex[i], sex_L1[i]);
  }

  // feature survide
  if (Survived[0] > 0 && (float)Survived[0]/ rows > min_sup) count_survived_L1 ++;
  if (Survived[1] > 0 && (float)Survived[1]/ rows > min_sup) count_survived_L1 ++;
  int survived_L1[2]={};
  if (count_survived_L1 == 2){
    survived_L1[0] = 1;
    survived_L1[1] = 1;
  }
  else {
    if (survived_L1[0] > 0 && ((float)survived_L1[0]/ rows > min_sup))  survived_L1[0] = 1;
    else survived_L1[1] = 1;
  }

  for (i=0; i < 2 ; i++) {
    printf("survived: %d ; count: %d ; check: %d \n", i, Survived[i], survived_L1[i]);
  }

  cout << "-----------------------------------------------------------------------" << endl;
  
  // // Tinh L2

  // Feature Age_Sex
  vector<Age_Sex> age_sex;
  for (int i=0; i < 3; i++)
    if (age_L1[i]==1){
      for (int j=0; j < 2; j ++){
        if (sex_L1[j] ==1){
          int count = CountDataSet(csv_data,rows, 1, 1, 0, 0, i, j , 0 , 0);
          if (count > 0 && (float) count / rows > min_sup){
            Age_Sex k;
            k.Age = i;
            k.count = count;
            k.Sex = j;
            age_sex.push_back(k);
          }
        }
      }
    }

    cout << "Age_Sex_L2" << endl;

    for (int i=0; i < age_sex.size(); i++){
      cout << "Age: " << age_sex[i].Age << " Sex: " << age_sex[i].Sex  << endl;
    }
  
  // feature Age_Pclass
  vector<Age_Pclass> age_pclass;
  for (int i=0; i < 3; i++)
    if (age_L1[i]==1){
      for (int j=0; j < 5; j ++){
        if (pclass_L1[j] ==1){
          int count = CountDataSet(csv_data,rows, 1, 0, 1, 0, i, 0 , j , 0);
          if (count > 0 && (float) count / rows > min_sup){
            Age_Pclass k;
            k.Age = i;
            k.count = count;
            k.Pclass = j;
            age_pclass.push_back(k);
          }
        }
      }
    }
  cout << "Age_Pclass_L2" << endl;
  for (int i=0; i < age_pclass.size(); i++){
      cout << "Age: " << age_pclass[i].Age << " Pclass: " << age_pclass[i].Pclass << endl;
    }

  // feature Age_Survived
  vector<Age_Survived> age_survived;
  for (int i=0; i < 3; i++)
    if (age_L1[i]==1){
      for (int j=0; j < 2; j ++){
        if (survived_L1[j] ==1){
          int count = CountDataSet(csv_data,rows, 1, 0, 0, 1, i, 0 , 0 , j);
          if (count > 0 && (float) count / rows > min_sup){
            Age_Survived k;
            k.Age = i;
            k.count = count;
            k.Survived = j;
            age_survived.push_back(k);
          }
        }
      }
    }

  cout << "Age_Survived_L2" << endl;
  for (int i=0; i < age_survived.size(); i++){
      cout << "Age: " << age_survived[i].Age << " Survived: " << age_survived[i].Survived  << endl;
    }

  // feature Sex_Pclass
  vector<Sex_Pclass> sex_pclass;
  for (int i=0; i < 2; i++)
    if (sex_L1[i]==1){
      for (int j=0; j < 5; j ++){
        if (pclass_L1[j] ==1){
          int count = CountDataSet(csv_data,rows, 0, 1, 0, 1, 0, i , 0 , j);
          if (count > 0 && (float) count / rows > min_sup){
            Sex_Pclass k;
            k.Sex = i;
            k.count = count;
            k.Pclass = j;
            sex_pclass.push_back(k);
          }
        }
      }
    }

  cout << "Sex_Pclass_L2" << endl;
  for (int i=0; i < sex_pclass.size(); i++){
      cout << "Sex: " << sex_pclass[i].Sex << " Pclass: " << sex_pclass[i].Pclass << endl;
    }

  // feature Sex_Survived

  vector<Sex_Survived> sex_survived;
  for (int i=0; i < 2; i++)
    if (sex_L1[i]==1){
      for (int j=0; j < 2; j ++){
        if (survived_L1[j] ==1){
          int count = CountDataSet(csv_data,rows, 0, 1, 1, 0, 0, i , j , 0);
          if (count > 0 && (float) count / rows > min_sup){
            Sex_Survived k;
            k.Sex = i;
            k.count = count;
            k.Survived = j;
            sex_survived.push_back(k);
          }
        }
      }
    }

  cout << "Sex_Survived_L2" << endl;
  for (int i=0; i < sex_survived.size(); i++){
      cout << "Sex: " << sex_survived[i].Sex << " Survived: " << sex_survived[i].Survived  << endl;
    }
  
  // feature Pclass_Survived

  vector<Pclass_Survived> pclass_survived;
  for (int i=0; i < 5; i++)
    if (pclass_L1[i]==1){
      for (int j=0; j < 2; j ++){
        if (survived_L1[j] ==1){
          int count = CountDataSet(csv_data,rows, 0, 0, 1, 1, 0, 0 , j , i);
          if (count > 0 && (float) count / rows > min_sup){
            Pclass_Survived k;
            k.Pclass = i;
            k.count = count;
            k.Survived = j;
            pclass_survived.push_back(k);
          }
        }
      }
    }

  cout << "Pclass_Survived_L2" << endl;
  for (int i=0; i < pclass_survived.size(); i++){
      cout << "Pclass: " << pclass_survived[i].Pclass << " Survived: " << pclass_survived[i].Survived  << endl;
    }

  cout << "--------------------------------------------------------------" << endl;

  // Tinh L3
  vector <Age_Sex_Pclass> age_sex_pclass;
  vector <Age_Sex_Survived> age_sex_survived;
  vector <Age_Pclass_Survived> age_pclass_survived;
  vector <Sex_Pclass_Survived> sex_pclass_survived;

  // feature Age_Sex_Pclass: Age_Sex + Age_Pclass
  for (int i=0; i < age_sex.size(); i++){
      for (int j=0; j < age_pclass.size(); j ++){
        if (age_sex[i].Age == age_pclass[j].Age){
          int count = CountDataSet(csv_data,rows,  1, 1, 0, 1, age_sex[i].Age, age_sex[i].Sex , 0 , age_pclass[j].Pclass);
          if (count > 0 && (float) count / rows > min_sup){
            Age_Sex_Pclass k;
            k.Age = age_sex[i].Age;
            k.Sex = age_sex[i].Sex;
            k.Pclass = age_pclass[j].Pclass;
            k.count = count;
            bool oke= true;
            for (int l=0; l < age_sex_pclass.size(); l++){
              if (age_sex_pclass[l].Sex == k.Sex && age_sex_pclass[l].Age == k.Age && age_sex_pclass[l].Pclass == k.Pclass){
                oke = false;
                break;
              }
            }
            if (oke){ age_sex_pclass.push_back(k);};
          }
        }
      }
    }
  
  // feature Age_Sex_Pclass: Age_Sex + Sex_Pclass
  for (int i=0; i < age_sex.size(); i++){
      for (int j=0; j < sex_pclass.size(); j ++){
        if (age_sex[i].Sex == sex_pclass[j].Sex){
          int count = CountDataSet(csv_data,rows,  1, 1, 0, 1, age_sex[i].Age, age_sex[i].Sex , 0 , sex_pclass[j].Pclass);
          if (count > 0 && (float) count / rows > min_sup){
            Age_Sex_Pclass k;
            k.Age = age_sex[i].Age;
            k.Sex = age_sex[i].Sex;
            k.Pclass = sex_pclass[j].Pclass;
            k.count = count;
            bool oke= true;
            for (int l=0; l < age_sex_pclass.size(); l++){
              if (age_sex_pclass[l].Sex == k.Sex && age_sex_pclass[l].Age == k.Age && age_sex_pclass[l].Pclass == k.Pclass){
                oke = false;
                break;
              }
            }
            if (oke){ age_sex_pclass.push_back(k);};
          }
        }
      }
    }

  // feature Age_Sex_Survived : Age_Sex + Age_Survived

  for (int i=0; i < age_sex.size(); i++){
      for (int j=0; j < age_survived.size(); j ++){
        if (age_sex[i].Age == age_survived[j].Age){
          int count = CountDataSet(csv_data,rows,  1, 1, 1, 0, age_sex[i].Age, age_sex[i].Sex , age_survived[j].Survived , 0);
          if (count > 0 && (float) count / rows > min_sup){
            Age_Sex_Survived k;
            k.Age = age_sex[i].Age;
            k.Sex = age_sex[i].Sex;
            k.Survived = age_survived[j].Survived;
            k.count = count;
            bool oke= true;
            for (int l=0; l < age_sex_survived.size(); l++){
              if (age_sex_survived[l].Sex == k.Sex && age_sex_survived[l].Age == k.Age && age_sex_survived[l].Survived == k.Survived){
                oke = false;
                break;
              }
            }
            if (oke){ age_sex_survived.push_back(k);};
          }
        }
      }
    }

  // feature Age_Sex_Survived : Age_Sex + Sex_Survived

  for (int i=0; i < age_sex.size(); i++){
      for (int j=0; j < sex_survived.size(); j ++){
        if (age_sex[i].Sex == sex_survived[j].Sex){
          int count = CountDataSet(csv_data,rows,  1, 1, 1, 0, age_sex[i].Age, age_sex[i].Sex , sex_survived[j].Survived , 0);
          if (count > 0 && (float) count / rows > min_sup){
            Age_Sex_Survived k;
            k.Age = age_sex[i].Age;
            k.Sex = age_sex[i].Sex;
            k.Survived = sex_survived[j].Survived;
            k.count = count;
            bool oke= true;
            for (int l=0; l < age_sex_survived.size(); l++){
              if (age_sex_survived[l].Sex == k.Sex && age_sex_survived[l].Age == k.Age && age_sex_survived[l].Survived == k.Survived){
                oke = false;
                break;
              }
            }
            if (oke){ age_sex_survived.push_back(k);};
          }
        }
      }
    }

  // feature Age_Pclass_Survived : Age_Sex + Pclass_Survived

  for (int i=0; i < age_sex.size(); i++){
      for (int j=0; j < pclass_survived.size(); j ++){
          int count = CountDataSet(csv_data,rows,  1, 0, 1, 1, age_sex[i].Age, 0 , pclass_survived[j].Survived , pclass_survived[j].Pclass);
          if (count > 0 && (float) count / rows > min_sup){
            Age_Pclass_Survived k;
            k.Age = age_sex[i].Age;
            k.Pclass = pclass_survived[j].Pclass;
            k.Survived = pclass_survived[j].Survived;
            k.count = count;
            bool oke= true;
            for (int l=0; l < age_pclass_survived.size(); l++){
              if (age_pclass_survived[l].Age == k.Age && age_pclass_survived[l].Pclass == k.Pclass && age_pclass_survived[l].Survived == k.Survived){
                oke = false;
                break;
              }
            }
            if (oke){ age_pclass_survived.push_back(k);};
        }
      }
    }

  // feature Age_Pclass_Survived: Age_Pclass + Pclass_Survived
  
  for (int i=0; i < age_pclass.size(); i++){
      for (int j=0; j < pclass_survived.size(); j ++)
          if (age_pclass[i].Pclass == pclass_survived[j].Pclass){
          int count = CountDataSet(csv_data,rows,  1, 0, 1, 1, age_pclass[i].Age, 0 , pclass_survived[j].Survived , pclass_survived[j].Pclass);
          if (count > 0 && (float) count / rows > min_sup){
            Age_Pclass_Survived k;
            k.Age = age_pclass[i].Age;
            k.Pclass = pclass_survived[j].Pclass;
            k.Survived = pclass_survived[j].Survived;
            k.count = count;
            bool oke= true;
            for (int l=0; l < age_pclass_survived.size(); l++){
              if (age_pclass_survived[l].Age == k.Age && age_pclass_survived[l].Pclass == k.Pclass && age_pclass_survived[l].Survived == k.Survived){
                oke = false;
                break;
              }
            }
            if (oke){ age_pclass_survived.push_back(k);};
        }
      }
    }

  // feature Age_Pclass_Survived: Age_Pclass + Age_Survived
  for (int i=0; i < age_pclass.size(); i++){
      for (int j=0; j < age_survived.size(); j ++)
          if (age_pclass[i].Pclass == age_survived[j].Age){
          int count = CountDataSet(csv_data,rows,  1, 0, 1, 1, age_pclass[i].Age, 0 , age_survived[j].Survived , age_pclass[i].Pclass);
          if (count > 0 && (float) count / rows > min_sup){
            Age_Pclass_Survived k;
            k.Age = age_pclass[i].Age;
            k.Pclass = age_pclass[j].Pclass;
            k.Survived = age_survived[j].Survived;
            k.count = count;
            bool oke= true;
            for (int l=0; l < age_pclass_survived.size(); l++){
              if (age_pclass_survived[l].Age == k.Age && age_pclass_survived[l].Pclass == k.Pclass && age_pclass_survived[l].Survived == k.Survived){
                oke = false;
                break;
              }
            }
            if (oke){ age_pclass_survived.push_back(k);};
        }
      }
    }


  // feature Sex_Pclass_Survived: Sex_Survived +  Pclass_Survived

  for (int i=0; i < sex_survived.size(); i++){
      for (int j=0; j < pclass_survived.size(); j ++)
          if (sex_survived[i].Survived == pclass_survived[j].Survived){
          int count = CountDataSet(csv_data,rows,  0, 1, 1, 1, 0, sex_survived[i].Sex , sex_survived[j].Survived , pclass_survived[i].Pclass);
          if (count > 0 && (float) count / rows > min_sup){
            Sex_Pclass_Survived k;
            k.Sex = sex_survived[i].Sex;
            k.Pclass = pclass_survived[j].Pclass;
            k.Survived = pclass_survived[j].Survived;
            k.count = count;
            bool oke= true;
            for (int l=0; l < sex_pclass_survived.size(); l++){
              if (sex_pclass_survived[l].Sex == k.Sex && sex_pclass_survived[l].Pclass == k.Pclass && sex_pclass_survived[l].Survived == k.Survived){
                oke = false;
                break;
              }
            }
            if (oke){ sex_pclass_survived.push_back(k);};
        }
      }
    }

  cout << "Age_Sex_Pclass" << endl;
  for (int i=0; i < age_sex_pclass.size(); i++)
    cout << "Age: " << age_sex_pclass[i].Age << " Sex: " << age_sex_pclass[i].Sex << " Pclass: " << age_sex_pclass[i].Pclass << endl;
  
  cout << "Age_Sex_Survived" << endl;
  
  for (int i=0; i < age_sex_survived.size(); i++)
    cout << "Age: " << age_sex_survived[i].Age << " Sex: " << age_sex_survived[i].Sex << " Survived: " << age_sex_survived[i].Survived << endl;

  cout << "Age_Pclass_Survived" << endl;
  
  for (int i=0; i < age_pclass_survived.size(); i++)
    cout << "Age: " << age_pclass_survived[i].Age << " Pclass: " << age_pclass_survived[i].Pclass << " Survived: " << age_pclass_survived[i].Survived << endl;

  cout << "Sex_Pclass_Survived" << endl;

  for (int i=0; i < sex_pclass_survived.size(); i++)
    cout << "Sex: " << sex_pclass_survived[i].Sex << " Pclass: " << sex_pclass_survived[i].Pclass << " Survived: " << sex_pclass_survived[i].Survived << endl;
  

  cout << "--------------------------------------------------------------" << endl;

  // Tinh L4
  vector<Age_Sex_Pclass_Survived> age_sex_pclass_survived;

  // feature Age_Sex_Pclass + Age_Sex_Survived

  for (int i=0; i < age_sex_pclass.size(); i++)
    for (int j=0; j < age_sex_survived.size(); j++)
      if (age_sex_pclass[i].Age == age_sex_survived[j].Age && age_sex_pclass[i].Sex == age_sex_survived[j].Sex){
        
        int count = CountDataSet(csv_data,rows,  1, 1, 1, 1, age_sex_pclass[i].Age , age_sex_pclass[i].Sex, age_sex_survived[j].Survived , age_sex_pclass[i].Pclass);
        if (count > 0 && (float) count/rows > min_sup){
          Age_Sex_Pclass_Survived k;
          k.Age = age_sex_pclass[i].Age;
          k.Pclass = age_sex_pclass[i].Pclass;
          k.Sex = age_sex_pclass[i].Sex;
          k.Survived = age_sex_survived[j].Survived;

          bool ok = true;
          for (int l=0; l < age_sex_pclass_survived.size(); l++)
            if (age_sex_pclass_survived[l].Age == k.Age && age_sex_pclass_survived[l].Pclass == k.Pclass && age_sex_pclass_survived[l].Sex == k.Sex && age_sex_pclass_survived[l].Survived == k.Survived){
              ok =false;
              break;
            }
          if (ok){
            age_sex_pclass_survived.push_back(k);
          }
        }
      }
  
  // feature Age_Sex_Pclass +  Age_Pclass_Survived

  for (int i=0; i < age_sex_pclass.size(); i++)
    for (int j=0; j < age_pclass_survived.size(); j++)
      if (age_sex_pclass[i].Age == age_pclass_survived[j].Age && age_sex_pclass[i].Pclass == age_pclass_survived[j].Pclass){
        
        int count = CountDataSet(csv_data,rows,  1, 1, 1, 1, age_sex_pclass[i].Age , age_sex_pclass[i].Sex, age_pclass_survived[j].Survived , age_sex_pclass[i].Pclass);
        if (count > 0 && (float) count/rows > min_sup){
          Age_Sex_Pclass_Survived k;
          k.Age = age_sex_pclass[i].Age;
          k.Pclass = age_sex_pclass[i].Pclass;
          k.Sex = age_sex_pclass[i].Sex;
          k.Survived = age_pclass_survived[j].Survived;

          bool ok = true;
          for (int l=0; l < age_sex_pclass_survived.size(); l++)
            if (age_sex_pclass_survived[l].Age == k.Age && age_sex_pclass_survived[l].Pclass == k.Pclass && age_sex_pclass_survived[l].Sex == k.Sex && age_sex_pclass_survived[l].Survived == k.Survived){
              ok =false;
              break;
            }
          if (ok){
            age_sex_pclass_survived.push_back(k);
          }
        }
      }

  // feature Age_Sex_Pclass + Sex_Pclass_Survived 

  for (int i=0; i < age_sex_pclass.size(); i++)
    for (int j=0; j < sex_pclass_survived.size(); j++)
      if (age_sex_pclass[i].Sex == sex_pclass_survived[j].Sex && age_sex_pclass[i].Pclass == sex_pclass_survived[j].Pclass){
        
        int count = CountDataSet(csv_data,rows,  1, 1, 1, 1, age_sex_pclass[i].Age , age_sex_pclass[i].Sex, sex_pclass_survived[j].Survived , age_sex_pclass[i].Pclass);
        if (count > 0 && (float) count/rows > min_sup){
          Age_Sex_Pclass_Survived k;
          k.Age = age_sex_pclass[i].Age;
          k.Pclass = age_sex_pclass[i].Pclass;
          k.Sex = age_sex_pclass[i].Sex;
          k.Survived = sex_pclass_survived[j].Survived;

          bool ok = true;
          for (int l=0; l < age_sex_pclass_survived.size(); l++)
            if (age_sex_pclass_survived[l].Age == k.Age && age_sex_pclass_survived[l].Pclass == k.Pclass && age_sex_pclass_survived[l].Sex == k.Sex && age_sex_pclass_survived[l].Survived == k.Survived){
              ok =false;
              break;
            }
          if (ok){
            age_sex_pclass_survived.push_back(k);
          }
        }
      }

  // feature Age_Sex_Survived + Sex_Pclass_Survived

  for (int i=0; i < age_sex_survived.size(); i++)
    for (int j=0; j < sex_pclass_survived.size(); j++)
      if (age_sex_survived[i].Sex == sex_pclass_survived[j].Sex && age_sex_survived[i].Survived == sex_pclass_survived[j].Survived){
        
        int count = CountDataSet(csv_data,rows,  1, 1, 1, 1, age_sex_survived[i].Age , age_sex_survived[i].Sex, sex_pclass_survived[j].Survived , sex_pclass_survived[j].Pclass);
        if (count > 0 && (float) count/rows > min_sup){
          Age_Sex_Pclass_Survived k;
          k.Age = age_sex_survived[i].Age;
          k.Pclass = sex_pclass_survived[j].Pclass;
          k.Sex = age_sex_survived[i].Sex;
          k.Survived = sex_pclass_survived[j].Survived;

          bool ok = true;
          for (int l=0; l < age_sex_pclass_survived.size(); l++)
            if (age_sex_pclass_survived[l].Age == k.Age && age_sex_pclass_survived[l].Pclass == k.Pclass && age_sex_pclass_survived[l].Sex == k.Sex && age_sex_pclass_survived[l].Survived == k.Survived){
              ok =false;
              break;
            }
          if (ok){
            age_sex_pclass_survived.push_back(k);
          }
        }
      }

  // feature Age_Sex_Survived + Age_Pclass_Survived
  for (int i=0; i < age_sex_survived.size(); i++)
    for (int j=0; j < age_pclass_survived.size(); j++)
      if (age_sex_survived[i].Age == age_pclass_survived[j].Age && age_sex_survived[i].Survived == age_pclass_survived[j].Survived){
        
        int count = CountDataSet(csv_data,rows,  1, 1, 1, 1, age_sex_survived[i].Age , age_sex_survived[i].Sex, age_sex_survived[j].Survived , age_pclass_survived[j].Pclass);
        if (count > 0 && (float) count/rows > min_sup){
          Age_Sex_Pclass_Survived k;
          k.Age = age_sex_survived[i].Age;
          k.Pclass = age_pclass_survived[j].Pclass;
          k.Sex = age_sex_survived[i].Sex;
          k.Survived = age_pclass_survived[j].Survived;

          bool ok = true;
          for (int l=0; l < age_sex_pclass_survived.size(); l++)
            if (age_sex_pclass_survived[l].Age == k.Age && age_sex_pclass_survived[l].Pclass == k.Pclass && age_sex_pclass_survived[l].Sex == k.Sex && age_sex_pclass_survived[l].Survived == k.Survived){
              ok =false;
              break;
            }
          if (ok){
            age_sex_pclass_survived.push_back(k);
          }
        }
      }
  
  // feature Age_Pclass_Survived + Sex_Pclass_Surivived

  for (int i=0; i < age_pclass_survived.size(); i++)
    for (int j=0; j < sex_pclass_survived.size(); j++)
      if (age_pclass_survived[i].Pclass == sex_pclass_survived[j].Pclass && age_pclass_survived[i].Survived == sex_pclass_survived[j].Survived){
        
        int count = CountDataSet(csv_data,rows,  1, 1, 1, 1, age_pclass_survived[i].Age , sex_pclass_survived[j].Sex, age_pclass_survived[i].Survived , age_pclass_survived[i].Pclass);
        if (count > 0 && (float) count/rows > min_sup){
          Age_Sex_Pclass_Survived k;
          k.Age = age_pclass_survived[i].Age;
          k.Pclass = age_pclass_survived[i].Pclass;
          k.Sex = sex_pclass_survived[j].Sex;
          k.Survived = age_pclass_survived[i].Survived;

          bool ok = true;
          for (int l=0; l < age_sex_pclass_survived.size(); l++)
            if (age_sex_pclass_survived[l].Age == k.Age && age_sex_pclass_survived[l].Pclass == k.Pclass && age_sex_pclass_survived[l].Sex == k.Sex && age_sex_pclass_survived[l].Survived == k.Survived){
              ok =false;
              break;
            }
          if (ok){
            age_sex_pclass_survived.push_back(k);
          }
        }
      }
  for (int i=0; i < age_sex_pclass_survived.size(); i++)
    cout <<"Age: " << age_sex_pclass_survived[i].Age << " Sex: " << age_sex_pclass_survived[i].Sex << " Pclass: " << age_sex_pclass_survived[i].Pclass << " Survived: " << age_sex_pclass_survived[i].Survived << endl;

  return 0;
}



