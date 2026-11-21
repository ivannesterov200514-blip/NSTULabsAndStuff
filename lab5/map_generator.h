#pragma once
#include "perlin_noise.h"
#include <string>
#include <vector>

using namespace std;

class MapGenerator {
private:
    const Noise& noise;
    char get_terrain(double height, double moisture) const;

public:
    MapGenerator(const Noise& noise);
    vector<string> generate(int width, int height) const;
};