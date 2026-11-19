#include "binary_storage.h"

#include <cstdint>
#include <fstream>

const std::uint32_t MAX_RECORDS = 1000000;
const std::uint32_t MAX_STRING_SIZE = 1024 * 1024;

static bool write_number(std::ofstream& file, std::uint32_t number) {
    for (int i = 0; i < 4; i++) {
        file.put((char)(number & 255));
        number = number >> 8;
    }
    return file.good();
}

static bool read_number(std::ifstream& file, std::uint32_t& number) {
    number = 0;
    for (int i = 0; i < 4; i++) {
        int byte = file.get();
        if (byte == -1) return false;
        number = number | ((std::uint32_t)byte << (i * 8));
    }
    return true;
}

static bool write_string(std::ofstream& file, const std::string& value) {
    if (value.size() > MAX_STRING_SIZE) return false;
    if (!write_number(file, (std::uint32_t)value.size())) return false;
    file.write(value.data(), value.size());
    return file.good();
}

static bool read_string(std::ifstream& file, std::string& value) {
    std::uint32_t size;
    if (!read_number(file, size) || size > MAX_STRING_SIZE) return false;
    value.resize(size);
    if (size > 0) file.read(&value[0], size);
    return file.good();
}

bool write_binary_records(const std::string& filename,
                          const std::vector<BinaryRecord>& records) {
    if (records.size() > MAX_RECORDS) return false;
    for (const BinaryRecord& record : records) {
        if (record.name.size() > MAX_STRING_SIZE || record.values.size() > MAX_RECORDS)
            return false;
        for (const std::string& value : record.values)
            if (value.size() > MAX_STRING_SIZE) return false;
    }

    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) return false;

    file.write("LB3B", 4); 
    if (!write_number(file, 1)) return false; 
    if (!write_number(file, (std::uint32_t)records.size())) return false;

    for (const BinaryRecord& record : records) {
        file.put(record.type);
        if (!write_string(file, record.name)) return false;
        if (!write_number(file, (std::uint32_t)record.values.size())) return false;
        for (const std::string& value : record.values)
            if (!write_string(file, value)) return false;
    }
    file.close();
    return !file.fail();
}

bool read_binary_records(const std::string& filename,
                         std::vector<BinaryRecord>& records) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) return false;

    char header[4];
    file.read(header, 4);
    if (!file || std::string(header, 4) != "LB3B") return false;

    std::uint32_t version, record_count;
    if (!read_number(file, version) || version != 1) return false;
    if (!read_number(file, record_count) || record_count > MAX_RECORDS) return false;

    std::vector<BinaryRecord> loaded;
    for (std::uint32_t i = 0; i < record_count; i++) {
        BinaryRecord record;
        if (!file.get(record.type)) return false;
        if (!read_string(file, record.name)) return false;

        std::uint32_t value_count;
        if (!read_number(file, value_count) || value_count > MAX_RECORDS) return false;
        for (std::uint32_t j = 0; j < value_count; j++) {
            std::string value;
            if (!read_string(file, value)) return false;
            record.values.push_back(value);
        }
        loaded.push_back(record);
    }

    if (file.peek() != -1) return false;
    if (file.bad()) return false;
    records = loaded; 
    return true;
}
