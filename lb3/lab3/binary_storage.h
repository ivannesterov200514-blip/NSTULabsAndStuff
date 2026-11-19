#pragma once

#include <string>
#include <vector>

using namespace std;

struct BinaryRecord {
    char type;
    string name;
    vector<string> values;
};

bool write_binary_records(const string& filename,
                          const vector<BinaryRecord>& records);
bool read_binary_records(const string& filename,
                         vector<BinaryRecord>& records);