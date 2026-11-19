#pragma once

#include <string>
#include <vector>

struct BinaryRecord {
    char type;
    std::string name;
    std::vector<std::string> values;
};

bool write_binary_records(const std::string& filename,
                          const std::vector<BinaryRecord>& records);
bool read_binary_records(const std::string& filename,
                         std::vector<BinaryRecord>& records);
