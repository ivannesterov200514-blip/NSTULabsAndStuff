#include "taskmanager.h"
#include <iostream>
#include <chrono>

using namespace std;

int main() {
    const int N = 100000;
    TaskManager manager;
    auto start = chrono::steady_clock::now();

    for (int i = 0; i < N; i++) {
        manager.createTask("test", "payload", i % 101);
    }

    auto end = chrono::steady_clock::now();
    double time = chrono::duration<double, milli>(end - start).count();
    cout << "Добавление " << N << " задач: " << time << " мс" << endl;
    return 0;
}