#pragma once
#include <string>
#include <vector>

using namespace std;

struct FileRecord {
    char type;
    string name;
    vector<string> values;
};

bool save_records_to_file(const string& filename, const vector<FileRecord>& records);
bool load_records_from_file(const string& filename, vector<FileRecord>& records);
