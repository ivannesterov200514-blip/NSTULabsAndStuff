#include "map_generator.h"
#include <ctime>
#include <iostream>

using namespace std;

int main() {
    int width, height;

    cout << "Введите ширину и высоту карты: ";
    if (!(cin >> width >> height) || width < 1 || height < 1 || width > 200 || height > 100) {
        cout << "Некорректные размеры карты (1-200 по ширине, 1-100 по высоте)" << endl;
        return 1;
    }

    PerlinNoise noise(static_cast<unsigned int>(time(nullptr)));
    MapGenerator generator(noise);
    vector<string> map = generator.generate(width, height);

    for (const string& row : map) {
        cout << row << endl;
    }

    cout << "~ - море, . - берег, , - равнина, T - лес, : - пустыня, ^ - горы" << endl;
    return 0;
}