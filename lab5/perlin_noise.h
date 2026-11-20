#pragma once
#include <array>

using namespace std;

class Noise {
public:
    virtual double get_value(double x, double y) const = 0;
    virtual ~Noise() = default;
};

class PerlinNoise : public Noise {
private:
    array<int, 512> permutation;

    static double fade(double t);
    static double lerp(double a, double b, double t);
    static double grad(int hash, double x, double y);
    double noise(double x, double y) const;

public:
    PerlinNoise(unsigned int seed);
    double get_value(double x, double y) const override;
};
