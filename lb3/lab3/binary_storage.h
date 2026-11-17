#pragma once

#include <string>
#include <vector>

// Одна запись: тип структуры, её имя, все сохранённые значения.
struct BinaryRecord {
    char type;
    std::string name;
    std::vector<std::string> values;
};

// Файл .bin: "LB3B", версия (4 байта), число структур (4 байта),
// затем для каждой структуры: тип (1 байт), имя, число значений, значения.
// Перед каждой строкой записана её длина (4 байта).
// Числа хранятся в little-endian, формат совместим с первой версией проекта.
bool write_binary_records(const std::string& filename,
                          const std::vector<BinaryRecord>& records);
bool read_binary_records(const std::string& filename,
                         std::vector<BinaryRecord>& records);
