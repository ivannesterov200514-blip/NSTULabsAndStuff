
#include "taskmanager.h"
#include <iostream>
#include <chrono>
#include <iomanip>

using namespace std;

int main() {
    const int N = 100000;
    TaskManager manager;

    auto start = chrono::steady_clock::now();

    for (int i = 0; i < N; i++) {
        manager.createTask("test", "payload", i % 101);
    }

    auto end = chrono::steady_clock::now();
    double addTime = chrono::duration<double, milli>(end - start).count();

    auto searchStart = chrono::steady_clock::now();
    uint64_t searchSum = 0;

    for (int i = 1; i <= N; i++) {
        Task* task = manager.getTask(i);
        searchSum += task->id;
    }

    auto searchEnd = chrono::steady_clock::now();
    double searchTime = chrono::duration<double, milli>(searchEnd - searchStart).count();

    int resizeCount = manager.getHashResizeCount();
    double maxLoadFactor = manager.getHashMaxLoadFactor();

    auto extractStart = chrono::steady_clock::now();
    uint64_t extractSum = 0;

    for (int i = 0; i < N; i++) {
        Task* task = manager.startNextTask();
        extractSum += task->id;
    }

    auto extractEnd = chrono::steady_clock::now();
    double extractTime = chrono::duration<double, milli>(extractEnd - extractStart).count();

    cout << fixed << setprecision(3);
    cout << "[Бенчмарк на " << N << " задачах]" << endl;
    cout << "Добавление: " << addTime << " мс" << endl;
    cout << "Поиск по ID: " << searchTime << " мс" << endl;
    cout << "Последовательное извлечение: " << extractTime << " мс" << endl;
    cout << "Количество расширений хеш-таблицы: " << resizeCount << endl;
    cout << "Максимальный коэффициент заполнения: " << maxLoadFactor << endl;
    cout << "Контрольная сумма поиска: " << searchSum << endl;
    cout << "Контрольная сумма извлечения: " << extractSum << endl;

    return 0;
}
