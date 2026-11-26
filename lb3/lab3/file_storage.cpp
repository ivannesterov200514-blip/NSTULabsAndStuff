#include "file_storage.h"
#include <cstdint>
#include <fstream>
#include <sstream>
#include <cctype>

using namespace std;

static bool valid_type(char type);

const uint32_t MAX_RECORDS = 1000000;
const uint32_t MAX_STRING_SIZE = 1024 * 1024;

static bool write_number(ofstream& file, uint32_t number) {
    for (int i = 0; i < 4; i++) {
        file.put((char)(number & 255));
        number = number >> 8;
    }
    return file.good();
}

static bool read_number(ifstream& file, uint32_t& number) {
    number = 0;
    for (int i = 0; i < 4; i++) {
        int byte = file.get();
        if (byte == -1) return false;
        number = number | ((uint32_t)byte << (i * 8));
    }
    return true;
}

static bool write_string(ofstream& file, const string& value) {
    if (value.size() > MAX_STRING_SIZE) return false;
    if (!write_number(file, (uint32_t)value.size())) return false;
    file.write(value.data(), value.size());
    return file.good();
}

static bool read_string(ifstream& file, string& value) {
    uint32_t size;
    if (!read_number(file, size) || size > MAX_STRING_SIZE) return false;
    value.resize(size);
    if (size > 0) file.read(&value[0], size);
    return file.good();
}

static bool write_binary_records(const string& filename, const vector<FileRecord>& records) {
    if (records.size() > MAX_RECORDS) return false;
    for (const FileRecord& record : records) {
        if (!valid_type(record.type) || record.name.empty() || record.name.size() > MAX_STRING_SIZE || record.values.size() > MAX_RECORDS)
            return false;
        for (const string& value : record.values)
            if (value.size() > MAX_STRING_SIZE) return false;
    }

    ofstream file(filename, ios::binary);
    if (!file.is_open()) return false;

    file.write("LB3B", 4); 
    if (!write_number(file, 1)) return false; 
    if (!write_number(file, (uint32_t)records.size())) return false;

    for (const FileRecord& record : records) {
        file.put(record.type);
        if (!write_string(file, record.name)) return false;
        if (!write_number(file, (uint32_t)record.values.size())) return false;
        for (const string& value : record.values)
            if (!write_string(file, value)) return false;
    }
    file.close();
    return !file.fail();
}

static bool read_binary_records(const string& filename, vector<FileRecord>& records) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) return false;

    char header[4];
    file.read(header, 4);
    if (!file || string(header, 4) != "LB3B") return false;

    uint32_t version, record_count;
    if (!read_number(file, version) || version != 1) return false;
    if (!read_number(file, record_count) || record_count > MAX_RECORDS) return false;

    vector<FileRecord> loaded;
    for (uint32_t i = 0; i < record_count; i++) {
        FileRecord record;
        if (!file.get(record.type) || !valid_type(record.type)) return false;
        if (!read_string(file, record.name) || record.name.empty()) return false;

        uint32_t value_count;
        if (!read_number(file, value_count) || value_count > MAX_RECORDS) return false;
        for (uint32_t j = 0; j < value_count; j++) {
            string value;
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

static bool binary_filename(const string& filename) {
    return filename.size() >= 4 && filename.compare(filename.size() - 4, 4, ".bin") == 0;
}

static bool valid_type(char type) {
    return type == 'M' || type == 'F' || type == 'L' || type == 'S' ||
           type == 'Q' || type == 'T' || type == 'C' || type == 'K';
}

static bool has_spaces(const string& value) {
    for (unsigned char c : value) {
        if (isspace(c)) return true;
    }
    return false;
}

static bool write_text_records(const string& filename, const vector<FileRecord>& records) {
    for (const FileRecord& record : records) {
        if (!valid_type(record.type) || record.name.empty() ||
            has_spaces(record.name) || record.name.size() > MAX_STRING_SIZE ||
            record.values.size() > MAX_RECORDS) return false;
        if ((record.type == 'C' || record.type == 'K') && record.values.size() % 2 != 0) return false;
        for (const string& value : record.values) {
            if (value.empty() || has_spaces(value) || value.size() > MAX_STRING_SIZE) return false;
        }
    }
    ofstream out(filename);
    if (!out) return false;
    for (const FileRecord& record : records) {
        out << record.type << ' ' << record.name;
        for (const string& value : record.values) out << ' ' << value;
        out << '\n';
    }
    out.close();
    return !out.fail();
}

static bool read_text_records(const string& filename, vector<FileRecord>& records) {
    ifstream in(filename);
    if (!in) return false;
    vector<FileRecord> loaded;
    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        istringstream ss(line);
        string type, name, word;
        if (!(ss >> type >> name) || type.size() != 1 || !valid_type(type[0])) return false;
        FileRecord record{type[0], name, {}};
        while (ss >> word) {
            if (word.size() > MAX_STRING_SIZE) return false;
            record.values.push_back(word);
            if (record.values.size() > MAX_RECORDS) return false;
        }
        if ((record.type == 'C' || record.type == 'K') && record.values.size() % 2 != 0) return false;
        loaded.push_back(record);
        if (loaded.size() > MAX_RECORDS) return false;
    }
    if (in.bad()) return false;
    records = loaded;
    return true;
}

bool save_records_to_file(const string& filename, const vector<FileRecord>& records) {
    if (binary_filename(filename)) return write_binary_records(filename, records);
    return write_text_records(filename, records);
}

bool load_records_from_file(const string& filename, vector<FileRecord>& records) {
    if (binary_filename(filename)) return read_binary_records(filename, records);
    return read_text_records(filename, records);
}