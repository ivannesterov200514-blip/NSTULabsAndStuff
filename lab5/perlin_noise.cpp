#include "perlin_noise.h"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

using namespace std;

PerlinNoise::PerlinNoise(unsigned int seed) {
    array<int, 256> values;
    iota(values.begin(), values.end(), 0);

    mt19937 generator(seed);
    shuffle(values.begin(), values.end(), generator);

    for (int i = 0; i < 512; i++) {
        permutation[i] = values[i % 256];
    }
}

double PerlinNoise::fade(double t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

double PerlinNoise::lerp(double a, double b, double t) {
    return a + t * (b - a);
}

double PerlinNoise::grad(int hash, double x, double y) {
    switch (hash & 7) {
    case 0: return x + y;
    case 1: return -x + y;
    case 2: return x - y;
    case 3: return -x - y;
    case 4: return x;
    case 5: return -x;
    case 6: return y;
    default: return -y;
    }
}

double PerlinNoise::noise(double x, double y) const {
    int xi = static_cast<int>(floor(x)) & 255;
    int yi = static_cast<int>(floor(y)) & 255;
    double xf = x - floor(x);
    double yf = y - floor(y);

    double u = fade(xf);
    double v = fade(yf);

    int aa = permutation[permutation[xi] + yi];
    int ab = permutation[permutation[xi] + yi + 1];
    int ba = permutation[permutation[xi + 1] + yi];
    int bb = permutation[permutation[xi + 1] + yi + 1];

    double top = lerp(grad(aa, xf, yf), grad(ba, xf - 1, yf), u);
    double bottom = lerp(grad(ab, xf, yf - 1), grad(bb, xf - 1, yf - 1), u);
    return lerp(top, bottom, v);
}

double PerlinNoise::get_value(double x, double y) const {
    double result = 0;
    double amplitude = 1;
    double frequency = 1;
    double totalAmplitude = 0;

    for (int i = 0; i < 4; i++) {
        result += noise(x * frequency, y * frequency) * amplitude;
        totalAmplitude += amplitude;
        amplitude *= 0.5;
        frequency *= 2;
    }

    double value = 0.5 + result / totalAmplitude * 0.5;
    return max(0.0, min(1.0, value));
}
