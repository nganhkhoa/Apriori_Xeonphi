#include <vector>
#include <set>
#include <functional>
#include <string>
#include <map>

typedef std::vector<std::string> record_t;

struct setFrequency_t {
  // std::set<std::string> itemSet;
  std::map<std::string, std::string> itemSet;
  int support = 0;
};

void read_csv(
    char* filename, std::vector<record_t>* records,
    std::vector<char*> selected_fields);
