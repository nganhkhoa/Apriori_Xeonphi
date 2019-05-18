#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>

#include <iterator>
#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <map>

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

vector<setFrequency_t>* create_table(
    vector<setFrequency_t>* L, map<string, set<string>>* classifications,
    size_t itemTake) {
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
    delete L;
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
  delete L;
  return newL;
}

int main(int argc, char** argv) {
  if (argc < 4) {
    printf(
        "./apriori titanic_train.csv <min_support> <field_1> <field_2>...\n");
    return 0;
  }

  float min_support = atof(argv[2]);
  vector<record_t> records;
  vector<char*> selected_fields;

  for (int i = 3; i < argc; i++)
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

  vector<setFrequency_t>* L = NULL;
  vector<setFrequency_t> L1;
  int count = 1;
  do { // at least one time
    L = create_table(L, &classifications, count);
    if (!L || L->size() == 0)
      break;

    if (count == 1)
      L1 = *L;

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

  // note, because L is overwritten every loop
  // so if you decide to keep an L, create a copy of it before end of loop
  // for example L1

  if (L)
    delete L;

  return 0;
}
