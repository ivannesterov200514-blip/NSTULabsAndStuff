#pragma once
#include <iostream>
#include <string>

using namespace std;

class Task {
    int id;
    string type;
    string payload;
    int priority;
    string status;
    string created_at;
    string updated_at;
    string error;
};