#include <stdio.h>
#include <stdlib.h>
// #include <math.h>
#include <string.h>
#include <errno.h>

#include <iterator>
#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <map>
#include <limits>
#include <cmath>

#include <omp.h>
#include <libcsv/csv.h>

#include "csv_data.h"

using std::cout;
using std::endl;
using std::map;
using std::pair;
using std::set;
using std::string;
using std::vector;
using std::fabs;

const auto EPSILON = std::numeric_limits<double>::epsilon();

vector <vector<int>> Generate(int count){
  vector<int> s;
  vector<vector<int>> result;
  for (int i=0; i < count; i++) s.push_back(0);
  do {
    int j = count-1;
    while ( j >= 0)
    if (s[j]==1) {
        j--;
    }
    else break;
    
    s[j]=1;
    for (int k= j+1; k < count;  k++)
      s[k] =0;
    bool ok = true;
    for (int i=0; i < count; i++)
      if (s[i]==0) {
        ok = false;
        break;
      }
    if (ok) break;
    else result.push_back(s);
  }
  while (true);
  return result;
}
vector<setFrequency_t>* create_table( vector<setFrequency_t>* L, map<string, set<string>>* classifications,size_t itemTake) {
  auto newL = new vector<setFrequency_t>();
  if (!L) {
    // first table init
    for (auto it = classifications->begin(); it != classifications->end();
         it++) {
      for (string const& s : it->second) {
        setFrequency_t t;
        t.itemSet.insert(pair<string, string>(it->first, s));
        newL->push_back(t);
      }
    }
    return newL;
  }

  if (L->size() <= 1) {
    // delete L;
    return newL;
  }

  // generate table set by combination
  auto oldT1 = L->begin();
  auto oldT2 = next(oldT1, 1);
  while (true) {
    auto set1 = oldT1->itemSet;
    auto set2 = oldT2->itemSet;
    setFrequency_t t;

    // merging itemSet maps
    // insert will not insert item if same key is used
    t.itemSet.insert(set1.begin(), set1.end());
    t.itemSet.insert(set2.begin(), set2.end());

    if (t.itemSet.size() != itemTake) {
      oldT2++;
      if (oldT2 == L->end()) {
        oldT1++;
        oldT2 = next(oldT1, 1);
        if (oldT2 == L->end())
          break;
      }
      continue;
    }

    bool matched = false;
    for (auto s = newL->begin(); s != newL->end(); s++) {
      if ((*s).itemSet == t.itemSet) {
        matched = true;
        break;
      }
    }
    if (!matched)
      newL->push_back(t);

#ifdef DEBUGGING
      // cout << "New set" << endl;
      // for (auto it = t.itemSet.begin(); it != t.itemSet.end(); it++) {
      //   cout << it->first << "-" << it->second << ";";
      // }
      // cout << (matched ? "matched" : "not matched") << endl;
#endif

    oldT2++;
    if (oldT2 == L->end()) {
      oldT1++;
      oldT2 = next(oldT1, 1);
      if (oldT2 == L->end())
        break;
    }
  }
  // delete L;
  return newL;
}

int main(int argc, char** argv) {
  if (argc < 5) {
    printf(
        "./apriori titanic_train.csv <min_support> <min_confidence> <field_1> <field_2>...\n");
    return 0;
  }

  float min_support = atof(argv[2]);
  float min_confidence = atof(argv[3]);
  vector<record_t> records;
  vector<char*> selected_fields;

  for (int i = 4; i < argc; i++)
    selected_fields.push_back(argv[i]);

  map<string, set<string>> classifications;
  map<string, int> field_idx;
  int idx = 0;
  for (auto field = selected_fields.begin(); field != selected_fields.end();
       field++) {
    classifications.insert(pair<string, set<string>>(string(*field), {}));
    field_idx.insert(pair<string, int>(string(*field), idx++));
  }

  printf("Read data file %s\n", argv[1]);
  read_csv(argv[1], &records, selected_fields);
  printf("Done read file, total %ld transactions\n", records.size());

  // age classification
  for (auto record = records.begin(); record != records.end(); record++) {
    int idx = field_idx["Age"];
    int age = atoi((*record)[idx].c_str());
    (*record)[idx] = age >= 18 ? "Aldult" : "Child";
  }

#ifdef DEBUGGING
  printf("After age classification\n");
  auto record = records.begin();
  for (int i = 0; i < 10; i++) {
    for (auto c = (*record).begin(); c != (*record).end(); c++)
      printf("%s\t", (*c).c_str());
    printf("\n");
    record++;
  }
#endif

  // find all values of field
  for (auto record = records.begin(); record != records.end(); record++) {
    for (size_t i = 0; i < selected_fields.size(); i++)
      classifications[selected_fields[i]].insert((*record)[i]);
  }

#ifdef DEBUGGING
  for (auto it = classifications.begin(); it != classifications.end(); it++) {
    cout << "key : " << it->first << "\t";
    cout << "value : ";
    for (string const& s : it->second) {
      cout << s << " ";
    }
    cout << endl;
  }
#endif

  table* L = NULL;
  vector<table*> allL;
  int count = 1;
  do { // at least one time
    L = create_table(L, &classifications, count);
    if (!L || L->size() == 0)
      break;

    allL.push_back(L);

    // find support for table
    // TODO: parallel by each row in L / each item set
    for (auto row = L->begin(); row != L->end(); row++) {
      for (auto record = records.begin(); record != records.end(); record++) {
        bool isItemSet = true;
        for (auto it = row->itemSet.begin(); it != row->itemSet.end(); it++) {
          int idx = field_idx[it->first];
          if ((*record)[idx] != it->second)
            isItemSet = false;
        }
        if (isItemSet)
          row->support++;
      }
    }

#ifdef DEBUGGING
    printf("Table:\n");
    for (auto row = L->begin(); row != L->end(); row++) {
      for (auto it = row->itemSet.begin(); it != row->itemSet.end(); it++) {
        cout << it->first << "-" << it->second << ";";
      }
      cout << row->support << " ";
      cout << ((row->support < min_support * records.size()) ? "no" : "yes")
           << endl;
    }
#endif

    // less then min support reduction
    for (auto row = L->begin(); row != L->end();) {
      if (row->support < min_support * records.size())
        row = L->erase(row);
      else
        row++;
    }

    count++;
  } while (true);

  // process all L
  vector<vector<int>> tohop = Generate(selected_fields.size());

#ifdef DEBUGGING
  // for (size_t i=0; i < tohop.size(); i++){
  //   for (size_t j=0; j < tohop[i].size(); j++)
  //     cout << tohop[i][j] << " ";
  //   cout << endl;
  // }
#endif

  const table* lastL = allL.back();
  map<string, string> supportItemSet;
  map<string, string> toBeSupportItemSet;

  for (auto row = lastL->begin(); row != lastL->end(); row++) {
    cout << "row:\t";
    for (auto it = row->itemSet.begin(); it != row->itemSet.end(); it++) {
      cout << it->first << "-" << it->second << ";";
    }
    cout << row->support << endl;
    for (size_t i=0; i < tohop.size(); i++) {
      for (size_t j=0; j < tohop[i].size(); j++) {
        const string& field = selected_fields[j];
        const string& value = row->itemSet.at(field);
        if (tohop[i][j] != 0) {
          toBeSupportItemSet.insert(pair<string, string>(
            field, value
          ));
        }
        else {
          supportItemSet.insert(pair<string, string>(
            field, value
          ));
        }
      }
      int supportItemCount = supportItemSet.size();
      const table* supportTable = allL[supportItemCount - 1];

      double support = 0.0;
      double confidence = 0.0;

      for (auto it = supportTable->begin(); it != supportTable->end(); it++)
        if (it->itemSet == supportItemSet) {
          support = it->support;
        }

      if (support == 0)
        cout << "Co quan gi do sai roi" << endl;
      else
        confidence = row->support / support;

      for (auto it = supportItemSet.begin(); it != supportItemSet.end(); it++) {
        cout << it->first << "-" << it->second << ";";
      }
      cout << " -> ";
      for (auto it = toBeSupportItemSet.begin(); it != toBeSupportItemSet.end(); it++) {
        cout << it->first << "-" << it->second << ";";
      }
      cout << " -> ";
      printf("%f / %f = %f > %f", row->support, support, confidence, min_confidence);
      cout << " => " << ((confidence > min_confidence) ? "yes" : "no") << endl;

      supportItemSet.clear();
      toBeSupportItemSet.clear();
    }
  }

  for (auto it = allL.begin(); it != allL.end(); it++)
    delete *it;

  return 0;
}