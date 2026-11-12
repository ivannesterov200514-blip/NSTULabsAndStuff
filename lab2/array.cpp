#include "array.h"

static void arr_resize (Arr* arr, int newcap) {
    string* newdata = new string[newcap];
    for (int i = 0; i < arr->size; i++) newdata[i] = arr->data[i];
    delete[] arr->data;
    arr->data = newdata;
    arr->capacity = newcap;
}

string arr_search (Arr* arr, int idx) {
    if (idx >= arr->size || idx < 0) {
        cout << "Ошибка: Такого индекса нет\n";
        return "";
    }
    return arr->data[idx];
}

void arr_insert_tail (Arr* arr, string val) {
    if (arr->size >= arr->capacity) arr_resize(arr, arr->capacity*2);
    arr->data[arr->size] = val;
    arr->size++;
}

void gistogram (Arr* arr) {
    int counts[128] = {0};
    for (int i = 0; i < arr->size; i++) {
        string s = arr_search(arr, i);
        if (s.empty()) continue;
        unsigned char c = s[0];
        if (c == ' ' || c == '\n' || c == '\r' || c == '\t') continue;
        if (c >= 128) {
            cout << "Ошибка: Символ вне ASCII не поддерживается\n";
            return;
        }
        counts[c]++;
    }

    string symbols;
    int heights[128];
    int n = 0;
    int max_h = 0;
    for (int code = 0; code < 128; code++) {
        if (counts[code] > 0) {
            symbols += (char)code;
            heights[n] = counts[code];
            if (counts[code] > max_h) max_h = counts[code];
            n++;
        }
    }
    if (n == 0) {
        cout << "Ошибка: Нет символов для гистограммы\n";
        return;
    }

    for (int level = max_h; level >= 1; level--) {
        string line;
        for (int k = 0; k < n; k++) {
            line += (heights[k] >= level) ? '#' : ' ';
        }
        size_t end = line.find_last_not_of(' ');
        cout << line.substr(0, end + 1) << endl;
    }
    cout << symbols << endl;
}
