#pragma once
#include <iostream>
#include <string>
#include <stdint.h>

using namespace std;

struct Task {
    uint64_t id;
    string type;
    string payload;
    unsigned int priority;
    string status;
    string created_at;
    string updated_at;
    string error;
};