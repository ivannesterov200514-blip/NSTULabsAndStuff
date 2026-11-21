#include "map_generator.h"

using namespace std;

MapGenerator::MapGenerator(const Noise& noise) : noise(noise) {}

char MapGenerator::get_terrain(double height, double moisture) const {
    if (height < 0.44) return '~';
    if (height < 0.48) return '.';
    if (height > 0.64) return '^';
    if (moisture < 0.45) return ':';
    if (moisture > 0.52) return 'T';
    return ',';
}

vector<string> MapGenerator::generate(int width, int height) const {
    vector<string> map(height, string(width, ' '));

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double h = noise.get_value(x * 0.065, y * 0.065);
            double m = noise.get_value(x * 0.055 + 37, y * 0.055 + 91);
            h = 0.5 + (h - 0.5) * 2.2;
            m = 0.5 + (m - 0.5) * 2.2;
            map[y][x] = get_terrain(h, m);
        }
    }

    return map;
}