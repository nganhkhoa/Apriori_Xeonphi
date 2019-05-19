#include <vector>
#include <set>
#include <functional>
#include <string>
#include <map>

typedef std::map<std::string, std::string> feature;

struct setFrequency_t {
  // std::set<std::string> itemSet;
  feature itemSet;
  double support = 0;
};

typedef std::vector<std::string> record_t;
typedef std::vector<setFrequency_t> table;


void read_csv(
    char* filename, std::vector<record_t>* records,
    std::vector<char*> selected_fields);

